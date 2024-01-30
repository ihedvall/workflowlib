/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include <gtest/gtest.h>

#include "workflow/ievent.h"
#include <cmath>
#include "util/logstream.h"
#include "util/logconfig.h"
#include "util/timestamp.h"

#include <thread>
using namespace util::log;
using namespace util::time;
using namespace testing;
using namespace std::chrono_literals;

namespace {

class MockCyclicEvent : public workflow::IEvent {
 public:

  void Tick() override {
    workflow::IEvent::Tick();
    ++nof_ticks;
  };

  size_t nof_ticks = 0;

};

class MockPeriodicEvent : public workflow::IEvent {
 public:

  void Tick() override {
    workflow::IEvent::Tick();
    ++nof_ticks;
  };

  size_t nof_ticks = 0;
};

}
namespace workflow::test {

TEST(IEvent, TestGeneral) {
  IEvent event;

  constexpr std::string_view kOrigName = "StarBuck";
  event.Name(kOrigName.data());
  EXPECT_STREQ(event.Name().c_str(), kOrigName.data());

  constexpr std::string_view kOrigDesc = "Cylon";
  event.Description(kOrigDesc.data());
  EXPECT_STREQ(event.Description().c_str(), kOrigDesc.data());

  event.Type(EventType::Parameter);
  EXPECT_EQ(event.Type(), EventType::Parameter);

  event.Period(666);
  EXPECT_EQ(event.Period(), 666);

  constexpr std::string_view kOrigParameter = "Gaius";
  event.Parameter(kOrigParameter.data());
  EXPECT_STREQ(event.Parameter().c_str(), kOrigParameter.data());
}

TEST(IEvent, CyclicEvent) {
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  MockCyclicEvent event;
  event.Type(EventType::Cyclic);
  event.Period(100); // 100 ms
  const auto start_time = TimeStampToNs();
  event.Init();
  std::this_thread::sleep_for(20s);
  event.Exit();
  const auto stop_time = TimeStampToNs();
  auto period = (stop_time - start_time) / 1'000'000;
  // period is ms
  period /= event.nof_ticks;
  LOG_TRACE() << "Period (ms): " << period;

  EXPECT_GT(period, 80);
  EXPECT_LT(period, 120);
}

TEST(IEvent, PeriodicEvent) {
  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToConsole);
  log_config.CreateDefaultLogger();

  MockPeriodicEvent event;
  event.Type(EventType::Periodic);
  event.Period(100);

  const auto start_time = TimeStampToNs();

  event.Init();
  std::this_thread::sleep_for(20s);
  event.Exit();

  const auto stop_time = TimeStampToNs();
  auto period = (stop_time - start_time) / 1'000'000;
  // period is ms
  period /= event.nof_ticks;
  LOG_TRACE() << "Period (ms): " << period;

  EXPECT_GT(period, 95);
  EXPECT_LT(period, 105);
}
}