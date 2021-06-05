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

#include "udpserver.h"

#include "usermessage.h"
#include "log.h"
#include "cross3.h"
#include "proxy.h"
#include "resource.h"


LPCSTR UdpServer::_discoveryWord = "WHEREAREYOU?";


UdpServer::UdpServer()
    : _window(NULL)
    , _legacy(false)
    , _legacyPeer(0)
{
    _messageId = UserMessage::reserve();
}

bool UdpServer::start(LPCTSTR address, UINT port)
{
    if (!Create(PF_INET, SOCK_DGRAM))
    {
        Log::error(_legacy ?
            IDS_ERROR_UDP_LEGACY_SERVER_CREATE :
            IDS_ERROR_UDP_SERVER_CREATE);
        return false;
    }

    if (Bind(address, port) != 0)
    {
        Disconnect();
        Log::format(Log::Error, _legacy ?
            IDS_ERROR_UDP_LEGACY_SERVER_BIND :
            IDS_ERROR_UDP_SERVER_BIND, port);
        return false;
    }

    if (StartAsync(_window, _messageId, FD_READ) != 0)
    {
        Disconnect();
        Log::error(_legacy ?
            IDS_ERROR_UDP_LEGACY_SERVER_ASYNC :
            IDS_ERROR_UDP_SERVER_ASYNC);
        return false;
    }

    Log::format(Log::None, _legacy ?
        IDS_NOTICE_UDP_LEGACY_SERVER_STARTED :
        IDS_NOTICE_UDP_SERVER_STARTED, port);
    return true;
}

void UdpServer::stop()
{
    Disconnect();
}

bool UdpServer::windowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg != _messageId || static_cast<SOCKET>(wParam) != GetSocket())
        return false;

    if (WSAGETSELECTERROR(lParam))
    {
        stop();
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_READ)
    {
        char buffer[33];
        memset(buffer, 0, sizeof(buffer));

        sockaddr_in client;
        memset(&client, 0, sizeof(client));

        int clientSize = static_cast<int>(sizeof(client));

        ReceiveFrom(buffer, sizeof(buffer) - 1, 0,
            reinterpret_cast<sockaddr*>(&client), &clientSize);

        std::string output;

        if (strcmp(buffer, _discoveryWord) == 0)
        {
            if (_robotSerialNo.empty())
            {
                Cross3::instance()->getVariable("$KR_SERIALNO",
                    _robotSerialNo);
                Cross3::instance()->getVariable("$MODEL_NAME[]",
                    _robotModelName);
            }

            output = "KUKA|" + _robotModelName + "|" + _robotSerialNo;
        }
        else if (buffer[0] == '@')
        {
            output = Win32xx::TtoA(Proxy::getVariable(buffer).c_str());
        }

        if (output.size() > 0)
        {
            if (_legacy)
                client.sin_port = htons(_legacyPeer);

            clientSize = static_cast<int>(sizeof(client));

            SendTo(output.c_str(), static_cast<int>(output.size()), 0,
                reinterpret_cast<const sockaddr*>(&client), clientSize);
        }
    }

    return true;
}
