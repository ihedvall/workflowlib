/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */


#include "periodicevent.h"
#include <chrono>
#include <util/timestamp.h>


using namespace std::chrono_literals;
using namespace util::time;
namespace workflow {

PeriodicEvent::PeriodicEvent() {
  Name("PeriodicEvent"),
  Description("Event that happens periodically each set period (ms).");
  Type(EventType::Periodic);

}

PeriodicEvent::PeriodicEvent(const IEvent& source)
    :IEvent(source) {
}

void PeriodicEvent::Init() {
  IEvent::Init();
  if (working_thread_.joinable()) {
    stop_thread_ = true;
    working_thread_.join();
  }
  // Start Working thread
  step_time_ = static_cast<int64_t>(Period()) * 1'000'000; // Now in ns
  if (step_time_ < 10'000'000) {
    step_time_ = 1'000'000'000;
  }
  const auto now = std::chrono::system_clock::now();
  next_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>
      (now.time_since_epoch()).count();
  next_time_ /= step_time_;
  next_time_ *= step_time_;
  next_time_ += step_time_;

  stop_thread_ = false;
  working_thread_ = std::thread(&PeriodicEvent::PeriodicTask, this);
}

void PeriodicEvent::Exit() {
  stop_thread_ = true;
  if (working_thread_.joinable()) {
    working_thread_.join();
  }

  IEvent::Exit();
}

void PeriodicEvent::PeriodicTask() {
  while (!stop_thread_) {
    const std::chrono::nanoseconds temp(next_time_);
    const std::chrono::time_point<std::chrono::system_clock,
                                  std::chrono::nanoseconds> abs_time(temp);
    std::this_thread::sleep_until(abs_time);
    next_time_ += step_time_;
    Tick();
  }
}

}  // namespace workflow