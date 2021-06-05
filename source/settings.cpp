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

#include "settings.h"


LPCTSTR Settings::DefaultTcpAddress = _T("0.0.0.0");
LPCTSTR Settings::DefaultUdpAddress = _T("0.0.0.0");
LPCTSTR Settings::KeyPath = _T("Software\\C3 Bridge Interface");


Settings::Settings()
{
    reset();
}

void Settings::load()
{
    reset();

    int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    Win32xx::CRegKey key;

    if (key.Open(HKEY_CURRENT_USER, KeyPath, KEY_READ) != ERROR_SUCCESS)
    {
        windowPositionX = (screenWidth - windowWidth) / 2;
        windowPositionY = (screenHeight - windowHeight) / 2;
        return;
    }

    windowWidth = keyRead(key, _T("WindowWidth"), windowWidth);
    windowHeight = keyRead(key, _T("WindowHeight"), windowHeight);
    windowPositionX = keyRead(key, _T("WindowPositionX"), 
        (screenWidth - windowWidth) / 2);
    windowPositionY = keyRead(key, _T("WindowPositionY"),
        (screenHeight - windowHeight) / 2);
    windowIsMaximized = keyReadBool(key, _T("WindowIsMaximized"),
        windowIsMaximized);

    networkTcpAddress = keyReadString(key, _T("NetworkTcpAddress"),
        networkTcpAddress);
    if (!checkAddress(networkTcpAddress))
        networkTcpAddress = DefaultTcpAddress;

    networkUdpAddress = keyReadString(key, _T("NetworkUdpAddress"),
        networkUdpAddress);
    if (!checkAddress(networkUdpAddress))
        networkUdpAddress = DefaultUdpAddress;

    networkTcpPort = keyRead(key, _T("NetworkTcpPort"), networkTcpPort);
    if (networkTcpPort < LimitMinimumPort || networkTcpPort > LimitMaximumPort)
        networkTcpPort = DefaultTcpPort;

    networkUdpPort = keyRead(key, _T("NetworkUdpPort"), networkUdpPort);
    if (networkUdpPort < LimitMinimumPort || networkUdpPort > LimitMaximumPort)
        networkUdpPort = DefaultUdpPort;

    networkUdpLegacyPort = keyRead(key, _T("NetworkUdpLegacyPort"),
        networkUdpLegacyPort);
    if (networkUdpLegacyPort < LimitMinimumPort ||
        networkUdpLegacyPort > LimitMaximumPort)
    {
        networkUdpLegacyPort = DefaultUdpLegacyPort;
    }

    networkUdpLegacyPeer = keyRead(key, _T("NetworkUdpLegacyPeer"),
        networkUdpLegacyPeer);
    if (networkUdpLegacyPeer < LimitMinimumPort ||
        networkUdpLegacyPeer > LimitMaximumPort)
    {
        networkUdpLegacyPeer = DefaultUdpLegacyPeer;
    }

    networkIdleTimeout = keyRead(key, _T("NetworkIdleTimeout"),
        networkIdleTimeout);

    networkMessageTimeout = keyRead(key, _T("NetworkMessageTimeout"),
        networkMessageTimeout);

    networkTcpEnabled = keyReadBool(key, _T("NetworkTcpEnabled"),
        networkTcpEnabled);
    networkUdpEnabled = keyReadBool(key, _T("NetworkUdpEnabled"),
        networkUdpEnabled);
    networkUdpLegacyEnabled = keyReadBool(key, _T("NetworkUdpLegacyEnabled"),
        networkUdpLegacyEnabled);

    logLimit = keyRead(key, _T("LogLimit"), logLimit);
    logSeverity = keyRead(key, _T("LogSeverity"), logSeverity);
}

void Settings::reset()
{
    windowPositionX = DefaultX;
    windowPositionY = DefaultY;
    windowWidth = DefaultWidth;
    windowHeight = DefaultHeight;
    windowMinimumWidth = DefaultWidth;
    windowMinimumHeight = DefaultHeight;
    windowIsMaximized = false;

    networkTcpAddress = DefaultTcpAddress;
    networkUdpAddress = DefaultUdpAddress;
    networkTcpPort = DefaultTcpPort;
    networkUdpPort = DefaultUdpPort;
    networkUdpLegacyPort = DefaultUdpLegacyPort;
    networkUdpLegacyPeer = DefaultUdpLegacyPeer;
    networkIdleTimeout = DefaultTimeout;
    networkMessageTimeout = DefaultTimeout;
    networkTcpEnabled = true;
    networkUdpEnabled = true;
    networkUdpLegacyEnabled = true;

    logLimit = DefaultLogLimit;
    logSeverity = DefaultLogSeverity;
}

