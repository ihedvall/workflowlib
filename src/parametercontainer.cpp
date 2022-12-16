/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/parametercontainer.h"
#include <ranges>
#include <algorithm>
#include <util/stringutil.h>

using namespace util::string;

namespace workflow {

IParameter* ParameterContainer::AddParameter(
    std::unique_ptr<IParameter>& parameter) {

  if (!parameter) {
    return nullptr;
  }
  const std::string& par_name = parameter->Name();
  if (par_name.empty()) {
    return nullptr;
  }

  auto exist = std::ranges::find_if(parameter_list_, [&] (const auto& itr) {
    if (unique_name_ && ignore_case_name_) {
      const auto& temp = itr.first;
      return IEquals(par_name, temp);
    }
    if (unique_name_) {
      const auto& temp = itr.first;
      return par_name == temp;
    }
    return false;
  });

  if (exist != parameter_list_.end()) {
    exist->second = std::move(parameter);
    return exist->second.get();
  }

  auto ret = parameter_list_.insert({parameter->Name(), std::move(parameter)});
  if (ret == parameter_list_.end()) {
    return nullptr;
  }
  return ret->second.get();
}

void ParameterContainer::DeleteParameter(const IParameter& parameter) {
  auto itr = std::ranges::find_if(parameter_list_, [&] (const auto& itr) {
    return itr.second.get() == &parameter;
  });
  if (itr != parameter_list_.end()) {
    parameter_list_.erase(itr);
  }
}

void ParameterContainer::DeleteParameter(const std::string& name) {
  auto itr = std::ranges::find_if(parameter_list_, [&] (const auto& find) {
    const auto* par = find.second.get();
    return par != nullptr && ignore_case_name_ ? IEquals(name, par->Name())
                                               : name == par->Name();
  });
  if (itr != parameter_list_.end()) {
    parameter_list_.erase(itr);
  }
}

IParameter* ParameterContainer::GetParameter(const std::string& name) const {
  if (ignore_case_name_) {
    const auto exist = std::ranges::find_if(parameter_list_,
                                            [&] (const auto& itr) {
      const auto& temp = itr.first;
      return IEquals(name, temp);
    });
    return exist != parameter_list_.cend() ? exist->second.get() : nullptr;
  }

  const auto itr = parameter_list_.find(name);
  return itr != parameter_list_.cend() ? itr->second.get() : nullptr;
}

void ParameterContainer::Init() {
  for (auto& itr : parameter_list_ ) {
    auto* parameter = itr.second.get();
    if (parameter != nullptr) {
      parameter->Init();
    }
  }
}

void ParameterContainer::Tick() {
  // By default, it doesn't scan through the parameter list
}

void ParameterContainer::Exit() {
  for (auto& itr : parameter_list_ ) {
    auto* parameter = itr.second.get();
    if (parameter != nullptr) {
      parameter->Exit();
    }
  }
}

}  // namespace workflow