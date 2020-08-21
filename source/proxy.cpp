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

#include "proxy.h"
#include "utilities.h"


BYTE    Proxy::_features[32] = { 0 };
LPCTSTR Proxy::_type         = _T("C3 BRIDGE INTERFACE");


Win32xx::CString Proxy::getVariable(const Win32xx::CString& variable)
{
    Win32xx::CString result;

    if (variable == _T("@PROXY_TYPE"))
    {
        result = _type;
    }
    else if (variable == _T("@PROXY_VERSION"))
    {
        result.Format(_T("%u.%u"), VersionMajor, VersionMinor);

        if (VersionType == VersionOpenSource)
        {
            result += _T(" (OPEN SOURCE)");
        }
        else if (VersionType == VersionProprietary)
        {
            result += _T(" (PROPRIETARY)");
        }
    }
    else if (variable == _T("@PROXY_FEATURES"))
    {
        size_t highest = 0;
		size_t octet = 0;

        for (octet = 31; octet > 0; --octet)
        {
            if (_features[octet] > 0)
            {
                highest = octet;
                break;
            }
        }

        std::basic_string<TCHAR> value((highest + 1) * 8, _T('0'));

        for (octet = 0; octet < 32; ++octet)
        {
            if (octet > highest && octet != 0)
                break;

            for (size_t i = 0; i < 8; ++i)
            {
                if (_features[octet] & (1 << i))
                    value[value.size() - (octet * 8 + i) - 1] = _T('1');
            }
        }

        result = value.c_str();
    }
    else if (variable == _T("@PROXY_HOSTNAME"))
    {
        result = Utilities::getComputerName();
    }
    else if (variable == _T("@PROXY_TIME"))
    {
        result = Utilities::getIsoDateTime();
    }

    return result;
}

void Proxy::registerFeature(BYTE id)
{
    _features[id / 8] |= (1 << (id % 8));
}

BYTE Proxy::getFeatureOctet(BYTE id)
{
    return (id < 32) ? _features[id] : 0;
}
