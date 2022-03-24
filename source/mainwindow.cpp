/*
 * Copyright (c) 2020-2022 Dmitry Lavygin <vdm.inbox@gmail.com>
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
#include "log.h"
#include "usermessage.h"


MainWindow* MainWindow::_globalPointer = NULL;


void MainWindow::addLogLine(LPCTSTR text)
{
    if (_globalPointer)
        _globalPointer->_view.AddString(text);
}

Settings* MainWindow::settings()
{
    if (_globalPointer)
        return &_globalPointer->_settings;

    return NULL;
}

MainWindow::MainWindow()
{
    _globalPointer = this;

    memset(&_trayIcon, 0, sizeof(_trayIcon));

    _trayIcon.cbSize = sizeof(_trayIcon);
    _trayIcon.uID = IDW_MAIN;
    _trayIcon.uFlags = NIF_ICON | NIF_MESSAGE;
    _trayIcon.uCallbackMessage = UserMessage::reserve();

    SetView(_view);
}

MainWindow::~MainWindow()
{
    if (_globalPointer == this)
        _globalPointer = NULL;
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

    Log::Severity severity = Log::severity();

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

    case IDS_LOG_SEVERITY_FATAL:
        severity = Log::Fatal;
        break;

    case IDS_LOG_SEVERITY_ERROR:
        severity = Log::Error;
        break;

    case IDS_LOG_SEVERITY_WARNING:
        severity = Log::Warning;
        break;

    case IDS_LOG_SEVERITY_INFO:
        severity = Log::Info;
        break;

    case IDS_LOG_SEVERITY_DEBUG:
        severity = Log::Debug;
        break;

    case IDS_LOG_SEVERITY_VERBOSE:
        severity = Log::Verbose;
        break;

    default:
        break;
    }

    if (severity != Log::severity())
    {
        Log::setSeverity(severity);
        updateLogLevelMenu();
        return TRUE;
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

    Log::Severity severity = static_cast<Log::Severity>(_settings.logSeverity);

    std::vector<Win32xx::CString> arguments = Win32xx::GetCommandLineArgs();

    for (size_t i = 0; i < arguments.size(); ++i)
    {
        if (arguments[i] == _T("-debug"))
        {
            ShowWindow(IsIconic() ? SW_RESTORE : SW_SHOW);

            if (severity != Log::None && severity < Log::Debug)
                severity = Log::Debug;
        }
        else if (arguments[i] == _T("-verbose"))
        {
            if (severity != Log::None && severity < Log::Verbose)
                severity = Log::Verbose;
        }
    }

    // Log Settings
    Log::setSeverity(severity);
    updateLogLevelMenu();
    _view.setLineLimit(_settings.logLimit);

    _server.setWindow(GetHwnd());
    _server.setIdleTimeout(_settings.networkIdleTimeout);
    _server.setMessageTimeout(_settings.networkMessageTimeout);

    _serverUdp.setWindow(GetHwnd());
    _serverUdpLegacy.setWindow(GetHwnd());

    if (_settings.networkTcpEnabled)
        _server.start(_settings.networkTcpAddress, _settings.networkTcpPort);

    if (_settings.networkUdpEnabled)
    {
        _serverUdp.start(_settings.networkUdpAddress,
            _settings.networkUdpPort);
    }

    if (_settings.networkUdpLegacyEnabled)
    {
        _serverUdpLegacy.setLegacy(true, _settings.networkUdpLegacyPeer);
        _serverUdpLegacy.start(_settings.networkUdpAddress,
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

LRESULT MainWindow::OnInitMenuPopup(UINT msg, WPARAM wparam, LPARAM lparam)
{
    return CWnd::WndProcDefault(WM_INITMENUPOPUP, wparam, lparam);
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
    else if (uMsg == _trayIcon.uCallbackMessage)
    {
        onTrayIcon(wParam, lParam);
        return 0;
    }
    else if (_server.windowProc(uMsg, wParam, lParam))
    {
        return 0;
    }
    else if (_serverUdp.windowProc(uMsg, wParam, lParam))
    {
        return 0;
    }
    else if (_serverUdpLegacy.windowProc(uMsg, wParam, lParam))
    {
        return 0;
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

void MainWindow::updateLogLevelMenu()
{
    UINT item = IDS_LOG_SEVERITY_VERBOSE;

    switch (Log::severity())
    {
    case Log::Fatal:
        item = IDS_LOG_SEVERITY_FATAL;
        break;
    case Log::Error:
        item = IDS_LOG_SEVERITY_ERROR;
        break;
    case Log::Warning:
        item = IDS_LOG_SEVERITY_WARNING;
        break;
    case Log::Info:
        item = IDS_LOG_SEVERITY_INFO;
        break;
    case Log::Debug:
        item = IDS_LOG_SEVERITY_DEBUG;
        break;
    case Log::Verbose:
    default:
        item = IDS_LOG_SEVERITY_VERBOSE;
        break;
    }

    Win32xx::CMenu menu = GetMenu();
    menu.CheckMenuRadioItem(IDS_LOG_SEVERITY_FATAL, IDS_LOG_SEVERITY_VERBOSE, item, MF_BYCOMMAND);
}
