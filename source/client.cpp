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
#include "utilities.h"
#include "bstring.h"


Client::Client()
    : _inputBuffer(InputBufferSize)
    , _outputBuffer(OutputBufferSize)
{
}

Client::~Client()
{
}

size_t Client::bytesAvailable() const
{
    u_long result = 0;

    if (ioCtlSocket(FIONREAD, &result) != 0)
        return 0;

    return static_cast<size_t>(result);
}

void Client::OnDisconnect()
{
}

void Client::OnReceive()
{
    size_t available = bytesAvailable();

    if (available == 0 || available > _inputBuffer.bytesFree())
        return;

    char intermediate[256];

    int received = Receive(intermediate, sizeof(intermediate), 0);
    if (received <= 0)
    {
        // TODO: Handle this!
        return;
    }

    _inputBuffer.put(intermediate, static_cast<size_t>(received));

    RingStream stream(_inputBuffer, RingStream::BigEndian);

    while (stream.available(2 * sizeof(WORD)))
    {
        WORD id = 0;
        WORD length = 0;

        stream.get(id);
        stream.get(length);

        if (!stream.available(static_cast<size_t>(length)))
            break;

        if (length)
        {
            BYTE type = 0;
            stream.get(type);

            switch (type)
            {
            // Variables Handling
            case C3BI::CommandReadVariableAscii:
                handleReadVariableAscii(id, stream);
                break;
            case C3BI::CommandWriteVariableAscii:
                handleWriteVariableAscii(id, stream);
                break;
            case C3BI::CommandReadArrayAscii:
            case C3BI::CommandWriteArrayAscii:
                break;
            case C3BI::CommandReadVariable:
                handleReadVariable(id, stream);
                break;
            case C3BI::CommandWriteVariable:
                handleWriteVariable(id, stream);
                break;
            case C3BI::CommandReadMultiple:
                handleReadMultiple(id, stream);
                break;
            case C3BI::CommandWriteMultiple:
                handleWriteMultiple(id, stream);
                break;

            // Program Handling
            case C3BI::CommandProgramControl:
                handleProgramState(id, stream);
                break;

            // Motion Handling
            case C3BI::CommandMotion:
                handleMotion(id, stream);
                break;

            // KCP Key Handling
            case C3BI::CommandKcpAction:
                handleKcpAction(id, stream);
                break;

            // Proxy Information Handling
            case C3BI::CommandProxyInfo:
                handleProxyInfo(id, stream);
                break;

            case C3BI::CommandProxyFeatures:
                handleProxyFeatures(id, stream);
                break;

            /*
             * These capabilities have not been tested and can be dangerous.
            // File Handling
            case C3BI::CommandFileSetAttribute:
                handleFileSetAttribute(id, stream);
                break;

            case C3BI::CommandFileNameList:
                handleFileNameList(id, stream);
                break;

            case C3BI::CommandFileCreate:
                handleFileCreate(id, stream);
                break;

            case C3BI::CommandFileDelete:
                handleFileDelete(id, stream);
                break;

            case C3BI::CommandFileCopy:
                handleFileCopy(id, stream);
                break;

            case C3BI::CommandFileMove:
                handleFileMove(id, stream);
                break;

            case C3BI::CommandFileGetProperties:
                handleFileGetProperties(id, stream);
                break;

            case C3BI::CommandFileGetFullName:
                handleFileFileGetFullName(id, stream);
                break;

            case C3BI::CommandFileGetKrcName:
                handleFileFileGetKrcName(id, stream);
                break;

            case C3BI::CommandFileWriteContent:
                handleFileFileWriteContent(id, stream);
                break;

            case C3BI::CommandFileReadContent:
                handleFileFileReadContent(id, stream);
                break;
             */

            // CrossCommEXE Functions Handling
            case C3BI::CommandCrossConfirmAll:
                handleCrossConfirmAll(id, stream);
                break;

            default:
                break;
            }
        }

        _inputBuffer.skip(static_cast<size_t>(length) + 2 * sizeof(WORD));

        stream.reset();
    }
}

void Client::OnSend()
{
}

