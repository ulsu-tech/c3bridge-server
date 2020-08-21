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

#include "mainwindow.h"
#include "resource.h"
#include "cross3.h"
#include "proxy.h"
#include "utilities.h"


#define WM_TRAY_ICON   (WM_USER + 1)
#define WM_TCP_SERVER  (WM_USER + 2)
#define WM_TCP_SESSION (WM_USER + 3)
#define WM_UDP_SERVER  (WM_USER + 4)


LPCSTR MainWindow::_discoveryWord = "WHEREAREYOU?";


MainWindow::MainWindow()
{
    memset(&_trayIcon, 0, sizeof(_trayIcon));

    _trayIcon.cbSize = sizeof(_trayIcon);
    _trayIcon.uID = IDW_MAIN;
    _trayIcon.uFlags = NIF_ICON | NIF_MESSAGE;
    _trayIcon.uCallbackMessage = WM_TRAY_ICON;

    SetView(_view);
}

MainWindow::~MainWindow()
{
}

void MainWindow::log(LPCTSTR message)
{
    Win32xx::CTime time = Win32xx::CTime::GetCurrentTime();
    _view.AddString(time.Format(IDS_FORMAT_DATETIME) + message);
}

void MainWindow::log(UINT messageId)
{
    Win32xx::CTime time = Win32xx::CTime::GetCurrentTime();
    _view.AddString(time.Format(IDS_FORMAT_DATETIME) + 
        Win32xx::LoadString(messageId));
}

void MainWindow::log(const Win32xx::CString& message)
{
    Win32xx::CTime time = Win32xx::CTime::GetCurrentTime();
    _view.AddString(time.Format(IDS_FORMAT_DATETIME) + message);
}

BOOL MainWindow::OnCommand(WPARAM wParam, LPARAM lParam)
{
    if (reinterpret_cast<HWND>(lParam) == GetView().GetHwnd())
    {
        switch (HIWORD(wParam))
        {
        case LBN_SELCANCEL:
        case LBN_SELCHANGE:
        case LBN_DBLCLK:
            GetMenu().EnableMenuItem(IDM_EDIT_COPY,
                _view.SelectedItemsCount() > 0 ? MF_ENABLED : MF_GRAYED);

            return TRUE;

        default:
            break;
        }

        return FALSE;
    }


    switch (LOWORD(wParam))
    {
    case IDM_FILE_EXIT:
        OnFileExit();
        return TRUE;

    case IDM_EDIT_COPY:
        _view.CopyToClipboard();
        return TRUE;

    case IDM_EDIT_CLEAR:
        _view.ResetContent();
        GetMenu().EnableMenuItem(IDM_EDIT_COPY, 
            _view.SelectedItemsCount() > 0 ? MF_ENABLED : MF_GRAYED);

        return TRUE;

    case IDM_HELP_ABOUT:
        OnHelp();
        return TRUE;

    default:
        break;
    }

    return FALSE;
}

void MainWindow::OnClose()
{
    ShowWindow(SW_HIDE);
}

void MainWindow::OnDestroy()
{
    _settings.save();

    Shell_NotifyIcon(NIM_DELETE, &_trayIcon);

    GetMenuBar().Destroy();
    GetToolBar().Destroy();
    GetReBar().Destroy();
    GetStatusBar().Destroy();
    GetView().Destroy();

    ::PostQuitMessage(0);   // Terminates the application
}

void MainWindow::OnFileExit()
{
    WINDOWPLACEMENT placement;
    placement.length = sizeof(placement);

    if (GetWindowPlacement(placement) != FALSE)
    {
        _settings.windowPositionX = placement.rcNormalPosition.left;
        _settings.windowPositionY = placement.rcNormalPosition.top;
        _settings.windowWidth = placement.rcNormalPosition.right - 
            placement.rcNormalPosition.left;
        _settings.windowHeight = placement.rcNormalPosition.bottom -
            placement.rcNormalPosition.top;

        _settings.windowIsMaximized = (placement.showCmd == SW_MAXIMIZE);
    }

    ShowWindow(SW_HIDE);
    Destroy();
}

void MainWindow::OnInitialUpdate()
{
    _trayIcon.hWnd = GetHwnd();
    _trayIcon.hIcon = Win32xx::GetApp()->LoadIcon(IDW_MAIN);

    Shell_NotifyIcon(NIM_ADD, &_trayIcon);

    if (_settings.networkTcpEnabled)
        startTcpServer(_settings.networkTcpAddress, _settings.networkTcpPort);

    if (_settings.networkUdpEnabled)
        startUdpServer(_settings.networkUdpAddress, _settings.networkUdpPort);

    if (_settings.networkUdpLegacyEnabled)
    {
        startUdpLegacyServer(_settings.networkUdpAddress,
            _settings.networkUdpLegacyPort);
    }
}

