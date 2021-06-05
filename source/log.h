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


#ifndef LOG_H
#define LOG_H


class Log
{
public:
    enum Severity
    {
        None    = 0,
        Fatal   = 1,
        Error   = 2,
        Warning = 3,
        Info    = 4,
        Debug   = 5,
        Verbose = 6
    };

public:
    static Severity severity();
    static void setSeverity(Severity severity);

    static void message(Severity severity, LPCTSTR text);
    static void message(Severity severity, UINT resource);
    static void format(Severity severity, LPCTSTR text, ...);
    static void format(Severity severity, UINT resource, ...);

    static void print(LPCTSTR text);
    static void print(UINT resource);

    static void fatal(LPCTSTR text);
    static void fatal(UINT resource);

    static void error(LPCTSTR text);
    static void error(UINT resource);

    static void warning(LPCTSTR text);
    static void warning(UINT resource);

    static void info(LPCTSTR text);
    static void info(UINT resource);

    static void debug(LPCTSTR text);
    static void debug(UINT resource);

    static void verbose(LPCTSTR text);
    static void verbose(UINT resource);

private:
    static Severity _severity;
};


inline Log::Severity Log::severity()
{
    return _severity;
}

inline void Log::setSeverity(Log::Severity severity)
{
    _severity = severity;
}

inline void Log::print(LPCTSTR text)
{
    message(None, text);
}

inline void Log::print(UINT resource)
{
    message(None, resource);
}

inline void Log::fatal(LPCTSTR text)
{
    message(Fatal, text);
}

inline void Log::fatal(UINT resource)
{
    message(Fatal, resource);
}

inline void Log::error(LPCTSTR text)
{
    message(Error, text);
}

inline void Log::error(UINT resource)
{
    message(Error, resource);
}

inline void Log::warning(LPCTSTR text)
{
    message(Warning, text);
}

inline void Log::warning(UINT resource)
{
    message(Warning, resource);
}

inline void Log::info(LPCTSTR text)
{
    message(Info, text);
}

inline void Log::info(UINT resource)
{
    message(Info, resource);
}

inline void Log::debug(LPCTSTR text)
{
    message(Debug, text);
}

inline void Log::debug(UINT resource)
{
    message(Debug, resource);
}

inline void Log::verbose(LPCTSTR text)
{
    message(Verbose, text);
}

inline void Log::verbose(UINT resource)
{
    message(Verbose, resource);
}


#endif // LOG_H