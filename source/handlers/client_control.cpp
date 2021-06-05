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


void Client::handleProgramState(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    uint8_t command = C3BI::ProgramNone;
    int16_t submitIndex = 0;

    bool boolean = false;

    BString name;
    BString parameters;

    ok = stream.getUInt8(command);
    ok = ok ? stream.getInt16(submitIndex) : false;

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
        ok = ok ? stream.getBStringWithSize(name) : false;
        ok = ok ? stream.getBStringWithSize(parameters) : false;
        ok = ok ? stream.getBool(boolean) : false;

        if (ok)
        {
            code = Cross3::instance()->programSelect(name, parameters,
                boolean);
        }

        break;

    case C3BI::ProgramRun:
        ok = ok ? stream.getBStringWithSize(name) : false;
        ok = ok ? stream.getBStringWithSize(parameters) : false;
        ok = ok ? stream.getBool(boolean) : false;

        if (ok)
            code = Cross3::instance()->programRun(name, parameters, boolean);

        break;

    default:
        if (ok)
            code = C3BI::ErrorNotImplemented;

        break;
    }

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.setResponseCode(code, ok);
    output.putUInt8(command);
    reply(output);
}

void Client::handleMotion(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    uint8_t command = C3BI::MotionNone;

    BString position;

    ok = stream.getUInt8(command);
    ok = ok ? stream.getBStringWithSize(position) : false;

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

    // ANSWER
    output.setResponseCode(code, ok);
    output.putUInt8(command);
    reply(output);
}

void Client::handleKcpAction(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    uint8_t command = C3BI::KcpActionNone;

    int32_t interpreter = 0;
    int32_t axis = 0;
    int32_t key = 0;

    bool off = false;
    bool direction = false;

    ok = stream.getUInt8(command);
    ok = ok ? stream.getInt32(interpreter) : false;
    // Same value position for Interpreter or Axis inside the message
    axis = interpreter;

    ok = ok ? stream.getInt32(key) : false;
    ok = ok ? stream.getBool(direction) : false;
    ok = ok ? stream.getBool(off) : false;

    if (ok)
    {
        switch (command)
        {
        case C3BI::KcpActionStart:
            code = Cross3::instance()->kcpKeyStart(interpreter, direction,
                off);
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

    // ANSWER
    output.setResponseCode(code, ok);
    output.putUInt8(command);
    reply(output);
}
