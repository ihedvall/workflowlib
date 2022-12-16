/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <atomic>
#include <condition_variable>
#include <vector>

namespace workflow {

class IWorkflow {
 public:
  void Name(const std::string& name) {name_ = name;}
  [[nodiscard]] const std::string& Name() const {return name_;}

  void Description(const std::string& desc) {description_ = desc;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  [[nodiscard]] std::vector<std::string>& StartEvents() {return start_events_;}
  [[nodiscard]] const std::vector<std::string>& StartEvents() const {
    return start_events_;
  }

  virtual void OnStart();
  [[nodiscard]] bool IsRunning() const {return running_;}
 protected:
  std::atomic<bool> start_ = false;
  std::condition_variable start_condition_;
  std::atomic<bool> running_ = false;
 private:
  std::string name_;
  std::string description_;
  std::vector<std::string> start_events_;
};

}  // namespace workflow
