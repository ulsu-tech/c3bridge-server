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

#include <c3bi.h>

#include "cross3.h"
#include "callbackerrorimplementation.h"


class Cross3Destroyer
{
public:
    ~Cross3Destroyer()
    {
        if (Cross3::_instance)
        {
            delete Cross3::_instance;
            Cross3::_instance = NULL;
        }
    }
};


static Cross3Destroyer s_destroyer;


Cross3*   Cross3::_instance = NULL;
LPCOLESTR Cross3::_clientId = OLESTR("C3 Bridge Interface");


Cross3* Cross3::instance()
{
    if (!_instance)
        _instance = new Cross3();

    return _instance;
}

unsigned short Cross3::getErrorCode(HRESULT result)
{
    if (SUCCEEDED(result))
        return C3BI::ErrorSuccess;

    switch (result)
    {
    case E_FAIL:
        return C3BI::ErrorGeneral;
    case E_ACCESSDENIED:
        return C3BI::ErrorAccess;
    case E_INVALIDARG:
        return C3BI::ErrorArgument;
    case E_OUTOFMEMORY:
        return C3BI::ErrorMemory;
    case E_POINTER:
        return C3BI::ErrorPointer;
    case E_UNEXPECTED:
        return C3BI::ErrorUnexpected;
    case E_NOTIMPL:
        return C3BI::ErrorNotImplemented;
    case E_NOINTERFACE:
        return C3BI::ErrorNoInterface;
    default:
        return C3BI::ErrorGeneral;
    }
}

unsigned short Cross3::getVariable(const std::string& name, std::string& value)
{
    if (!_syncVar)
        return C3BI::ErrorNoInterface;

    BString sName(name);
    BString sValue;

    HRESULT code = _syncVar->ShowVar(sName.bstr(), sValue.address());

    if (FAILED(code))
        return getErrorCode(code);

    value = sValue.toString();

    return C3BI::ErrorSuccess;
}

unsigned short Cross3::getVariable(const BString& name, BString& value)
{
    if (!_syncVar)
        return C3BI::ErrorNoInterface;

    HRESULT code = _syncVar->ShowVar(name.bstr(), value.address());

    if (FAILED(code))
        return getErrorCode(code);

    return C3BI::ErrorSuccess;
}

unsigned short Cross3::setVariable(const std::string& name, std::string& value)
{
    if (!_syncVar)
        return C3BI::ErrorNoInterface;

    BString sName(name);
    BString sValue(value);

    HRESULT code = _syncVar->SetVar(sName.bstr(), sValue.bstr());
    
    if (FAILED(code))
        return getErrorCode(code);

    return C3BI::ErrorSuccess;
}

unsigned short Cross3::setVariable(const BString& name, BString& value)
{
    if (!_syncVar)
        return C3BI::ErrorNoInterface;

    HRESULT code = _syncVar->SetVar(name.bstr(), value.bstr());

    if (FAILED(code))
        return getErrorCode(code);

    return C3BI::ErrorSuccess;
}

unsigned short Cross3::programReset(short interpreter)
{
    if (!_syncSelect)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncSelect->Reset(interpreter == 0 ? eInterpreterSubmit : eInterpreterRobot));
}

unsigned short Cross3::programStart(short interpreter)
{
    if (!_syncSelect)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncSelect->Start(interpreter == 0 ? eInterpreterSubmit : eInterpreterRobot));
}

unsigned short Cross3::programStop(short interpreter)
{
    if (!_syncSelect)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncSelect->Stop(interpreter == 0 ? eInterpreterSubmit : eInterpreterRobot));
}

unsigned short Cross3::programCancel(short interpreter)
{
    if (!_syncSelect)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncSelect->Cancel(interpreter == 0 ? eInterpreterSubmit : eInterpreterRobot));
}

unsigned short Cross3::programSelect(const BString& name, const BString& parameters, bool force)
{
    if (!_syncSelect)
        return C3BI::ErrorNoInterface;

    VARIANT_BOOL bForce = force ? TRUE : FALSE;

    return getErrorCode(_syncSelect->Select(name.bstr(), parameters.bstr(), bForce));
}

unsigned short Cross3::programRun(const BString& name, const BString& parameters, bool force)
{
    if (!_syncSelect)
        return C3BI::ErrorNoInterface;

    VARIANT_BOOL bForce = force ? TRUE : FALSE;

    return getErrorCode(_syncSelect->Run(name.bstr(), parameters.bstr(), bForce));
}

