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


#ifndef BSTRING_H
#define BSTRING_H


class BString
{
public:
    BString();
    BString(const BString& other);
    BString(LPCSTR string);
    BString(LPCWSTR string);
    BString(const std::string& string);
    BString(const std::wstring& string);
    BString(BSTR bstr, bool copy);
    ~BString();

    BString& operator=(const BString& other);
    BString& operator=(LPCSTR string);
    BString& operator=(LPCWSTR string);
    BString& operator=(const std::string& string);
    BString& operator=(const std::wstring& string);

    bool operator==(const BString& other);
    bool operator==(LPCSTR string);
    bool operator==(LPCWSTR string);
    bool operator==(const std::string& string);
    bool operator==(const std::wstring& string);

    bool operator!=(const BString& other);
    bool operator!=(LPCSTR string);
    bool operator!=(LPCWSTR string);
    bool operator!=(const std::string& string);
    bool operator!=(const std::wstring& string);

    OLECHAR operator[](size_t index);

    BSTR* address();
    BSTR bstr() const;

    void attach(BSTR bstr);

    UINT length() const;

    void clear();
    void resize(UINT size);

    std::string toString() const;
    std::wstring toWideString() const;

private:
    void free();

private:
    class Data
    {
    public:
        explicit Data(LPCSTR string);
        explicit Data(LPCWSTR string);
        explicit Data(BSTR bstr, bool copy);
        explicit Data(UINT size);
        ~Data();

        OLECHAR at(UINT index) const;

        bool compare(LPCSTR string);
        bool compare(LPCWSTR string);
        bool compare(const Data* other);

        UINT length() const;

        void toString(std::string& string);
        void toWideString(std::wstring& string);

        LONG addReference();
        LONG release();

    private:
        LONG _counter;
        BSTR _string;

        friend class BString;
    };

private:
    Data* _data;
};


#endif // BSTRING_H