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

#ifndef OCEANBASE_SQL_OB_CREATE_DATABASE_RESOLVER_H_
#define OCEANBASE_SQL_OB_CREATE_DATABASE_RESOLVER_H_
#include "sql/resolver/ddl/ob_ddl_resolver.h"

namespace oceanbase {
namespace sql {
class ObCreateDatabaseResolver : public ObDDLResolver {
  static const int64_t IF_NOT_EXIST = 0;
  static const int64_t DBNAME = 1;
  static const int64_t DATABASE_OPTION = 2;
  static const int64_t DATABASE_NODE_COUNT = 3;

  public:
  explicit ObCreateDatabaseResolver(ObResolverParams& params);
  virtual ~ObCreateDatabaseResolver();
  virtual int resolve(const ParseNode& parse_tree);

  private:
  // disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObCreateDatabaseResolver);
};

}  // end namespace sql
}  // end namespace oceanbase

#endif /* OCEANBASE_SQL_OB_CREATE_DATABASE_RESOLVER_H_*/
