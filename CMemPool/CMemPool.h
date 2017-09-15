/*
 * CMemPool.h
 *
 *  Created on: 2017年9月13日
 *      Author: michael
 */

#ifndef CMEMPOOL_H_
#define CMEMPOOL_H_
/**
 * c header
 */
#include <stdint.h>
#include <stdlib.h>
/**
 * c++ header
 */
#include <atomic>
#include <vector>
#include <mutex>

const static int NBLOCKSPLICES = 2;
namespace BSL
{

    const static int DEFALUT_SPLICE[8] = {16,32,64,128,256,512,1024,2048};

    struct MemBlock{
        void* pStart;///<内存块开始地址
        void* pEnd;///<内存块结束地址
        const uint8_t nBlockSplices;///内存块分片数量
        uint16_t nSpliceSize;///<内存块分片大小
        std::atomic_int nFetchIndex;///<分配内存片原子计数
        std::atomic_int nReturnIndex;///<回收内存片原子计数
        std::vector<void*> vecpSplices;///<管理分片容器
        MemBlock(size_t nSize);
        void* FetchSplice(size_t nSize);
        void RecoverSplice(void* pSplice);
    };

    class CMemPool
    {
    public:
        CMemPool();
        ~CMemPool();
    public:
        /**
         * @brief GetMem 获取内存块
         * @param nSize 大小
         * @return void* 内存指针
         */
        void* alloc(size_t nSize);
        /**
         * @brief ReturnMem 归还内存块
         * @param pSplice 内存指针
         */
        void dealloc(void* pSplice);
        /**
         * @brief ReleaseBlock 释放内存块
         * @param pBlock 内存指针
         */
        void ReleaseBlock(void* pBlock);
    private:

        void Init(void);
        /**
         * @brief Create 创建指定大小的内存块
         * @param nSize 指定的大小
         */
        void* Create(size_t nSize);
        /**
         * @brief EnlargePool 扩充内存池容量
         * @param pBlock 内存指针
         */
        void EnlargePool(void* pBlock);
        /**
         * @brief ReleaseSplice 释放内存片
         * @param pSplice 内存指针
         */
        void ReleaseSplice(void* pSplice);


    private:
        std::mutex m_Mutex;
        std::vector<MemBlock*> m_vecpBlocks;
    };

}//namespace BSL



#endif /* CMEMPOOL_H_ */
