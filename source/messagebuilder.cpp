/*
 * Copyright (c) 2020-2021 Dmitry Lavygin <vdm.inbox@gmail.com>
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

#include "messagebuilder.h"
#include "bstring.h"


MessageBuilder::MessageBuilder(char* data, size_t size, ByteOrder order)
    : _currentOrder(order)
    , _systemOrder(LittleEndian)
    , _data(data)
    , _offset(HeaderSize + TypeSize)
    , _size(size)
    , _payloadSize(0)
    , _tag(0)
    , _type(0)
    , _code(0)
    , _success(false)
{
    union
    {
        uint8_t  c[sizeof(uint32_t)];
        uint32_t i;
    } endian;

    endian.i = 1;

    _systemOrder = (endian.c[0] == 1) ? LittleEndian : BigEndian;
}

void MessageBuilder::reset()
{
    _code = 0;
    _success = false;

    _offset = HeaderSize + TypeSize;
    _payloadSize = 0;
}

bool MessageBuilder::build()
{
    if (!_data || _payloadSize > _size - HeaderAndFooter)
        return false;

    StandardHeader* header = reinterpret_cast<StandardHeader*>(_data);
    header->tag  = _tag;
    header->size = static_cast<uint16_t>(_payloadSize + TypeSize + FooterSize);
    header->type = _type;

    Footer* footer = reinterpret_cast<Footer*>(&_data[_offset]);
    footer->code = _code;
    footer->success = _success ? 1 : 0;

    if (_currentOrder != _systemOrder)
    {
        header->tag = (header->tag << 8) | (header->tag >> 8);
        header->size = (header->size << 8) | (header->size >> 8);
        footer->code = (footer->code << 8) | (footer->code >> 8);
    }

    return true;
}

void MessageBuilder::putUInt8(uint8_t value)
{
    _payloadSize += sizeof(value);

    if (_data && _payloadSize <= _size - HeaderAndFooter)
    {
        _data[_offset] = value;
        _offset += sizeof(value);
    }
}

void MessageBuilder::putUInt16(uint16_t value)
{
    _payloadSize += sizeof(value);

    if (_data && _payloadSize <= _size - HeaderAndFooter)
    {
        if (_currentOrder != _systemOrder)
            value = (value << 8) | (value >> 8);

        memcpy(&_data[_offset], &value, sizeof(value));
        _offset += sizeof(value);
    }
}

void MessageBuilder::putUInt32(uint32_t value)
{
    _payloadSize += sizeof(value);

    if (_data && _payloadSize <= _size - HeaderAndFooter)
    {
        if (_currentOrder != _systemOrder)
        {
            value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0x00FF00FF);
            value = (value << 16) | (value >> 16);
        }

        memcpy(&_data[_offset], &value, sizeof(value));
        _offset += sizeof(value);
    }
}

void MessageBuilder::putString(const std::string& value)
{
    if (value.size() < 1)
        return;

    _payloadSize += value.size();

    if (_data && _payloadSize <= _size - HeaderAndFooter)
    {
        memcpy(&_data[_offset], value.c_str(), value.size());
        _offset += value.size();
    }
}

void MessageBuilder::putWString(const std::wstring& value)
{
    size_t size = value.size() * sizeof(std::wstring::value_type);

    if (size < sizeof(std::wstring::value_type))
        return;

    _payloadSize += size;

    if (_data && _payloadSize <= _size - HeaderAndFooter)
    {
        memcpy(&_data[_offset], value.c_str(), size);
        _offset += size;
    }
}

void MessageBuilder::putBString(const BString& value)
{
    size_t size = value.length() * sizeof(OLECHAR);

    BSTR string = value.bstr();

    if (size < sizeof(OLECHAR) || !string)
        return;

    _payloadSize += size;

    if (_data && _payloadSize <= _size - HeaderAndFooter)
    {
        memcpy(&_data[_offset], string, size);
        _offset += size;
    }
}

void MessageBuilder::putArray(const uint8_t* value, size_t size)
{
    if (!value || size == 0)
        return;

    _payloadSize += size;

    if (_data && _payloadSize <= _size - HeaderAndFooter)
    {
        memcpy(&_data[_offset], value, size);
        _offset += size;
    }
}

void MessageBuilder::putStringWithSize(const std::string& value)
{
    putUInt16(static_cast<uint16_t>(value.size()));
    putString(value);
}

void MessageBuilder::putWStringWithSize(const std::wstring& value)
{
    putUInt16(static_cast<uint16_t>(value.size()));
    putWString(value);
}

void MessageBuilder::putBStringWithSize(const BString& value)
{
    putUInt16(static_cast<uint16_t>(value.length()));
    putBString(value);
}
