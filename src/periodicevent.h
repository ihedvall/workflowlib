/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include "workflow/ievent.h"
#include <thread>
#include <mutex>
#include <atomic>

namespace workflow {

class PeriodicEvent : public IEvent {
 public:
  PeriodicEvent();
  explicit PeriodicEvent(const IEvent& source);
  void Init() override;
  void Exit() override;
 private:
  std::thread working_thread_;
  std::atomic<bool> stop_thread_ = true;
  int64_t next_time_ = 0;
  int64_t step_time_ = 0;
  void PeriodicTask();
};

}  // namespace workflow
