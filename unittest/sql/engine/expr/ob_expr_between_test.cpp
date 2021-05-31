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

#include "sql/engine/expr/ob_expr_between.h"
#include "ob_expr_test_utils.h"
#include <gtest/gtest.h>
using namespace oceanbase::common;
using namespace oceanbase::sql;

class ObExprBetweenTest : public ::testing::Test {
  public:
  ObExprBetweenTest();
  virtual ~ObExprBetweenTest();
  virtual void SetUp();
  virtual void TearDown();

  private:
  // disallow copy
  ObExprBetweenTest(const ObExprBetweenTest& other);
  ObExprBetweenTest& operator=(const ObExprBetweenTest& other);

  protected:
  // data members
};

ObExprBetweenTest::ObExprBetweenTest()
{}

ObExprBetweenTest::~ObExprBetweenTest()
{}

void ObExprBetweenTest::SetUp()
{}

void ObExprBetweenTest::TearDown()
{}

#define T(t1, v1, t2, v2, t3, v3, res) COMPARE3_EXPECT(ObExprBetween, &buf, calc_result3, t1, v1, t2, v2, t3, v3, res)
TEST_F(ObExprBetweenTest, basic_test)
{
  // special value
  ObExprStringBuf buf;
  T(min, 0, min, 0, min, 0, MY_TRUE);
  T(min, 0, min, 0, null, 0, MY_NULL);
  T(min, 0, min, 0, max, 0, MY_TRUE);
  T(min, 0, null, 0, min, 0, MY_NULL);
  T(min, 0, null, 0, null, 0, MY_NULL);
  T(min, 0, null, 0, max, 0, MY_NULL);
  T(min, 0, max, 0, min, 0, MY_FALSE);
  T(min, 0, max, 0, null, 0, MY_FALSE);
  T(min, 0, max, 0, max, 0, MY_FALSE);
  T(null, 0, min, 0, min, 0, MY_NULL);
  T(null, 0, min, 0, null, 0, MY_NULL);
  T(null, 0, min, 0, max, 0, MY_NULL);
  T(null, 0, null, 0, min, 0, MY_NULL);
  T(null, 0, null, 0, null, 0, MY_NULL);
  T(null, 0, null, 0, max, 0, MY_NULL);
  T(null, 0, max, 0, min, 0, MY_NULL);
  T(null, 0, max, 0, null, 0, MY_NULL);
  T(null, 0, max, 0, max, 0, MY_NULL);
  T(max, 0, min, 0, min, 0, MY_FALSE);
  T(max, 0, min, 0, null, 0, MY_NULL);
  T(max, 0, min, 0, max, 0, MY_TRUE);
  T(max, 0, null, 0, min, 0, MY_FALSE);
  T(max, 0, null, 0, null, 0, MY_NULL);
  T(max, 0, null, 0, max, 0, MY_NULL);
  T(max, 0, max, 0, min, 0, MY_FALSE);
  T(max, 0, max, 0, null, 0, MY_NULL);
  T(max, 0, max, 0, max, 0, MY_TRUE);

  // int
  T(int, -1, int, 0, int, 2, MY_FALSE);
  T(int, 0, int, 0, int, 2, MY_TRUE);
  T(int, 1, int, 0, int, 2, MY_TRUE);
  T(int, 2, int, 0, int, 2, MY_TRUE);
  T(int, 3, int, 0, int, 2, MY_FALSE);
  T(int, -1, int, 0, int, 0, MY_FALSE);
  T(int, 0, int, 0, int, 0, MY_TRUE);
  T(int, 1, int, 0, int, 0, MY_FALSE);
  T(int, 0, int, 2, int, 0, MY_FALSE);
  T(int, 2, int, 2, int, 0, MY_FALSE);

  // int vs special
  T(int, 0, int, 1, null, 0, MY_FALSE);
  T(int, 1, int, 0, null, 0, MY_NULL);
  T(int, 0, null, 0, int, 2, MY_NULL);
  T(null, 0, int, 0, int, 2, MY_NULL);
  T(min, 0, int, 0, int, 2, MY_FALSE);
  T(max, 0, int, 0, int, 2, MY_FALSE);
  T(null, 0, min, 0, int, 2, MY_NULL);
  T(null, 0, min, 0, max, 0, MY_NULL);
  T(min, 0, min, 0, int, 2, MY_TRUE);
  T(max, 0, int, 0, max, 0, MY_TRUE);

  // int vs varchar
  T(int, -1, varchar, "0", int, 2, MY_FALSE);
  T(int, 0, varchar, "0", int, 2, MY_TRUE);
  T(int, 1, varchar, "0", int, 2, MY_TRUE);
  T(int, 2, varchar, "0", int, 2, MY_TRUE);
  T(int, 3, varchar, "0", int, 2, MY_FALSE);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
