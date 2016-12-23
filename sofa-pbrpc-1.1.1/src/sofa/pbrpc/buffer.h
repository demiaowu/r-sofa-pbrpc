// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_BUFFER_H_
#define _SOFA_PBRPC_BUFFER_H_

#include <deque>
#include <string>

#include <google/protobuf/io/zero_copy_stream.h>

#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/buf_handle.h>

namespace sofa {
namespace pbrpc {

// Defined in this file.
class ReadBuffer;
typedef sofa::pbrpc::shared_ptr<ReadBuffer> ReadBufferPtr;
class WriteBuffer;
typedef sofa::pbrpc::shared_ptr<WriteBuffer> WriteBufferPtr;

typedef std::deque<BufHandle> BufHandleList;
typedef std::deque<BufHandle>::iterator BufHandleListIterator;
typedef std::deque<BufHandle>::reverse_iterator BufHandleListReverseIterator;

//	这里面将ReadBuffer和WriteBuffer分别继承ZeroCopyInputStream和ZeroCopyOutputStream
// 	是为了让google protobuf的接口也可以操作这两个buffer，如将message序列化到buffer中，

class ReadBuffer : public google::protobuf::io::ZeroCopyInputStream
{
public:
    ReadBuffer();
    virtual ~ReadBuffer();

    // Append a buf handle to the buffer.
    //
    // Preconditions:
    // * No method Next(), Backup() or Skip() have been called before.
    // * The size of "buf_handle" should be greater than 0.
    // * For the first one, size of "buf_handle" should be greater than 0.
    // * For the second one, "read_buffer" should not be NULL.
    void Append(const BufHandle& buf_handle);
    void Append(const ReadBuffer* read_buffer);

    // Get the total byte count of the buffer.
    // 总的字节数
    int64 TotalCount() const;

    // Get the block count occupied by the buffer.
	// 被占用块计数
    int BlockCount() const;

    // Get the total block size occupied by the buffer.
    // 总的占用字节数
    int64 TotalBlockSize() const;

    // Trans buffer to string.
    std::string ToString() const;

    // implements ZeroCopyInputStream ----------------------------------
    bool Next(const void** data, int* size);
    void BackUp(int count);
    bool Skip(int count);
    int64 ByteCount() const;

private:
    BufHandleList _buf_list;
    int64 _total_block_size; // total block size in the buffer
    int64 _total_bytes; // total bytes in the buffer
    BufHandleListIterator _cur_it;
    int _cur_pos;
    int _last_bytes; // last read bytes
    int64 _read_bytes; // total read bytes

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(ReadBuffer);
}; // class ReadBuffer

class WriteBuffer : public google::protobuf::io::ZeroCopyOutputStream
{
public:
    WriteBuffer();
    virtual ~WriteBuffer();

    // Get the total capacity of the buffer.
    int64 TotalCapacity() const;

    // Get the block count occupied by the buffer.
	// 块计数
    int BlockCount() const;

    // Get the total block size occupied by the buffer.
    int64 TotalBlockSize() const;

    // Swap out data from this output stream and append to the input stream "is".
    // The "is" should not be null.
    //
    // Postconditions:
    // * This buffer is cleared afterward, just as a new output buffer.
    void SwapOut(ReadBuffer* is);

    // Reserve some space in the buffer.
    // If succeed, return the head position of reserved space.
    // If failed, return -1.
    //
    // Preconditions:
    // * "count" > 0
    int64 Reserve(int count);

    // Set data in the buffer, start from "pos".
    //
    // Preconditions:
    // * "pos" >= 0
    // * "data" != NULL && "size" > 0
    // * "pos" + "size" <= ByteCount()
    void SetData(int64 pos, const char* data, int size);

    // implements ZeroCopyOutputStream ---------------------------------
    bool Next(void** data, int* size);
	// 回退count个字节
    void BackUp(int count);
	// 写入字节总数
    int64 ByteCount() const;

    // Append string to the buffer
    // If succeed, return true
    // If failed, return false
    bool Append(const std::string& data);
    bool Append(const char* data, int size);

private:
    // Add a new block to the end of the buffer.
	// 给写buffer增加一个block
    bool Extend();

private:
	// 对buffer来说，都是通过管理block句柄来管理数据block
    BufHandleList _buf_list;									// block句柄列表
    int64 _total_block_size; // total block size in the buffer	// block size包含隐藏头 
    int64 _total_capacity; // total capacity in the buffer		// 容量不包含隐藏头
	// // 最后一次写入到最后一个block的字节数, 
    int _last_bytes; // last write bytes						
    int64 _write_bytes; // total write bytes					// 总的写入字节

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(WriteBuffer);
}; // class WriteBuffer

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BUFFER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
