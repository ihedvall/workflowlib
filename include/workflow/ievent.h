/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <string>
#include <atomic>
#include <memory>
#include "workflow/irunner.h"
#include "workflow/iparameter.h"

namespace workflow {
enum class WorkflowEventType {
  ManualTrig,
  CyclicTrig,
  ScheduleTrig,
  ExternalTrig
};

class IEvent : public IParameter {
 public:
  IEvent();

  ~IEvent() override;

  void Type(WorkflowEventType type) {type_ = type;}
  [[nodiscard]] WorkflowEventType Type() const {return type_;}

  virtual std::unique_ptr<IEvent> Create() = 0;

  void AddRunner(std::unique_ptr<IRunner>& runner);

 protected:

 private:

  WorkflowEventType type_ = WorkflowEventType::ManualTrig;
  std::unique_ptr<IRunner> runner_;
};

}  // namespace workflow
