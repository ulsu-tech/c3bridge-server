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


#ifndef PROXY_H
#define PROXY_H


#define PROXY_TYPE                "C3 BRIDGE INTERFACE"
#define PROXY_NAME                "C3 Bridge Interface Server"
#define PROXY_PRODUCT             "C3 Bridge Interface"
#define PROXY_COMPANY             "S.P. Kapitsa Research Institute of Technology of Ulyanovsk State University"
#define PROXY_COPYRIGHT           "Copyright (C) 2020-2021 Dmitry Lavygin <vdm.inbox@gmail.com>"
#define PROXY_INTERNALNAME        "C3BRIDGE"
#define PROXY_FILENAME            "C3BRIDGE.EXE"
#define PROXY_DOMAIN              "ulsu.tech"

#define PROXY_VERSION_OPEN_SOURCE 0
#define PROXY_VERSION_PROPRIETARY 1
#define PROXY_VERSION_FREEWARE    2
#define PROXY_VERSION_INTERNAL    3

#define PROXY_VERSION_MAJOR       1
#define PROXY_VERSION_MINOR       3
#define PROXY_VERSION_TYPE        PROXY_VERSION_OPEN_SOURCE

#define PROXY_STR_HELPER(x)       #x
#define PROXY_STR(x)              PROXY_STR_HELPER(x)

#define PROXY_VERSION_STRING      PROXY_STR(PROXY_VERSION_MAJOR) "." \
                                  PROXY_STR(PROXY_VERSION_MINOR) "." \
                                  PROXY_STR(PROXY_VERSION_TYPE) ".0"


class Proxy
{
public:
    static Win32xx::CString getComputerName();
    static Win32xx::CString getIsoDateTime();
    static Win32xx::CString getVariable(const Win32xx::CString& variable);
};


#endif // PROXY_H
