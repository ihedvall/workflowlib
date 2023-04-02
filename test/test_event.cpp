/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "workflow/ievent.h"
#include <cmath>
#include "util/logstream.h"
#include "util/logconfig.h"
#include <chrono>
#include <thread>
using namespace util::log;
using namespace testing;
using namespace std::chrono_literals;

namespace {
class MockCyclicEvent : public workflow::IEvent {
 public:

  void Tick() override {

    workflow::IEvent::Tick();
    LOG_TRACE() << "Tick()";
  };
};

class MockPeriodicEvent : public workflow::IEvent {
 public:

  void Tick() override {
    workflow::IEvent::Tick();
    LOG_TRACE() << "Tick()";
  };
};
}
namespace workflow::test {

TEST(IEvent, CyclicEvent) {
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  MockCyclicEvent event;
  event.Type(EventType::Cyclic);
  event.Period(1000);
  event.Init();
  std::this_thread::sleep_for(10s);
  event.Exit();
}

TEST(IEvent, PeriodicEvent) {
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  MockPeriodicEvent event;
  event.Type(EventType::Periodic);
  event.Period(1000);
  event.Init();
  std::this_thread::sleep_for(20s);
  event.Exit();
}
}