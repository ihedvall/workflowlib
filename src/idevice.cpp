/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/idevice.h"

using namespace util::xml;

namespace workflow {

void IDevice::SaveXml(IXmlNode& root) const {
  auto& device_root = root.AddNode("Device");

  device_root.SetAttribute("name", name_);
  device_root.SetAttribute("id", identity_);

  device_root.SetProperty("Name", name_);
  device_root.SetProperty("Description", description_);
  device_root.SetProperty("Identity", identity_);
  device_root.SetProperty("Protocol", protocol_);
  device_root.SetProperty("Bus", bus_);
}

void IDevice::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  description_ = root.Property<std::string>("Description");
  identity_ = root.Property<int>("Identity");
  protocol_ = root.Property<std::string>("Protocol");
  bus_ = root.Property<std::string>("Bus");
}

}  // namespace workflow