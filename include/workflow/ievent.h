/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <vector>
#include <thread>

#include "workflow/irunner.h"
#include <util/ixmlnode.h>

namespace workflow {

class IWorkflow;

enum class EventType {
  Init,
  Exit,
  Cyclic,
  Periodic,
  Parameter,
};

class IEvent {
 public:
  IEvent() = default;
  virtual ~IEvent();
  IEvent(const IEvent& event);
  [[nodiscard]] bool operator == (const IEvent& event) const;

  void Name(const std::string& name) { name_ = name;}
  [[nodiscard]] const std::string& Name() const {return name_;}

  void Description(const std::string& description) {description_ = description;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  void Type(EventType type) {type_ = type;}
  [[nodiscard]] EventType Type() const {return type_;}
  void EventTypeAsString(const std::string& type);
  [[nodiscard]] std::string EventTypeAsString() const;

  void Period(uint64_t period) {period_ = period;}
  [[nodiscard]] uint64_t Period() const {return period_;}

  void Parameter(const std::string& parameter) {parameter_ = parameter;}
  [[nodiscard]] const std::string& Parameter() const {return parameter_;}

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  void AttachWorkflow(IWorkflow* workflow);
  void DetachWorkflows();

 protected:

 private:
  std::string name_;
  std::string description_;
  std::string parameter_;
  uint64_t period_ = 1000; ///< Period in ms
  EventType type_ = EventType::Cyclic;
  std::vector<IWorkflow*> workflow_list_;

  std::thread working_thread_;
  std::atomic<bool> stop_thread_ = true;
  uint64_t step_time_ = 0;
  int64_t next_time_ = 0;

  void PeriodicTask();
  void CyclicTask();
};

}  // namespace workflow