unsigned short Cross3::motionPtp(const BString& position, bool relative)
{
    if (!_syncMotion)
        return C3BI::ErrorNoInterface;

    if (relative)
        return getErrorCode(_syncMotion->PtpRel(position.bstr()));

    return getErrorCode(_syncMotion->Ptp(position.bstr()));
}

unsigned short Cross3::motionLin(const BString& position, bool relative)
{
    if (!_syncMotion)
        return C3BI::ErrorNoInterface;

    if (relative)
        return getErrorCode(_syncMotion->LinRel(position.bstr()));

    return getErrorCode(_syncMotion->Lin(position.bstr()));
}

unsigned short Cross3::kcpKeyStart(long interpreter, bool backward, bool off)
{
    if (!_syncKcpKey)
        return C3BI::ErrorNoInterface;

    EKInterpreter eInterpreter = interpreter ? eInterpreterRobot : eInterpreterSubmit;

    VARIANT_BOOL bBackward = backward ? TRUE : FALSE;
    VARIANT_BOOL bOff = off ? TRUE : FALSE;

    return getErrorCode(_syncKcpKey->Start(eInterpreter, bBackward, bOff));
}

unsigned short Cross3::kcpKeyStop(long interpreter, bool off)
{
    if (!_syncKcpKey)
        return C3BI::ErrorNoInterface;

    EKInterpreter eInterpreter = interpreter ? eInterpreterRobot : eInterpreterSubmit;

    VARIANT_BOOL bOff = off ? TRUE : FALSE;

    return getErrorCode(_syncKcpKey->Stop(eInterpreter, bOff));
}

unsigned short Cross3::kcpKeyMove(long axis, long key, bool direction, bool off)
{
    if (!_syncKcpKey)
        return C3BI::ErrorNoInterface;

    VARIANT_BOOL bDirection = direction ? TRUE : FALSE;
    VARIANT_BOOL bOff = off ? TRUE : FALSE;

    return getErrorCode(_syncKcpKey->Move(axis, bDirection, key, bOff));
}

unsigned short Cross3::kcpKeyMove6D(bool off)
{
    if (!_syncKcpKey)
        return C3BI::ErrorNoInterface;

    VARIANT_BOOL bOff = off ? TRUE : FALSE;

    return getErrorCode(_syncKcpKey->Move6D(bOff));
}

unsigned short Cross3::fileSetAttribute(const BString& name, long attribute, long mask)
{
    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncFile->SetAttribute(name.bstr(), attribute, mask));
}

unsigned short Cross3::fileNameList(const BString& path, long type, long flags)
{
    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    VARIANT names;
    VARIANT info;

    return getErrorCode(_syncFile->NameList(path.bstr(), type, flags, &names, &info));
}

unsigned short Cross3::fileCreate(const BString& name, unsigned short itemType, unsigned char modulePart,
    const BString& templateName, bool createAlways)
{
    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    VARIANT_BOOL bCreateAlways = createAlways ? TRUE : FALSE;

    return getErrorCode(_syncFile->Create(name.bstr(), static_cast<EKItemType>(itemType),
        static_cast<EKModulePart>(modulePart), templateName.bstr(), bCreateAlways));
}

unsigned short Cross3::fileDelete(const BString& name, bool deleteAlways)
{
    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    VARIANT_BOOL bDeleteAlways = deleteAlways ? TRUE : FALSE;

    return getErrorCode(_syncFile->Delete(name.bstr(), bDeleteAlways));
}

unsigned short Cross3::fileCopy(const BString& source, const BString& destination, long flags)
{
    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncFile->Copy(source.bstr(), destination.bstr(), flags));
}

unsigned short Cross3::fileMove(const BString& source, const BString& destination, long flags)
{
    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncFile->Move(source.bstr(), destination.bstr(), flags));
}

unsigned short Cross3::fileGetProperties(const BString& name, long mask, FileProperties& properties)
{
    properties.name.clear();
    properties.itemType = 0;
    properties.lowSize = 0;
    properties.highSize = 0;
    properties.attributes = 0;
    properties.lowTimeCreated = 0;
    properties.highTimeCreated = 0;
    properties.biasCreated = 0;
    properties.lowTimeAccess = 0;
    properties.highTimeAccess = 0;
    properties.biasAccess = 0;
    properties.lowTimeModified = 0;
    properties.highTimeModified = 0;
    properties.biasModified = 0;
    properties.editMode = 0;

    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncFile->GetProperties(name.bstr(), mask,
        &properties.itemType, properties.name.address(),
        &properties.lowSize, &properties.highSize, &properties.attributes,
        &properties.lowTimeCreated, &properties.highTimeCreated, &properties.biasCreated,
        &properties.lowTimeAccess, &properties.highTimeAccess, &properties.biasAccess,
        &properties.lowTimeModified, &properties.highTimeModified, &properties.biasModified,
        &properties.editMode));
}

