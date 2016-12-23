// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_BUF_HANDLE_H_
#define _SOFA_PBRPC_BUF_HANDLE_H_

namespace sofa {
namespace pbrpc {
// buffer的块句柄
struct BufHandle
{
    char* data; // block header	块数据指针——在句柄只包含了数据指针
    int   size; // data size	block中可读/可写数据的大小
	// 同一个东西，在不同的地方所代表的含义不一样，可以使用union实现不同地方使用不同的名称
    union {
        int capacity; // block capacity, used by WriteBuffer块的容量——用于写buffer，表明这个块有多少空间可写
        int offset;   // start position in the block, used by ReadBuffer	块的开始偏移，用于读buffer
    };

    BufHandle(char* _data, int _capacity)
        : data(_data)
        , size(0)
        , capacity(_capacity) {}

    BufHandle(char* _data, int _size, int _offset)
        : data(_data)
        , size(_size)
        , offset(_offset) {}
}; // class BufHandle

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BUF_HANDLE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
