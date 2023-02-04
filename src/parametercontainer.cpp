/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/parametercontainer.h"
#include <ranges>
#include <algorithm>
#include <set>
#include <util/stringutil.h>

using namespace util::string;
using namespace util::xml;
namespace workflow {

ParameterContainer::ParameterContainer(const ParameterContainer& container)
  : unique_name_(container.unique_name_),
    ignore_case_name_(container.ignore_case_name_)
{
  for (const auto& device : container.device_list_) {
    if (!device.second) {
      continue;
    }
    auto temp = std::make_unique<IDevice>(*device.second);
    device_list_.insert({temp->Name(), std::move(temp)});
  }

  for (const auto& parameter : container.parameter_list_) {
    if (!parameter.second) {
      continue;
    }
    auto temp = std::make_unique<IParameter>(*parameter.second);
    parameter_list_.insert({temp->Name(), std::move(temp)});
  }
}

bool ParameterContainer::operator==(const ParameterContainer& container) const {
  if (unique_name_ != container.unique_name_) return false;
  if (ignore_case_name_ != container.ignore_case_name_) return false;
  const auto device_equal =
      std::ranges::equal(device_list_,container.device_list_,
        [] (const auto& device1, const auto& device2) {
        if (!device1.second && !device2.second) return true;
        return device1.second && device2.second &&
               (*device1.second == *device2.second); });

  const auto parameter_equal =
      std::ranges::equal(parameter_list_,container.parameter_list_,
          [] (const auto& parameter1, const auto& parameter2) {
                 if (!parameter1.second && !parameter2.second) return true;
                 return parameter1.second && parameter2.second &&
                                  (*parameter1.second == *parameter2.second);
                         });
  return device_equal && parameter_equal;
}

IDevice* ParameterContainer::AddDevice(std::unique_ptr<IDevice>& device) {
   if (!device) {
    return nullptr;
  }
  const std::string& name = device->Name();
  if (name.empty()) {
    return nullptr;
  }

  auto exist = std::ranges::find_if(device_list_, [&] (const auto& itr) {
    if (unique_name_ && ignore_case_name_) {
      const auto& temp = itr.first;
      return IEquals(name, temp);
    }
    if (unique_name_) {
      const auto& temp = itr.first;
      return name == temp;
    }
    return false;
  });

  if (exist != device_list_.end()) {
    exist->second = std::move(device);
    return exist->second.get();
  }

  auto ret = device_list_.insert({device->Name(), std::move(device)});
  if (!ret.second || ret.first == device_list_.end()) {
    return nullptr;
  }
  return ret.first->second.get();
}

void ParameterContainer::DeleteDevice(const IDevice& device) {
  auto itr = std::ranges::find_if(device_list_, [&] (const auto& itr) {
    return itr.second.get() == &device;
  });
  if (itr != device_list_.end()) {
    device_list_.erase(itr);
  }
}

void ParameterContainer::DeleteDevice(const std::string& name) {
  auto itr = std::ranges::find_if(device_list_, [&] (const auto& find) {
    const auto* device = find.second.get();
    return device != nullptr && ignore_case_name_ ?
                                               IEquals(name, device->Name())
                                               : name == device->Name();
  });
  if (itr != device_list_.end()) {
    device_list_.erase(itr);
  }
}

IDevice* ParameterContainer::GetDevice(const std::string& name) const {
  if (ignore_case_name_) {
    const auto exist = std::ranges::find_if(device_list_,
                                            [&] (const auto& itr) {
                                              const auto& temp = itr.first;
                                              return IEquals(name, temp);
                                            });
    return exist != device_list_.cend() ? exist->second.get() : nullptr;
  }

  const auto itr = device_list_.find(name);
  return itr != device_list_.cend() ? itr->second.get() : nullptr;
}

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

std::vector<std::string> ParameterContainer::Units() const {
  std::set<std::string, util::string::IgnoreCase> temp;
  for (const auto& itr : parameter_list_) {
    const auto& parameter = itr.second;
    if (!parameter) {
      continue;
    }
    auto unit = parameter->Unit();
    util::string::Trim(unit);
    temp.insert(unit);
  }
  std::vector<std::string> list;
  for (auto& utf8 : temp) {
    list.emplace_back(utf8);
  }
  return list;
}

void ParameterContainer::Clear() {
  parameter_list_.clear();
  device_list_.clear();
}

bool ParameterContainer::Empty() const {
  return device_list_.empty() && parameter_list_.empty() &&
      !unique_name_ && !ignore_case_name_;
}

void ParameterContainer::SaveXml(IXmlNode& root) const {
  if (Empty()) {
    return;
  }

  auto& container_root = root.AddNode("ParameterContainer");

  container_root.SetProperty("UniqueName", unique_name_);
  container_root.SetProperty("IgnoreCase", ignore_case_name_);

  if (!device_list_.empty()) {
    auto& device_root = container_root.AddNode("DeviceList");
    for (const auto& device : device_list_) {
      device.second->SaveXml(device_root);
    }
  }

  if (!parameter_list_.empty()) {
    auto& parameter_root = container_root.AddNode("ParameterList");
    for (const auto& parameter : parameter_list_) {
      parameter.second->SaveXml(parameter_root);
    }
  }
}

void ParameterContainer::ReadXml(const IXmlNode& root) {
  const auto* container_root = root.GetNode("ParameterContainer");
  if (container_root == nullptr) {
    return;
  }
  unique_name_ = container_root->Property<bool>("UniqueName");
  ignore_case_name_ = container_root->Property<bool>("IgnoreCase");

  const auto* device_root = root.GetNode("DeviceList");
  if (device_root != nullptr) {
    IXmlNode::ChildList list;
    device_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Device")) {
        continue;
      }
      auto device = std::make_unique<IDevice>();
      device->ReadXml(*item);
    }
  }

  const auto* parameter_root = root.GetNode("ParameterList");
  if (parameter_root != nullptr) {
    IXmlNode::ChildList list;
    parameter_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Parameter")) {
        continue;
      }
      auto parameter = std::make_unique<IParameter>();
      parameter->ReadXml(*item);
    }
  }

}


}  // namespace workflow