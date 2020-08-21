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


#ifndef CLIENT_H
#define CLIENT_H


#include "ringbuffer.h"


class RingStream;


class Client : public Win32xx::CSocket
{
public:
    Client();
    virtual ~Client();

    size_t bytesAvailable() const;

    virtual void OnDisconnect();
    virtual void OnReceive();
    virtual void OnSend();

private:
    // Variables Handling
    void handleReadVariableAscii(WORD id, RingStream& stream);
    void handleWriteVariableAscii(WORD id, RingStream& stream);
    void handleReadVariable(WORD id, RingStream& stream);
    void handleWriteVariable(WORD id, RingStream& stream);
    void handleReadMultiple(WORD id, RingStream& stream);
    void handleWriteMultiple(WORD id, RingStream& stream);

    // Program Handling
    void handleProgramState(WORD id, RingStream& stream);

    // Motion Handling
    void handleMotion(WORD id, RingStream& stream);

    // KCP Key Handling
    void handleKcpAction(WORD id, RingStream& stream);

    // Proxy Information Handling
    void handleProxyInfo(WORD id, RingStream& stream);
    void handleProxyFeatures(WORD id, RingStream& stream);

    // File Handling
    void handleFileSetAttribute(WORD id, RingStream& stream);
    void handleFileNameList(WORD id, RingStream& stream);
    void handleFileCreate(WORD id, RingStream& stream);
    void handleFileDelete(WORD id, RingStream& stream);
    void handleFileCopy(WORD id, RingStream& stream);
    void handleFileMove(WORD id, RingStream& stream);
    void handleFileGetProperties(WORD id, RingStream& stream);
    void handleFileFileGetFullName(WORD id, RingStream& stream);
    void handleFileFileGetKrcName(WORD id, RingStream& stream);
    void handleFileFileWriteContent(WORD id, RingStream& stream);
    void handleFileFileReadContent(WORD id, RingStream& stream);

    // CrossCommEXE Functions Handling
    void handleCrossConfirmAll(WORD id, RingStream& stream);

private:
    enum
    {
        InputBufferSize = 66000,
        OutputBufferSize = 1024
    };

    RingBuffer _inputBuffer;
    std::vector<char> _outputBuffer;
};


typedef Win32xx::Shared_Ptr<Client> PClient;


#endif // CLIENT_H
