/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>
#include <vector>
#include <any>
#include <map>
#include <sstream>
#include "workflow/iparameter.h"

namespace workflow {

enum class RunnerType : int {
  InternalRunner,
  DllRunner,
  ExeRunner,
  LuaRunner,
  PythonRunner,
};

class IRunner {
 public:

  virtual ~IRunner() = default;

  void Name(const std::string& name) {name_ = name;}
  [[nodiscard]] const std::string& Name() const {return name_;}

  void Description(const std::string& desc) {description_ = desc;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  void Documentation(const std::string& doc) {documentation_ = doc;}
  [[nodiscard]] const std::string& Documentation() const {
    return documentation_;
  }

  void Type(RunnerType type) {type_ = type;}
  [[nodiscard]] RunnerType Type() const {return type_;}

  void Period(double period) {period_ = period;}
  [[nodiscard]] double Period() const {return period_;}

  [[nodiscard]] std::vector<std::string>& InputArguments() {
    return input_arguments_;
  }
  [[nodiscard]] const std::vector<std::string>& InputArguments() const {
    return input_arguments_;
  }

  [[nodiscard]] std::vector<IParameter*>& Parameters() {
    return parameter_list_;
  }
  [[nodiscard]] const std::vector<IParameter*>& Parameters() const {
    return parameter_list_;
  }

  void LastError(const std::string& error) {
    last_error_ = error;
  }
  [[nodiscard]] const std::string& LastError() const { return last_error_; }
  virtual void Init() = 0;
  virtual void Tick() = 0;
  virtual void Exit() = 0;

 protected:
  IRunner() = default;
 private:
  std::string name_;
  std::string description_;
  std::string documentation_;
  std::string last_error_;
  std::vector<std::string> input_arguments_;
  std::vector<IParameter*> parameter_list_;

  RunnerType type_ = RunnerType::InternalRunner;
  double period_ = 0; ///< Seconds
};


}  // namespace workflow
