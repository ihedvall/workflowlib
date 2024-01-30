/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <atomic>
#include <condition_variable>
#include <vector>
#include <memory>
#include <any>
#include <array>

#include "workflow/irunner.h"
#include <util/ixmlnode.h>
#include <util/idirectory.h>

namespace workflow {
using RunnerList = std::vector<std::unique_ptr<IRunner>>;

class WorkflowServer;

class IWorkflow {
 public:
  explicit IWorkflow( WorkflowServer* server);
  virtual ~IWorkflow() = default;
  IWorkflow(const IWorkflow& workflow);
  IWorkflow& operator = (const IWorkflow& workflow);

  [[nodiscard]] bool operator == ( const IWorkflow& workflow) const;

  void Name(const std::string& name) {name_ = name;}
  [[nodiscard]] const std::string& Name() const {return name_;}

  void Description(const std::string& desc) {description_ = desc;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  void StartEvent(const std::string& event) {start_event_ = event;}
  [[nodiscard]] const std::string& StartEvent() const {
    return start_event_;
  }

  [[nodiscard]] RunnerList& Runners() {return runner_list_;}
  [[nodiscard]] const IRunner* GetRunner(const std::string& name) const;
  [[nodiscard]] IRunner* GetRunner(const std::string& name);
  void AddRunner(const IRunner& runner);
  void DeleteRunner(const IRunner* runner);
  [[nodiscard]] const IRunner* GetRunnerByTemplateName(const std::string& name)
      const;
  virtual void OnStart();
  [[nodiscard]] bool IsRunning() const {return running_;}

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

  void MoveUp(const IRunner* runner);
  void MoveDown(const IRunner* runner);

  void Init();
  void Tick(); ///< Runs all runners/tasks
  void Exit();

  template<typename T>
  T* GetData();

  template<typename T>
  bool InitData(const T& value);

  void ClearData();

  [[nodiscard]] IWorkflow* GetWorkflow(const std::string& schedule_name);

 protected:
  std::atomic<bool> start_ = false;
  std::condition_variable start_condition_;
  std::atomic<bool> running_ = false;
  RunnerList runner_list_;

  WorkflowServer* server_ = nullptr;
 private:
  IWorkflow() = default;
  std::string name_;
  std::string description_;
  std::string start_event_;
  std::any data_;
};

template <typename T>
T* IWorkflow::GetData() {
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
bool IWorkflow::InitData(const T& value) {
  try {
    data_ = std::make_any<T>(value);
  } catch (const std::exception&) {
    return false;
  }
  return true;
}

}  // namespace workflow
