// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_TRAN_BUF_POOL_H_
#define _SOFA_PBRPC_TRAN_BUF_POOL_H_

#include <sofa/pbrpc/common_internal.h>

#ifndef SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE
// base_block_size = 1K
#define SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE (1024u)
#endif

#ifndef SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR
// max_block_size = 1024<<5 = 32K
#define SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR 5
#endif

namespace sofa {
namespace pbrpc {

// Reference counted tran buf pool.
//
//   Block = BlockSize(int) + RefCount(int) + Data(char[capacity])
//
// Thread safe.
// 缓冲区池
class TranBufPool
{
public:
    // Allocate a block.  Return NULL if failed.
    //
    //   block_size = SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE << factor
    //
    // Postconditions:
    // * If succeed, the reference count of the block is equal to 1.
	// 分配一个数据块
    // 根据factor分配，factor的值为std::min( SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR/* 5 */, (int)_buf_list.size() )
    //      也就是factor的取值范围为 [0, 1, 2, 3, 4, 5]
    // 也就是block的大小为1k, 2k, 4k, 8k, 16k, 32k
    //
    // 注意每一个block都有一个隐藏头，占用两个字节
    //  block的结构如下：
    //      | block size - int 占4个字节 | 引用计数 - int 占4个字节 | p指针，真正数据存放的位置 |
    inline static void * malloc(int factor = 0)
    {
        void * p = ::malloc(SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE /* 1k*/ << factor);
        if (p != NULL)
        {
            // 初始化block的隐藏头，隐藏头的大小为两个int
            *(reinterpret_cast<int*>(p)) = SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE << factor; // block size
            *(reinterpret_cast<int*>(p) + 1) = 1;                                         // 引用计数  
            p = reinterpret_cast<int*>(p) + 2;
        }
        return p;
    }

    // Return block size pointed by "p", including the hidden header.
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    // 块大小: 包含隐藏头
    inline static int block_size(void * p)
    {
        return *(reinterpret_cast<int*>(p) - 2);
    }

    // Return capacity size pointed by "p", not including the hidden header.
    //
    //   capacity = block_size - sizeof(int) * 2
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    // 容量，block可以存放数据的容量，应该减去隐藏头大小
    inline static int capacity(void * p)
    {
        return *(reinterpret_cast<int*>(p) - 2) - sizeof(int) * 2;
    }

    // Increase the reference count of the block.
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    // block引用计数加1
    inline static void add_ref(void * p)
    {
        sofa::pbrpc::atomic_inc(reinterpret_cast<int*>(p) - 1);
    }

    // Decrease the reference count of the block.  If the reference count equals
    // to 0 afterward, then put the block back to the free list
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    // 当引用计数为零，则释放这个数据块
    inline static void free(void * p)
    {
        if (sofa::pbrpc::atomic_dec_ret_old(reinterpret_cast<int*>(p) - 1) == 1)
        {
            ::free(reinterpret_cast<int*>(p) - 2);
        }
    }
}; // class TranBufPool

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_TRAN_BUF_POOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
