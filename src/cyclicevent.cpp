/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "cyclicevent.h"
#include <chrono>


using namespace std::chrono_literals;

namespace workflow {

CyclicEvent::CyclicEvent() {
  Name("CyclicEvent"),
  Description("Event that happens periodically each set period (ms).");
  Type(EventType::Periodic);
}

CyclicEvent::CyclicEvent(const IEvent& source)
    :IEvent(source) {
}

void CyclicEvent::Init() {
  IEvent::Init();
  if (working_thread_.joinable()) {
    stop_thread_ = true;
    working_thread_.join();
  }
  // Start Working thread
  step_time_ = Period() * 1'000'000; // Now in ns
  if (step_time_ < 10'000'000) {
    step_time_ = 1'000'000'000;
  }
  stop_thread_ = false;
  working_thread_ = std::thread(&CyclicEvent::CyclicTask, this);
}

void CyclicEvent::Exit() {
  stop_thread_ = true;
  if (working_thread_.joinable()) {
    working_thread_.join();
  }
  IEvent::Exit();
}

void CyclicEvent::CyclicTask() {
  while (!stop_thread_) {
    Tick();
    std::chrono::nanoseconds duration(step_time_);
    std::this_thread::sleep_for(duration);
  }
}


}  // namespace workflow