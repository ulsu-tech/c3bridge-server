/*
 * Copyright (c) 2020-2022 Dmitry Lavygin <vdm.inbox@gmail.com>
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

#include "proxy.h"
#include "client.h"
#include "mainwindow.h"


Win32xx::CString Proxy::getComputerName()
{
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;

    Win32xx::CString result;

    GetComputerName(result.GetBuffer(size), &size);

    result.ReleaseBuffer();

    return result;
}

Win32xx::CString Proxy::getIsoDateTime()
{
    SYSTEMTIME time;
    ::GetSystemTime(&time);

    Win32xx::CString result;

    // ISO 8601 extended format yyyy-MM-ddTHH:mm:ss
    // with a time-zone suffix (Z for UTC)
    // Example: 2020-06-25T12:14:00Z
    result.Format(_T("%.4hu-%.2hu-%.2huT%.2hu:%.2hu:%.2huZ"),
        time.wYear, time.wMonth, time.wDay,
        time.wHour, time.wMinute, time.wSecond);

    return result;
}

Win32xx::CString Proxy::getVariable(const Win32xx::CString& variable)
{
    Win32xx::CString result;

    if (variable == _T("@PROXY_TYPE"))
    {
        result = _T(PROXY_TYPE);
    }
    else if (variable == _T("@PROXY_VERSION"))
    {
        result.Format(_T("%u.%u"), PROXY_VERSION_MAJOR, PROXY_VERSION_MINOR);

#if (PROXY_VERSION_TYPE == PROXY_VERSION_OPEN_SOURCE)
        result += _T(" (OPEN SOURCE)");
#elif (PROXY_VERSION_TYPE == PROXY_VERSION_PROPRIETARY)
        result += _T(" (PROPRIETARY)");
#elif (PROXY_VERSION_TYPE == PROXY_VERSION_FREEWARE)
        result += _T(" (FREEWARE)");
#elif (PROXY_VERSION_TYPE == PROXY_VERSION_INTERNAL)
        result += _T(" (INTERNAL BUILD)");
#endif
    }
    else if (variable == _T("@PROXY_FEATURES"))
    {
        bool print = false;

        std::basic_string<TCHAR> value;
        value.reserve(Client::TotalHandlers);

        for (size_t index = Client::TotalHandlers; index > 0; --index)
        {
            if (Client::_handlers[index - 1])
            {
                print = true;
                value.push_back(_T('1'));
            }
            else if (print || index <= 8)
            {
                value.push_back(_T('0'));
            }
        }

        result = value.c_str();
    }
    else if (variable == _T("@PROXY_HOSTNAME"))
    {
        result = getComputerName();
    }
    else if (variable == _T("@PROXY_TIME"))
    {
        result = getIsoDateTime();
    }
    else if (variable == _T("@PROXY_ADDRESS"))
    {
        Settings* settings = MainWindow::settings();
        if (settings)
            result = settings->networkTcpAddress;
    }
    else if (variable == _T("@PROXY_PORT"))
    {
        Settings* settings = MainWindow::settings();
        if (settings)
            result.Format(_T("%u"), settings->networkTcpPort);
    }
    else if (variable == _T("@PROXY_ENABLED"))
    {
        Settings* settings = MainWindow::settings();
        if (settings)
            result = (settings->networkTcpEnabled ? _T("TRUE") : _T("FALSE"));
    }

    return result;
}
