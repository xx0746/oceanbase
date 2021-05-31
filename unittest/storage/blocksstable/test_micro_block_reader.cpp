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

#include <gtest/gtest.h>

#define private public
#define protected public
#include "storage/blocksstable/ob_micro_block_writer.h"
#include "storage/blocksstable/ob_micro_block_reader.h"
#include "storage/blocksstable/ob_row_cache.h"
#include "storage/ob_i_store.h"
#include "ob_row_generate.h"
#include "storage/blocksstable/ob_column_map.h"
#include "common/rowkey/ob_rowkey.h"

namespace oceanbase {
using namespace common;
using namespace blocksstable;
using namespace storage;
using namespace share::schema;

#define INVALID_ITERATOR ObIMicroBlockReader::INVALID_ROW_INDEX

namespace unittest {
class TestMicroBlockReader : public ::testing::Test {
  public:
  static const int64_t rowkey_column_count = 2;
  // Every ObObjType from ObTinyIntType to ObHexStringType inclusive.
  // Skip ObNullType and ObExtendType because for external usage, a column type
  // can't be NULL or NOP.
  static const int64_t column_num = ObHexStringType;
  static const int64_t macro_block_size = 2L * 1024 * 1024L;

  public:
  TestMicroBlockReader() : allocator_(ObModIds::TEST)
  {
    reset_row();
  }
  void SetUp();
  virtual void TearDown()
  {}
  static void SetUpTestCase()
  {}
  static void TearDownTestCase()
  {}
  void reset_row()
  {
    row_.row_val_.cells_ = reinterpret_cast<ObObj*>(obj_buf_);
    row_.row_val_.count_ = OB_ROW_MAX_COLUMNS_COUNT;
  }

