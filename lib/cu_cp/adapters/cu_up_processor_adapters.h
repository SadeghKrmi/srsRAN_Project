/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../task_schedulers/cu_up_task_scheduler.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_up_processor.h"

namespace srsgnb {
namespace srs_cu_cp {

class cu_up_processor_to_cu_cp_task_scheduler : public cu_up_processor_task_scheduler
{
public:
  cu_up_processor_to_cu_cp_task_scheduler() {}

  void connect_cu_cp(cu_up_task_scheduler& cu_cp_task_sched_) { cu_cp_task_sched = &cu_cp_task_sched_; }

  void schedule_async_task(cu_up_index_t cu_up_index, async_task<void>&& task) override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    cu_cp_task_sched->handle_cu_up_async_task(cu_up_index, std::move(task));
  }

  unique_timer make_unique_timer() override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->make_unique_timer();
  }
  timer_manager& get_timer_manager() override
  {
    srsgnb_assert(cu_cp_task_sched != nullptr, "CU-CP task scheduler handler must not be nullptr");
    return cu_cp_task_sched->get_timer_manager();
  }

private:
  cu_up_task_scheduler* cu_cp_task_sched = nullptr;
};

/// Adapter between CU-UP Processor and CU-CP, to handle CU-UP specific procedure outcomes (e.g. CU-CP E1 Setup Failure)
class cu_up_processor_cu_cp_adapter : public cu_up_processor_cu_up_management_notifier
{
public:
  void connect_cu_cp(cu_cp_cu_up_handler& cu_cp_mng_) { cu_up_handler = &cu_cp_mng_; }

  void on_cu_up_remove_request_received(const cu_up_index_t cu_up_index) override
  {
    srsgnb_assert(cu_up_handler != nullptr, "CU-UP handler must not be nullptr");
    cu_up_handler->handle_cu_up_remove_request(cu_up_index);
  }

private:
  cu_cp_cu_up_handler* cu_up_handler = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsgnb