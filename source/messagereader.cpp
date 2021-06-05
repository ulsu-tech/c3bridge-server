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

#include "messagereader.h"
#include "bstring.h"


MessageReader::MessageReader(char* data, size_t size, ByteOrder order)
    : _currentOrder(order)
    , _systemOrder(LittleEndian)
{
    union
    {
        uint8_t  c[sizeof(uint32_t)];
        uint32_t i;
    } endian;

    endian.i = 1;

    _systemOrder = (endian.c[0] == 1) ? LittleEndian : BigEndian;

    reset(data, size);
}

void MessageReader::reset(char* data, size_t size)
{
    _data = data;
    _offset = 0;
    _size = size;
    _error = false;
}

bool MessageReader::getUInt8(uint8_t& value)
{
    if (!_data || _offset + sizeof(value) > _size)
    {
        _error = true;
        return false;
    }

    value = static_cast<uint8_t>(_data[_offset]);
    _offset += sizeof(value);

    return true;
}

bool MessageReader::getUInt16(uint16_t& value)
{
    if (!_data || _offset + sizeof(value) > _size)
    {
        _error = true;
        return false;
    }

    memcpy(&value, &_data[_offset], sizeof(value));
    _offset += sizeof(value);

    if (_currentOrder != _systemOrder)
        value = (value << 8) | (value >> 8);

    return true;
}

bool MessageReader::getUInt32(uint32_t& value)
{
    if (!_data || _offset + sizeof(value) > _size)
    {
        _error = true;
        return false;
    }

    memcpy(&value, &_data[_offset], sizeof(value));
    _offset += sizeof(value);

    if (_currentOrder != _systemOrder)
    {
        value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0x00FF00FF);
        value = (value << 16) | (value >> 16);
    }

    return true;
}

bool MessageReader::getBool(bool& value)
{
    uint8_t boolean;
    bool result = getUInt8(boolean);
    value = (boolean != 0);
    return result;
}

bool MessageReader::getString(std::string& value, size_t size)
{
    if (!_data || _offset + size > _size)
    {
        value.resize(0);
        _error = true;
        return false;
    }

    if (size < 1)
    {
        value.resize(0);
        return true;
    }

    value.resize(size, '\0');

    memcpy(&value[0], &_data[_offset], size);
    _offset += size;

    return true;
}

bool MessageReader::getWString(std::wstring& value, size_t size)
{
    if (!_data ||
       (_offset + size * sizeof(std::wstring::value_type)) > _size)
    {
        value.resize(0);
        _error = true;
        return false;
    }

    if (size < 1)
    {
        value.resize(0);
        return true;
    }

    value.resize(size, L'\0');

    size *= sizeof(std::wstring::value_type);

    memcpy(&value[0], &_data[_offset], size);
    _offset += size;

    return true;
}

bool MessageReader::getBString(BString& value, size_t size)
{
    if (!_data || (_offset + size * sizeof(OLECHAR)) > _size)
    {
        value.clear();
        _error = true;
        return false;
    }

    if (size < 1)
    {
        value.clear();
        return true;
    }

    value.resize(static_cast<UINT>(size));

    BSTR output = value.bstr();

    if (!output)
    {
        _error = true;
        return false;
    }

    size *= sizeof(OLECHAR);

    memcpy(output, &_data[_offset], size);
    _offset += size;

    return true;
}

bool MessageReader::getArray(uint8_t* value, size_t size)
{
    if (!value || !_data || _offset + size > _size)
    {
        _error = true;
        return false;
    }

    if (size == 0)
        return true;

    memcpy(value, &_data[_offset], size);
    _offset += size;

    return true;
}

bool MessageReader::getStringWithSize(std::string& value)
{
    uint16_t size = 0;

    if (!getUInt16(size))
        return false;
    
    return getString(value, size);
}

bool MessageReader::getWStringWithSize(std::wstring& value)
{
    uint16_t size = 0;

    if (!getUInt16(size))
        return false;

    return getWString(value, size);
}

bool MessageReader::getBStringWithSize(BString& value)
{
    uint16_t size = 0;

    if (!getUInt16(size))
        return false;

    return getBString(value, size);

}