unsigned short Cross3::fileGetFullName(const BString& name, BString& fullName)
{
    fullName.clear();

    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncFile->GetFullName(name.bstr(), fullName.address()));
}

unsigned short Cross3::fileGetKrcName(const BString& name, BString& krcName)
{
    krcName.clear();

    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_syncFile->GetKrcName(name.bstr(), krcName.address()));
}

unsigned short Cross3::fileReadContent(const BString& name, long flags, std::vector<char>& content)
{
    content.clear();

    if (!_syncFile)
        return C3BI::ErrorNoInterface;

    VARIANT vContent;

    unsigned short result = getErrorCode(_syncFile->CopyFile2Mem(name.bstr(), flags, &vContent));

    VariantClear(&vContent);

    return result;
}

unsigned short Cross3::crossConfirmAll()
{
    if (!_asyncError || !_callbackError)
        return C3BI::ErrorNoInterface;

    return getErrorCode(_asyncError->Confirm(_callbackError, 0, 0));
}

Cross3::Cross3()
    : _serviceFactory(NULL)
    , _syncVar(NULL)
    , _syncSelect(NULL)
    , _syncFile(NULL)
    , _syncMotion(NULL)
    , _syncKcpKey(NULL)
    , _syncVolume(NULL)
    , _adviseMessage(NULL)
    , _asyncError(NULL)
    , _callbackError(NULL)
{
    CoInitialize(NULL);

    HRESULT result = CoCreateInstance(CLSID_KrcServiceFactory, NULL, CLSCTX_LOCAL_SERVER, 
        IID_IKServiceFactory, reinterpret_cast<void**>(&_serviceFactory));

    if (FAILED(result) || !_serviceFactory)
        return;

    getInterface(OLESTR("WBC_KrcLib.SyncVar"), __uuidof(_syncVar), reinterpret_cast<void**>(&_syncVar));
    getInterface(OLESTR("WBC_KrcLib.SyncSelect"), __uuidof(_syncSelect), reinterpret_cast<void**>(&_syncSelect));
    getInterface(OLESTR("WBC_KrcLib.SyncFile"), __uuidof(_syncFile), reinterpret_cast<void**>(&_syncFile));
    getInterface(OLESTR("WBC_KrcLib.SyncMotion"), __uuidof(_syncMotion), reinterpret_cast<void**>(&_syncMotion));
    getInterface(OLESTR("WBC_KrcLib.SyncKcpKey"), __uuidof(_syncKcpKey), reinterpret_cast<void**>(&_syncKcpKey));
    getInterface(OLESTR("WBC_KrcLib.SyncVolume"), __uuidof(_syncVolume), reinterpret_cast<void**>(&_syncVolume));
    getInterface(OLESTR("WBC_KrcLib.AdviseMessage"), __uuidof(_adviseMessage),
        reinterpret_cast<void**>(&_adviseMessage));
    getInterface(OLESTR("WBC_KrcLib.AdviseMessage"), __uuidof(_asyncError),
        reinterpret_cast<void**>(&_asyncError));

    if (_asyncError)
        _callbackError = new CallbackErrorImplementation();
}

Cross3::~Cross3()
{
    if (_callbackError)
    {
        delete _callbackError;
        _callbackError = NULL;
    }

    CoUninitialize();
}

HRESULT Cross3::getInterface(LPCOLESTR name, REFIID riid, void** object)
{
    if (object)
        *object = NULL;

    if (!_serviceFactory)
        return E_FAIL;

    BString serviceId(name);
    BString clientId(_clientId);

    IKService* service = NULL;
    HRESULT result = _serviceFactory->GetService(serviceId.bstr(), clientId.bstr(), &service);

    if (FAILED(result))
        return result;

    if (!service)
        return E_FAIL;

    result = service->QueryInterface(riid, object);

    if (FAILED(result) && object)
        *object = NULL;

    service->Release();
    return result;
}
