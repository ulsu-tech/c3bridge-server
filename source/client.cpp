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

#include "client.h"

#include "log.h"
#include "cross3.h"
#include "bstring.h"
#include "memoryallocator.h"
#include "messagebuilder.h"
#include "messagereader.h"
#include "proxy.h"
#include "resource.h"


//
// Static Members
//
Client::Handler Client::_handlers[Client::TotalHandlers] = {};


//
// Static Methods
//
void Client::registerHandlers()
{
    memset(_handlers, 0, sizeof(_handlers));

    // Variables Handling
    _handlers[C3BI::CommandReadVariableAscii] = &handleReadVariableAscii;
    _handlers[C3BI::CommandWriteVariableAscii] = &handleWriteVariableAscii;
    _handlers[C3BI::CommandReadVariable] = &handleReadVariable;
    _handlers[C3BI::CommandWriteVariable] = &handleWriteVariable;
    _handlers[C3BI::CommandReadMultiple] = &handleReadMultiple;
    _handlers[C3BI::CommandWriteMultiple] = &handleWriteMultiple;

    // Program Handling
    _handlers[C3BI::CommandProgramControl] = &handleProgramState;

    // Motion Handling
    _handlers[C3BI::CommandMotion] = &handleMotion;

    // KCP Key Handling
    _handlers[C3BI::CommandKcpAction] = &handleKcpAction;

    // Proxy Information Handling
    _handlers[C3BI::CommandProxyInfo] = &handleProxyInfo;
    _handlers[C3BI::CommandProxyFeatures] = &handleProxyFeatures;
    _handlers[C3BI::CommandProxyBenchmark] = &handleBenchmark;

    // File Handling
    _handlers[C3BI::CommandFileSetAttribute] = &handleFileSetAttribute;
    _handlers[C3BI::CommandFileNameList] = &handleFileNameList;
    _handlers[C3BI::CommandFileCreate] = &handleFileCreate;
    _handlers[C3BI::CommandFileDelete] = &handleFileDelete;
    _handlers[C3BI::CommandFileCopy] = &handleFileCopy;
    _handlers[C3BI::CommandFileMove] = &handleFileMove;
    _handlers[C3BI::CommandFileGetProperties] = &handleFileGetProperties;
    _handlers[C3BI::CommandFileGetFullName] = &handleFileGetFullName;
    _handlers[C3BI::CommandFileGetKrcName] = &handleFileGetKrcName;
    _handlers[C3BI::CommandFileWriteContent] = &handleFileWriteContent;
    _handlers[C3BI::CommandFileReadContent] = &handleFileReadContent;

    // CrossCommEXE Functions Handling
    _handlers[C3BI::CommandCrossConfirmAll] = &handleCrossConfirmAll;
}


//
// Regular Methods
//
Client::Client(HWND window, UINT messageId, uint32_t id)
    : _window(window)
    , _messageId(messageId)
    , _id(id)
    , _timeout(0)
    , _timeoutIdle(0)
    , _timeoutMessage(0)
    , _elapsed(0)
    , _ignoreEvents(false)
{
    memset(&_input, 0, sizeof(_input));
    memset(&_output, 0, sizeof(_output));

    _output.completed = true;
}

Client::~Client()
{
    MemoryAllocator::deallocate(reinterpret_cast<void**>(&_input.buffer));
    MemoryAllocator::deallocate(reinterpret_cast<void**>(&_output.buffer));    
}

void Client::checkTimeout(uint32_t elapsed)
{
    if (_timeout > 0 && elapsed - _elapsed >= _timeout)
        performDisconnect(ErrorTimeout);
}

void Client::setEvents(long events)
{
#ifdef _MSC_VER
#pragma warning ( push )
#pragma warning ( disable : 4996 )
    ::WSAAsyncSelect(*this, _window, _messageId, events);
#pragma warning ( pop )
#endif // _MSC_VER
}

