/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/parametercontainer.h"
#include <ranges>
#include <algorithm>
#include <set>
#include <locale>
#include <util/stringutil.h>
#include <util/ixmlnode.h>

using namespace util::string;
using namespace util::xml;

namespace {

bool SortDevices(const std::unique_ptr<workflow::Device>& dev1,
                const std::unique_ptr<workflow::Device>& dev2) {
  if (!dev1 || !dev2) {
    return false;
  }
  return std::strcoll(dev1->Name().c_str(), dev2->Name().c_str()) < 0;
}

bool SortParameters(const std::unique_ptr<workflow::Parameter>& par1,
                const std::unique_ptr<workflow::Parameter>& par2) {
  if (!par1 || !par2) {
    return false;
  }
  const auto compare = std::strcoll(par1->Device().c_str(), par2->Device().c_str());
  if (compare > 0) {
    return false;
  }
  if (compare < 0) {
    return true;
  }

  return std::strcoll(par1->Name().c_str(), par2->Name().c_str()) < 0;
}

}

namespace workflow {

bool ParameterContainer::operator==(const ParameterContainer& container) const {

  if (ignore_case_name_ != container.ignore_case_name_) return false;
  const auto device_equal =
      std::ranges::equal(device_list_,container.device_list_,
        [] (const auto& device1, const auto& device2) {
        if (!device1 && !device2) return true;
        return device1 && device2 &&
               (*device1 == *device2); });

  const auto parameter_equal =
      std::ranges::equal(parameter_list_,container.parameter_list_,
          [] (const auto& parameter1, const auto& parameter2) {
                 if (!parameter1 && !parameter2) return true;
                 return parameter1 && parameter2 &&
                                  (*parameter1 == *parameter2);
                         });
  return device_equal && parameter_equal;
}

Device* ParameterContainer::CreateDevice(const std::string& device_name) {
   if (device_name.empty()) {
    return nullptr;
  }
  auto* device = GetDevice(device_name);
  if (device != nullptr) {
    return device;
  }

  auto new_device = std::make_unique<Device>();
  if (new_device) {
    new_device->Name(device_name);
    device_list_.emplace_back( std::move(new_device) );
  }
  return GetDevice(device_name);
}


void ParameterContainer::DeleteDevice(const std::string& name) {
  auto itr = std::ranges::find_if(device_list_, [&] (const auto& find) {
    const auto* device = find.get();
    return device != nullptr && ignore_case_name_ ?
                                               IEquals(name, device->Name())
                                               : name == device->Name();
  });
  if (itr != device_list_.end()) {
    device_list_.erase(itr);
  }
}

Device* ParameterContainer::GetDevice(const std::string& name) const {
  const auto exist = std::ranges::find_if(device_list_,
                                          [&] (const auto& itr) ->bool {
                                return ignore_case_name_ ? IEquals(name, itr->Name()) : name == itr->Name();
                                          });
  return exist != device_list_.cend() ? exist->get() : nullptr;
}

Parameter* ParameterContainer::CreateParameter(const std::string& device_name,
                                               const std::string& parameter_name) {
  if (parameter_name.empty()) {
    return nullptr;
  }
  auto* parameter = GetParameter(device_name,parameter_name);
  if (parameter != nullptr) {
    return parameter;
  }

  auto new_parameter = std::make_unique<Parameter>();
  if (new_parameter) {
    new_parameter->Name(parameter_name);
    new_parameter->Device(device_name);
    parameter_list_.emplace_back(std::move(new_parameter));
  }
  return GetParameter(device_name, parameter_name);
}

void ParameterContainer::DeleteParameter(const std::string& device_name,
                                         const std::string& parameter_name) {
  auto itr = std::ranges::find_if(parameter_list_, [&] (const auto& parameter) {
    if (!parameter) {
      return false;
    }
    return ignore_case_name_ ?
        IEquals(device_name, parameter->Device()) && IEquals(parameter_name, parameter->Name())
        : device_name == parameter->Device() && parameter_name == parameter->Name();
    });
  if (itr != parameter_list_.end()) {
    parameter_list_.erase(itr);
  }
}

Parameter* ParameterContainer::GetParameter(const std::string& device, const std::string& name) const {
   const auto exist = std::ranges::find_if(parameter_list_,
                                            [&] (const auto& parameter) {
      return ignore_case_name_ ?
        IEquals(name, parameter->Name()) && IEquals(device, parameter->Device()) :
        name == parameter->Name() && device == parameter->Device();
    });
    return exist != parameter_list_.cend() ? exist->get() : nullptr;
}

void ParameterContainer::Init() {
  for (auto& parameter : parameter_list_ ) {
    if (parameter) {
      parameter->Init();
    }
  }
}

void ParameterContainer::Tick() {
  // By default, it doesn't scan through the parameter list
}

void ParameterContainer::Exit() {
  for (auto& parameter : parameter_list_ ) {
    if (parameter) {
      parameter->Exit();
    }
  }
}

std::vector<std::string> ParameterContainer::Units() const {
  std::set<std::string, std::locale> temp;
  for (const auto& parameter : parameter_list_) {
    if (!parameter) {
      continue;
    }
    auto unit = parameter->Unit(); // Need top copy string here so I can trim it
    util::string::Trim(unit);
    temp.insert(unit);
  }
  std::vector<std::string> list;
  for (const auto& utf8 : temp) {
    list.emplace_back(utf8);
  }
  return list;
}

void ParameterContainer::Clear() {
  parameter_list_.clear();
  device_list_.clear();
}

bool ParameterContainer::Empty() const {
  return device_list_.empty() && parameter_list_.empty();
}

void ParameterContainer::SaveXml(IXmlNode& root) const {
  if (Empty()) {
    return;
  }

  auto& container_root = root.AddNode("ParameterContainer");

  container_root.SetProperty("IgnoreCase", ignore_case_name_);

  if (!device_list_.empty()) {
    auto& device_root = container_root.AddNode("DeviceList");
    for (const auto& device : device_list_) {
      if (device) {
        device->SaveXml(device_root);
      }
    }
  }

  if (!parameter_list_.empty()) {
    auto& parameter_root = container_root.AddNode("ParameterList");
    for (const auto& parameter : parameter_list_) {
      if (parameter) {
        parameter->SaveXml(parameter_root);
      }
    }
  }
}

void ParameterContainer::ReadXml(const IXmlNode& root) {
  const auto* container_root = root.GetNode("ParameterContainer");
  if (container_root == nullptr) {
    return;
  }
  ignore_case_name_ = container_root->Property<bool>("IgnoreCase");

  const auto* device_root = root.GetNode("DeviceList");
  if (device_root != nullptr) {
    IXmlNode::ChildList list;
    device_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Device")) {
        continue;
      }
      auto device = std::make_unique<Device>();
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
      auto parameter = std::make_unique<Parameter>();
      parameter->ReadXml(*item);
    }
  }

}

void ParameterContainer::Sort() {
  std::sort(device_list_.begin(),device_list_.end(), SortDevices);
  std::sort(parameter_list_.begin(),parameter_list_.end(), SortParameters);
}

}  // namespace workflow