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
enum class EventType {
  Internal,
  External,
  Periodic,
  Parameter,
};

class IEvent : public IParameter {
 public:
  IEvent();
  ~IEvent() override;
  IEvent(const IEvent& event);
  [[nodiscard]] bool operator == (const IEvent& event) const;

  void Type(EventType type) {type_ = type;}
  [[nodiscard]] EventType Type() const {return type_;}
  void EventTypeAsString(const std::string& type);
  [[nodiscard]] std::string EventTypeAsString() const;

  //virtual std::unique_ptr<IEvent> Create() = 0;

  void AddRunner(std::unique_ptr<IRunner>& runner);

 protected:

 private:

  EventType type_ = EventType::External;
  std::unique_ptr<IRunner> runner_;
};

}  // namespace workflow
