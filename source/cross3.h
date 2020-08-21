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


#ifndef CROSS3_H
#define CROSS3_H


#include "cross3krc.h"
#include "bstring.h"


class CallbackErrorImplementation;


class Cross3
{
public:
    struct FileProperties
    {
        BString name;
        long itemType;
        long lowSize;
        long highSize;
        long attributes;
        long lowTimeCreated;
        long highTimeCreated;
        long biasCreated;
        long lowTimeAccess;
        long highTimeAccess;
        long biasAccess;
        long lowTimeModified;
        long highTimeModified;
        long biasModified;
        long editMode;
    };

public:
    static Cross3* instance();
    static unsigned short getErrorCode(HRESULT result);

    unsigned short getVariable(const std::string& name, std::string& value);
    unsigned short getVariable(const BString& name, BString& value);
    unsigned short setVariable(const std::string& name, std::string& value);
    unsigned short setVariable(const BString& name, BString& value);

    unsigned short programReset(short interpreter);
    unsigned short programStart(short interpreter);
    unsigned short programStop(short interpreter);
    unsigned short programCancel(short interpreter);
    unsigned short programSelect(const BString& name, const BString& parameters, bool force);
    unsigned short programRun(const BString& name, const BString& parameters, bool force);

    unsigned short motionPtp(const BString& position, bool relative);
    unsigned short motionLin(const BString& position, bool relative);

    unsigned short kcpKeyStart(long interpreter, bool backward, bool off);
    unsigned short kcpKeyStop(long interpreter, bool off);
    unsigned short kcpKeyMove(long axis, long key, bool direction, bool off);
    unsigned short kcpKeyMove6D(bool off);

    unsigned short fileSetAttribute(const BString& name, long attribute, long mask);
    unsigned short fileNameList(const BString& path, long type, long flags);
    unsigned short fileCreate(const BString& name, unsigned short itemType, unsigned char modulePart,
        const BString& templateName, bool createAlways);
    unsigned short fileDelete(const BString& name, bool deleteAlways);
    unsigned short fileCopy(const BString& source, const BString& destination, long flags);
    unsigned short fileMove(const BString& source, const BString& destination, long flags);
    unsigned short fileGetProperties(const BString& name, long mask, FileProperties& properties);
    unsigned short fileGetFullName(const BString& name, BString& fullName);
    unsigned short fileGetKrcName(const BString& name, BString& krcName);
    unsigned short fileReadContent(const BString& name, long flags, std::vector<char>& content);

    unsigned short crossConfirmAll();

private:
    Cross3();
    ~Cross3();
    Cross3(const Cross3&);              // Disable copy construction
    Cross3& operator=(const Cross3&);   // Disable assignment operator

    HRESULT getInterface(LPCOLESTR name, REFIID riid, void** object);

private:
    static Cross3*   _instance;
    static LPCOLESTR _clientId;

    IKServiceFactory* _serviceFactory;
    ICKSyncVar* _syncVar;
    ICKSyncSelect* _syncSelect;
    ICKSyncFile* _syncFile;
    ICKSyncMotion* _syncMotion;
    ICKSyncKcpKey* _syncKcpKey;
    ICKSyncVolume* _syncVolume;
    ICKAdviseMessage* _adviseMessage;
    IKAsyncError* _asyncError;

    CallbackErrorImplementation* _callbackError;

    friend class Cross3Destroyer;
};


#endif // CROSS3_H