  protected:
  ObRowGenerate row_generate_;
  ObColumnMap column_map_;
  ObArenaAllocator allocator_;
  ObStoreRow row_;
  char obj_buf_[common::OB_ROW_MAX_COLUMNS_COUNT * sizeof(ObObj)];
};

void TestMicroBlockReader::SetUp()
{
  const int64_t table_id = 3001;
  ObTableSchema table_schema;
  ObColumnSchemaV2 column;
  // init table schema
  table_schema.reset();
  ASSERT_EQ(OB_SUCCESS, table_schema.set_table_name("test_row_writer"));
  table_schema.set_tenant_id(1);
  table_schema.set_tablegroup_id(1);
  table_schema.set_database_id(1);
  table_schema.set_table_id(table_id);
  table_schema.set_rowkey_column_num(rowkey_column_count);
  table_schema.set_max_used_column_id(column_num);
  // init column
  char name[OB_MAX_FILE_NAME_LENGTH];
  memset(name, 0, sizeof(name));
  for (int64_t i = 0; i < column_num; ++i) {
    ObObjType obj_type = static_cast<ObObjType>(i + 1);
    column.reset();
    column.set_table_id(table_id);
    column.set_column_id(i + OB_APP_MIN_COLUMN_ID);
    sprintf(name, "test%020ld", i);
    ASSERT_EQ(OB_SUCCESS, column.set_column_name(name));
    column.set_collation_type(common::CS_TYPE_UTF8MB4_GENERAL_CI);
    column.set_data_type(obj_type);
    if (obj_type == common::ObIntType) {
      column.set_rowkey_position(1);
    } else if (obj_type == common::ObUTinyIntType) {
      column.set_rowkey_position(2);
    } else {
      column.set_rowkey_position(0);
    }
    ASSERT_EQ(OB_SUCCESS, table_schema.add_column(column));
  }
  // init ObRowGenerate
  ASSERT_EQ(OB_SUCCESS, row_generate_.init(table_schema));
}

TEST_F(TestMicroBlockReader, test_success)
{
  int ret = OB_SUCCESS;
  const int64_t test_row_num = 10;
  /*** build micro block buf with ObMicroBlockWriter ***/
  oceanbase::common::ObObj objs[column_num];
  ObStoreRow row;
  ObMicroBlockWriter writer;
  ret = writer.init(macro_block_size, rowkey_column_count, column_num);
  ASSERT_EQ(OB_SUCCESS, ret);
  for (int64_t i = 0; i < test_row_num; ++i) {
    row.row_val_.cells_ = objs;
    row.row_val_.count_ = column_num;
    ASSERT_EQ(OB_SUCCESS, row_generate_.get_next_row(row));
    ASSERT_EQ(OB_SUCCESS, writer.append_row(row));
  }
  char* buf = NULL;
  int64_t size = 0;
  ret = writer.build_block(buf, size);
  ASSERT_EQ(OB_SUCCESS, ret);

  /*** init column_map ***/
  ObArray<ObColDesc> columns;
  ASSERT_EQ(OB_SUCCESS, row_generate_.get_schema().get_column_ids(columns));
  ASSERT_EQ(OB_SUCCESS,
      column_map_.init(allocator_,
          row_generate_.get_schema().get_schema_version(),
          row_generate_.get_schema().get_rowkey_column_num(),
          column_num,
          columns));
  /*** init reader ***/
  ObMicroBlockReader reader;
  ObMicroBlockData block(buf, size);
  ASSERT_EQ(OB_SUCCESS, reader.init(block, &column_map_));

  /*** init row cache ***/
  const int64_t bucket_num = 1024;
  const int64_t max_cache_size = 1024 * 1024 * 512;
  const int64_t block_size = common::OB_MALLOC_BIG_BLOCK_SIZE;
  ObKVGlobalCache::get_instance().init(bucket_num, max_cache_size, block_size);
  ObRowCache cache;
  ASSERT_EQ(OB_SUCCESS, cache.init("row_cache", 1));

  /*** test lower bound ***/
  ObStoreRowkey rowkey;
  rowkey.set_min();
  int64_t iter = ObIMicroBlockReader::INVALID_ROW_INDEX;
  bool equal = false;
  ret = reader.find_bound(rowkey, true, reader.begin(), reader.end(), iter, equal);
  ASSERT_EQ(OB_SUCCESS, ret);
  ASSERT_TRUE(INVALID_ITERATOR != iter);
  ASSERT_EQ(reader.begin(), iter);

  /*** get block row success test ***/
  const ObStoreRow* generated_row = NULL;
  const ObStoreRow* block_row = NULL;
  for (int32_t i = 0; i < test_row_num; ++i) {
    reset_row();
    ASSERT_EQ(OB_SUCCESS, reader.get_row(i, row_));
    block_row = &row_;
    // check exist
    bool exist = false;
    ASSERT_EQ(OB_SUCCESS, row_generate_.check_one_row(*block_row, exist));
    ASSERT_TRUE(exist) << "i: " << i;
    // check obj equal: every obj should be same except not exist row
    ASSERT_EQ(OB_SUCCESS, row_generate_.get_next_row(i, row));
    generated_row = &row;
    for (int64_t j = 0; j < column_num; ++j) {
      ASSERT_TRUE(block_row->row_val_.cells_[j] == generated_row->row_val_.cells_[j])
          << "\n i: " << i << " j: " << j << "\n reader:  " << to_cstring(block_row->row_val_.cells_[j])
          << "\n writer:  " << to_cstring(generated_row->row_val_.cells_[j]);
    }
  }

  /*** test batch get rows ***/
  reader.reset();
  ASSERT_EQ(OB_SUCCESS, reader.init(block, &column_map_));
  // init batch rows
  const int64_t batch_row_capacity = ObIMicroBlockReader::OB_MAX_BATCH_ROW_COUNT;
  ObStoreRow batch_rows[batch_row_capacity];
  void* obj_buf = allocator_.alloc(batch_row_capacity * column_num * sizeof(ObObj));
  for (int i = 0; i < batch_row_capacity; ++i) {
    batch_rows[i].row_val_.cells_ = reinterpret_cast<ObObj*>(obj_buf) + column_num * i;
    batch_rows[i].row_val_.count_ = column_num;
  }
  int64_t batch_count = 0;
  ASSERT_EQ(OB_SUCCESS, reader.get_rows(0, test_row_num, batch_row_capacity, batch_rows, batch_count));
  ASSERT_LE(batch_count, test_row_num);
  for (int64_t i = 0; i < batch_count; ++i) {
    bool exist = false;
    const ObStoreRow& cur_row = batch_rows[i];
    ASSERT_EQ(OB_SUCCESS, row_generate_.check_one_row(cur_row, exist));
    ASSERT_TRUE(exist) << "i: " << i;
    // every obj should equal
    ASSERT_EQ(OB_SUCCESS, row_generate_.get_next_row(i, row));
    for (int64_t j = 0; j < column_num; ++j) {
      ASSERT_TRUE(row.row_val_.cells_[j] == cur_row.row_val_.cells_[j])
          << "\n i: " << i << " j: " << j << "\n writer:  " << to_cstring(row.row_val_.cells_[j])
          << "\n reader:  " << to_cstring(cur_row.row_val_.cells_[j]);
    }
  }
  ObKVGlobalCache::get_instance().destroy();
}

TEST_F(TestMicroBlockReader, not_init)
{
  int ret = OB_SUCCESS;
  ObMicroBlockReader reader;
  ObStoreRowkey rowkey;
  rowkey.set_min();
  int64_t iter = INVALID_ITERATOR;
  bool equal = false;
  ret = reader.find_bound(rowkey, true, reader.begin(), reader.end(), iter, equal);
  ASSERT_EQ(OB_NOT_INIT, ret);
  reset_row();
  ASSERT_EQ(OB_NOT_INIT, reader.get_row(iter, row_));
}

/*TEST_F(TestMicroBlockReader, misc_function)
{
  int ret = OB_SUCCESS;
  const int64_t test_row_num = 10;
  oceanbase::common::ObObj objs[column_num];
  ObStoreRow row;
  //ObMicroBlockWriter init the reader buf
  ObMicroBlockWriter writer;
  ret = writer.init(macro_block_size, column_num, rowkey_column_count);
  ASSERT_EQ(OB_SUCCESS, ret);
  for(int64_t i = 0; i < test_row_num; ++i){
    row.row_val_.cells_ = objs;
    row.row_val_.count_ = column_num;
    ASSERT_EQ(OB_SUCCESS, row_generate_.get_next_row(row));
    ASSERT_EQ(OB_SUCCESS, writer.append_row(row));
  }
  char *buf = NULL;
  int64_t size = 0;
  ret = writer.build_block(buf, size);
  ASSERT_EQ(OB_SUCCESS, ret);
  ObMicroBlockReader reader;
  ObMicroBlockData block(buf, size);
  //column_map invalid
  ASSERT_EQ(OB_INVALID_ARGUMENT, reader.init(block, column_map_));
  //init column_map
  ASSERT_EQ(OB_SUCCESS, column_map_.init(column_num, rowkey_column_count));
  for (int64_t i = 0; i < column_num; ++i){
      ObObjType obj_type = row.row_val_.cells_[i].get_type();
      ObObjMeta obj_meta;
      obj_meta.set_type(obj_type);
      if (ObVarcharType == obj_type || ObCharType == obj_type || ObExtendType == obj_type) {
        obj_meta.set_collation_type(CS_TYPE_UTF8MB4_GENERAL_CI);
      }
      ASSERT_EQ(OB_SUCCESS, column_map_.add_outside_column(obj_meta, obj_meta, i));
  }
  //init reader
  ret = reader.init(block, column_map_);
  ASSERT_EQ(OB_SUCCESS, ret);
  //init twice
  ASSERT_EQ(OB_INIT_TWICE, reader.init(block, column_map_));

  //BlockData to_cstring
  ObMicroBlockData  block_data;
  const char *out = to_cstring(block_data);
  ASSERT_TRUE(NULL != out);
  //index is NULL
  ObStoreRowkey rowkey;
  ObMicroBlockReader::const_iterator iter = NULL;
  ObStoreRowkey rowkey2;
  ret = reader.get_rowkey(iter, rowkey2);
  ASSERT_EQ(OB_INVALID_ARGUMENT, ret);
  ObRowCacheValue value;
  ASSERT_EQ(OB_INVALID_ARGUMENT, reader.get_cached_value(iter, value, MacroBlockId(0, 1, 0, 2)));
  const ObStoreRow *read_row;
  ASSERT_EQ(OB_INVALID_ARGUMENT, reader.get_row(iter, read_row));
  ASSERT_TRUE(NULL == read_row);
  //get_cached_value where value is NULL
  ASSERT_EQ(OB_INVALID_ARGUMENT, reader.get_cached_row(rowkey, value, column_map_, read_row));
  ASSERT_TRUE(NULL == read_row);
  ASSERT_EQ(OB_INVALID_ARGUMENT, reader.get_cached_row(rowkey, value, column_map_, read_row));
  //get_row error
  const int32_t begin = 10;
  iter = &begin;
  //ASSERT_EQ(OB_INVALID_ARGUMENT, reader.get_row(iter, read_row));
  //get_rowkey error
  ASSERT_NE(OB_SUCCESS, reader.get_rowkey(iter, rowkey2));
}*/

/*
TEST_F(TestMicroBlockReader, test_percise_compare)
{
  int ret = OB_SUCCESS;
  const int64_t test_row_num = 10;
  oceanbase::common::ObObj objs[column_num];
  ObStoreRow row;
  //ObMicroBlockWriter init the reader buf
  ObMicroBlockWriter writer;
  ret = writer.init(macro_block_size, column_num, rowkey_column_count);
  ASSERT_EQ(OB_SUCCESS, ret);
  for(int64_t i = 0; i < test_row_num; ++i){
    row.row_val_.cells_ = objs;
    row.row_val_.count_ = column_num;
    ASSERT_EQ(OB_SUCCESS, row_generate_.get_next_row(row));
    if(i == test_row_num - 1){//row not exist test
      row.flag_ = ObActionFlag::OP_ROW_DOES_NOT_EXIST;
    }
    ASSERT_EQ(OB_SUCCESS, writer.append_row(row));
  }
  char *buf = NULL;
  int64_t size = 0;
  ret = writer.build_block(buf, size);
  ASSERT_EQ(OB_SUCCESS, ret);
  //init column_map
  ASSERT_EQ(OB_SUCCESS, column_map_.init(column_num, rowkey_column_count, column_num, allocator_));
  ObArray<ObColDesc> columns;
  ret = row_generate_.get_schema().get_column_ids(columns);
  ASSERT_EQ(OB_SUCCESS, ret);
  for (int64_t i = 0; i < columns.count(); ++i){
      ObObjMeta obj_meta  = columns.at(i).col_type_;;
      ASSERT_EQ(OB_SUCCESS, column_map_.add_outside_column(obj_meta, obj_meta, i));
  }
  //init reader
  ObMicroBlockReader reader;
  ObMicroBlockData block(buf, size);
  ret = reader.init(block, column_map_);
  ASSERT_EQ(OB_SUCCESS, ret);

  ObMicroBlockReader::Iterator iter = INVALID_ITERATOR;
  //get cached row success test
  ObStoreRowkey rowkey_rel(objs, rowkey_column_count);
  ASSERT_EQ(OB_SUCCESS, row_generate_.get_next_row(2, row));
  bool equal = false;
  ret = reader.lower_bound(rowkey_rel, iter, equal);
  ASSERT_EQ(OB_SUCCESS, ret);
  ASSERT_TRUE(INVALID_ITERATOR != iter);
  ASSERT_TRUE(equal);
  reset_row();
  ret = reader.get_row(iter, row_);
  ASSERT_EQ(OB_SUCCESS, ret);


  equal = false;
  objs[1].set_null();
  ret = reader.lower_bound(rowkey_rel, iter, equal);
  ASSERT_EQ(OB_SUCCESS, ret);
  ASSERT_TRUE(INVALID_ITERATOR != iter);
  ASSERT_FALSE(equal);
  reset_row();
  ret = reader.get_row(iter, row_);
  ASSERT_EQ(OB_SUCCESS, ret);
}
*/

}  // end namespace unittest
}  // end namespace oceanbase

int main(int argc, char** argv)
{
  oceanbase::common::ObLogger::get_logger().set_log_level("INFO");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
