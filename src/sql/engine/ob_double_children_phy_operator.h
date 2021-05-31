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

#ifndef _OB_DOUBLE_CHILDREN_PHY_OPERATOR_H
#define _OB_DOUBLE_CHILDREN_PHY_OPERATOR_H

#include "sql/engine/ob_phy_operator.h"

namespace oceanbase {
namespace sql {
class ObDoubleChildrenPhyOperator : public ObPhyOperator {
  public:
  explicit ObDoubleChildrenPhyOperator(common::ObIAllocator& alloc);
  virtual ~ObDoubleChildrenPhyOperator();
  /// Just two children are allowed to set
  virtual int set_child(int32_t child_idx, ObPhyOperator& child_operator);
  virtual ObPhyOperator* get_child(int32_t child_idx) const;
  virtual int32_t get_child_num() const
  {
    return 2;
  }
  virtual void reset();
  virtual void reuse();
  virtual int accept(ObPhyOperatorVisitor& visitor) const;

  private:
  // disallow copy
  DISALLOW_COPY_AND_ASSIGN(ObDoubleChildrenPhyOperator);

  protected:
  // data members
  ObPhyOperator* left_op_;
  ObPhyOperator* right_op_;
};
}  // end namespace sql
}  // end namespace oceanbase

#endif /* _OB_DOUBLE_CHILDREN_PHY_OPERATOR_H */
