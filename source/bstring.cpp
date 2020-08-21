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

#include "bstring.h"


//
// BString
//

BString::BString()
    : _data(NULL)
{
}

BString::BString(const BString& other)
    : _data(other._data)
{
    if (_data)
        _data->addReference();
}

BString::BString(LPCSTR string)
    : _data(new Data(string))
{
}

BString::BString(LPCWSTR string)
    : _data(new Data(string))
{
}

BString::BString(const std::string& string)
    : _data(new Data(string.c_str()))
{
}

BString::BString(const std::wstring& string)
    : _data(new Data(string.c_str()))
{
}

BString::BString(BSTR bstr, bool copy)
    : _data(new Data(bstr, copy))
{
}

BString::~BString()
{
    free();
}

BString& BString::operator=(const BString& other)
{
    if (this != &other)
    {
        free();

        _data = other._data;
        
        if (_data)
            _data->addReference();
    }

    return *this;
}

BString& BString::operator=(LPCSTR string)
{
    Data* newData = new Data(string);

    free();

    _data = newData;

    return *this;
}

BString& BString::operator=(LPCWSTR string)
{
    Data* newData = new Data(string);

    free();

    _data = newData;

    return *this;
}

BString& BString::operator=(const std::string& string)
{
    free();

    if (!string.empty())
        _data = new Data(string.c_str());

    return *this;
}

BString& BString::operator=(const std::wstring& string)
{
    free();

    if (!string.empty())
        _data = new Data(string.c_str());

    return *this;
}

bool BString::operator==(const BString& other)
{
    if (_data == other._data)
        return true;

    if (other._data == NULL)
        return (length() == 0);

    if (_data == NULL)
    {
        return (other.length() == 0);
    }
    else
    {
        return _data->compare(other._data);
    }
}

bool BString::operator==(LPCSTR string)
{
    return _data ? _data->compare(string) : (string == NULL);
}

bool BString::operator==(LPCWSTR string)
{
    return _data ? _data->compare(string) : (string == NULL);
}

bool BString::operator==(const std::string& string)
{
    return _data ? _data->compare(string.c_str()) : string.empty();
}

bool BString::operator==(const std::wstring& string)
{
    return _data ? _data->compare(string.c_str()) : string.empty();
}

bool BString::operator!=(const BString& other)
{
    if (_data == other._data)
        return false;

    if (other._data == NULL)
        return (length() != 0);

    if (_data == NULL)
    {
        return (other.length() != 0);
    }
    else
    {
        return !_data->compare(other._data);
    }
}

bool BString::operator!=(LPCSTR string)
{
    return _data ? !_data->compare(string) : (string != NULL);
}

bool BString::operator!=(LPCWSTR string)
{
    return _data ? !_data->compare(string) : (string != NULL);
}

bool BString::operator!=(const std::string& string)
{
    return _data ? !_data->compare(string.c_str()) : !string.empty();
}

bool BString::operator!=(const std::wstring& string)
{
    return _data ? !_data->compare(string.c_str()) : !string.empty();
}

OLECHAR BString::operator[](size_t index)
{
    return _data ? _data->at(index) : OLESTR('\0');
}

BSTR* BString::address()
{
    attach(NULL);
    return _data ? &_data->_string : NULL;
}

BSTR BString::bstr() const
{
    return _data ? _data->_string : NULL;
}

void BString::attach(BSTR bstr)
{
    free();
    _data = new Data(bstr, false);
}

UINT BString::length() const
{
    return _data ? _data->length() : 0;
}

void BString::clear()
{
    free();
}

void BString::resize(UINT size)
{
    free();
    _data = new Data(size);
}

std::string BString::toString() const
{
    std::string result;

    if (_data)
        _data->toString(result);

    return result;
}

std::wstring BString::toWideString() const
{
    std::wstring result;

    if (_data)
        _data->toWideString(result);

    return result;
}

void BString::free()
{
    if (_data)
    {
        _data->release();
        _data = NULL;
    }
}


//
// BString::Data
//

