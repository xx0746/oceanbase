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

#ifndef OB_SSTABLE_ROW_GETTER_H_
#define OB_SSTABLE_ROW_GETTER_H_

#include "ob_sstable_row_iterator.h"

namespace oceanbase {
namespace storage {

class ObSSTableRowGetter : public ObSSTableRowIterator {
  public:
  ObSSTableRowGetter();
  virtual ~ObSSTableRowGetter();
  virtual void reset();
  virtual void reuse() override;

  protected:
  virtual int get_handle_cnt(const void* query_range, int64_t& read_handle_cnt, int64_t& micro_handle_cnt);
  virtual int prefetch_read_handle(ObSSTableReadHandle& read_handle);
  virtual int fetch_row(ObSSTableReadHandle& read_handle, const ObStoreRow*& store_row);
  virtual int get_range_count(const void* query_range, int64_t& range_count) const;

  protected:
  bool has_prefetched_;
};

}  // namespace storage
}  // namespace oceanbase

#endif /* OB_SSTABLE_ROW_GETTER_H_ */
