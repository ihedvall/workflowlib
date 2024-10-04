/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <string>

namespace util::xml {

class IXmlNode;

} // end namespace util::xml

namespace workflow {

class Device {
 public:
  [[nodiscard]] bool operator == (const Device& device) const = default;
  [[nodiscard]] bool operator < (const Device& device) const;

  void Name(const std::string& name) {name_ = name;}
  [[nodiscard]] const std::string& Name() const {return name_;}

  void Description(const std::string& desc) {description_ = desc;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  void Identity(int identity) {identity_ = identity;}
  [[nodiscard]] int Identity() const {return identity_;}

  void Protocol(const std::string& protocol) { protocol_ = protocol; }
  [[nodiscard]] const std::string& Protocol() const { return protocol_; }

  void Bus(const std::string& bus) { bus_ = bus; }
  [[nodiscard]] const std::string& Bus() const { return bus_; }

  void SaveXml(util::xml::IXmlNode& root) const;
  void ReadXml(const util::xml::IXmlNode& root);

 private:
  std::string name_;
  std::string description_;
  int identity_ = 0;
  std::string protocol_;
  std::string bus_;
};

}  // namespace workflow
