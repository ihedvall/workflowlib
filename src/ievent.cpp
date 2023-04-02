/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/ievent.h"
#include <util/stringutil.h>
#include "workflow/iworkflow.h"
#include <chrono>
#include <util/timestamp.h>

using namespace util::xml;
using namespace std::chrono_literals;
using namespace util::time;

namespace workflow {

IEvent::~IEvent() {
  stop_thread_ = true;
  if (working_thread_.joinable()) {
    working_thread_.join();
  }
}

IEvent::IEvent(const IEvent& event)
:  name_(event.name_),
   description_(event.description_),
   type_(event.type_),
   parameter_(event.parameter_),
   period_(event.period_)
{
}

bool IEvent::operator==(const IEvent& event) const {
  if (name_ != event.name_) return false;
  if (description_ != event.description_) return false;
  if (type_ != event.type_) return false;
  if (parameter_ != event.parameter_) return false;
  if (period_ != event.period_) return false;
  return true;
}


void IEvent::EventTypeAsString(const std::string& type) {
  IEvent temp;
  for (auto index = static_cast<int>(EventType::Init);
       index <= static_cast<int>(EventType::Parameter);
       ++index) {
    temp.Type(static_cast<EventType>(index));
    const auto type_string = temp.EventTypeAsString();
    if (util::string::IEquals(type, type_string, 4)) {
      Type(temp.Type());
      return;
    }
  }
}

std::string IEvent::EventTypeAsString() const {
  switch (Type()) {
    case EventType::Init:
      return "Init Event";

    case EventType::Exit:
      return "Exit Event";

    case EventType::Cyclic:
      return "Cyclic Event";

    case EventType::Periodic:
      return "Periodic Event";

    case EventType::Parameter:
      return "Parameter";

    default:
      break;
  }
  return {};
}

void IEvent::SaveXml(IXmlNode& root) const {
  auto& event_root = root.AddNode("Event");
  event_root.SetAttribute("name", name_);
  event_root.SetProperty("Name", name_);
  event_root.SetProperty("Description", description_);
  event_root.SetProperty("Type", EventTypeAsString());
  event_root.SetProperty("Parameter", parameter_);
  event_root.SetProperty("Period", period_);
}

void IEvent::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  description_ = root.Property<std::string>("Description");
  EventTypeAsString( root.Property<std::string>("Type"));
  parameter_ = root.Property<std::string>("Parameter");
  period_ = root.Property<uint64_t>("Period", 1000);
}

void IEvent::Init() {
  switch (type_) {
    case EventType::Init: {
      for (auto* workflow : workflow_list_) {
        if (workflow != nullptr) {
          workflow->Tick();
        }
      }
      break;
    }

    case EventType::Cyclic:
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
      working_thread_ = std::thread(&IEvent::CyclicTask, this);
      break;

    case EventType::Periodic: {
      if (working_thread_.joinable()) {
        stop_thread_ = true;
        working_thread_.join();
      }
      // Start Working thread
      step_time_ = static_cast<uint64_t>(Period()) * 1'000'000;  // Now in ns
      if (step_time_ < 10'000'000) {
        step_time_ = 1'000'000'000;
      }
      const auto now = std::chrono::system_clock::now();
      next_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(
                       now.time_since_epoch())
                       .count();
      next_time_ /= static_cast<int64_t>(step_time_);
      next_time_ *= static_cast<int64_t>(step_time_);
      next_time_ += static_cast<int64_t>(step_time_);

      stop_thread_ = false;
      working_thread_ = std::thread(&IEvent::PeriodicTask, this);
      break;
    }

    case EventType::Exit:
    default:
      break;
  }
}

void IEvent::Tick() {
  switch (type_) {
    case EventType::Exit:
    case EventType::Init:
      // Do nothing in tick
      break;


    default: {
      // Do tick all attached workflow
      for (auto* workflow : workflow_list_) {
        if (workflow != nullptr) {
          workflow->Tick();
        }
      }
      break;
    }
  }
}

void IEvent::Exit() {
  switch (type_) {

    case EventType::Exit: {
      for (auto* workflow : workflow_list_) {
        if (workflow != nullptr) {
          workflow->Tick();
        }
      }
      break;
    }

    case EventType::Periodic:
    case EventType::Cyclic:
      stop_thread_ = true;
      if (working_thread_.joinable()) {
        working_thread_.join();
      }
      break;

    default:
      break;
  }
}

void IEvent::AttachWorkflow(IWorkflow* workflow){
  if (workflow != nullptr) {
    workflow_list_.emplace_back(workflow);
  }
}

void IEvent::DetachWorkflows() {
  workflow_list_.clear();
}

void IEvent::CyclicTask() {
  while (!stop_thread_) {
    Tick();
    std::chrono::nanoseconds duration(step_time_);
    std::this_thread::sleep_for(duration);
  }
}

void IEvent::PeriodicTask() {
  while (!stop_thread_) {
    const auto now = std::chrono::system_clock::now();
    const std::chrono::nanoseconds temp(next_time_);
    const std::chrono::time_point<std::chrono::system_clock,
                                  std::chrono::nanoseconds> abs_time(temp);
    if (now >= abs_time) {
      next_time_ += static_cast<int64_t>(step_time_);
      Tick();
    }
    std::this_thread::sleep_for(1ms);
  }
}
}  // namespace workflow