void Settings::save()
{
    Win32xx::CRegKey key;

    if (key.Create(HKEY_CURRENT_USER, KeyPath) != ERROR_SUCCESS)
        return;

    if (key.Open(HKEY_CURRENT_USER, KeyPath) != ERROR_SUCCESS)
        return;

    keyWrite(key, _T("WindowWidth"), windowWidth);
    keyWrite(key, _T("WindowHeight"), windowHeight);
    keyWrite(key, _T("WindowPositionX"), windowPositionX);
    keyWrite(key, _T("WindowPositionY"), windowPositionY);
    keyWriteBool(key, _T("WindowIsMaximized"), windowIsMaximized);

    if (!checkAddress(networkTcpAddress))
        networkTcpAddress = DefaultTcpAddress;

    keyWriteString(key, _T("NetworkTcpAddress"), networkTcpAddress);

    if (!checkAddress(networkUdpAddress))
        networkUdpAddress = DefaultUdpAddress;

    keyWriteString(key, _T("NetworkUdpAddress"), networkUdpAddress);

    if (networkTcpPort < LimitMinimumPort || networkTcpPort > LimitMaximumPort)
        networkTcpPort = DefaultTcpPort;

    keyWrite(key, _T("NetworkTcpPort"), networkTcpPort);

    if (networkUdpPort < LimitMinimumPort || networkUdpPort > LimitMaximumPort)
        networkUdpPort = DefaultUdpPort;

    keyWrite(key, _T("NetworkUdpPort"), networkUdpPort);

    if (networkUdpLegacyPort < LimitMinimumPort ||
        networkUdpLegacyPort > LimitMaximumPort)
    {
        networkUdpLegacyPort = DefaultUdpLegacyPort;
    }

    keyWrite(key, _T("NetworkUdpLegacyPort"), networkUdpLegacyPort);

    if (networkUdpLegacyPeer < LimitMinimumPort ||
        networkUdpLegacyPeer > LimitMaximumPort)
    {
        networkUdpLegacyPeer = DefaultUdpLegacyPeer;
    }

    keyWrite(key, _T("NetworkUdpLegacyPeer"), networkUdpLegacyPeer);

    keyWrite(key, _T("NetworkIdleTimeout"), networkIdleTimeout);
    keyWrite(key, _T("NetworkMessageTimeout"), networkMessageTimeout);

    keyWriteBool(key, _T("NetworkTcpEnabled"), networkTcpEnabled);
    keyWriteBool(key, _T("NetworkUdpEnabled"), networkUdpEnabled);
    keyWriteBool(key, _T("NetworkUdpLegacyEnabled"), networkUdpLegacyEnabled);

    keyWrite(key, _T("LogLimit"), logLimit);
    keyWrite(key, _T("LogSeverity"), logSeverity);

    if (logSeverity > LimitMaximumSeverity)
        logSeverity = DefaultLogSeverity;
}

bool Settings::checkAddress(const Win32xx::CString& address)
{
    int octetIndex = 0;
    int octetValue = 0;
    int octetCounter = 0;

    for (int i = 0; i < address.GetLength(); ++i)
    {
        if (address[i] == _T('.'))
        {
            if (octetCounter < 1)
                return false;

            octetIndex++;
            octetValue = 0;
            octetCounter = 0;
        }
        else if (address[i] >= _T('0') && address[i] <= _T('9'))
        {
            if (octetCounter > 0)
                octetValue *= 10;

            octetValue += address[i] - _T('0');
            octetCounter++;
        }
        else
        {
            return false;
        }

        if (octetIndex > 3 || octetValue > 255 || octetCounter > 3)
            return false;
    }

    return true;
}

bool Settings::keyReadBool(Win32xx::CRegKey& key, LPCTSTR name, bool defaultValue)
{
    DWORD dwValue;

    if (key.QueryDWORDValue(name, dwValue) == ERROR_SUCCESS)
    {
        return (dwValue != 0);
    }
    else
    {
        return defaultValue;
    }
}

Win32xx::CString Settings::keyReadString(Win32xx::CRegKey& key, LPCTSTR name, 
    const Win32xx::CString& defaultValue)
{
    Win32xx::CString result;
    DWORD bufferSize = 0;

    if (key.QueryStringValue(name, NULL, &bufferSize) != ERROR_SUCCESS)
    {
        return defaultValue;
    }

    if (key.QueryStringValue(name, result.GetBuffer(bufferSize),
        &bufferSize) == ERROR_SUCCESS)
    {
        result.ReleaseBuffer();
        return result;
    }
    else
    {
        result.ReleaseBuffer();
        return defaultValue;
    }
}

bool Settings::keyWriteBool(Win32xx::CRegKey& key, LPCTSTR name, bool value)
{
    return key.SetDWORDValue(name, static_cast<DWORD>(value)) == ERROR_SUCCESS;
}

bool Settings::keyWriteString(Win32xx::CRegKey& key, LPCTSTR name, const Win32xx::CString& value)
{
    return key.SetStringValue(name, value) == ERROR_SUCCESS;
}
