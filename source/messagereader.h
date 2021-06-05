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


#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H


class BString;


class MessageReader
{
public:
    enum ByteOrder
    {
        BigEndian,
        LittleEndian
    };

public:
    MessageReader(char* data, size_t size, ByteOrder order = BigEndian);

    void reset(char* data, size_t size);

    bool getUInt8(uint8_t& value);
    bool getUInt16(uint16_t& value);
    bool getUInt32(uint32_t& value);

    bool getBool(bool& value);
    bool getInt8(int8_t& value);
    bool getInt16(int16_t& value);
    bool getInt32(int32_t& value);

    bool getString(std::string& value, size_t size);
    bool getWString(std::wstring& value, size_t size);
    bool getBString(BString& value, size_t size);

    bool getArray(uint8_t* value, size_t size);

    bool getStringWithSize(std::string& value);
    bool getWStringWithSize(std::wstring& value);
    bool getBStringWithSize(BString& value);

private:
    ByteOrder _currentOrder;
    ByteOrder _systemOrder;

    char*    _data;

    size_t   _offset;
    size_t   _size;
    bool     _error;
};


inline bool MessageReader::getInt8(int8_t& value)
{
    return getUInt8(reinterpret_cast<uint8_t&>(value));
}

inline bool MessageReader::getInt16(int16_t& value)
{
    return getUInt16(reinterpret_cast<uint16_t&>(value));
}

inline bool MessageReader::getInt32(int32_t& value)
{
    return getUInt32(reinterpret_cast<uint32_t&>(value));
}


#endif // MESSAGEREADER_H
