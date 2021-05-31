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

#ifndef OCEANBASE_SQL_OB_EXPR_SUBQUERY_LESS_THAN_H_
#define OCEANBASE_SQL_OB_EXPR_SUBQUERY_LESS_THAN_H_

#include "lib/ob_name_def.h"
#include "sql/engine/expr/ob_expr_operator.h"

namespace oceanbase {
namespace sql {
class ObExprSubQueryLessThan : public ObSubQueryRelationalExpr {
  public:
  explicit ObExprSubQueryLessThan(common::ObIAllocator& alloc);
  virtual ~ObExprSubQueryLessThan();

  virtual int cg_expr(ObExprCGCtx& op_cg_ctx, const ObRawExpr& raw_expr, ObExpr& rt_expr) const override
  {
    return ObSubQueryRelationalExpr::cg_expr(op_cg_ctx, raw_expr, rt_expr);
  }

  private:
  virtual int compare_single_row(const common::ObNewRow& left_row, const common::ObNewRow& right_row,
      common::ObExprCtx& expr_ctx, common::ObObj& result) const;

  private:
  DISALLOW_COPY_AND_ASSIGN(ObExprSubQueryLessThan);
};
}  // namespace sql
}  // namespace oceanbase
#endif  // OCEANBASE_SQL_OB_EXPR_SUBQUERY_LESS_THAN_H_
