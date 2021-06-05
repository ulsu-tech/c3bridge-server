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


#ifndef C3BI_H
#define C3BI_H


namespace C3BI
{
    enum Error
    {
        ErrorGeneral                      = 0,
        ErrorSuccess                      = 1,
        ErrorAccess                       = 2,
        ErrorArgument                     = 3,
        ErrorMemory                       = 4,
        ErrorPointer                      = 5,
        ErrorUnexpected                   = 6,
        ErrorNotImplemented               = 7,
        ErrorNoInterface                  = 8,
        ErrorProtocol                     = 9,
        ErrorLongAnswer                   = 10
    };

    enum Command
    {
        // Variables Handling
        CommandReadVariableAscii          = 0,
        CommandWriteVariableAscii         = 1,
        CommandReadArrayAscii             = 2,
        CommandWriteArrayAscii            = 3,
        CommandReadVariable               = 4,
        CommandWriteVariable              = 5,
        CommandReadMultiple               = 6,
        CommandWriteMultiple              = 7,

        // Reserved
        CommandReserved8                  = 8,
        CommandReserved9                  = 9,

        // Program Handling
        CommandProgramControl             = 10,

        // Motion Handling
        CommandMotion                     = 11,

        // KCP Key Handling
        CommandKcpAction                  = 12,

        // Proxy Information Handling
        CommandProxyInfo                  = 13,
        CommandProxyFeatures              = 14,
        CommandProxyInfoEx                = 15,
        CommandProxyCrossInfo             = 16,
        CommandProxyBenchmark             = 17,

        // Reserved
        CommandReserved18                 = 18,
        CommandReserved19                 = 19,

        // File Handling
        CommandFileSetAttribute           = 20,
        CommandFileNameList               = 21,
        CommandFileCreate                 = 22,
        CommandFileDelete                 = 23,
        CommandFileCopy                   = 24,
        CommandFileMove                   = 25,
        CommandFileGetProperties          = 26,
        CommandFileGetFullName            = 27,
        CommandFileGetKrcName             = 28,
        CommandFileWriteContent           = 29,
        CommandFileReadContent            = 30,

        // Reserved
        CommandReserved31                 = 31,
        CommandReserved32                 = 32,
        CommandReserved33                 = 33,
        CommandReserved34                 = 34,
        CommandReserved35                 = 35,
        CommandReserved36                 = 36,
        CommandReserved37                 = 37,
        CommandReserved38                 = 38,
        CommandReserved39                 = 39,
        CommandReserved40                 = 40,
        CommandReserved41                 = 41,
        CommandReserved42                 = 42,
        CommandReserved43                 = 43,
        CommandReserved44                 = 44,
        CommandReserved45                 = 45,
        CommandReserved46                 = 46,
        CommandReserved47                 = 47,
        CommandReserved48                 = 48,
        CommandReserved49                 = 49,

        // CrossCommEXE Functions Handling
        CommandCrossSetInfoOn             = 50,
        CommandCrossSetInfoOff            = 51,
        CommandCrossGetRobotDirectory     = 52,
        CommandCrossDownloadDiskToRobot   = 53,
        CommandCrossDownloadMemToRobot    = 54,
        CommandCrossUploadFromRobotToDisk = 55,
        CommandCrossUploadFromRobotToMem  = 56,
        CommandCrossDeleteRobotProgram    = 57,
        CommandCrossRobotLevelStop        = 58,
        CommandCrossControlLevelStop      = 59,
        CommandCrossRunControlLevel       = 60,
        CommandCrossSelectModul           = 61,
        CommandCrossCancelModul           = 62,
        CommandCrossConfirmAll            = 63,
        CommandCrossKrcOk                 = 64,
        CommandCrossIoRestart             = 65,
        CommandCrossReserved66            = 66,
        CommandCrossReserved67            = 67,
        CommandCrossReserved68            = 68,
        CommandCrossReserved69            = 69,

        // Reserved
        CommandReserved70                 = 70,
        CommandReserved71                 = 71,
        CommandReserved72                 = 72,
        CommandReserved73                 = 73,
        CommandReserved74                 = 74,
        CommandReserved75                 = 75,
        CommandReserved76                 = 76,
        CommandReserved77                 = 77,
        CommandReserved78                 = 78,
        CommandReserved79                 = 79,
        CommandReserved80                 = 80,
        CommandReserved81                 = 81,
        CommandReserved82                 = 82,
        CommandReserved83                 = 83,
        CommandReserved84                 = 84,
        CommandReserved85                 = 85,
        CommandReserved86                 = 86,
        CommandReserved87                 = 87,
        CommandReserved88                 = 88,
        CommandReserved89                 = 89,
        CommandReserved90                 = 90,
        CommandReserved91                 = 91,
        CommandReserved92                 = 92,
        CommandReserved93                 = 93,
        CommandReserved94                 = 94,
        CommandReserved95                 = 95,
        CommandReserved96                 = 96,
        CommandReserved97                 = 97,
        CommandReserved98                 = 98,
        CommandReserved99                 = 99,
        CommandReserved100                = 100,
        CommandReserved101                = 101,
        CommandReserved102                = 102,
        CommandReserved103                = 103,
        CommandReserved104                = 104,
        CommandReserved105                = 105,
        CommandReserved106                = 106,
        CommandReserved107                = 107,
        CommandReserved108                = 108,
        CommandReserved109                = 109,
        CommandReserved110                = 110,
        CommandReserved111                = 111,
        CommandReserved112                = 112,
        CommandReserved113                = 113,
        CommandReserved114                = 114,
        CommandReserved115                = 115,
        CommandReserved116                = 116,
        CommandReserved117                = 117,
        CommandReserved118                = 118,
        CommandReserved119                = 119,
        CommandReserved120                = 120,
        CommandReserved121                = 121,
        CommandReserved122                = 122,
        CommandReserved123                = 123,
        CommandReserved124                = 124,
        CommandReserved125                = 125,
        CommandReserved126                = 126,
        CommandReserved127                = 127,
        CommandReserved128                = 128,

        // Extended Command
        CommandExtended                   = 255
    };

    enum ProgramControl
    {
        ProgramNone                       = 0,
        ProgramReset                      = 1,
        ProgramStart                      = 2,
        ProgramStop                       = 3,
        ProgramCancel                     = 4,
        ProgramSelect                     = 5,
        ProgramRun                        = 6,
        ProgramDisableStart               = 7,
        ProgramResetSumbit                = 8,
        ProgramStartSumbit                = 9,
        ProgramStopSumbit                 = 10,
        ProgramCancelSumbit               = 11,
        ProgramSelectSumbit               = 12,
        ProgramRunSumbit                  = 13
    };

    enum MotionType
    {
        MotionNone                        = 0,
        MotionPtp                         = 1,
        MotionLin                         = 2,
        MotionPtpRelative                 = 3,
        MotionLinRelative                 = 4
    };

    enum KcpActionType
    {
        KcpActionNone                     = 0,
        KcpActionStart                    = 1,
        KcpActionStop                     = 2,
        KcpActionMove                     = 3,
        KcpActionMove6D                   = 4
    };

    enum FileIoOperation
    {
        FileIoNone                        = 0,
        FileIoBegin                       = 1,
        FileIoData                        = 2,
        FileIoGetSize                     = 3,
        FileIoEnd                         = 4,
        FileIoGetChecksum                 = 5
    };
}


#endif // C3BI_H
