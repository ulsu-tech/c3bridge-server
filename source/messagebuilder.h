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


#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H


class BString;


class MessageBuilder
{
public:
    enum ByteOrder
    {
        BigEndian,
        LittleEndian
    };

    enum
    {
        HeaderSize = 4,
        TypeSize = 1,
        FooterSize = 3,
        MaximumPayload = 65535 - FooterSize - TypeSize,
        MaximumMessage = HeaderSize + MaximumPayload + FooterSize + TypeSize,
        HeaderAndFooter = HeaderSize + TypeSize + FooterSize
    };

public:
    MessageBuilder(char* data, size_t size, ByteOrder order = BigEndian);

    void reset();
    bool build();

    char* data();
    size_t size() const;
    uint8_t type() const;
    uint16_t tag() const;
    uint16_t errorCode() const;
    bool successFlag() const;

    void setType(uint8_t type);
    void setTag(uint16_t tag);
    void setResponseCode(uint16_t errorCode, bool successFlag);

    void putUInt8(uint8_t value);
    void putUInt16(uint16_t value);
    void putUInt32(uint32_t value);

    void putBool(bool value);
    void putInt8(int8_t value);
    void putInt16(int16_t value);
    void putInt32(int32_t value);

    void putString(const std::string& value);
    void putWString(const std::wstring& value);
    void putBString(const BString& value);

    void putArray(const uint8_t* value, size_t size);

    void putStringWithSize(const std::string& value);
    void putWStringWithSize(const std::wstring& value);
    void putBStringWithSize(const BString& value);

private:
#pragma pack(push, 1)
    struct StandardHeader
    {
        uint16_t tag;
        uint16_t size;
        uint8_t  type;
    };

    struct Footer
    {
        uint16_t code;
        uint8_t  success;
    };
#pragma pack(pop)

private:
    ByteOrder _currentOrder;
    ByteOrder _systemOrder;

    char*    _data;

    size_t   _offset;
    size_t   _size;
    size_t   _payloadSize;

    uint16_t _tag;
    uint8_t  _type;
    uint16_t _code;
    bool     _success;
};


inline char* MessageBuilder::data()
{
    return _data;
}

inline size_t MessageBuilder::size() const
{
    return HeaderSize + TypeSize + FooterSize + _payloadSize;
}

inline uint8_t MessageBuilder::type() const
{
    return _type;
}

inline uint16_t MessageBuilder::tag() const
{
    return _tag;
}

inline uint16_t MessageBuilder::errorCode() const
{
    return _code;
}

inline bool MessageBuilder::successFlag() const
{
    return _success;
}

inline void MessageBuilder::setType(uint8_t type)
{
    _type = type;
}

inline void MessageBuilder::setTag(uint16_t tag)
{
    _tag = tag;
}

inline void MessageBuilder::setResponseCode(uint16_t errorCode,
    bool successFlag)
{
    _code = errorCode;
    _success = successFlag;
}

inline void MessageBuilder::putBool(bool value)
{
    putUInt8(value ? 1 : 0);
}

inline void MessageBuilder::putInt8(int8_t value)
{
    putUInt8(static_cast<uint8_t>(value));
}

inline void MessageBuilder::putInt16(int16_t value)
{
    putUInt16(static_cast<uint16_t>(value));
}

inline void MessageBuilder::putInt32(int32_t value)
{
    putUInt32(static_cast<uint32_t>(value));
}


#endif // MESSAGEBUILDER_H
