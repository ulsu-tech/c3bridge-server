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

#include <c3bi.h>

#include "client.h"
#include "ringstream.h"
#include "bufferstream.h"
#include "cross3.h"
#include "proxy.h"
#include "bstring.h"


void Client::handleReadVariableAscii(WORD id, RingStream& stream)
{
    bool ok = false;
    WORD code = C3BI::ErrorGeneral;

    WORD size = 0;
    ok = stream.get(size);

    std::string variable;
    ok = (ok && size > 0) ? stream.get(variable, size) : false;

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

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandReadVariableAscii));

    // Payload data
    output.append(static_cast<WORD>(value.size()));
    output.append(value);

    // Error code
    output.append(code);
    output.append(ok);

    payloadSize = output.size() - payloadSize;

    if (payloadSize <= 0xFFFF)
    {
        // Fill header
        output.set(0, id);
        output.set(sizeof(WORD), static_cast<WORD>(payloadSize));

        Send(output.data(), static_cast<int>(output.size()), 0);
    }
}

void Client::handleWriteVariableAscii(WORD id, RingStream& stream)
{
    bool ok = false;
    WORD code = C3BI::ErrorGeneral;

    WORD size = 0;
    ok = stream.get(size);

    std::string variable;
    ok = (ok && size > 0) ? stream.get(variable, size) : false;

    if (ok)
        ok = stream.get(size);

    std::string value;
    ok = (ok && size > 0) ? stream.get(value, size) : false;

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

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandWriteVariableAscii));

    // Payload data
    output.append(static_cast<WORD>(value.size()));
    output.append(value);

    // Error code
    output.append(code);
    output.append(ok);

    payloadSize = output.size() - payloadSize;

    if (payloadSize <= 0xFFFF)
    {
        // Fill header
        output.set(0, id);
        output.set(sizeof(WORD), static_cast<WORD>(payloadSize));

        Send(output.data(), static_cast<int>(output.size()), 0);
    }
}

void Client::handleReadVariable(WORD id, RingStream& stream)
{
    bool ok = false;
    WORD code = C3BI::ErrorProtocol;

    WORD size = 0;
    ok = stream.get(size);

    BString variable;
    ok = (ok && size > 0) ? stream.get(variable, size) : false;

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

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandReadVariable));

    // Payload data
    output.append(static_cast<WORD>(value.length()));
    output.append(value);

    // Error code
    output.append(code);
    output.append(ok);

    payloadSize = output.size() - payloadSize;

    if (payloadSize <= 0xFFFF)
    {
        // Fill header
        output.set(0, id);
        output.set(sizeof(WORD), static_cast<WORD>(payloadSize));

        Send(output.data(), static_cast<int>(output.size()), 0);
    }
}

void Client::handleWriteVariable(WORD id, RingStream& stream)
{
    bool ok = false;
    WORD code = C3BI::ErrorProtocol;

    WORD size = 0;
    ok = stream.get(size);

    BString variable;
    ok = (ok && size > 0) ? stream.get(variable, size) : false;

    if (ok)
        ok = stream.get(size);

    BString value;
    ok = (ok && size > 0) ? stream.get(value, size) : false;

    if (ok)
    {
        code = Cross3::instance()->setVariable(variable, value);

        if (code != C3BI::ErrorSuccess)
        {
            value.clear();

            ok = false;
        }
    }

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandWriteVariable));

    // Payload data
    output.append(static_cast<WORD>(value.length()));
    output.append(value);

    // Error code
    output.append(code);
    output.append(ok);

    payloadSize = output.size() - payloadSize;

    if (payloadSize <= 0xFFFF)
    {
        // Fill header
        output.set(0, id);
        output.set(sizeof(WORD), static_cast<WORD>(payloadSize));

        Send(output.data(), static_cast<int>(output.size()), 0);
    }
}

