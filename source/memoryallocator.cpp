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

#include "memoryallocator.h"


class MemoryAllocatorDestroyer
{
public:
    ~MemoryAllocatorDestroyer()
    {
        if (MemoryAllocator::_instance)
        {
            delete MemoryAllocator::_instance;
            MemoryAllocator::_instance = NULL;
        }
    }
};


static MemoryAllocatorDestroyer s_destroyer;


MemoryAllocator* MemoryAllocator::_instance = NULL;


MemoryAllocator* MemoryAllocator::instance()
{
    if (!_instance)
        _instance = new MemoryAllocator();

    return _instance;
}

void* MemoryAllocator::allocate(size_t size)
{
    MemoryAllocator* allocator = instance();
    return allocator ? allocator->internalAllocate(size) : NULL;
}

void MemoryAllocator::reallocate(void** memory, size_t size)
{
    MemoryAllocator* allocator = instance();
    if (allocator)
        allocator->internalReallocate(memory, size);
}

void MemoryAllocator::deallocate(void** memory)
{
    MemoryAllocator* allocator = instance();
    if (allocator)
        allocator->internalDeallocate(memory);
}

MemoryAllocator::MemoryAllocator()
    : _sizeAllocated(0)
    , _sizeHighPoint(0)
    , _shrinkPoint(DefaultShrinkPoint)
    , _shrinkCount(DefaultShrinkCount)
{
}

MemoryAllocator::~MemoryAllocator()
{
    std::list<Buffer>::const_iterator iterator;

    for (iterator = _list.begin(); iterator != _list.end(); ++iterator)
    {
        if (iterator->memory)
            ::free(iterator->memory);
    }

    _list.clear();
}

void MemoryAllocator::collect()
{

}

void* MemoryAllocator::internalAllocate(size_t size)
{
    if (!size)
        return NULL;

    Buffer* buffer = NULL;

    std::list<Buffer>::iterator iterator;

    for (iterator = _list.begin(); iterator != _list.end(); ++iterator)
    {
        if (!iterator->used && iterator->size >= size)
        {
            // Search smallest buffer
            if (buffer)
            {
                if (buffer->size > iterator->size)
                    buffer = &(*iterator);
            }
            else
            {
                buffer = &(*iterator);
            }
        }
    }

    if (!buffer)
    {
        Buffer newBuffer;
        newBuffer.memory = ::malloc(size);
        newBuffer.size = size;

        if (newBuffer.memory)
        {
            _list.push_back(newBuffer);
            buffer = &_list.back();

            _sizeAllocated += size;

            if (_sizeAllocated > _sizeHighPoint)
                _sizeHighPoint = _sizeAllocated;
        }
    }

    if (buffer)
    {
        buffer->used = true;
        return buffer->memory;
    }

    return NULL;
}

void MemoryAllocator::internalReallocate(void** memory, size_t size)
{
    if (memory && *memory)
    {
        std::list<Buffer>::iterator iterator;

        for (iterator = _list.begin(); iterator != _list.end(); ++iterator)
        {
            if (iterator->memory == *memory)
            {
                if (iterator->size >= size)
                {
                    iterator->used = true;
                    return;
                }

                iterator->used = false;
                break;
            }
        }
    }

    if (memory)
        *memory = internalAllocate(size);
}

void MemoryAllocator::internalDeallocate(void** memory)
{
    if (memory && *memory)
    {
        std::list<Buffer>::iterator iterator;

        for (iterator = _list.begin(); iterator != _list.end(); ++iterator)
        {
            if (iterator->memory == *memory)
            {
                iterator->used = false;
                *memory = NULL;
                break;
            }
        }
    }

    collect();
}
