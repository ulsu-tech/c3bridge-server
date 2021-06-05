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


#ifndef MEMORYALLOCATOR_H_INCLUDED
#define MEMORYALLOCATOR_H_INCLUDED


class MemoryAllocator
{
public:
    static MemoryAllocator* instance();

    static void* allocate(size_t size);
    static void  reallocate(void** memory, size_t size);
    static void  deallocate(void** memory);

private:
    enum
    {
        DefaultShrinkPoint = 264000,
        DefaultShrinkCount = 4
    };

    struct Buffer
    {
        void*  memory;
        size_t size;
        bool   used;
    };

    friend class MemoryAllocatorDestroyer;

private:
    MemoryAllocator();
    ~MemoryAllocator();
    MemoryAllocator(const MemoryAllocator&);
    MemoryAllocator& operator=(const MemoryAllocator&);

    void collect();

    void* internalAllocate(size_t size);
    void  internalReallocate(void** memory, size_t size);
    void  internalDeallocate(void** memory);

private:
    static MemoryAllocator* _instance;

    std::list<Buffer> _list;

    size_t _sizeAllocated;
    size_t _sizeHighPoint;
    size_t _shrinkPoint;
    size_t _shrinkCount;
};


#endif // MEMORYALLOCATOR_H_INCLUDED
