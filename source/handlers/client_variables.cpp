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
#include "log.h"
#include "resource.h"


void Client::handleReadVariableAscii(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = false;
    uint16_t code = C3BI::ErrorGeneral;

    // REQUEST PAYLOAD
    std::string variable;
    ok = stream.getStringWithSize(variable);

    std::string value;

    if (ok)
    {
        code = Cross3::instance()->getVariable(variable, value);

        if (code != C3BI::ErrorSuccess)
        {
            ok = false;
            code = C3BI::ErrorGeneral;

            if (variable == "PING")
            {
                value = "PONG";

                ok = true;
                code = C3BI::ErrorSuccess;
            }
            else if (variable.size() > 0 && variable[0] == '@')
            {
                value = Win32xx::TtoA(
                    Proxy::getVariable(variable.c_str()).c_str());

                if (value.size() > 0)
                {
                    ok = true;
                    code = C3BI::ErrorSuccess;
                }
            }
        }
    }

#if defined(UNICODE) || defined(_UNICODE)
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_READ_VARIABLE_ASCIIW,
        _id, variable.c_str(), value.c_str());
#else
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_READ_VARIABLE_ASCII,
        _id, variable.c_str(), value.c_str());
#endif

    // ANSWER
    output.putStringWithSize(value);
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleWriteVariableAscii(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = false;
    uint16_t code = C3BI::ErrorGeneral;

    // REQUEST PAYLOAD
    std::string variable;
    std::string value;

    ok = stream.getStringWithSize(variable);
    ok = ok ? stream.getStringWithSize(value) : false;

    if (ok)
    {
        code = Cross3::instance()->setVariable(variable, value);

        if (code != C3BI::ErrorSuccess)
        {
#if defined(_MSC_VER) && _MSC_VER < 1300   // For Visual Studio 6
			value = "";
#else
            value.clear();
#endif

            ok = false;
            code = C3BI::ErrorGeneral;
        }
    }

#if defined(UNICODE) || defined(_UNICODE)
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_WRITE_VARIABLE_ASCIIW,
        _id, variable.c_str(), value.c_str());
#else
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_WRITE_VARIABLE_ASCII,
        _id, variable.c_str(), value.c_str());
#endif

    // ANSWER
    output.putStringWithSize(value);
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleReadVariable(MessageReader& stream, MessageBuilder& output)
{
    bool ok = false;
    uint16_t code = C3BI::ErrorProtocol;

    // REQUEST PAYLOAD
    BString variable;
    ok = stream.getBStringWithSize(variable);

    BString value;

    if (ok)
    {
        code = Cross3::instance()->getVariable(variable, value);

        if (code != C3BI::ErrorSuccess)
        {
            ok = false;

            if (variable == OLESTR("PING"))
            {
                value = OLESTR("PONG");

                ok = true;
                code = C3BI::ErrorSuccess;
            }
            else if (variable.length() > 0 && variable[0] == OLESTR('@'))
            {
                value = Proxy::getVariable(variable.bstr()).c_str();

                if (value.length() > 0)
                {
                    ok = true;
                    code = C3BI::ErrorSuccess;
                }
            }
        }
    }

#if defined(UNICODE) || defined(_UNICODE)
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_READ_VARIABLEW,
        _id, variable.toWideString().c_str(), value.toWideString().c_str());
#else
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_READ_VARIABLE,
        _id, variable.toWideString().c_str(), value.toWideString().c_str());
#endif

    // ANSWER
    output.putBStringWithSize(value);
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleWriteVariable(MessageReader& stream, MessageBuilder& output)
{
    bool ok = false;
    uint16_t code = C3BI::ErrorProtocol;

    // REQUEST PAYLOAD
    BString variable;
    BString value;

    ok = stream.getBStringWithSize(variable);
    ok = ok ? stream.getBStringWithSize(value) : false;

    if (ok)
    {
        code = Cross3::instance()->setVariable(variable, value);

        if (code != C3BI::ErrorSuccess)
        {
            value.clear();
            ok = false;
        }
    }

#if defined(UNICODE) || defined(_UNICODE)
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_WRITE_VARIABLEW,
        _id, variable.toWideString().c_str(), value.toWideString().c_str());
#else
    Log::format(Log::Debug, IDS_CLIENT_DEBUG_WRITE_VARIABLE,
        _id, variable.toWideString().c_str(), value.toWideString().c_str());
#endif

    // ANSWER
    output.putBStringWithSize(value);
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleReadMultiple(MessageReader& stream, MessageBuilder& output)
{
    bool ok = false;
    bool error = false;
    uint16_t code = C3BI::ErrorProtocol;

    // REQUEST PAYLOAD
    uint8_t count = 0;
    ok = stream.getUInt8(count);

    BString variable;
    BString value;

    // ANSWER
    output.putUInt8(count);

    for (size_t i = 0; i < count; ++i)
    {
        variable.clear();
        value.clear();

        ok = ok ? stream.getBStringWithSize(variable) : false;

        if (!ok)
            break;

        code = Cross3::instance()->getVariable(variable, value);

        if (code != C3BI::ErrorSuccess)
        {
            if (variable == OLESTR("PING"))
            {
                value = OLESTR("PONG");
            }
            else if (variable.length() > 0 && variable[0] == OLESTR('@'))
            {
                value = Proxy::getVariable(variable.bstr()).c_str();
            }

            if (value.length() < 1)
            {
                error = true;
            }
            else
            {
                code = C3BI::ErrorSuccess;
            }
        }

        // ANSWER PAYLOAD (CONTINUED)
        output.putUInt8(static_cast<uint8_t>(code));
        output.putBStringWithSize(value);
    }

    if (ok)
    {
        ok = !error;
        code = error ? C3BI::ErrorGeneral : C3BI::ErrorSuccess;
    }
    else
    {
        output.reset();
        output.putUInt8(0);
        code = C3BI::ErrorProtocol;
    }

    output.setResponseCode(code, ok);

    if (!output.build())
    {
        output.reset();
        output.putUInt8(0);
        output.setResponseCode(C3BI::ErrorLongAnswer, false);
    }

    reply(output);
}

void Client::handleWriteMultiple(MessageReader& stream, MessageBuilder& output)
{
    bool ok = false;
    bool error = false;
    uint16_t code = C3BI::ErrorProtocol;

    // REQUEST PAYLOAD
    uint8_t count = 0;
    ok = stream.getUInt8(count);

    BString variable;
    BString value;

    // ANSWER
    output.putUInt8(count);

    for (size_t i = 0; i < count; ++i)
    {
        variable.clear();
        value.clear();

        ok = ok ? stream.getBStringWithSize(variable) : false;
        ok = ok ? stream.getBStringWithSize(value) : false;

        if (!ok)
            break;

        code = Cross3::instance()->setVariable(variable, value);

        if (code == C3BI::ErrorSuccess)
        {
            Cross3::instance()->getVariable(variable, value);
        }
        else
        {
            error = true;
        }

        // ANSWER PAYLOAD (CONTINUED)
        output.putUInt8(static_cast<uint8_t>(code));
        output.putBStringWithSize(value);
    }

    if (ok)
    {
        ok = !error;
        code = error ? C3BI::ErrorGeneral : C3BI::ErrorSuccess;
    }
    else
    {
        output.reset();
        output.putUInt8(0);
        code = C3BI::ErrorProtocol;
    }

    output.setResponseCode(code, ok);

    if (!output.build())
    {
        output.reset();
        output.putUInt8(0);
        output.setResponseCode(C3BI::ErrorLongAnswer, false);
    }

    reply(output);
}
