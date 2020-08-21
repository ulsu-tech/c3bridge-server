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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "view.h"
#include "client.h"
#include "settings.h"
#include "aboutdialog.h"


class MainWindow : public Win32xx::CFrame
{
public:
    MainWindow();
    virtual ~MainWindow();

    void log(LPCTSTR message);
    void log(UINT messageId);
    void log(const Win32xx::CString& message);

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    virtual void OnClose();
    virtual void OnDestroy();
    virtual void OnFileExit();
    virtual void OnInitialUpdate();
    virtual BOOL OnHelp();
    virtual void PreCreate(CREATESTRUCT& cs);
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void onTrayIcon(WPARAM wParam, LPARAM lParam);
    void onTcpServerEvent(WPARAM wParam, LPARAM lParam);
    void onTcpSessionEvent(WPARAM wParam, LPARAM lParam);
    void onUdpServerEvent(WPARAM wParam, LPARAM lParam);

private:
    bool startTcpServer(LPCTSTR address, UINT port);
    void stopTcpServer();

    bool startUdpLegacyServer(LPCTSTR address, UINT port);
    void stopUdpLegacyServer();

    bool startUdpServer(LPCTSTR address, UINT port);
    void stopUdpServer();

private:
    static LPCSTR _discoveryWord;

    View _view;
    Settings _settings;
    AboutDialog _dialogAbout;

    Win32xx::CSocket _server;
    Win32xx::CSocket _serverUdpLegacy;
    Win32xx::CSocket _serverUdp;

    std::list<PClient> _clients;

    std::string _robotModelName;
    std::string _robotSerialNo;

    NOTIFYICONDATA _trayIcon;
};


#endif // MAINWINDOW_H
