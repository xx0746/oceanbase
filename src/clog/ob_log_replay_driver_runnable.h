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

#ifndef OCEANBASE_CLOG_OB_LOG_REPLAY_DRIVER_RUNNABLE_
#define OCEANBASE_CLOG_OB_LOG_REPLAY_DRIVER_RUNNABLE_

#include "ob_log_define.h"
#include "share/ob_thread_pool.h"

namespace oceanbase {
namespace storage {
class ObPartitionService;
}
namespace clog {
class ObLogReplayDriverRunnable : public share::ObThreadPool {
  public:
  ObLogReplayDriverRunnable();
  virtual ~ObLogReplayDriverRunnable();

  public:
  int init(storage::ObPartitionService* partition_service);
  void destroy();
  void run1();

  private:
  void try_replay_loop();

  storage::ObPartitionService* partition_service_;
  bool can_start_service_;
  bool is_inited_;

  private:
  DISALLOW_COPY_AND_ASSIGN(ObLogReplayDriverRunnable);
};

}  // namespace clog
}  // namespace oceanbase

#endif  // OCEANBASE_CLOG_OB_LOG_Replay_DRIVER_RUNNABLE_H_