BOOL MainWindow::OnHelp()
{
    if (!_dialogAbout.IsWindow())
    {
        HWND focus = ::GetFocus();

        if (focus == GetMenuBar().GetHwnd())
            focus = GetHwnd();

        _dialogAbout.DoModal(*this);

        ::SetFocus(focus);
    }

    return TRUE;
}

void MainWindow::PreCreate(CREATESTRUCT& cs)
{
    UseIndicatorStatus(FALSE);
    UseMenuStatus(FALSE);
    UseReBar(FALSE);
    UseThemes(FALSE);
    UseToolBar(FALSE);

    _settings.load();

    cs.style = WS_OVERLAPPEDWINDOW;

    cs.x = _settings.windowPositionX;
    cs.y = _settings.windowPositionY;
    cs.cx = _settings.windowWidth;
    cs.cy = _settings.windowHeight;

    if (_settings.windowIsMaximized)
        cs.style |= WS_MAXIMIZE;
}

LRESULT MainWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_GETMINMAXINFO)
    {
        LPMINMAXINFO info = reinterpret_cast<LPMINMAXINFO>(lParam);
        info->ptMinTrackSize.x = _settings.windowMinimumWidth;
        info->ptMinTrackSize.y = _settings.windowMinimumHeight;
        return 0;
    }

    switch (uMsg)
    {
    case WM_TRAY_ICON:
        onTrayIcon(wParam, lParam);
        return 0;

    case WM_TCP_SERVER:
        onTcpServerEvent(wParam, lParam);
        return 0;

    case WM_TCP_SESSION:
        onTcpSessionEvent(wParam, lParam);
        return 0;

    case WM_UDP_SERVER:
        onUdpServerEvent(wParam, lParam);
        return 0;

    default:
        break;
    }

    return WndProcDefault(uMsg, wParam, lParam);
}

void MainWindow::onTrayIcon(WPARAM wParam, LPARAM lParam)
{
    if (wParam != IDW_MAIN)
        return;

    if (lParam == WM_LBUTTONDBLCLK)
    {
        ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);
    }
    else if (lParam == WM_RBUTTONUP)
    {
        Win32xx::CMenu topMenu(IDM_TRAY);
        Win32xx::CMenu menu = topMenu.GetSubMenu(0);

        menu.SetDefaultItem(IDM_TRAY_OPEN);

        SetForegroundWindow();

        Win32xx::CPoint position = Win32xx::GetCursorPos();

        UINT item = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_NONOTIFY,
            position.x, position.y, GetHwnd(), NULL);

        switch (item)
        {
        case IDM_TRAY_OPEN:
            ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);
            break;

        case IDM_TRAY_EXIT:
            OnFileExit();
            break;

        default:
            break;
        }
    }
}

