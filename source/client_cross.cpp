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


void Client::handleCrossConfirmAll(WORD id, RingStream& stream)
{
    bool ok = true;
    WORD code = Cross3::instance()->crossConfirmAll();

    //
    // ANSWER
    //
    BufferStream output(_outputBuffer, BufferStream::BigEndian);

    // Empty header
    output.skip(2 * sizeof(WORD));
    size_t payloadSize = output.size();

    // Payload type
    output.append(static_cast<BYTE>(C3BI::CommandCrossConfirmAll));

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