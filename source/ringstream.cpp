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

#include "ringstream.h"
#include "ringbuffer.h"
#include "bstring.h"


RingStream::RingStream(RingBuffer& buffer, ByteOrder order)
    : _buffer(buffer)
{
    union
    {
        unsigned char c[sizeof(unsigned int)];
        unsigned int  i;
    } endian;

    endian.i = 1;

    _systemOrder = (endian.c[0] == 1) ? LittleEndian : BigEndian;
    _currentOrder = order;

    reset();
}

void RingStream::reset()
{
    _offset = 0;
    _length = _buffer.bytesUsed();
}

bool RingStream::get(unsigned char& value)
{
    if (_buffer.getInSitu(&value, sizeof(unsigned char), _offset) == sizeof(unsigned char))
    {
        _offset += sizeof(unsigned char);
        return true;
    }

    return false;
}

bool RingStream::get(unsigned short& value)
{
    if (_buffer.getInSitu(&value, sizeof(unsigned short), _offset) == sizeof(unsigned short))
    {
        _offset += sizeof(unsigned short);

        if (_currentOrder != _systemOrder)
            value = (value << 8) | (value >> 8);

        return true;
    }

    return false;
}

bool RingStream::get(unsigned int& value)
{
    if (_buffer.getInSitu(&value, sizeof(unsigned int), _offset) == sizeof(unsigned int))
    {
        _offset += sizeof(unsigned int);

        if (_currentOrder != _systemOrder)
        {
            value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0x00FF00FF);
            value = (value << 16) | (value >> 16);
        }

        return true;
    }

    return false;
}

bool RingStream::get(short& value)
{
    if (_buffer.getInSitu(&value, sizeof(short), _offset) == sizeof(short))
    {
        _offset += sizeof(short);

        if (_currentOrder != _systemOrder)
            value = (value << 8) | (value >> 8);

        return true;
    }

    return false;
}

bool RingStream::get(signed int& value)
{
    if (_buffer.getInSitu(&value, sizeof(signed int), _offset) == sizeof(signed int))
    {
        _offset += sizeof(signed int);

        if (_currentOrder != _systemOrder)
        {
            value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0x00FF00FF);
            value = (value << 16) | (value >> 16);
        }

        return true;
    }

    return false;
}

bool RingStream::get(bool& value)
{
    unsigned char boolean;
    bool result = get(boolean);
    value = (boolean != 0);
    return result;
}

bool RingStream::get(std::string& value, size_t size)
{
    value.resize(size, '\0');

    if (_buffer.getInSitu(&value[0], size, _offset) == size)
    {
        _offset += size;
        return true;
    }

    return false;
}

bool RingStream::get(std::wstring& value, size_t size)
{
    value.resize(size, '\0');

    size *= sizeof(std::wstring::value_type);

    if (_buffer.getInSitu(&value[0], size, _offset) == size)
    {
        _offset += size;
        return true;
    }

    return false;
}

bool RingStream::get(BString& value, size_t size)
{
    value.resize(static_cast<UINT>(size));

    BSTR output = value.bstr();

    if (!output)
        return false;

    size *= sizeof(OLECHAR);

    if (_buffer.getInSitu(output, size, _offset) == size)
    {
        _offset += size;
        return true;
    }

    return false;
}
