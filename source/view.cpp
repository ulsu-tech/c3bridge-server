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

#include "view.h"


View::View()
    : _aveCharWidth(0)
    , _maxTextWidth(0)
{
}

View::~View()
{
}

int View::AddString(LPCTSTR pString)
{
    Win32xx::CSize size = GetDC().GetTextExtentPoint32(pString);

    if (size.cx > _maxTextWidth)
    {
        _maxTextWidth = size.cx;
        SetHorizontalExtent(_maxTextWidth + _aveCharWidth);
    }

    return CListBox::AddString(pString);
}

void View::ResetContent()
{
    _maxTextWidth = 0;
    SetHorizontalExtent(_maxTextWidth);
    CListBox::ResetContent();
}

void View::CopyToClipboard()
{
    DWORD style = GetStyle();

    if ((style & LBS_NOSEL) == LBS_NOSEL ||
        ((style & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) != 0 &&
        (style & LBS_HASSTRINGS) != LBS_HASSTRINGS))
    {
        return;
    }

    std::vector<int> selection;

    int count = 0;

    // Multiple-selection list box
    if (style & (LBS_EXTENDEDSEL | LBS_MULTIPLESEL))
    {
        count = GetSelCount();

        if (count < 1)
            return;

        selection.resize(static_cast<size_t>(count));

        count = GetSelItems(count, &selection[0]);

        if (count < 1)
            return;
    }
    // Single-selection list box
    else
    {
        selection.resize(1);

        selection[0] = GetCurSel();

        if (selection[0] < 0)
            return;

        count = 1;
    }

    std::basic_string<TCHAR> content;
    std::basic_string<TCHAR> line;

    for (int i = 0; i < count; ++i)
    {
        int length = GetTextLen(selection[i]);

        if (length < 1)
            continue;

        if (length + 1 > static_cast<int>(line.length()))
            line.resize(static_cast<size_t>(length + 1));

        GetText(selection[i], &line[0]);

        line[length] = _T('\0');

        content += &line[0];
        content += _T("\r\n");
    }

    if (content.length() < 1)
        return;

    if (!::OpenClipboard(GetHwnd()) || !::EmptyClipboard())
        return;

    HGLOBAL global = ::GlobalAlloc(GMEM_MOVEABLE, (content.length() + 1) * sizeof(TCHAR));

    if (global)
    {
        LPTSTR globalString = static_cast<LPTSTR>(::GlobalLock(global));

        if (globalString)
        {
            memcpy(globalString, &content[0], (content.length() + 1) * sizeof(TCHAR));

            ::GlobalUnlock(global);

#ifdef _UNICODE
            if (::SetClipboardData(CF_UNICODETEXT, global) == NULL)
#else
            if (::SetClipboardData(CF_TEXT, global) == NULL)
#endif
            {
                ::GlobalFree(global);
            }
        }
        else
        {
            ::GlobalFree(global);
        }
    }

    ::CloseClipboard();
}

int View::SelectedItemsCount()
{
    DWORD style = GetStyle();

    if ((style & LBS_NOSEL) == LBS_NOSEL ||
        ((style & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) != 0 &&
        (style & LBS_HASSTRINGS) != LBS_HASSTRINGS))
    {
        return 0;
    }

    // Multiple-selection list box
    if (style & (LBS_EXTENDEDSEL | LBS_MULTIPLESEL))
    {
        int count = GetSelCount();

        if (count < 0)
            count = 0;

        return count;
    }
    // Single-selection list box
    else
    {
        if (GetCurSel() < 0)
            return 0;

        return 1;
    }
}

void View::OnAttach()
{
    TEXTMETRIC metric;
    GetDC().GetTextMetrics(metric);
    _aveCharWidth = metric.tmAveCharWidth;

    SetHorizontalExtent(_maxTextWidth);
}

void View::PreCreate(CREATESTRUCT& cs)
{
    cs.dwExStyle = WS_EX_CLIENTEDGE;
    cs.style = WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
        LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | LBS_EXTENDEDSEL;
}
