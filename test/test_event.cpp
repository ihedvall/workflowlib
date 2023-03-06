/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "workflow/ievent.h"
#include <cmath>

#include "cyclicevent.h"
#include "periodicevent.h"
#include "util/logstream.h"
#include "util/logconfig.h"
#include <chrono>
#include <thread>
using namespace util::log;
using namespace testing;
using namespace std::chrono_literals;

namespace {
class MockCyclicEvent : public workflow::CyclicEvent {
 public:

  void Tick() override {
    workflow::CyclicEvent::Tick();
    LOG_TRACE() << "Tick()";
  };
};

class MockPeriodicEvent : public workflow::PeriodicEvent {
 public:

  void Tick() override {
    workflow::PeriodicEvent::Tick();
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
  event.Period(1000);
  event.Init();
  std::this_thread::sleep_for(10s);
  event.Exit();
}
}