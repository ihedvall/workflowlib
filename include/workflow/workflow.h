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

#include "workflow/itask.h"
#include <util/ixmlnode.h>
#include <util/idirectory.h>

namespace workflow {

using TaskList = std::vector<std::unique_ptr<ITask>>;

class WorkflowServer;

class Workflow {
 public:
  explicit Workflow(WorkflowServer* server);
  virtual ~Workflow() = default;
  Workflow(const Workflow& workflow);
  Workflow& operator = (const Workflow& workflow);

  [[nodiscard]] bool operator == ( const Workflow& workflow) const;

  void Name(const std::string& name) {name_ = name;}
  [[nodiscard]] const std::string& Name() const {return name_;}

  void Description(const std::string& desc) {description_ = desc;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  void StartEvent(const std::string& event) {start_event_ = event;}
  [[nodiscard]] const std::string& StartEvent() const {
    return start_event_;
  }

  [[nodiscard]] TaskList& Tasks() {return task_list_;}
  [[nodiscard]] const ITask* GetTask(const std::string& name) const;
  [[nodiscard]] ITask* GetTask(const std::string& name);
  void AddTask(const ITask& task);
  void DeleteTask(const ITask* task);
  [[nodiscard]] const ITask* GetTaskByTemplateName(const std::string& name)
      const;
  virtual void OnStart();
  [[nodiscard]] bool IsRunning() const {return running_;}

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

  void MoveUp(const ITask* task);
  void MoveDown(const ITask* task);

  void Init();
  void Tick(); ///< Runs all runners/tasks
  void Exit();

  template<typename T>
  T* GetData();

  template<typename T>
  bool InitData(const T& value);

  void ClearData();

  [[nodiscard]] Workflow* GetWorkflow(const std::string& schedule_name);

 protected:
  std::atomic<bool> start_ = false;
  std::condition_variable start_condition_;
  std::atomic<bool> running_ = false;
  TaskList task_list_;

  WorkflowServer* server_ = nullptr;
 private:
  Workflow() = default;
  std::string name_;
  std::string description_;
  std::string start_event_;
  std::any data_;
};

template <typename T>
T* Workflow::GetData() {
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
bool Workflow::InitData(const T& value) {
  try {
    data_ = std::make_any<T>(value);
  } catch (const std::exception&) {
    return false;
  }
  return true;
}

}  // namespace workflow
