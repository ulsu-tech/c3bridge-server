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


#ifndef SETTINGS_H
#define SETTINGS_H


class Settings
{
public:
    Settings();

    void load();
    void reset();
    void save();

public:
    long                     windowPositionX;
    long                     windowPositionY;
    long                     windowWidth;
    long                     windowHeight;
    long                     windowMinimumWidth;
    long                     windowMinimumHeight;
    bool                     windowIsMaximized;

    Win32xx::CString         networkTcpAddress;
    Win32xx::CString         networkUdpAddress;
    uint32_t                 networkTcpPort;
    uint32_t                 networkUdpPort;
    uint32_t                 networkUdpLegacyPort;
    uint32_t                 networkUdpLegacyPeer;
    uint32_t                 networkMessageTimeout;
    uint32_t                 networkIdleTimeout;
    bool                     networkTcpEnabled;
    bool                     networkUdpEnabled;
    bool                     networkUdpLegacyEnabled;

    uint32_t                 logLimit;
    uint32_t                 logSeverity;

private:
    static bool checkAddress(const Win32xx::CString& address);

    template <typename T>
    static T keyRead(Win32xx::CRegKey& key, LPCTSTR name, T defaultValue)
    {
        DWORD dwValue;

        if (key.QueryDWORDValue(name, dwValue) == ERROR_SUCCESS)
        {
            return static_cast<T>(dwValue);
        }
        else
        {
            return defaultValue;
        }
    }

    static bool keyReadBool(Win32xx::CRegKey& key, LPCTSTR name,
        bool defaultValue);
    static Win32xx::CString keyReadString(Win32xx::CRegKey& key, LPCTSTR name,
        const Win32xx::CString& defaultValue);

    template <typename T>
    static bool keyWrite(Win32xx::CRegKey& key, LPCTSTR name, T value)
    {
        return key.SetDWORDValue(name, static_cast<DWORD>(value)) ==
            ERROR_SUCCESS;
    }

    static bool keyWriteBool(Win32xx::CRegKey& key, LPCTSTR name, bool value);
    static bool keyWriteString(Win32xx::CRegKey& key, LPCTSTR name,
        const Win32xx::CString& value);

private:
    enum Defaults
    {
        DefaultX             = 0,
        DefaultY             = 0,
        DefaultWidth         = 500,
        DefaultHeight        = 200,
        DefaultTcpPort       = 7000,
        DefaultUdpPort       = 7000,
        DefaultUdpLegacyPort = 6999,
        DefaultUdpLegacyPeer = 7000,
        DefaultTimeout       = 0,
        DefaultLogLimit      = 4000,
        DefaultLogSeverity   = 4
    };

    enum Limits
    {
        LimitMinimumPort     = 1,
        LimitMaximumPort     = 65534,
        LimitMaximumSeverity = 6
    };

    static LPCTSTR DefaultTcpAddress;
    static LPCTSTR DefaultUdpAddress;
    static LPCTSTR KeyPath;
};


#endif // SETTINGS_H
