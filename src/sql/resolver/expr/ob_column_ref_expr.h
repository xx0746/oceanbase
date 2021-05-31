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

#ifndef OB_COLUMN_REF_EXPR_H
#define OB_COLUMN_REF_EXPR_H

#include "sql/resolver/expr/ob_expr.h"

namespace oceanbase {
namespace jit {
namespace expr {

class ObColumnRefExpr : virtual public ObExpr {
  public:
  ObColumnRefExpr(ObItemType expr_type = T_INVALID) : ObExpr(expr_type)
  {
    set_expr_class(EXPR_COLUMN_REF);
  }

  ObColumnRefExpr(common::ObIAllocator& alloc) : ObExpr(alloc)
  {
    set_expr_class(EXPR_COLUMN_REF);
  }

  virtual ~ObColumnRefExpr()
  {}
};

}  // namespace expr
}  // namespace jit
}  // namespace oceanbase

#endif
