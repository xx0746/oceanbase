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

#ifndef OCEANBASE_SQL_RESOLVER_CMD_BOOTSTRAP_STMT_
#define OCEANBASE_SQL_RESOLVER_CMD_BOOTSTRAP_STMT_

#include "share/ob_rpc_struct.h"
#include "sql/resolver/cmd/ob_system_cmd_stmt.h"

namespace oceanbase {
namespace sql {
class ObBootstrapStmt : public ObSystemCmdStmt {
  public:
  ObBootstrapStmt() : ObSystemCmdStmt(stmt::T_BOOTSTRAP), bootstrap_arg_()
  {}
  explicit ObBootstrapStmt(common::ObIAllocator* name_pool) : ObSystemCmdStmt(name_pool, stmt::T_BOOTSTRAP)
  {}
  virtual ~ObBootstrapStmt()
  {}

  void set_cluster_type(common::ObClusterType type)
  {
    bootstrap_arg_.cluster_type_ = type;
  }
  int set_primary_rs_list(const common::ObIArray<common::ObAddr>& rs_list)
  {
    return bootstrap_arg_.primary_rs_list_.assign(rs_list);
  }
  common::ObClusterType get_cluster_type() const
  {
    return bootstrap_arg_.cluster_type_;
  }
  obrpc::ObServerInfoList& get_server_info_list()
  {
    return bootstrap_arg_.server_list_;
  }
  void set_primary_cluster_id(const int64_t primary_cluster_id)
  {
    bootstrap_arg_.primary_cluster_id_ = primary_cluster_id;
  }
  TO_STRING_KV(N_STMT_TYPE, ((int)stmt_type_), K_(bootstrap_arg));

  public:
  obrpc::ObBootstrapArg bootstrap_arg_;
  DISALLOW_COPY_AND_ASSIGN(ObBootstrapStmt);
};

}  // namespace sql
}  // namespace oceanbase
#endif /* OCEANBASE_SQL_RESOLVER_CMD_OB_BOOTSTRAP_STMT_ */