void Client::setIdleTimeout(uint32_t value)
{
    _timeout = value;
    _timeoutIdle = value;
    _elapsed = ::GetTickCount();
}

void Client::setMessageTimeout(uint32_t value)
{
    _timeoutMessage = value;
    _elapsed = ::GetTickCount();
}

void Client::OnDisconnect(uint16_t reason)
{
    _fileIn.clear();
    _fileOut.clear();

    switch (reason)
    {
        case WSAENETDOWN:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_NET_DOWN, _id);
            break;

        case WSAECONNABORTED:
        case WSAECONNRESET:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_RESET, _id);
            break;

        case ErrorNone:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED, _id);
            break;

        case ErrorTimeout:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_TIMEOUT, _id);
            break;

        case ErrorHeader:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_HEADER, _id);
            break;

        case ErrorInputBuffer:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_INPUT_BUFFER, _id);
            break;

        case ErrorOutputBuffer:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_OUTPUT_BUFFER, _id);
            break;

        case ErrorBuildMessage:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_BUILD, _id);
            break;

        case ErrorUnknownInput:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_UNKNOWN, _id);
            break;

        default:
            Log::format(Log::Info, IDS_CLIENT_DISCONNECTED_DEFAULT, _id,
                reason);
            break;
    }
}

void Client::OnReceive()
{
    if (_ignoreEvents)
        return;

    if (bytesAvailable() == 0)
    {
        // Re-enable FD_READ signal
        char byte;
        Receive(&byte, sizeof(byte), MSG_PEEK);
        return;
    }

    // The reply to the previous message is still being sent
    // Reject all incoming data
    if (!_output.completed)
    {
        dropPendingData();
        return;
    }

    if (!onHeader())
        return;

    if (!onCollect())
        return;

    MessageReader reader(_input.buffer, _input.size);

    MemoryAllocator::reallocate(reinterpret_cast<void**>(&_output.buffer),
        MessageBuilder::MaximumMessage);

    if (!_output.buffer)
    {
        performDisconnect(ErrorOutputBuffer);
        return;
    }

    _output.offset = 0;
    _output.size = 0;
    _output.completed = true;

    MessageBuilder output(_output.buffer, MessageBuilder::MaximumMessage);
    output.setTag(_input.tag);
    output.setType(_input.type);

    Log::format(Log::Verbose, IDS_CLIENT_INPUT_MESSAGE, _id, _input.tag,
        _input.type, _input.size);

    if (_handlers[_input.type])
        (this->*(_handlers[_input.type]))(reader, output);

    MemoryAllocator::deallocate(reinterpret_cast<void**>(&_input.buffer));

    if (_output.completed)
        MemoryAllocator::deallocate(reinterpret_cast<void**>(&_output.buffer));

    _timeout = _timeoutIdle;
    _elapsed = ::GetTickCount();
}

void Client::OnSend()
{
    if (_ignoreEvents)
        return;

    if (_output.completed)
    {
        MemoryAllocator::deallocate(reinterpret_cast<void**>(&_output.buffer));
        return;
    }

    size_t rest = _output.size - _output.offset;

    while (rest > 0)
    {
        int result = Send(&_output.buffer[_output.offset],
            static_cast<int>(rest), 0);

        if (result <= 0)
        {
            uint16_t reason = static_cast<uint16_t>(::WSAGetLastError());
            if (reason != WSAEWOULDBLOCK)
                performDisconnect(reason);

            return;
        }

        _output.offset += static_cast<size_t>(result);
        rest -= static_cast<size_t>(result);
    }

    MemoryAllocator::deallocate(reinterpret_cast<void**>(&_output.buffer));
    _output.offset = 0;
    _output.size = 0;
    _output.completed = true;
}

size_t Client::bytesAvailable() const
{
    u_long result = 0;

    if (ioCtlSocket(FIONREAD, &result) != 0)
        return 0;

    return static_cast<size_t>(result);
}

