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


#ifndef CLIENT_H
#define CLIENT_H


class MessageReader;
class MessageBuilder;


class Client : public Win32xx::CSocket
{
public:
    static void registerHandlers();

    Client(HWND window, UINT messageId, uint32_t id = 0);
    virtual ~Client();

    void checkTimeout(uint32_t elapsed);
    void setEvents(long events);
    void setIdleTimeout(uint32_t value);
    void setMessageTimeout(uint32_t value);

    virtual void OnDisconnect(uint16_t reason);
    virtual void OnReceive();
    virtual void OnSend();

private:
    // Variables Handling
    void handleReadVariableAscii(MessageReader& stream, MessageBuilder& output);
    void handleWriteVariableAscii(MessageReader& stream, MessageBuilder& output);
    void handleReadVariable(MessageReader& stream, MessageBuilder& output);
    void handleWriteVariable(MessageReader& stream, MessageBuilder& output);
    void handleReadMultiple(MessageReader& stream, MessageBuilder& output);
    void handleWriteMultiple(MessageReader& stream, MessageBuilder& output);

    // Program Handling
    void handleProgramState(MessageReader& stream, MessageBuilder& output);

    // Motion Handling
    void handleMotion(MessageReader& stream, MessageBuilder& output);

    // KCP Key Handling
    void handleKcpAction(MessageReader& stream, MessageBuilder& output);

    // Proxy Information Handling
    void handleProxyInfo(MessageReader& stream, MessageBuilder& output);
    void handleProxyFeatures(MessageReader& stream, MessageBuilder& output);
    void handleBenchmark(MessageReader& stream, MessageBuilder& output);

    // File Handling
    void handleFileSetAttribute(MessageReader& stream, MessageBuilder& output);
    void handleFileNameList(MessageReader& stream, MessageBuilder& output);
    void handleFileCreate(MessageReader& stream, MessageBuilder& output);
    void handleFileDelete(MessageReader& stream, MessageBuilder& output);
    void handleFileCopy(MessageReader& stream, MessageBuilder& output);
    void handleFileMove(MessageReader& stream, MessageBuilder& output);
    void handleFileGetProperties(MessageReader& stream,
        MessageBuilder& output);
    void handleFileGetFullName(MessageReader& stream, MessageBuilder& output);
    void handleFileGetKrcName(MessageReader& stream, MessageBuilder& output);
    void handleFileWriteContent(MessageReader& stream, MessageBuilder& output);
    void handleFileReadContent(MessageReader& stream, MessageBuilder& output);
    void handleFileDummy(MessageReader& stream, MessageBuilder& output);

    // CrossCommEXE Functions Handling
    void handleCrossConfirmAll(MessageReader& stream, MessageBuilder& output);

private:
    enum
    {
        HeaderSize        = 5,
        MinimumSize       = 1024,
        AbyssSize         = 256,
        TotalHandlers     = 256,
        SafePayloadSize   = 64512 // 63 KiB
    };

    enum Error
    {
        ErrorNone         = 0,
        ErrorTimeout      = 60000,
        ErrorHeader       = 60001,
        ErrorInputBuffer  = 60002,
        ErrorOutputBuffer = 60003,
        ErrorBuildMessage = 60004,
        ErrorUnknownInput = 60005
    };

    struct Input
    {
        uint16_t tag;
        uint16_t size;
        uint8_t  type;

        char*    buffer;
        char     header[HeaderSize];
        size_t   offset;

        bool     headerReceived;
    };

    struct Output
    {
        char*    buffer;
        size_t   offset;
        size_t   size;

        bool     completed;
    };

    typedef void (Client::*Handler)(MessageReader& stream,
        MessageBuilder& output);

private:
    size_t bytesAvailable() const;
    void performDisconnect(uint16_t reason);
    void dropPendingData();
    void reply(MessageBuilder& message);

    bool onHeader();
    bool onCollect();

private:
    static Handler _handlers[TotalHandlers];

    HWND _window;
    UINT _messageId;

    Input  _input;
    Output _output;

    std::vector<uint8_t> _fileIn;
    std::vector<uint8_t> _fileOut;

    uint32_t _id;
    uint32_t _timeout;
    uint32_t _timeoutIdle;
    uint32_t _timeoutMessage;
    uint32_t _elapsed;

    bool _ignoreEvents;

    friend class Proxy;
};


typedef Win32xx::Shared_Ptr<Client> PClient;


#endif // CLIENT_H
