/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include "workflow/parameter.h"
#include "workflow/device.h"
namespace util::xml {

class IXmlNode;

} // end namespace util::xml

namespace workflow {

using DeviceList = std::vector<std::unique_ptr<Device>>;
using ParameterList = std::vector<std::unique_ptr<Parameter>>;

class ParameterContainer {
 public:
  ParameterContainer() = default;
  virtual ~ParameterContainer() = default;
  ParameterContainer(const ParameterContainer& container);
  [[nodiscard]] bool operator == (const ParameterContainer& container) const;

  void IgnoreCase(bool ignore) {ignore_case_name_ = ignore; }
  [[nodiscard]] bool IgnoreCase() const {return ignore_case_name_;}

  [[nodiscard]] const DeviceList& Devices() const {
    return device_list_;
  }

  [[nodiscard]] const ParameterList& Parameters() const {
    return parameter_list_;
  }

  [[nodiscard]] std::vector<std::string> Units() const;

  //template <typename T>
  //void ParametersByType(std::vector<T*>& list) const;

  Device* CreateDevice(const std::string& device_name);
  [[nodiscard]] Device* GetDevice(const std::string& name) const;
  void DeleteDevice(const std::string& name);

  Parameter* CreateParameter(const std::string& device_name,
                             const std::string& parameter_name);
  [[nodiscard]] Parameter* GetParameter(const std::string& device,
                                        const std::string& name) const;
  void DeleteParameter(const std::string& device_name,
                       const std::string& parameter_name);

  void Clear();
  [[nodiscard]] bool Empty() const;
  void Sort();

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

 private:
  DeviceList device_list_;
  ParameterList parameter_list_;

  bool ignore_case_name_ = false;
};
/*
template <typename T>
void ParameterContainer::ParametersByType(std::vector<T*>& list) const {
  for (const auto& parameter : parameter_list_) {
    auto* par = parameter.get();
    if (par == nullptr) {
      continue;
    }
    auto* par_type = dynamic_cast<T*>(par);
    if (par_type != nullptr) {
      list.push_back(par_type);
    }
  }
}
*/
}  // namespace workflow