BString::Data::Data(LPCSTR string)
    : _counter(1)
    , _string(NULL)
{
    INT size = ::lstrlenA(string) + 1;

    if (!string || size < 1)
        return;

    INT wideSize = ::MultiByteToWideChar(CP_ACP, 0, string, size, NULL, 0);

    if (wideSize < 1)
        return;

    WCHAR* wideString = (WCHAR*) ::malloc((size_t) wideSize * sizeof(WCHAR));

    if (!wideString)
        return;

    if (::MultiByteToWideChar(CP_ACP, 0, string, size, wideString, wideSize) > 0)
        _string = ::SysAllocString(wideString);

    ::free(wideString);
}

BString::Data::Data(LPCWSTR string)
    : _counter(1)
{
    _string = ::SysAllocString(string);
}

BString::Data::Data(BSTR bstr, bool copy)
    : _counter(1)
{
    if (copy && bstr != NULL)
    {
        _string = ::SysAllocStringByteLen((LPCSTR) bstr, ::SysStringByteLen(bstr));
    }
    else
    {
        _string = bstr;
    }
}

BString::Data::Data(UINT size)
    : _counter(1)
{
    _string = ::SysAllocStringLen(NULL, size);
}

BString::Data::~Data()
{
    ::SysFreeString(_string);
    _string = NULL;
    _counter = 0;
}

OLECHAR BString::Data::at(UINT index) const
{
    return _string ? _string[index] : OLESTR('\0');
}

bool BString::Data::compare(LPCSTR string)
{
    INT thisLength = (INT) length();
    INT otherLength = ::lstrlenA(string);

    if (thisLength != otherLength)
        return false;

    otherLength++;

    INT wideLength = ::MultiByteToWideChar(CP_ACP, 0, string, otherLength, NULL, 0);

    if (wideLength < 1)
        return false;

    WCHAR* wideString = (WCHAR*) ::malloc((size_t) wideLength * sizeof(WCHAR));

    if (!wideString)
        return false;

    ::MultiByteToWideChar(CP_ACP, 0, string, otherLength, wideString, wideLength);

    WCHAR* leftString = (WCHAR*) _string;
    WCHAR* rightString = wideString;

    while (thisLength > 0)
    {
        if (*leftString != *rightString)
        {
            ::free(wideString);
            return false;
        }

        leftString++;
        rightString++;
        thisLength--;
    }

    ::free(wideString);
    return true;
}

bool BString::Data::compare(LPCWSTR string)
{
    UINT thisLength = length();
    UINT otherLength = (UINT) ::lstrlenW(string);

    if (thisLength != otherLength)
        return false;

    WCHAR* leftString = (WCHAR*) _string;

    while (thisLength > 0)
    {
        if (*leftString != *string)
            return false;

        leftString++;
        string++;
        thisLength--;
    }

    return true;
}

bool BString::Data::compare(const BString::Data* other)
{
    UINT thisLength = length();
    UINT otherLength = other ? other->length() : 0;

    if (thisLength != otherLength)
        return false;

    WCHAR* leftString = (WCHAR*) _string;
    WCHAR* rightString = (WCHAR*) other->_string;

    while (thisLength > 0)
    {
        if (*leftString != *rightString)
            return false;

        leftString++;
        rightString++;
        thisLength--;
    }

    return true;
}

UINT BString::Data::length() const
{
    return ::SysStringLen(_string);
}

void BString::Data::toString(std::string& string)
{
    INT size = (INT) length();

    string.resize(0);

    if (_string && size > 0)
    {
        INT mbSize = ::WideCharToMultiByte(CP_ACP, 0, _string, size, NULL, 0, NULL, NULL);

        if (mbSize > 0)
        {
            string.resize((size_t) mbSize);

            ::WideCharToMultiByte(CP_ACP, 0, _string, size, &string[0], mbSize, NULL, NULL);
        }
    }
}

void BString::Data::toWideString(std::wstring& string)
{
    string = _string;
}

LONG BString::Data::addReference()
{
    ::InterlockedIncrement(&_counter);
    return _counter;
}

LONG BString::Data::release()
{
    LONG value = ::InterlockedDecrement(&_counter);

    if (value < 1)
        delete this;

    return value;
}
