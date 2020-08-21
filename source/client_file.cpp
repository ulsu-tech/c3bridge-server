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
#include "bstring.h"


void Client::handleFileSetAttribute(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    INT32 attribute = 0;
    INT32 mask = 0;

    WORD size = 0;
    BString name;

    ok = stream.get(attribute);
    ok = ok ? stream.get(mask) : false;
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(name, size) : false;

    if (ok)
        code = Cross3::instance()->fileSetAttribute(name, attribute, mask);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileSetAttribute));

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

void Client::handleFileNameList(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    INT32 type = 0;
    INT32 flags = 0;

    WORD size = 0;
    BString path;

    ok = stream.get(type);
    ok = ok ? stream.get(flags) : false;
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(path, size) : false;

    if (ok)
        code = Cross3::instance()->fileNameList(path, type, flags);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileNameList));

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

void Client::handleFileCreate(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    WORD itemType = 0;
    BYTE  modulePart = 0;

    WORD size = 0;

    BString name;
    BString templateName;

    bool createAlways = false;

    ok = stream.get(itemType);
    ok = ok ? stream.get(modulePart) : false;
    ok = ok ? stream.get(createAlways) : false;
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(name, size) : false;
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(templateName, size) : false;

    if (ok)
        code = Cross3::instance()->fileCreate(name, itemType, modulePart, templateName, createAlways);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileCreate));

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

void Client::handleFileDelete(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    WORD size = 0;
    BString name;

    bool deleteAlways = false;

    ok = stream.get(deleteAlways);
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(name, size) : false;

    if (ok)
        code = Cross3::instance()->fileDelete(name, deleteAlways);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileDelete));

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

void Client::handleFileCopy(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    INT32 flags = 0;

    WORD size = 0;
    BString source;
    BString destination;

    ok = stream.get(flags);
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(source, size) : false;
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(destination, size) : false;

    if (ok)
        code = Cross3::instance()->fileCopy(source, destination, flags);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileCopy));

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

void Client::handleFileMove(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    INT32 flags = 0;

    WORD size = 0;
    BString source;
    BString destination;

    ok = stream.get(flags);
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(source, size) : false;
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(destination, size) : false;

    if (ok)
        code = Cross3::instance()->fileMove(source, destination, flags);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileMove));

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

void Client::handleFileGetProperties(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    INT32 mask = 0;

    WORD size = 0;
    BString name;

    ok = stream.get(mask);
    ok = ok ? stream.get(size) : false;
    ok = (ok && size > 0) ? stream.get(name, size) : false;

    Cross3::FileProperties properties;

    if (ok)
        code = Cross3::instance()->fileGetProperties(name, mask, properties);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileGetProperties));

    output.append(properties.itemType);
    output.append(properties.lowSize);
    output.append(properties.highSize);
    output.append(properties.attributes);
    output.append(properties.lowTimeCreated);
    output.append(properties.highTimeCreated);
    output.append(properties.biasCreated);
    output.append(properties.lowTimeAccess);
    output.append(properties.highTimeAccess);
    output.append(properties.biasAccess);
    output.append(properties.lowTimeModified);
    output.append(properties.highTimeModified);
    output.append(properties.biasModified);
    output.append(properties.editMode);

    output.append(static_cast<WORD>(properties.name.length()));
    output.append(properties.name);

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

void Client::handleFileFileGetFullName(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    WORD size = 0;
    BString name;

    ok = stream.get(size);
    ok = (ok && size > 0) ? stream.get(name, size) : false;

    BString value;

    if (ok)
        code = Cross3::instance()->fileGetFullName(name, value);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileGetFullName));

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

void Client::handleFileFileGetKrcName(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = C3BI::ErrorProtocol;

    WORD size = 0;
    BString name;

    ok = stream.get(size);
    ok = (ok && size > 0) ? stream.get(name, size) : false;

    BString value;

    if (ok)
        code = Cross3::instance()->fileGetKrcName(name, value);

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandFileGetKrcName));

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

void Client::handleFileFileWriteContent(WORD id, RingStream& stream)
{

}

void Client::handleFileFileReadContent(WORD id, RingStream& stream)
{

}
