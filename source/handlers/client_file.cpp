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
#include "log.h"


void Client::handleFileDummy(MessageReader& stream, MessageBuilder& output)
{
}

void Client::handleFileSetAttribute(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    int32_t attribute = 0;
    int32_t mask = 0;

    BString name;

    ok = stream.getInt32(attribute);
    ok = ok ? stream.getInt32(mask) : false;
    ok = ok ? stream.getBStringWithSize(name) : false;

    if (ok)
        code = Cross3::instance()->fileSetAttribute(name, attribute, mask);

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileNameList(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    int32_t type = 0;
    int32_t flags = 0;

    BString path;

    ok = stream.getInt32(type);
    ok = ok ? stream.getInt32(flags) : false;
    ok = ok ? stream.getBStringWithSize(path) : false;

    Cross3::FileNameList list;

    if (ok)
        code = Cross3::instance()->fileNameList(path, type, flags, list);

    ok = (code == C3BI::ErrorSuccess);

    uint16_t countOfNames = 0;
    uint16_t countOfInfo = 0;

    if (ok)
    {
        if (list.names.size() > UINT16_MAX || list.info.size() > UINT16_MAX)
        {
            code = C3BI::ErrorLongAnswer;
            ok = false;
        }
        else
        {
            size_t total = list.sizeOfNames + list.sizeOfInfo +
                (list.names.size() + list.info.size()) * sizeof(uint16_t);

            if (total > SafePayloadSize)
            {
                code = C3BI::ErrorLongAnswer;
                ok = false;
            }
            else
            {
                countOfNames = static_cast<uint16_t>(list.names.size());
                countOfInfo = static_cast<uint16_t>(list.info.size());
            }
        }
    }

    // ANSWER
    output.putUInt16(countOfNames);
    for (size_t i = 0; i < countOfNames; ++i)
        output.putBStringWithSize(list.names[i]);

    output.putUInt16(countOfInfo);
    for (size_t i = 0; i < countOfInfo; ++i)
        output.putBStringWithSize(list.info[i]);

    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileCreate(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    uint16_t itemType = 0;
    uint8_t  modulePart = 0;

    BString name;
    BString templateName;

    bool createAlways = false;

    ok = stream.getUInt16(itemType);
    ok = ok ? stream.getUInt8(modulePart) : false;
    ok = ok ? stream.getBool(createAlways) : false;
    ok = ok ? stream.getBStringWithSize(name) : false;
    ok = ok ? stream.getBStringWithSize(templateName) : false;

    if (ok)
    {
        code = Cross3::instance()->fileCreate(name, itemType, modulePart,
            templateName, createAlways);
    }

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileDelete(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    BString name;

    bool deleteAlways = false;

    ok = stream.getBool(deleteAlways);
    ok = ok ? stream.getBStringWithSize(name) : false;

    if (ok)
        code = Cross3::instance()->fileDelete(name, deleteAlways);

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileCopy(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    int32_t flags = 0;

    BString source;
    BString destination;

    ok = stream.getInt32(flags);
    ok = ok ? stream.getBStringWithSize(source) : false;
    ok = ok ? stream.getBStringWithSize(destination) : false;

    if (ok)
        code = Cross3::instance()->fileCopy(source, destination, flags);

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileMove(MessageReader& stream, MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    int32_t flags = 0;

    BString source;
    BString destination;

    ok = stream.getInt32(flags);
    ok = ok ? stream.getBStringWithSize(source) : false;
    ok = ok ? stream.getBStringWithSize(destination) : false;

    if (ok)
        code = Cross3::instance()->fileMove(source, destination, flags);

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileGetProperties(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    int32_t mask = 0;

    BString name;

    ok = stream.getInt32(mask);
    ok = ok ? stream.getBStringWithSize(name) : false;

    Cross3::FileProperties properties;

    if (ok)
        code = Cross3::instance()->fileGetProperties(name, mask, properties);

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.putInt32(properties.itemType);
    output.putInt32(properties.lowSize);
    output.putInt32(properties.highSize);
    output.putInt32(properties.attributes);
    output.putInt32(properties.lowTimeCreated);
    output.putInt32(properties.highTimeCreated);
    output.putInt32(properties.biasCreated);
    output.putInt32(properties.lowTimeAccess);
    output.putInt32(properties.highTimeAccess);
    output.putInt32(properties.biasAccess);
    output.putInt32(properties.lowTimeModified);
    output.putInt32(properties.highTimeModified);
    output.putInt32(properties.biasModified);
    output.putInt32(properties.editMode);
    output.putBStringWithSize(properties.name);

    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileGetFullName(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    BString name;

    ok = stream.getBStringWithSize(name);

    BString value;

    if (ok)
        code = Cross3::instance()->fileGetFullName(name, value);

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.putBStringWithSize(value);
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileGetKrcName(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    BString name;

    ok = stream.getBStringWithSize(name);

    BString value;

    if (ok)
        code = Cross3::instance()->fileGetKrcName(name, value);

    ok = (code == C3BI::ErrorSuccess);

    // ANSWER
    output.putBStringWithSize(value);
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileWriteContent(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    uint8_t  command = C3BI::FileIoNone;

    ok = stream.getUInt8(command);

    if (ok)
    {
        uint32_t total = 0;
        uint32_t offset = 0;
        uint32_t size = 0;
        int32_t  flags = 0;
        BString  name;

        switch (command)
        {
        case C3BI::FileIoBegin:
            ok = ok ? stream.getUInt32(total) : false;
            if (ok)
            {
                _fileOut.clear();
                _fileOut.resize(static_cast<size_t>(total));
                code = (_fileOut.size() == total) ?
                    C3BI::ErrorSuccess : C3BI::ErrorMemory;

                Log::format(Log::Debug, _T("Client <%u> FileWriteContent: IoBegin, total <%u> bytes."),
                    _id, total);
            }
            // ANSWER
            output.putUInt8(command);
            output.putUInt32(static_cast<uint32_t>(_fileOut.size()));
            break;

        case C3BI::FileIoGetSize:
            code = C3BI::ErrorSuccess;

            Log::format(Log::Debug, _T("Client <%u> FileWriteContent: IoGetSize, total <%u> bytes."),
                _id, static_cast<uint32_t>(_fileOut.size()));

            // ANSWER
            output.putUInt8(command);
            output.putUInt32(static_cast<uint32_t>(_fileOut.size()));
            break;

        case C3BI::FileIoEnd:
            ok = ok ? stream.getInt32(flags) : false;
            ok = ok ? stream.getBStringWithSize(name) : false;
            if (ok)
            {
                code = Cross3::instance()->fileWriteContent(name, flags,
                    _fileOut);
                ok = (code == C3BI::ErrorSuccess);
                if (ok)
                    _fileOut.clear();

                Log::format(Log::Debug, _T("Client <%u> FileWriteContent: IoEnd, name <%s>, flags <%u>."),
                    _id, name.toWideString().c_str(), flags);
            }
            // ANSWER
            output.putUInt8(command);
            break;

        case C3BI::FileIoData:
            ok = ok ? stream.getUInt32(offset) : false;
            ok = ok ? stream.getUInt32(size) : false;

            if (ok)
            {
                total = static_cast<uint32_t>(_fileOut.size());

                code = C3BI::ErrorSuccess;

                if (size > 0 && total > 0 && offset < total)
                {
                    if (offset + size > total)
                        size = total - offset;

                    ok = stream.getArray(&_fileOut[offset], size);
                    if (!ok)
                        code = C3BI::ErrorAccess;
                }
                else
                {
                    size = 0;
                }

                Log::format(Log::Debug, _T("Client <%u> FileWriteContent: IoData, offset <%u>, size <%u>, total <%u>."),
                    _id, offset, size, total);

                // ANSWER
                output.putUInt8(command);
                output.putUInt32(offset);
                output.putUInt32(size);
            }
            break;

        default:
            code = C3BI::ErrorNotImplemented;
            ok = false;

            Log::format(Log::Debug, _T("Client <%u> FileWriteContent: Invalid operation <%u>."),
                _id, command);

            // ANSWER
            output.putUInt8(command);
            break;
        }
    }

    // ANSWER (finale)
    output.setResponseCode(code, ok);
    reply(output);
}

void Client::handleFileReadContent(MessageReader& stream,
    MessageBuilder& output)
{
    bool ok = true;
    uint16_t code = C3BI::ErrorProtocol;

    uint8_t  command = C3BI::FileIoNone;

    ok = stream.getUInt8(command);

    if (ok)
    {
        uint32_t total = 0;
        uint32_t offset = 0;
        uint32_t size = 0;
        int32_t  flags = 0;
        BString  name;

        switch (command)
        {
        case C3BI::FileIoBegin:
            ok = ok ? stream.getInt32(flags) : false;
            ok = ok ? stream.getBStringWithSize(name) : false;
            if (ok)
            {
                _fileIn.clear();
                code = Cross3::instance()->fileReadContent(name, flags,
                    _fileIn);
                ok = (code == C3BI::ErrorSuccess);
            }
            // ANSWER
            output.putUInt8(command);
            output.putUInt32(static_cast<uint32_t>(_fileIn.size()));
            break;

        case C3BI::FileIoGetSize:
            code = C3BI::ErrorSuccess;
            // ANSWER
            output.putUInt8(command);
            output.putUInt32(static_cast<uint32_t>(_fileIn.size()));
            break;

        case C3BI::FileIoEnd:
            _fileIn.clear();
            code = C3BI::ErrorSuccess;
            // ANSWER
            output.putUInt8(command);
            break;

        case C3BI::FileIoData:
            ok = ok ? stream.getUInt32(offset) : false;
            ok = ok ? stream.getUInt32(size) : false;

            if (ok)
            {
                total = static_cast<uint32_t>(_fileIn.size());

                code = C3BI::ErrorSuccess;

                if (size > 0 && total > 0 && offset < total)
                {
                    if (offset + size > total)
                        size = total - offset;

                    if (size > SafePayloadSize)
                        size = SafePayloadSize;

                }
                else
                {
                    size = 0;
                }

                // ANSWER
                output.putUInt8(command);
                output.putUInt32(offset);
                output.putUInt32(size);
                if (size > 0)
                    output.putArray(&_fileIn[offset], size);
            }
            break;

        default:
            code = C3BI::ErrorNotImplemented;
            ok = false;
            // ANSWER
            output.putUInt8(command);
            break;
        }
    }

    // ANSWER (finale)
    output.setResponseCode(code, ok);
    reply(output);
}
