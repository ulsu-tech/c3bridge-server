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

#include <c3bi.h>

#include "cross3.h"
#include "bstring.h"
#include "client.h"
#include "messagebuilder.h"
#include "messagereader.h"
#include "proxy.h"


void Client::handleProxyInfo(MessageReader& stream, MessageBuilder& output)
{
    SYSTEMTIME time;
    ::GetSystemTime(&time);

    BString computerName = Proxy::getComputerName().c_str();

    // ANSWER
    output.setResponseCode(C3BI::ErrorSuccess, true);
    output.putUInt8(PROXY_VERSION_MAJOR);
    output.putUInt8(PROXY_VERSION_MINOR);
    output.putUInt8(PROXY_VERSION_TYPE);
    output.putUInt16(time.wYear);
    output.putUInt16(time.wMonth);
    output.putUInt16(time.wDayOfWeek);
    output.putUInt16(time.wDay);
    output.putUInt16(time.wHour);
    output.putUInt16(time.wMinute);
    output.putUInt16(time.wSecond);
    output.putUInt16(time.wMilliseconds);
    // Computer name
    output.putBStringWithSize(computerName);

    reply(output);
}

void Client::handleProxyFeatures(MessageReader& stream, MessageBuilder& output)
{
    // ANSWER
    output.setResponseCode(C3BI::ErrorSuccess, true);

    // Payload data
    for (size_t octet = 32; octet > 0; --octet)
    {
        uint8_t field = 0;

        for (size_t bit = 1; bit <= 8; ++bit)
        {
            field <<= 1;

            size_t index = octet * 8 - bit;

            if (index < TotalHandlers && _handlers[index])
                field |= 1;
        }

        output.putUInt8(field);
    }

    reply(output);
}

void Client::handleBenchmark(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    // REQUEST PAYLOAD
    bool write = false;
    uint8_t count = 0;
    uint32_t iterations = 0;

    ok = stream.getBool(write);
    ok = ok ? stream.getUInt8(count) : false;
    ok = ok ? stream.getUInt32(iterations) : false;

    if (iterations == UINT32_MAX)
        iterations = UINT32_MAX - 1;

    if (count > 10)
        count = 10;

    BString variables[10];
    BString values[10];

    for (uint8_t i = 0; i < count; ++i)
    {
        if (!ok)
            break;

        ok = ok ? stream.getBStringWithSize(variables[i]) : false;
        if (write)
            ok = ok ? stream.getBStringWithSize(values[i]) : false;
    }

    uint32_t start = 0;
    uint32_t stop = 0;

    if (ok)
    {
        if (write)
        {
            start = ::GetTickCount();
            for (uint32_t iteration = 0; iteration < iterations; ++iteration)
            {
                for (uint8_t i = 0; i < count; ++i)
                    Cross3::instance()->setVariable(variables[i], values[i]);
            }
            stop = ::GetTickCount();
        }
        else
        {
            start = ::GetTickCount();
            for (uint32_t iteration = 0; iteration < iterations; ++iteration)
            {
                for (uint8_t i = 0; i < count; ++i)
                    Cross3::instance()->getVariable(variables[i], values[i]);
            }
            stop = ::GetTickCount();
        }
        code = C3BI::ErrorSuccess;
    }

    // ANSWER
    output.setResponseCode(code, ok);
    output.putBool(write);
    output.putUInt8(count);
    output.putUInt32(iterations);
    output.putUInt32(start);
    output.putUInt32(stop);
    output.putUInt32(stop - start);
    reply(output);
}
