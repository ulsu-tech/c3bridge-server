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

#include "aboutdialog.h"
#include "resource.h"
#include "proxy.h"


AboutDialog::AboutDialog()
    : Win32xx::CDialog(IDW_ABOUT)
{

}

BOOL AboutDialog::OnInitDialog()
{
    Win32xx::CString type;

    if (Proxy::VersionType == Proxy::VersionOpenSource)
    {
        type.LoadString(IDS_ABOUT_INFO_OPEN_SOURCE);
    }
    else if (Proxy::VersionType == Proxy::VersionProprietary)
    {
        type.LoadString(IDS_ABOUT_INFO_PROPRIETARY);
    }

    Win32xx::CString text;
    text.Format(IDS_ABOUT_INFO, Proxy::VersionMajor, Proxy::VersionMinor, type.c_str());
    SetDlgItemText(IDC_INFO, text.c_str());

    text.LoadString(IDW_MAIN);
    SetDlgItemText(IDC_TITLE, text.c_str());

    text.LoadString(IDS_ABOUT_DISCLAIMER);
    SetDlgItemText(IDC_DISCLAIMER, text.c_str());

    return TRUE;
}
