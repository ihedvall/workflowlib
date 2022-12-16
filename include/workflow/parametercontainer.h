/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <map>
#include <string>
#include "workflow/iparameter.h"

namespace workflow {

using ParameterList = std::multimap<std::string, std::unique_ptr<IParameter>>;
class ParameterContainer {
 public:
  void UniqueName(bool unique) {unique_name_ = unique;}
  [[nodiscard]] bool UniqueName() const {return unique_name_;}

  void IgnoreCase(bool ignore) {ignore_case_name_ = ignore; }
  [[nodiscard]] bool IgnoreCase() const {return ignore_case_name_;}

  [[nodiscard]] const ParameterList& Parameters() const {
    return parameter_list_;
  }

  template <typename T>
  void ParametersByType(std::vector<T*>& list) const;

  IParameter* AddParameter(std::unique_ptr<IParameter>& parameter);
  [[nodiscard]] IParameter* GetParameter(const std::string& name) const;
  void DeleteParameter(const IParameter& parameter);
  void DeleteParameter(const std::string& name);
  void Clear() {parameter_list_.clear();}

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

 private:
  ParameterList parameter_list_;
  bool unique_name_ = false;
  bool ignore_case_name_ = false;
};

template <typename T>
void ParameterContainer::ParametersByType(std::vector<T*>& list) const {
  for (const auto& itr : parameter_list_) {
    auto* par = itr.second.get();
    if (par == nullptr) {
      continue;
    }
    auto* par_type = dynamic_cast<T*>(par);
    if (par_type != nullptr) {
      list.push_back(par_type);
    }
  }
}

}  // namespace workflow
