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

#ifndef OCEANBASE_OBSERVER_VIRTUAL_TABLE_OB_TABLE_PRIVILEGES_
#define OCEANBASE_OBSERVER_VIRTUAL_TABLE_OB_TABLE_PRIVILEGES_

#include "share/ob_virtual_table_scanner_iterator.h"
#include "share/schema/ob_priv_type.h"

namespace oceanbase {
namespace sql {
class ObSQLSessionInfo;
}
namespace share {
namespace schema {
class ObTablePriv;
}
}  // namespace share
namespace observer {
class ObInfoSchemaTablePrivilegesTable : public common::ObVirtualTableScannerIterator {
  public:
  class StaticInit {
    public:
    StaticInit();
  };
  friend class ObInfoSchemaTablePrivilegesTable::StaticInit;

  ObInfoSchemaTablePrivilegesTable();
  virtual ~ObInfoSchemaTablePrivilegesTable();

  virtual void reset();
  virtual int inner_get_next_row(common::ObNewRow*& row);

  inline void set_tenant_id(uint64_t tenant_id)
  {
    tenant_id_ = tenant_id;
  }
  inline void set_user_id(uint64_t user_id)
  {
    user_id_ = user_id;
  }

  private:
  enum TABLE_PRIVS_COLUMN {
    GRANTEE = 16,
    TABLE_CATALOG,
    TABLE_SCHEMA,
    TABLE_NAME,
    PRIVILEGE_TYPE,
    IS_GRANTABLE,
    MAX_TABLE_PRIVS_COLUMN
  };
  enum {
    MAX_COL_COUNT = 6,
    USERNAME_AUX_LEN = 6  // "''@''" + '\0'
  };

  int get_table_privs(const uint64_t tenant_id, const uint64_t user_id,
      common::ObArray<const share::schema::ObTablePriv*>& table_privs);
  int get_user_name_from_table_priv(
      const share::schema::ObTablePriv* table_priv, common::ObString& user_name, common::ObString& host_name);
  int fill_row_with_table_priv(const share::schema::ObTablePriv* table_priv);

  static const char* priv_type_strs[OB_PRIV_MAX_SHIFT + 1];
  uint64_t tenant_id_;
  uint64_t user_id_;

  DISALLOW_COPY_AND_ASSIGN(ObInfoSchemaTablePrivilegesTable);
};
}  // namespace observer
}  // namespace oceanbase
#endif /* OCEANBASE_OBSERVER_VIRTUAL_TABLE_OB_TABLE_PRIVILEGES_ */
