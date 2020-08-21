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


#ifndef BUFFERSTREAM_H
#define BUFFERSTREAM_H


class BString;


class BufferStream
{
public:
    enum ByteOrder
    {
        BigEndian,
        LittleEndian
    };

    BufferStream(std::vector<char>& buffer, ByteOrder order);

    void reset();

    char* data();

    size_t size() const;
    size_t skip(size_t size);

    void append(unsigned char value);
    void append(unsigned short value);
    void append(unsigned int value);
    void append(bool value);
    void append(int value);
    void append(long value);
    void append(const std::string& value);
    void append(const std::wstring& value);
    void append(const BString& value);

    void set(size_t position, unsigned short value);

private:
    std::vector<char>& _buffer;

    size_t _offset;

    ByteOrder _currentOrder;
    ByteOrder _systemOrder;
};


inline char* BufferStream::data()
{
    return &_buffer[0];
}

inline size_t BufferStream::size() const
{
    return _offset;
}


#endif // BUFFERSTREAM_H
