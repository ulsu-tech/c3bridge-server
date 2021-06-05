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

#include "log.h"
#include "resource.h"
#include "mainwindow.h"


Log::Severity Log::_severity = Log::None;


void Log::message(Severity severity, LPCTSTR text)
{
    if (_severity != None && severity != None && severity > _severity)
        return;

    Win32xx::CTime time = Win32xx::CTime::GetCurrentTime();

    Win32xx::CString severityLabel;

    switch (severity)
    {
    case Fatal:
        severityLabel.LoadString(IDS_LOG_SEVERITY_FATAL);
        break;
    case Error:
        severityLabel.LoadString(IDS_LOG_SEVERITY_ERROR);
        break;
    case Warning:
        severityLabel.LoadString(IDS_LOG_SEVERITY_WARNING);
        break;
    case Info:
        severityLabel.LoadString(IDS_LOG_SEVERITY_INFO);
        break;
    case Debug:
        severityLabel.LoadString(IDS_LOG_SEVERITY_DEBUG);
        break;
    case Verbose:
        severityLabel.LoadString(IDS_LOG_SEVERITY_VERBOSE);
        break;
    case None:
    default:
        break;
    }

    MainWindow::addLogLine(time.Format(IDS_LOG_FORMAT_DATETIME) +
        severityLabel + text);
}

void Log::message(Severity severity, UINT resource)
{
    if (_severity != None && severity != None && severity > _severity)
        return;

    Win32xx::CTime time = Win32xx::CTime::GetCurrentTime();

    Win32xx::CString severityLabel;

    switch (severity)
    {
    case Fatal:
        severityLabel.LoadString(IDS_LOG_SEVERITY_FATAL);
        break;
    case Error:
        severityLabel.LoadString(IDS_LOG_SEVERITY_ERROR);
        break;
    case Warning:
        severityLabel.LoadString(IDS_LOG_SEVERITY_WARNING);
        break;
    case Info:
        severityLabel.LoadString(IDS_LOG_SEVERITY_INFO);
        break;
    case Debug:
        severityLabel.LoadString(IDS_LOG_SEVERITY_DEBUG);
        break;
    case Verbose:
        severityLabel.LoadString(IDS_LOG_SEVERITY_VERBOSE);
        break;
    case None:
    default:
        break;
    }

    MainWindow::addLogLine(time.Format(IDS_LOG_FORMAT_DATETIME) + 
        severityLabel + Win32xx::LoadString(resource));
}

void Log::format(Severity severity, LPCTSTR text, ...)
{
    if (_severity != None && severity != None && severity > _severity)
        return;

    Win32xx::CString output;

    va_list args;
    va_start(args, text);
    output.FormatV(text, args);
    va_end(args);

    message(severity, output);
}

void Log::format(Severity severity, UINT resource, ...)
{
    if (_severity != None && severity != None && severity > _severity)
        return;

    Win32xx::CString specifiers;

    if (specifiers.LoadString(resource))
    {
        Win32xx::CString text;

        va_list args;
        va_start(args, resource);
        text.FormatV(specifiers.c_str(), args);
        va_end(args);

        message(severity, text);
    }
}
