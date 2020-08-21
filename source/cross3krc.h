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


#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef CROSS3KRC_H
#define CROSS3KRC_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif


typedef enum
{
    eInterpreterNone = 0xffffffff,
    eInterpreterSubmit = 0,
    eInterpreterRobot = 1
} 	EKInterpreter;

typedef enum
{
    eItemTypeUnknown = 0,
    eItemTypeDir = 1,
    eItemTypeVirtualDir = 2,
    eItemTypeArchiv = 4,
    eItemTypeBinFile = 8,
    eItemTypeTextFile = 16,
    eItemTypeModule = 32,
    eItemTypeRaw = 64,
    eItemTypeMotionFile = 128,
    eItemTypeProtectedFileContainer = 256
} 	EKItemType;

typedef enum
{
    eModulePartUnknown = 0,
    eModulePartSUB = 1,
    eModulePartSRC = 2,
    eModulePartDAT = 4,
    eModulePartSUBDAT = 5,
    eModulePartSRCDAT = 6,
    eModulePartTemplate = 8,
    eModulePartMotion = 16
} 	EKModulePart;

typedef enum
{
    eVolumeTypeUnknown = 0,
    eVolumeTypeKRC = 1,
    eVolumeTypeRemoveable = 2,
    eVolumeTypeFixed = 3,
    eVolumeTypeCDRom = 4,
    eVolumeTypeRemote = 5,
    eVolumeTypeRAMDisk = 6,
    eVolumeTypeArchive = 7
} 	EKVolumeType;

typedef enum
{
    eMessageTypeInfo = 1,
    eMessageTypeState = 2,
    eMessageTypeQuitt = 4,
    eMessageTypeEvent = 16,
    eStartSending = 4096
} 	EKMessageType;

typedef enum
{
    eMessageTypeWait = 32,
    eMessageTypeDialog = 256
} 	EKDialogType;


typedef struct tagTKMessage
{
    long nLowTimeStamp;
    long nHighTimeStamp;
    long nBiasTimeStamp;
    EKMessageType eMessageType;
    long nMessage;
    long nMessageHandle;
    BSTR bstrDBModule;
    BSTR bstrCause;
    VARIANT_BOOL bDBCause;
    BSTR bstrDBMessage;
    VARIANT varParams;
    VARIANT varDBParams;
} 	TKMessage;

typedef struct tagTKDialog
{
    long nLowTimeStamp;
    long nHighTimeStamp;
    long nBiasTimeStamp;
    EKDialogType eDialogType;
    long nMessage;
    long nMessageHandle;
    BSTR bstrDBModule;
    BSTR bstrCause;
    VARIANT_BOOL bDBCause;
    BSTR bstrDBMessage;
    VARIANT varParams;
    VARIANT varDBParams;
    VARIANT varAnswers;
    VARIANT varDBAnswers;
} 	TKDialog;


EXTERN_C const IID IID_IKService;

MIDL_INTERFACE("3E858FB8-2191-4893-8BF2-41E997C0A4BB")
IKService : public IUnknown
{
public:
};

EXTERN_C const IID IID_IKServiceFactory;

MIDL_INTERFACE("3E858FB9-2191-4893-8BF2-41E997C0A4BB")
IKServiceFactory : public IUnknown
{
public:
    virtual HRESULT __stdcall GetService(BSTR strServiceID, BSTR strClientID,
        IKService **ppService) = 0;
};

EXTERN_C const IID IID_ICKSyncVar;

MIDL_INTERFACE("E5B3AB70-B48E-11D4-B053-00A0D21AFA30")
ICKSyncVar : public IUnknown
{
public:
    virtual HRESULT __stdcall ShowVar(BSTR bstrVar, BSTR *pbstrVal) = 0;
    virtual HRESULT __stdcall SetVar(BSTR bstrVar, BSTR bstrVal) = 0;
    virtual HRESULT __stdcall ShowMultiVar(VARIANT varVars,
        VARIANT *pvarVals) = 0;
    virtual HRESULT __stdcall SetMultiVar(VARIANT varVars, 
        VARIANT varVals) = 0;
};

EXTERN_C const IID IID_ICKSyncSelect;

MIDL_INTERFACE("716C919E-BB4A-4DDB-A783-FDDF8A987419")
ICKSyncSelect : public IUnknown
{
public:
    virtual HRESULT __stdcall Reset(EKInterpreter eInterpreter) = 0;
    virtual HRESULT __stdcall Start(EKInterpreter eInterpreter) = 0;
    virtual HRESULT __stdcall Stop(EKInterpreter eInterpreter) = 0;
    virtual HRESULT __stdcall Cancel(EKInterpreter eInterpreter) = 0;
    virtual HRESULT __stdcall Select(BSTR bstrName, BSTR bstrParams,
        VARIANT_BOOL bForceSelect) = 0;
    virtual HRESULT __stdcall Run(BSTR bstrName, BSTR bstrParams,
        VARIANT_BOOL bForceRun) = 0;
};

EXTERN_C const IID IID_ICKSyncMotion;

MIDL_INTERFACE("D2BC04B8-5581-4395-9C2A-698DAD030886")
ICKSyncMotion : public IUnknown
{
public:
    virtual HRESULT __stdcall Ptp(BSTR bstrPosition) = 0;
    virtual HRESULT __stdcall Lin(BSTR bstrPosition) = 0;
    virtual HRESULT __stdcall PtpRel(BSTR bstrPosition) = 0;
    virtual HRESULT __stdcall LinRel(BSTR bstrPosition) = 0;
};

EXTERN_C const IID IID_ICKSyncKcpKey;

