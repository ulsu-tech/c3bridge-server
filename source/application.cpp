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

#include "application.h"
#include "proxy.h"
#include "cross3.h"


Application::Application()
{
}

Application::~Application()
{
}

BOOL Application::InitInstance()
{
    Cross3::instance();

    Proxy::registerFeature(C3BI::CommandReadVariableAscii);
    Proxy::registerFeature(C3BI::CommandWriteVariableAscii);
    //Proxy::registerFeature(C3BI::CommandReadArrayAscii);
    //Proxy::registerFeature(C3BI::CommandWriteArrayAscii);
    Proxy::registerFeature(C3BI::CommandReadVariable);
    Proxy::registerFeature(C3BI::CommandWriteVariable);
    Proxy::registerFeature(C3BI::CommandReadMultiple);
    Proxy::registerFeature(C3BI::CommandWriteMultiple);

    Proxy::registerFeature(C3BI::CommandProgramControl);
    Proxy::registerFeature(C3BI::CommandMotion);
    Proxy::registerFeature(C3BI::CommandKcpAction);

    Proxy::registerFeature(C3BI::CommandProxyInfo);
    Proxy::registerFeature(C3BI::CommandProxyFeatures);

    /*
     * These capabilities have not been tested and can be dangerous.
    Proxy::registerFeature(C3BI::CommandFileSetAttribute);
    Proxy::registerFeature(C3BI::CommandFileNameList);
    Proxy::registerFeature(C3BI::CommandFileCreate);
    Proxy::registerFeature(C3BI::CommandFileDelete);
    Proxy::registerFeature(C3BI::CommandFileCopy);
    Proxy::registerFeature(C3BI::CommandFileMove);
    Proxy::registerFeature(C3BI::CommandFileGetProperties);
    Proxy::registerFeature(C3BI::CommandFileGetFullName);
    Proxy::registerFeature(C3BI::CommandFileGetKrcName);
    Proxy::registerFeature(C3BI::CommandFileWriteContent);
    Proxy::registerFeature(C3BI::CommandFileReadContent);
     */

    Proxy::registerFeature(C3BI::CommandCrossConfirmAll);

    _mainWindow.Create();

    return TRUE;
}
