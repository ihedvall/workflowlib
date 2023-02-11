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
#include <util/ixmlnode.h>

namespace workflow {

enum class EventType {
  Internal,
  External,
  Periodic,
  Parameter,
};

class IEvent {
 public:
  IEvent();
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

  void Parameter(const std::string& parameter) {parameter_ = parameter;}
  [[nodiscard]] const std::string& Parameter() const {return parameter_;}

  //virtual std::unique_ptr<IEvent> Create() = 0;

  void AddRunner(std::unique_ptr<IRunner>& runner);

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

  virtual void Init();
  virtual void Tick();
  virtual void Exit();
 protected:

 private:
  std::string name_;
  std::string description_;
  std::string parameter_;
  EventType type_ = EventType::External;
  std::unique_ptr<IRunner> runner_;
};

}  // namespace workflow