MIDL_INTERFACE("988475F7-035C-472F-89FF-A7547B3BFA92")
ICKSyncKcpKey : public IUnknown
{
public:
    virtual HRESULT __stdcall Start(EKInterpreter eInterpreter,
        VARIANT_BOOL bBackward, VARIANT_BOOL bOff) = 0;
    virtual HRESULT __stdcall Stop(EKInterpreter eInterpreter,
        VARIANT_BOOL bOff) = 0;
    virtual HRESULT __stdcall Move(long nAxis, VARIANT_BOOL bPlusDirection,
        long nKey, VARIANT_BOOL bOff) = 0;
    virtual HRESULT __stdcall Move6D(VARIANT_BOOL bOff) = 0;
};

EXTERN_C const IID IID_ICKSyncFile;

MIDL_INTERFACE("D187B1C9-83D6-4B99-8CB2-F76FC50AF253")
ICKSyncFile : public IDispatch
{
public:
    virtual HRESULT __stdcall SetAttribute(BSTR bstrFullName, long nAttribute,
        long nMask) = 0;
    virtual HRESULT __stdcall NameList(BSTR bstrPath, long nType, long nFlags,
        VARIANT *pvarNames, VARIANT *pvarInfo) = 0;
    virtual HRESULT __stdcall Create(BSTR bstrFullName, EKItemType eType,
        EKModulePart eModulePart, BSTR bstrTemplate,
        VARIANT_BOOL bCreateAlways) = 0;
    virtual HRESULT __stdcall Delete(BSTR bstrFullName,
        VARIANT_BOOL bDeleteAlways) = 0;
    virtual HRESULT __stdcall Copy(BSTR bstrSrcName, BSTR bstrDestName,
        long nFlags) = 0;
    virtual HRESULT __stdcall Move(BSTR bstrSrcName, BSTR bstrDestName,
        long nFlags) = 0;
    virtual HRESULT __stdcall GetProperties(BSTR bstrName, long nMask,
        long *pnItemType, BSTR *pbstrName, long *pnLowSize, long *pnHighSize,
        long *pnAttributes, long *pnLowTimeCreated, long *pnHighTimeCreated,
        long *pnBiasCreated, long *pnLowTimeAccess, long *pnHighTimeAccess,
        long *pnBiasAccess, long *pnLowTimeModified, long *pnHighTimeModified,
        long *pnBiasModified, long *pnEditMode) = 0;
    virtual HRESULT __stdcall GetFullName(BSTR bstrKrcName,
        BSTR *pbstrFullName) = 0;
    virtual HRESULT __stdcall GetKrcName(BSTR bstrFullName,
        BSTR *pbstrKrcName) = 0;
    virtual HRESULT __stdcall CopyMem2File(VARIANT varContents,
        BSTR bstrDestName, long nFlags) = 0;
    virtual HRESULT __stdcall CopyFile2Mem(BSTR bstrSrcName,
        long nFlags, VARIANT *pvarContents) = 0;
};

EXTERN_C const IID IID_ICKSyncVolume;

MIDL_INTERFACE("F3044C4A-90D6-4CD8-A621-E5B6D294409A")
ICKSyncVolume : public IUnknown
{
public:
    virtual HRESULT __stdcall GetProperties(BSTR bstrDrive, BSTR *pbstrName,
        EKVolumeType *peVolumeType, long *pnLowTotal, long *pnHighTotal,
        long *pnLowFree, long *pnHighFree, long *pnLowUsed, 
        long *pnHighUsed) = 0;
    virtual HRESULT __stdcall SetName(BSTR bstrDrive, BSTR bstrNewName) = 0;
    virtual HRESULT __stdcall DriveList(VARIANT *pvarDrives) = 0;
};

EXTERN_C const IID IID_IKCallbackError;

MIDL_INTERFACE("382901A5-92B7-4E4B-BFDC-CB4B8F38A946")
IKCallbackError : public IUnknown
{
public:
    virtual HRESULT __stdcall OnError(long nID, TKMessage *pErrorData) = 0;
    virtual HRESULT __stdcall OnDialog(long nID, TKDialog *pDialogData) = 0;
};

EXTERN_C const IID IID_IKAsyncError;

MIDL_INTERFACE("382901A7-92B7-4E4B-BFDC-CB4B8F38A946")
IKAsyncError : public IUnknown
{
public:
    virtual HRESULT __stdcall Answer(IKCallbackError *pICallbackError,
        long nID, long nMsgHandle, short nAnswer) = 0;
    virtual HRESULT __stdcall Confirm(IKCallbackError *pICallbackError,
        long nID, long nMsgHandle) = 0;
};

EXTERN_C const IID IID_ICKConsumeMessage;

MIDL_INTERFACE("7BE9ABE4-B55A-11D4-913F-0050DA23487D")
ICKConsumeMessage : public IUnknown
{
public:
    virtual HRESULT __stdcall OnAddMessage(TKMessage *pMessage) = 0;
    virtual HRESULT __stdcall OnSubMessage(long nMessageHandle) = 0;
    virtual HRESULT __stdcall OnAddDialog(TKDialog *pDialog) = 0;
};

EXTERN_C const IID IID_ICKAdviseMessage;

MIDL_INTERFACE("7BE9ABE5-B55A-11D4-913F-0050DA23487D")
ICKAdviseMessage : public IUnknown
{
public:
    virtual HRESULT __stdcall Advise(ICKConsumeMessage *pConsumer,
        long dwMessageType) = 0;
    virtual HRESULT __stdcall Unadvise(void) = 0;
};


EXTERN_C const CLSID CLSID_KrcServiceFactory;

class DECLSPEC_UUID("307230E1-B48F-11D4-B053-00A0D21AFA30")
    KrcServiceFactory;


#endif // CROSS3KRC_H