void Client::handleProgramState(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    BYTE command = C3BI::ProgramNone;
    SHORT submitIndex = 0;

    WORD size = 0;
    bool boolean = false;

    BString name;
    BString parameters;

    ok = stream.get(command);
    ok = ok ? stream.get(submitIndex) : false;

    switch (command)
    {
    case C3BI::ProgramReset:
        if (ok)
            code = Cross3::instance()->programReset(submitIndex);

        break;
    case C3BI::ProgramStart:
        if (ok)
            code = Cross3::instance()->programStart(submitIndex);

        break;
    case C3BI::ProgramStop:
        if (ok)
            code = Cross3::instance()->programStop(submitIndex);

        break;
    case C3BI::ProgramCancel:
        if (ok)
            code = Cross3::instance()->programCancel(submitIndex);

        break;
    case C3BI::ProgramSelect:
        ok = ok ? stream.get(size) : false;
        ok = (ok && size > 0) ? stream.get(name, size) : false;
        ok = ok ? stream.get(size) : false;
        ok = (ok && size > 0) ? stream.get(parameters, size) : false;
        ok = ok ? stream.get(boolean) : false;

        if (ok)
            code = Cross3::instance()->programSelect(name, parameters, boolean);

        break;

    case C3BI::ProgramRun:
        ok = ok ? stream.get(size) : false;
        ok = (ok && size > 0) ? stream.get(name, size) : false;
        ok = ok ? stream.get(size) : false;
        ok = (ok && size > 0) ? stream.get(parameters, size) : false;
        ok = ok ? stream.get(boolean) : false;

        if (ok)
            code = Cross3::instance()->programRun(name, parameters, boolean);

        break;

    default:
        if (ok)
            code = C3BI::ErrorNotImplemented;

        break;
    }

    ok = (code == C3BI::ErrorSuccess);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandProgramControl));

    // Payload data
    output.append(command);

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

void Client::handleMotion(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    BYTE command = C3BI::MotionNone;
    WORD size = 0;

    BString position;

    ok = stream.get(command);
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(position, size) : false;

    if (ok)
    {
        switch (command)
        {
        case C3BI::MotionPtp:
            code = Cross3::instance()->motionPtp(position, false);
            break;
        case C3BI::MotionPtpRelative:
            code = Cross3::instance()->motionPtp(position, true);
            break;
        case C3BI::MotionLin:
            code = Cross3::instance()->motionLin(position, false);
            break;
        case C3BI::MotionLinRelative:
            code = Cross3::instance()->motionLin(position, true);
            break;
        default:
            code = C3BI::ErrorNotImplemented;
            break;
        }
    }

    ok = (code == C3BI::ErrorSuccess);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandMotion));

    // Payload data
    output.append(command);

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

void Client::handleKcpAction(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    BYTE command = C3BI::KcpActionNone;

    INT32 interpreter = 0;
    INT32 axis = 0;
    INT32 key = 0;

    bool off = false;
    bool direction = false;

    ok = stream.get(command);
    ok = ok ? stream.get(interpreter) : false;
    axis = interpreter; // Same value position for Interpreter or Axis inside the message

    ok = ok ? stream.get(key) : false;
    ok = ok ? stream.get(direction) : false;
    ok = ok ? stream.get(off) : false;

    if (ok)
    {
        switch (command)
        {
        case C3BI::KcpActionStart:
            code = Cross3::instance()->kcpKeyStart(interpreter, direction, off);
            break;
        case C3BI::KcpActionStop:
            code = Cross3::instance()->kcpKeyStop(interpreter, off);
            break;
        case C3BI::KcpActionMove:
            code = Cross3::instance()->kcpKeyMove(axis, key, direction, off);
            break;
        case C3BI::KcpActionMove6D:
            code = Cross3::instance()->kcpKeyMove6D(off);
            break;
        default:
            code = C3BI::ErrorNotImplemented;
            break;
        }
    }

    ok = (code == C3BI::ErrorSuccess);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandKcpAction));

    // Payload data
    output.append(command);

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

void Client::handleProxyInfo(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorSuccess;

    SYSTEMTIME time;
    ::GetSystemTime(&time);

    BString computerName = Utilities::getComputerName().c_str();

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandProxyInfo));

    // Payload data
    output.append(static_cast<BYTE>(Proxy::VersionMajor));
    output.append(static_cast<BYTE>(Proxy::VersionMinor));
    output.append(static_cast<BYTE>(Proxy::VersionType));
    output.append(time.wYear);
    output.append(time.wMonth);
    output.append(time.wDayOfWeek);
    output.append(time.wDay);
    output.append(time.wHour);
    output.append(time.wMinute);
    output.append(time.wSecond);
    output.append(time.wMilliseconds);
    // Computer name
    output.append(static_cast<WORD>(computerName.length()));
    output.append(computerName);

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

void Client::handleProxyFeatures(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorSuccess;

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandProxyFeatures));

    // Payload data
    for (BYTE index = 32; index > 0; --index)
        output.append(Proxy::getFeatureOctet(index - 1));

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
