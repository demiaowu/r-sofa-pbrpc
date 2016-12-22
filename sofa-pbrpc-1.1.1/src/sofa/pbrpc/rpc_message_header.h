// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_MESSAGE_HEADER_H_
#define _SOFA_PBRPC_RPC_MESSAGE_HEADER_H_

#include <sofa/pbrpc/common_internal.h>

namespace sofa {
namespace pbrpc {

// Magic string "SOFA" in little endian.
#define SOFA_RPC_MAGIC 1095126867u

// total 24 bytes
// 每个Rpc消息都有一个固定长度的消息头
struct RpcMessageHeader {
    union {
        char    magic_str[4];
        uint32  magic_str_value;
    };                    // 4 bytes	//  魔术字符串“SOFA”
    int32   meta_size;    // 4 bytes	//	RpcMeta的数据总长度。RpcMeta是protobuf格式的Message。
	//	Data的数据总长度。Data是protobuf格式的Request Message或者Response Message。
    int64   data_size;    // 8 bytes	
	//	message_size = meta_size + data_size。message_size实际为冗余信息，
	// 	主要用作meta_size和data_size的一致性检查。
    int64   message_size; // 8 bytes: message_size = meta_size + data_size, for check

    RpcMessageHeader()
        : magic_str_value(SOFA_RPC_MAGIC)
        , meta_size(0), data_size(0), message_size(0) {}

    bool CheckMagicString() const
    {
        return magic_str_value == SOFA_RPC_MAGIC;
    }
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_MESSAGE_HEADER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
