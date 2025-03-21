#pragma once
#include "Types.h"
#include "MemoryPool.h"

template <typename Type>
class ObjectPool
{
private:
    static int32        s_allocSize; // 메모리 영역 크기
    static MemoryPool   s_pool;

public:
    template<typename... Args>
    static Type* Pop(Args&&... args)
    {
        Type* memory = static_cast<Type*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
        new(memory)Type(std::forward<Args>(args)...); // placement new
        return memory;
    }

    static void Push(Type* obj)
    {
        obj->~Type();
        s_pool.Push(MemoryHeader::DetachHeader(obj));
    }

    template<typename... Args>
    static shared_ptr<Type> MakeShared(Args&&... _args)
    {
        shared_ptr<Type> sptr = { Pop(std::forward<Args>(_args)...), Push };
        return sptr;
    }
};

template <typename Type>
int32 ObjectPool<Type>::s_allocSize = sizeof(Type) + sizeof(MemoryHeader);

template <typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_allocSize };