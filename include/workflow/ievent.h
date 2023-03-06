/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <vector>
#include "workflow/irunner.h"
#include <util/ixmlnode.h>

namespace workflow {

class IWorkflow;

enum class EventType {
  Internal,
  External,
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

  //virtual std::unique_ptr<IEvent> Create() = 0;

  void AddRunner(std::unique_ptr<IRunner>& runner);

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  void AttachWorkflow(IWorkflow* workflow);
  void DetachWorkflows();

  template<typename T>
  T* GetData();

  template<typename T>
  bool InitData(const T* value);

  void ClearData() {data_.reset();}
 protected:

 private:
  std::string name_;
  std::string description_;
  std::string parameter_;
  uint64_t period_ = 1000; ///< Period in ms
  EventType type_ = EventType::External;
  std::unique_ptr<IRunner> runner_;
  std::vector<IWorkflow*> workflow_list_;
  std::any data_; ///< Event data
};

template <typename T>
T* IEvent::GetData() {
  try {
    if (!data_.has_value()) {
      return nullptr;
    }
    auto* value = std::any_cast<T>(&data_);
    return value;
  } catch (const std::exception&) {

  }
  return nullptr;
}

template <typename T>
bool IEvent::InitData(const T* value) {
  try {
    data_ = value != nullptr ? std::make_any<T>(*value) : std::make_any<T>();
  } catch (const std::exception&) {
    return false;
  }
  return true;
}

}  // namespace workflow