void MainWindow::onTcpServerEvent(WPARAM wParam, LPARAM lParam)
{
    if (WSAGETSELECTERROR(lParam) || WSAGETSELECTEVENT(lParam) == FD_CLOSE)
    {
        stopTcpServer();
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
    {
        PClient client = new Client();

        _server.Accept(*client, NULL, NULL);

        if (client->GetSocket() == INVALID_SOCKET)
        {
            using namespace Win32xx;

            TRACE(_T("Invalid accept socket"));
        }
        else
        {
            _clients.push_back(client);

            client->StartAsync(GetHwnd(), WM_TCP_SESSION, FD_READ | FD_WRITE | FD_CLOSE);
        }
    }
}

void MainWindow::onTcpSessionEvent(WPARAM wParam, LPARAM lParam)
{
    std::list<PClient>::iterator iterator;

    for (iterator = _clients.begin(); iterator != _clients.end(); ++iterator)
    {
        if ((*iterator).get() && (*iterator)->GetSocket() == static_cast<SOCKET>(wParam))
            break;
    }

    if (iterator == _clients.end())
    {
        ::closesocket(static_cast<SOCKET>(wParam));
    }
    else if (WSAGETSELECTERROR(lParam) || WSAGETSELECTEVENT(lParam) == FD_CLOSE)
    {
        (*iterator)->OnDisconnect();
        (*iterator)->Disconnect();

        _clients.erase(iterator);

        log(_T("Client session error or FD_CLOSE, TODO: closeSession()"));
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_READ)
    {
        (*iterator)->OnReceive();
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_WRITE)
    {
        (*iterator)->OnSend();
    }
}

void MainWindow::onUdpServerEvent(WPARAM wParam, LPARAM lParam)
{
    Win32xx::CSocket* server = NULL;

    if (_serverUdp.GetSocket() == static_cast<SOCKET>(wParam))
    {
        server = &_serverUdp;
    }
    else if (_serverUdpLegacy.GetSocket() == static_cast<SOCKET>(wParam))
    {
        server = &_serverUdpLegacy;
    }

    if (!server)
        return;

    if (WSAGETSELECTERROR(lParam) || WSAGETSELECTEVENT(lParam) == FD_CLOSE)
    {
        server->Disconnect();
    }
    else if (WSAGETSELECTEVENT(lParam) == FD_READ)
    {
        char buffer[33];
        memset(buffer, 0, sizeof(buffer));

        sockaddr_in client;
        memset(&client, 0, sizeof(client));

        int clientSize = static_cast<int>(sizeof(client));

        server->ReceiveFrom(buffer, sizeof(buffer) - 1, 0, 
            reinterpret_cast<sockaddr*>(&client), &clientSize);

        std::string output;

        if (strcmp(buffer, _discoveryWord) == 0)
        {
            if (_robotSerialNo.empty())
            {
                Cross3::instance()->getVariable("$KR_SERIALNO", _robotSerialNo);
                Cross3::instance()->getVariable("$MODEL_NAME[]", _robotModelName);
            }

            output = "KUKA|" + _robotModelName + "|" + _robotSerialNo;
        }
        else if (buffer[0] == '@')
        {
            output = Win32xx::TtoA(Proxy::getVariable(buffer).c_str());
        }

        if (output.size() > 0)
        {
            if (server == &_serverUdpLegacy)
                client.sin_port = htons(_settings.networkUdpLegacyPeer);

            clientSize = static_cast<int>(sizeof(client));

            server->SendTo(output.c_str(), static_cast<int>(output.size()), 0,
                reinterpret_cast<const sockaddr*>(&client), clientSize);
        }
    }
}

bool MainWindow::startTcpServer(LPCTSTR address, UINT port)
{
    if (!_server.Create(PF_INET, SOCK_STREAM))
    {
        log(IDS_ERROR_TCP_SERVER_CREATE);
        return false;
    }

    if (_server.Bind(address, port) != 0)
    {
        _server.Disconnect();

        Win32xx::CString message;
        message.Format(IDS_ERROR_TCP_SERVER_BIND, port);

        log(message);
        return false;
    }

    if (_server.Listen() != 0)
    {
        _server.Disconnect();
        log(IDS_ERROR_TCP_SERVER_LISTEN);
        return false;
    }

    if (_server.StartAsync(GetHwnd(), WM_TCP_SERVER, FD_ACCEPT | FD_CLOSE) != 0)
    {
        _server.Disconnect();
        log(IDS_ERROR_TCP_SERVER_ASYNC);
        return false;
    }

    Win32xx::CString message;
    message.Format(IDS_NOTICE_TCP_SERVER_STARTED, Utilities::getComputerName().c_str(), port);

    log(message);
    return true;
}

void MainWindow::stopTcpServer()
{
    _clients.clear();
    _server.Disconnect();
}

bool MainWindow::startUdpLegacyServer(LPCTSTR address, UINT port)
{
    if (!_serverUdpLegacy.Create(PF_INET, SOCK_DGRAM))
    {
        log(IDS_ERROR_UDP_LEGACY_SERVER_CREATE);
        return false;
    }

    if (_serverUdpLegacy.Bind(address, port) != 0)
    {
        _serverUdpLegacy.Disconnect();

        Win32xx::CString message;
        message.Format(IDS_ERROR_UDP_LEGACY_SERVER_BIND, port);

        log(message);
        return false;
    }

    if (_serverUdpLegacy.StartAsync(GetHwnd(), WM_UDP_SERVER, FD_READ | FD_CLOSE) != 0)
    {
        _serverUdpLegacy.Disconnect();
        log(IDS_ERROR_UDP_LEGACY_SERVER_ASYNC);
        return false;
    }

    Win32xx::CString message;
    message.Format(IDS_NOTICE_UDP_LEGACY_SERVER_STARTED, port);

    log(message);
    return true;
}

void MainWindow::stopUdpLegacyServer()
{
    _serverUdpLegacy.Disconnect();
}

bool MainWindow::startUdpServer(LPCTSTR address, UINT port)
{
    if (!_serverUdp.Create(PF_INET, SOCK_DGRAM))
    {
        log(IDS_ERROR_UDP_SERVER_CREATE);
        return false;
    }

    if (_serverUdp.Bind(address, port) != 0)
    {
        _serverUdp.Disconnect();

        Win32xx::CString message;
        message.Format(IDS_ERROR_UDP_SERVER_BIND, port);

        log(message);
        return false;
    }

    if (_serverUdp.StartAsync(GetHwnd(), WM_UDP_SERVER, FD_READ | FD_CLOSE) != 0)
    {
        _serverUdp.Disconnect();
        log(IDS_ERROR_UDP_SERVER_ASYNC);
        return false;
    }


    Win32xx::CString message;
    message.Format(IDS_NOTICE_UDP_SERVER_STARTED, port);

    log(message);
    return true;
}

void MainWindow::stopUdpServer()
{
    _serverUdp.Disconnect();
}
