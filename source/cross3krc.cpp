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


#ifdef __cplusplus
extern "C"{
#endif 

#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IKService,0x3E858FB8,0x2191,0x4893,0x8B,0xF2,0x41,0xE9,0x97,0xC0,0xA4,0xBB);
MIDL_DEFINE_GUID(IID, IID_IKServiceFactory,0x3E858FB9,0x2191,0x4893,0x8B,0xF2,0x41,0xE9,0x97,0xC0,0xA4,0xBB);
MIDL_DEFINE_GUID(IID, IID_ICKSyncVar,0xE5B3AB70,0xB48E,0x11D4,0xB0,0x53,0x00,0xA0,0xD2,0x1A,0xFA,0x30);
MIDL_DEFINE_GUID(IID, IID_ICKSyncMotion,0xD2BC04B8,0x5581,0x4395,0x9C,0x2A,0x69,0x8D,0xAD,0x03,0x08,0x86);
MIDL_DEFINE_GUID(IID, IID_ICKSyncVolume,0xF3044C4A,0x90D6,0x4CD8,0xA6,0x21,0xE5,0xB6,0xD2,0x94,0x40,0x9A);
MIDL_DEFINE_GUID(IID, IID_ICKSyncSelect,0x716C919E,0xBB4A,0x4DDB,0xA7,0x83,0xFD,0xDF,0x8A,0x98,0x74,0x19);
MIDL_DEFINE_GUID(IID, IID_ICKSyncFile,0xD187B1C9,0x83D6,0x4B99,0x8C,0xB2,0xF7,0x6F,0xC5,0x0A,0xF2,0x53);
MIDL_DEFINE_GUID(IID, IID_ICKSyncKcpKey,0x988475F7,0x035C,0x472F,0x89,0xFF,0xA7,0x54,0x7B,0x3B,0xFA,0x92);
MIDL_DEFINE_GUID(IID, IID_IKCallbackError, 0x382901A5, 0x92B7, 0x4E4B, 0xBF, 0xDC, 0xCB, 0x4B, 0x8F, 0x38, 0xA9, 0x46);
MIDL_DEFINE_GUID(IID, IID_IKAsyncError, 0x382901A7, 0x92B7, 0x4E4B, 0xBF, 0xDC, 0xCB, 0x4B, 0x8F, 0x38, 0xA9, 0x46);
MIDL_DEFINE_GUID(IID, IID_ICKConsumeMessage, 0x7BE9ABE4, 0xB55A, 0x11D4, 0x91, 0x3F, 0x00, 0x50, 0xDA, 0x23, 0x48, 0x7D);
MIDL_DEFINE_GUID(IID, IID_ICKAdviseMessage, 0x7BE9ABE5, 0xB55A, 0x11D4, 0x91, 0x3F, 0x00, 0x50, 0xDA, 0x23, 0x48, 0x7D);

MIDL_DEFINE_GUID(CLSID, CLSID_SyncVar,0x12163D64,0xC52B,0x11D4,0xB0,0x53,0x00,0xA0,0xD2,0x1A,0xFA,0x30);
MIDL_DEFINE_GUID(CLSID, CLSID_SyncMotion,0x817FCC28,0x81D8,0x49BF,0x98,0x77,0xEB,0x94,0x58,0xE8,0x53,0x72);
MIDL_DEFINE_GUID(CLSID, CLSID_SyncVolume,0xD466365A,0xA105,0x46E9,0x97,0x6E,0x00,0xF4,0xCF,0x70,0x44,0x8A);
MIDL_DEFINE_GUID(CLSID, CLSID_SyncSelect,0xF8DF9700,0xEE39,0x455A,0xA8,0x7D,0x53,0x31,0x09,0x05,0xB8,0x15);
MIDL_DEFINE_GUID(CLSID, CLSID_SyncFile,0x1AA9B282,0x9031,0x451D,0xAA,0x8A,0xD2,0xE6,0xA6,0xED,0x7B,0xA4);
MIDL_DEFINE_GUID(CLSID, CLSID_SyncKcpKey,0xC2B41662,0x1851,0x4CD1,0xAB,0x44,0x7E,0x7F,0x86,0x40,0xFD,0x96);
MIDL_DEFINE_GUID(CLSID, CLSID_AdviseMessage, 0xDAE059CB, 0xD0D3, 0x11D4, 0xB0, 0x53, 0x00, 0xA0, 0xD2, 0x1A, 0xFA, 0x30);
MIDL_DEFINE_GUID(CLSID, CLSID_KrcServiceFactory,0x307230E1,0xB48F,0x11D4,0xB0,0x53,0x00,0xA0,0xD2,0x1A,0xFA,0x30);


#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif
