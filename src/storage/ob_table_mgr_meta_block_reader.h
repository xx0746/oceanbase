/**
 * Copyright (c) 2021 OceanBase
 * OceanBase CE is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan PubL v2.
 * You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#ifndef SRC_STORAGE_OB_TABLE_MGR_META_BLOCK_READER_H_
#define SRC_STORAGE_OB_TABLE_MGR_META_BLOCK_READER_H_

#include "blocksstable/ob_meta_block_reader.h"
#include "lib/allocator/page_arena.h"

namespace oceanbase {
namespace storage {
class ObTableMgrMetaBlockReader : public blocksstable::ObMetaBlockReader {
  public:
  ObTableMgrMetaBlockReader();
  virtual ~ObTableMgrMetaBlockReader();

  protected:
  virtual int parse(const blocksstable::ObMacroBlockCommonHeader& common_header,
      const blocksstable::ObLinkedMacroBlockHeader& linked_header, const char* buf, const int64_t buf_len);

  private:
  common::ObArenaAllocator allocator_;
  char* read_buf_;
  int64_t offset_;
  int64_t buf_size_;
  DISALLOW_COPY_AND_ASSIGN(ObTableMgrMetaBlockReader);
};

}  // namespace storage
}  // namespace oceanbase

#endif /* SRC_STORAGE_OB_TABLE_MGR_META_BLOCK_READER_H_ */