void Client::handleReadMultiple(WORD id, RingStream& stream)
{
    bool ok = false;
    bool error = false;
    WORD code = C3BI::ErrorProtocol;

    BYTE count = 0;
    ok = stream.get(count);

    WORD size = 0;
    BString variable;
    BString value;

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandReadMultiple));

    // Payload count
    output.append(count);

    for (size_t i = 0; i < count; ++i)
    {
        variable.clear();
        value.clear();

        ok = ok ? stream.get(size) : false;

        ok = (ok && size > 0) ? stream.get(variable, size) : false;

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
                error = true;
        }

        // Payload data
        output.append(static_cast<BYTE>(code));
        output.append(static_cast<WORD>(value.length()));
        output.append(value);
    }

    if (!ok)
    {
        output.reset();

        // Empty header
        output.skip(2 * sizeof(WORD));
        size_t payloadSize = output.size();

        // Payload type
        output.append(static_cast<BYTE>(C3BI::CommandReadMultiple));

        // Payload count
        output.append(static_cast<BYTE>(0));

        code = C3BI::ErrorProtocol;
    }
    else
    {
        ok = !error;
        code = error ? C3BI::ErrorGeneral : C3BI::ErrorSuccess;
    }

    // Error code
    output.append(code);
    output.append(ok);

    payloadSize = output.size() - payloadSize;

    if (payloadSize > 0xFFFF)
    {
        output.reset();

        // Empty header
        output.skip(2 * sizeof(WORD));
        size_t payloadSize = output.size();

        // Payload type
        output.append(static_cast<BYTE>(C3BI::CommandReadMultiple));

        // Payload count
        output.append(static_cast<BYTE>(0));

        code = C3BI::ErrorLongAnswer;
        ok = false;

        // Error code
        output.append(code);
        output.append(ok);

        payloadSize = output.size() - payloadSize;
    }

    // Fill header
    output.set(0, id);
    output.set(sizeof(WORD), static_cast<WORD>(payloadSize));

    Send(output.data(), static_cast<int>(output.size()), 0);
}

void Client::handleWriteMultiple(WORD id, RingStream& stream)
{
    bool ok = false;
    bool error = false;
    WORD code = C3BI::ErrorProtocol;

    BYTE count = 0;
    ok = stream.get(count);

    WORD size = 0;
    BString variable;
    BString value;

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandWriteMultiple));

    // Payload count
    output.append(count);

    for (size_t i = 0; i < count; ++i)
    {
        variable.clear();
        value.clear();

        ok = ok ? stream.get(size) : false;

        ok = (ok && size > 0) ? stream.get(variable, size) : false;

        ok = ok ? stream.get(size) : false;

        ok = (ok && size > 0) ? stream.get(value, size) : false;

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

        // Payload data
        output.append(static_cast<BYTE>(code));
        output.append(static_cast<WORD>(value.length()));
        output.append(value);
    }

    if (!ok)
    {
        output.reset();

        // Empty header
        output.skip(2 * sizeof(WORD));
        size_t payloadSize = output.size();

        // Payload type
        output.append(static_cast<BYTE>(C3BI::CommandWriteMultiple));

        // Payload count
        output.append(static_cast<BYTE>(0));

        code = C3BI::ErrorProtocol;
    }
    else
    {
        ok = !error;
        code = error ? C3BI::ErrorGeneral : C3BI::ErrorSuccess;
    }

    // Error code
    output.append(code);
    output.append(ok);

    payloadSize = output.size() - payloadSize;

    if (payloadSize > 0xFFFF)
    {
        output.reset();

        // Empty header
        output.skip(2 * sizeof(WORD));
        size_t payloadSize = output.size();

        // Payload type
        output.append(static_cast<BYTE>(C3BI::CommandWriteMultiple));

        // Payload count
        output.append(static_cast<BYTE>(0));

        code = C3BI::ErrorLongAnswer;
        ok = false;

        // Error code
        output.append(code);
        output.append(ok);

        payloadSize = output.size() - payloadSize;
    }

    // Fill header
    output.set(0, id);
    output.set(sizeof(WORD), static_cast<WORD>(payloadSize));

    Send(output.data(), static_cast<int>(output.size()), 0);
}
