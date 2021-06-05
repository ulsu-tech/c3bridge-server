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

#include "tcpserver.h"

#include "usermessage.h"
#include "log.h"
#include "cross3.h"
#include "proxy.h"
#include "resource.h"


void CALLBACK TcpServer::staticTimerCallback(HWND window, UINT message,
    UINT_PTR pointer, DWORD elapsed)
{
    (void) window;
    (void) message;

    if (pointer)
        reinterpret_cast<TcpServer*>(pointer)->onTimer(elapsed);
}


TcpServer::TcpServer()
    : _window(NULL)
    , _lastId(1)
    , _timeoutIdle(0)
    , _timeoutMessage(0)
{
    _messageServer = UserMessage::reserve();
    _messageSession = UserMessage::reserve();
}

bool TcpServer::start(LPCTSTR address, UINT port)
{
    if (!Create(PF_INET, SOCK_STREAM))
    {
        Log::error(IDS_ERROR_TCP_SERVER_CREATE);
        return false;
    }

    if (Bind(address, port) != 0)
    {
        Disconnect();
        Log::format(Log::Error, IDS_ERROR_TCP_SERVER_BIND, port);
        return false;
    }

    if (Listen() != 0)
    {
        Disconnect();
        Log::error(IDS_ERROR_TCP_SERVER_LISTEN);
        return false;
    }

    if (StartAsync(_window, _messageServer, FD_ACCEPT) != 0)
    {
        Disconnect();
        Log::error(IDS_ERROR_TCP_SERVER_ASYNC);
        return false;
    }

    Log::format(Log::None, IDS_NOTICE_TCP_SERVER_STARTED,
        Proxy::getComputerName().c_str(), port);

    SetTimer(_window, reinterpret_cast<UINT_PTR>(this), TimerInterval,
        &TcpServer::staticTimerCallback);

    _lastId = 1;

    return true;
}

void TcpServer::stop()
{
    _clients.clear();
    Disconnect();
}

bool TcpServer::windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == _messageSession)
        return sessionProc(wParam, lParam);

    if (uMsg != _messageServer || static_cast<SOCKET>(wParam) != GetSocket())
        return false;

    if (WSAGETSELECTERROR(lParam) || WSAGETSELECTEVENT(lParam) == FD_CLOSE)
    {
        stop();
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
    {
        PClient client = new Client(_window, _messageSession, _lastId);

        sockaddr_in address;
        memset(&address, 0, sizeof(address));
        int size = sizeof(address);

        Accept(*client, reinterpret_cast<sockaddr*>(&address), &size);

        if (client->GetSocket() != INVALID_SOCKET)
        {
            if (size == sizeof(address))
            {
                Log::format(Log::Info, IDS_CLIENT_CONNECTED_ADDRESS, _lastId,
                    address.sin_addr.S_un.S_un_b.s_b1,
                    address.sin_addr.S_un.S_un_b.s_b2,
                    address.sin_addr.S_un.S_un_b.s_b3,
                    address.sin_addr.S_un.S_un_b.s_b4);
            }
            else
            {
                Log::format(Log::Info, IDS_CLIENT_CONNECTED, _lastId);
            }

            _clients.push_back(client);

            client->setEvents(FD_READ | FD_WRITE | FD_CLOSE);
            client->setMessageTimeout(_timeoutMessage);
            client->setIdleTimeout(_timeoutIdle);

            _lastId++;
        }
    }

    return true;
}

bool TcpServer::sessionProc(WPARAM wParam, LPARAM lParam)
{
    SOCKET socket = static_cast<SOCKET>(wParam);

    std::list<PClient>::iterator iterator;

    for (iterator = _clients.begin(); iterator != _clients.end(); ++iterator)
    {
        if ((*iterator).get() && (*iterator)->GetSocket() == socket)
        {
            break;
        }
    }

    if (iterator == _clients.end())
    {
        ::closesocket(socket);
    }
    else if (WSAGETSELECTERROR(lParam) ||
        WSAGETSELECTEVENT(lParam) == FD_CLOSE)
    {
        (*iterator)->OnDisconnect(WSAGETSELECTERROR(lParam));
        (*iterator)->Disconnect();

        _clients.erase(iterator);
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_READ)
    {
        (*iterator)->OnReceive();
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_WRITE)
    {
        (*iterator)->OnSend();
    }

    return true;
}

void TcpServer::onTimer(uint32_t elapsed)
{
    std::list<PClient>::const_iterator iterator;

    for (iterator = _clients.begin(); iterator != _clients.end(); ++iterator)
    {
        if ((*iterator).get())
            (*iterator)->checkTimeout(elapsed);
    }
}
