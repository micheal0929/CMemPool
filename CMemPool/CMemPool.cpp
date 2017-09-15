/*
 * CMemPool.cpp
 *
 *  Created on: 2017年9月13日
 *      Author: michael
 */

#include "CMemPool.h"
#include <iostream>
BSL::MemBlock::MemBlock (size_t nSize):
pStart(nullptr)
,pEnd(nullptr)
,nBlockSplices(NBLOCKSPLICES)
,nSpliceSize(0)
{
    std::cout << "malloc\n";
    pStart = malloc(nSize*NBLOCKSPLICES);
    pEnd = (char*)pStart + nSize*NBLOCKSPLICES;
    nSpliceSize = nSize;
    vecpSplices.resize(NBLOCKSPLICES);
    for(int i=0;i<NBLOCKSPLICES;++i)
    {
        vecpSplices[i] = pStart + i * nSpliceSize;
    }
}

void*
BSL::MemBlock::FetchSplice (size_t nSize)
{
    int index = nFetchIndex++;
    void* retPtr = nullptr;
    auto curSplice = vecpSplices[index%nBlockSplices];
    if(curSplice != nullptr)
    {
        retPtr = curSplice;
        vecpSplices[index%nBlockSplices] = nullptr;
    }
    return retPtr;
}

void
BSL::MemBlock::RecoverSplice (void* pSplice)
{
    int index = nReturnIndex++;
    auto& curSplice = vecpSplices[index%nBlockSplices];
    if(curSplice == nullptr)
    {
        curSplice = pSplice;
    }
}

BSL::CMemPool::CMemPool ()
{
    Init();
}

BSL::CMemPool::~CMemPool ()
{
}

void*
BSL::CMemPool::alloc (size_t nSize)
{

    for(auto v:m_vecpBlocks)
    {
        if(v->nSpliceSize >= nSize)
        {
            auto retptr = v->FetchSplice(nSize);
            if(retptr != nullptr)
                return retptr;
        }
    }
    //没有合适的内存块，则创建新的内存块
    int nExist = 0;
    for(auto v:DEFALUT_SPLICE)
    {
        if(v >= nSize)
            nExist = v;
    }
    return Create(nSize);

}

void
BSL::CMemPool::dealloc (void* pSplice)
{
    for(auto v:m_vecpBlocks)
    {
        if(v->pStart <= pSplice && v->pEnd > pSplice)
        {
            v->RecoverSplice(pSplice);
            return;
        }
    }
}

void
BSL::CMemPool::Init (void)
{
    Create(DEFALUT_SPLICE[7]);
}

void*
BSL::CMemPool::Create (size_t nSize)
{
    MemBlock* pBlock = new MemBlock(nSize);
    EnlargePool(pBlock);
    return pBlock;
}

void
BSL::CMemPool::EnlargePool (void* pBlock)
{
    if(nullptr == pBlock)
        return ;
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_vecpBlocks.push_back(reinterpret_cast<MemBlock*>(pBlock));
}

void
BSL::CMemPool::ReleaseSplice (void* pSplice)
{
    free(pSplice);
}

void
BSL::CMemPool::ReleaseBlock (void* pBlock)
{
    for(auto v:m_vecpBlocks)
    {
        for(auto splice:v->vecpSplices)
        {
            ReleaseSplice(splice);
        }
        delete v;
        v = NULL;
    }
}
