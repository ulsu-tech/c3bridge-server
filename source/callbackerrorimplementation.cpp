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

#include "callbackerrorimplementation.h"


CallbackErrorImplementation::CallbackErrorImplementation()
    : _counter(1)
{
}

HRESULT __stdcall CallbackErrorImplementation::QueryInterface(REFIID riid, void** ppvObject)
{
    if (ppvObject)
        *ppvObject = NULL;

    if (riid == __uuidof(IKCallbackError))
    {
        if (ppvObject)
        {
            *ppvObject = this;
            _counter++;
        }

        return S_OK;
    }
    else if (riid == __uuidof(IUnknown))
    {
        if (ppvObject)
        {
            *ppvObject = this;
            _counter++;
        }

        return S_OK;
    }

    return E_FAIL;
}

ULONG __stdcall CallbackErrorImplementation::AddRef(void)
{
    _counter++;
    return _counter;
}

ULONG __stdcall CallbackErrorImplementation::Release(void)
{
    if (_counter > 0)
        _counter--;

    return _counter;
}

HRESULT __stdcall CallbackErrorImplementation::OnError(long nID, TKMessage* pErrorData)
{
    return S_OK;
}

HRESULT __stdcall CallbackErrorImplementation::OnDialog(long nID, TKDialog* pDialogData)
{
    return S_OK;
}
