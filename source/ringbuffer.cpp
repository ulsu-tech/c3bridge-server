/*
 * Copyright (c) 2020 Dmitry Lavygin <vdm.inbox@gmail.com>
 * S.P. Kapitsa Research Institute of Technology of Ulyanovsk State University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 *     3. Neither the name of the copyright holder nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include "stdafx.h"

#include "ringbuffer.h"


RingBuffer::RingBuffer(size_t size)
    : _buffer(size)
    , _input(0)
    , _output(0)
{
}

size_t RingBuffer::bytesUsed() const
{
    if (_input < _output)
    {
        return _buffer.size() - _output + _input;
    }
    else
    {
        return _input - _output;
    }
}

size_t RingBuffer::bytesFree() const
{
    if (_input < _output)
    {
        return _output - _input - 1;
    }
    else
    {
        return _buffer.size() - _input + _output - 1;
    }
}

bool RingBuffer::put(const void* data, size_t size)
{
    if (size == 0)
        return true;

    if (_input < _output)
    {
        size_t limit = _output - _input - 1;

        if (limit < size)
            return false;

        memcpy(&_buffer[_input], data, size);
        _input += size;
    }
    else
    {
        size_t tail = _buffer.size() - _input;
        size_t head = _output - 1;

        if (_output == 0)
        {
            tail--;
            head = 0;
        }

        if (tail + head < size)
            return false;

        if (size < tail)
        {
            tail = size;
            head = 0;
        }
        else
        {
            head = size - tail;
        }

        memcpy(&_buffer[_input], data, tail);
        _input += tail;

        if (head)
        {
            memcpy(&_buffer[0], static_cast<const char*>(data) + tail, head);
            _input = head;
        }
    }

    if (_input >= _buffer.size())
        _input = 0;

    return true;
}

size_t RingBuffer::get(void* data, size_t size)
{
    size_t result = getInSitu(data, size, 0);
    skip(result);
    return result;
}

size_t RingBuffer::getInSitu(void* data, size_t size, size_t offset) const
{
    size_t limit = bytesUsed();

    if (limit <= offset)
        return 0;

    if (size < limit - offset)
        limit = offset + size;

    char value = 0;

    for (size_t i = offset; i < limit; ++i)
    {
        if (_output + i >= _buffer.size())
        {
            value = _buffer[_output + i - _buffer.size()];
        }
        else
        {
            value = _buffer[_output + i];
        }

        static_cast<char*>(data)[i - offset] = value;
    }

    return limit - offset;
}

void RingBuffer::skip(size_t count)
{
    if (_input < _output)
    {
        _output += count;

        if (_output >= _buffer.size())
        {
            _output -= _buffer.size();

            if (_output > _input)
                _output = _input;
        }
    }
    else
    {
        _output += count;

        if (_output > _input)
            _output = _input;
    }
}
