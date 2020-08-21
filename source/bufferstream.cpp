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

#include "bufferstream.h"
#include "bstring.h"


BufferStream::BufferStream(std::vector<char>& buffer, ByteOrder order)
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

void BufferStream::reset()
{
    _offset = 0;
}

size_t BufferStream::skip(size_t size)
{
    if (_buffer.size() < _offset + size)
        _buffer.resize(_offset + size);

    size_t result = _offset;

    _offset += size;

    return result;
}

void BufferStream::append(unsigned char value)
{
    if (_buffer.size() < _offset + sizeof(value))
        _buffer.resize(_offset + 2 * sizeof(value));

    _buffer[_offset] = static_cast<char>(value);
    _offset += sizeof(value);
}

void BufferStream::append(unsigned short value)
{
    if (_buffer.size() < _offset + sizeof(value))
        _buffer.resize(_offset + 2 * sizeof(value));

    if (_currentOrder != _systemOrder)
        value = (value << 8) | (value >> 8);

    memcpy(&_buffer[_offset], &value, sizeof(value));
    _offset += sizeof(value);
}

void BufferStream::append(unsigned int value)
{
    if (_buffer.size() < _offset + sizeof(value))
        _buffer.resize(_offset + 2 * sizeof(value));

    if (_currentOrder != _systemOrder)
    {
        value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0x00FF00FF);
        value = (value << 16) | (value >> 16);
    }

    memcpy(&_buffer[_offset], &value, sizeof(value));
    _offset += sizeof(value);
}

void BufferStream::append(bool value)
{
    append(static_cast<unsigned char>(value ? 1 : 0));
}

void BufferStream::append(const std::string& value)
{
    if (value.size() < 1)
        return;

    if (_buffer.size() < _offset + value.size())
        _buffer.resize(_offset + value.size());

    memcpy(&_buffer[_offset], value.c_str(), value.size());
    _offset += value.size();
}

void BufferStream::append(const std::wstring& value)
{
    size_t size = value.size() * sizeof(std::wstring::value_type);

    if (size < sizeof(std::wstring::value_type))
        return;

    if (_buffer.size() < _offset + size)
        _buffer.resize(_offset + size);

    memcpy(&_buffer[_offset], value.c_str(), size);
    _offset += size;
}

void BufferStream::append(const BString& value)
{
    size_t size = static_cast<size_t>(value.length()) * sizeof(OLECHAR);

    BSTR string = value.bstr();

    if (size < sizeof(OLECHAR) || !string)
        return;

    if (_buffer.size() < _offset + size)
        _buffer.resize(_offset + size);

    memcpy(&_buffer[_offset], string, size);
    _offset += size;
}

void BufferStream::append(int value)
{
    if (_buffer.size() < _offset + sizeof(value))
        _buffer.resize(_offset + 2 * sizeof(value));

    if (_currentOrder != _systemOrder)
    {
        value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0x00FF00FF);
        value = (value << 16) | (value >> 16);
    }

    memcpy(&_buffer[_offset], &value, sizeof(value));
    _offset += sizeof(value);
}

void BufferStream::append(long value)
{
    append(static_cast<int>(value));
}

void BufferStream::set(size_t position, unsigned short value)
{
    if (_buffer.size() < position + sizeof(value))
        _buffer.resize(position + 2 * sizeof(value));

    if (_currentOrder != _systemOrder)
        value = (value << 8) | (value >> 8);

    memcpy(&_buffer[position], &value, sizeof(value));
}
