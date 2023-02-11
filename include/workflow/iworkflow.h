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

#include "workflow/irunner.h"
#include <util/ixmlnode.h>

namespace workflow {
using RunnerList = std::vector<std::unique_ptr<IRunner>>;

class IWorkflow {
 public:
  IWorkflow() = default;
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

  virtual void OnStart();
  [[nodiscard]] bool IsRunning() const {return running_;}

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

  void MoveUp(const IRunner* runner);
  void MoveDown(const IRunner* runner);
 protected:
  std::atomic<bool> start_ = false;
  std::condition_variable start_condition_;
  std::atomic<bool> running_ = false;
  RunnerList runner_list_;

 private:
  std::string name_;
  std::string description_;
  std::string start_event_;
};

}  // namespace workflow