void Client::performDisconnect(uint16_t reason)
{
    _ignoreEvents = true;

    setEvents(0);
    ::PostMessage(_window, _messageId, static_cast<WPARAM>(GetSocket()),
        MAKELONG(FD_CLOSE, reason));
}

void Client::dropPendingData()
{
    setEvents(FD_WRITE | FD_CLOSE);

    char abyss[AbyssSize];

    int received = 1;

    while (received > 0)
    {
        received = Receive(abyss, sizeof(abyss), 0);
    }

    setEvents(FD_READ | FD_WRITE | FD_CLOSE);

    // Re-enable FD_READ signal
    Receive(abyss, sizeof(abyss), 0);
}

void Client::reply(MessageBuilder& message)
{
    if (message.build())
    {
        _output.offset = 0;
        _output.size = message.size();
        _output.completed = false;

        Log::format(Log::Verbose, IDS_CLIENT_OUTPUT_MESSAGE, _id, message.tag(),
            message.type(), message.size() - MessageBuilder::HeaderSize -
            MessageBuilder::TypeSize, message.errorCode(),
            message.successFlag() ? _T("SUCCESS") : _T("FAILURE"));

        OnSend();
    }
    else
    {
        performDisconnect(ErrorBuildMessage);
    }
}

bool Client::onHeader()
{
    if (_input.headerReceived)
        return true;

    size_t rest = 0;

    if (_input.offset < HeaderSize)
        rest = HeaderSize - _input.offset;

    int received = Receive(&_input.header[_input.offset],
        static_cast<int>(rest), 0);

    if (received <= 0)
    {
        uint16_t reason = static_cast<uint16_t>(::WSAGetLastError());
        if (reason != WSAEWOULDBLOCK)
            performDisconnect(reason);

        return false;
    }

    _input.offset += static_cast<size_t>(received);

    if (_input.offset < HeaderSize)
        return false;

    MessageReader reader(_input.header, HeaderSize);
    reader.getUInt16(_input.tag);
    reader.getUInt16(_input.size);
    reader.getUInt8(_input.type);

    if (_input.size == 0)
    {
        performDisconnect(ErrorHeader);
        return false;
    }

    // Remove size of (_input.type)
    _input.size -= sizeof(_input.type);

    if (!_handlers[_input.type])
    {
        Log::format(Log::Debug, IDS_CLIENT_INPUT_UNKNOWN, _id, _input.tag,
            _input.type, _input.size);
        performDisconnect(ErrorUnknownInput);
        return false;
    }

    if (_input.size > MinimumSize)
    {
        MemoryAllocator::reallocate(
            reinterpret_cast<void**>(&_input.buffer), _input.size);
    }
    else if (_input.size > 0)
    {
        MemoryAllocator::reallocate(
            reinterpret_cast<void**>(&_input.buffer), MinimumSize);
    }

    _input.offset = 0;
    _input.headerReceived = true;

    _timeout = _timeoutMessage;

    if (_timeout == 0)
        _timeout = _timeoutIdle;

    _elapsed = ::GetTickCount();

    return true;
}

bool Client::onCollect()
{
    if (_input.size > 0)
    {
        if (!_input.buffer)
        {
            performDisconnect(ErrorInputBuffer);
            return false;
        }

        int received = Receive(&_input.buffer[_input.offset], _input.size - _input.offset, 0);

        if (received <= 0)
        {
            uint16_t reason = static_cast<uint16_t>(::WSAGetLastError());
            if (reason != WSAEWOULDBLOCK)
                performDisconnect(reason);

            return false;
        }

        _elapsed = ::GetTickCount();

        _input.offset += static_cast<size_t>(received);
    }

    if (_input.offset < _input.size)
        return false;

    _input.offset = 0;
    _input.headerReceived = false;

    _timeout = 0;

    return true;
}
