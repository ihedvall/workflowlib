/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "workflow/iparameter.h"
#include "workflow/idevice.h"
#include <util/ixmlnode.h>

namespace workflow {
using DeviceList = std::map<std::string, std::unique_ptr<IDevice>>;
using ParameterList = std::multimap<std::string, std::unique_ptr<IParameter>>;

class ParameterContainer {
 public:
  ParameterContainer(const ParameterContainer& container);
  [[nodiscard]] bool operator == (const ParameterContainer& container) const;

  void UniqueName(bool unique) {unique_name_ = unique;}
  [[nodiscard]] bool UniqueName() const {return unique_name_;}

  void IgnoreCase(bool ignore) {ignore_case_name_ = ignore; }
  [[nodiscard]] bool IgnoreCase() const {return ignore_case_name_;}

  [[nodiscard]] const DeviceList& Devices() const {
    return device_list_;
  }

  [[nodiscard]] const ParameterList& Parameters() const {
    return parameter_list_;
  }

  [[nodiscard]] std::vector<std::string> Units() const;

  template <typename T>
  void ParametersByType(std::vector<T*>& list) const;

  IDevice* AddDevice(std::unique_ptr<IDevice>& device);
  [[nodiscard]] IDevice* GetDevice(const std::string& name) const;
  void DeleteDevice(const IDevice& device);
  void DeleteDevice(const std::string& name);

  IParameter* AddParameter(std::unique_ptr<IParameter>& parameter);
  [[nodiscard]] IParameter* GetParameter(const std::string& name) const;
  void DeleteParameter(const IParameter& parameter);
  void DeleteParameter(const std::string& name);

  void Clear();
  [[nodiscard]] bool Empty() const;

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

 private:
  DeviceList device_list_;
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
