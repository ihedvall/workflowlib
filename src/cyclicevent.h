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

class CyclicEvent : public IEvent {
 public:
  CyclicEvent();
  explicit CyclicEvent(const IEvent& source);
  void Init() override;
  void Exit() override;
 private:
  std::thread working_thread_;
  std::atomic<bool> stop_thread_ = true;
  uint64_t step_time_ = 0; ///< Nanoseconds delay time
  void CyclicTask();
};

}  // namespace workflow
