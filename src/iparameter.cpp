/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/iparameter.h"
#include <ranges>
#include <algorithm>
#include <cstring>
#include <util/stringutil.h>

using namespace util::xml;

namespace workflow {

IParameter::~IParameter() {

}

IParameter::IParameter(const IParameter& parameter)
: name_(parameter.name_),
  unit_(parameter.unit_),
  description_(parameter.description_),
  device_(parameter.device_),
  signal_(parameter.signal_),
  identity_(parameter.identity_),
  display_name_(parameter.display_name_),
  data_type_(parameter.data_type_),
  enum_list_(parameter.enum_list_)
{
}

bool IParameter::operator==(const IParameter& parameter) const {
  if (name_ != parameter.name_) return false;
  if (unit_ != parameter.unit_) return false;
  if (description_ != parameter.description_) return false;
  if (device_ != parameter.device_) return false;
  if (signal_ != parameter.signal_) return false;
  if (identity_ != parameter.identity_) return false;
  if (display_name_ != parameter.display_name_) return false;
  if (data_type_ != parameter.data_type_) return false;
  if (enum_list_ != parameter.enum_list_) return false;
  return true;
}

template <>
bool IParameter::GetValue(bool& value) const {
  std::scoped_lock lock(value_lock_);
  switch (DataType()) {
    case ParameterDataType::BooleanType:
    case ParameterDataType::UnsignedType:
      value = value_uint_ > 0;
      break;

    case ParameterDataType::SignedType:
      value = value_int_ > 0;
      break;

    case ParameterDataType::FloatType:
      value = value_float_ > 0.5;
      break;

    case ParameterDataType::StringType:
      value = false;
      if (!value_text_.empty()) {
        switch (value_text_[0]) {
          case 'T':  // "True"
          case 't':  // "true"
          case 'Y':  // "Yes"
          case 'y':  // "yes"
          case '1':  // "1"
            value = true;
            break;

          case 'O':  // "ON"
          case 'o':  // "on"
            value = value_text_.size() > 1 &&
                    (value_text_[1] == 'N' || value_text_[1] == 'n');
            break;

          case 'E':  // "Enabled"
          case 'e':  // "enabled"
            value = true;
            break;

          default:
            break;
        }
      }
      break;

    case ParameterDataType::EnumType: {
      const auto itr = enum_list_.find(value_int_);
      if (itr == enum_list_.cend()) {
        break;
      }
      const auto& temp = itr->second;
      if (!temp.empty()) {
        switch (temp[0]) {
          case 'T':  // "True"
          case 't':  // "true"
          case 'Y':  // "Yes"
          case 'y':  // "yes"
          case '1':  // "1"
            value = true;
            break;

          case 'O':  // "ON"
          case 'o':  // "on"
            value = temp.size() > 1 && (temp[1] == 'N' || temp[1] == 'n');
            break;

          case 'E':  // "Enabled"
          case 'e':  // "enabled"
            value = true;
            break;

          default:
            break;
        }
      }
      break;
    }

    case ParameterDataType::ByteArrayType:
    default:
      break;
  }
  return valid_;
}

template <>
bool IParameter::GetValue(std::string& value) const {
  std::scoped_lock lock(value_lock_);
  switch (DataType()) {
    case ParameterDataType::BooleanType:
      value = value_uint_ ? "1" : "0";
      break;


    case ParameterDataType::SignedType:
      value = std::to_string(value_int_);
      break;

    case ParameterDataType::UnsignedType:
      value = std::to_string(value_uint_);
      break;

    case ParameterDataType::FloatType: {
      std::ostringstream temp;
      temp << value_float_;
      value = temp.str();
      break;
    }

    case ParameterDataType::StringType:
      value = value_text_;
      break;

    case ParameterDataType::EnumType: {
      const auto itr = enum_list_.find(value_int_);
      if (itr != enum_list_.cend()) {
        value = itr->second;
      }
      break;
    }

    case ParameterDataType::ByteArrayType: {
      const std::string text(reinterpret_cast<const char*>(
                               value_array_.data()),
                               value_array_.size());
      value = text;
      break;
    }

    default:
     break;
  }
  return valid_;
}

template <>
bool IParameter::GetValue(ByteArray& value) const {
  std::scoped_lock lock(value_lock_);

  switch (DataType()) {
    case ParameterDataType::BooleanType:
      value.resize(1);
      value[0] = value_uint_ > 0 ? 1 : 0;
      break;

    case ParameterDataType::SignedType:
      value.resize(sizeof(value_int_));
      memcpy(value.data(), &value_int_, value.size());
      break;

    case ParameterDataType::UnsignedType:
      value.resize(sizeof(value_uint_));
      memcpy(value.data(), &value_uint_, value.size());
      break;

    case ParameterDataType::FloatType:
      value.resize(sizeof(value_float_));
      memcpy(value.data(), &value_float_, value.size());
      break;

    case ParameterDataType::StringType:
      value.resize(value_array_.size());
      memcpy(value.data(), value_array_.data(), value.size());
      break;

    case ParameterDataType::EnumType: {
      const auto itr = enum_list_.find(value_int_);
      if (itr != enum_list_.cend()) {
        value.resize(itr->second.size());
        memcpy(value.data(), itr->second.data(), value.size());
      } else {
        value.clear();
     }
     break;
    }

    case ParameterDataType::ByteArrayType:
      value = value_array_;
      break;


    default:
      break;
  }
  return valid_;
}

template <>
void IParameter::SetValue(bool valid, const bool& value) {
  std::scoped_lock lock(value_lock_);
  valid_ = valid;

  switch (data_type_) {
    case ParameterDataType::BooleanType:
    case ParameterDataType::UnsignedType:
      value_uint_ = value ? 1 : 0;
      break;

    case ParameterDataType::EnumType:
    case ParameterDataType::SignedType:
      value_int_ = value ? 1 : 0;
      break;

    case ParameterDataType::FloatType:
      value_float_ = value ? 1.0 : 0.0;
      break;

    case ParameterDataType::StringType:
      value_text_ = value ? "1" : "0";
      break;


    case ParameterDataType::ByteArrayType:
      value_array_.resize(1);
      value_array_[0] = value ? 1 : 0;
       break;

    default:
      break;
  }
}

template <>
void IParameter::SetValue(bool valid, const std::string& value) {
  std::scoped_lock lock(value_lock_);
  valid_ = valid;

  switch (data_type_) {
    case ParameterDataType::BooleanType: {
      if (value.empty()) {
        value_uint_ = 0;
      } else {
        switch(value[0]) {
          case 'T': // True
          case 't':
          case 'Y': // Yes
          case 'y':
          case 'E': // "Enabled"
          case 'e':
          case '1':
            value_uint_ = 1;
            break;
          case 'O': // ON
          case 'o':
            if (value.size() > 1 && value[1] == 'N' || value[1] == 'n') {
              value_uint_ = 1;
            } else {
              value_uint_ = 0;
            }
            break;

          default:
            value_uint_ = 0;
            break;
        }

      }
      break;
    }

    case ParameterDataType::UnsignedType:
      try {
        value_uint_ = std::stoull(value);
      } catch (const std::exception&) {}
      break;

    case ParameterDataType::EnumType:
      try {
        const auto itr = std::ranges::find_if(enum_list_,
                                              [&] (const auto& enum_itr) {
          return enum_itr.second == value;
        });
        if (itr != enum_list_.cend()) {
          value_int_ = itr->first;
        } else {
          value_int_ = std::stoll(value);
        }
      } catch (const std::exception&) {}
      break;

    case ParameterDataType::SignedType:
      try {
        value_int_ = std::stoll(value);
      } catch (const std::exception&) {}
      break;

    case ParameterDataType::FloatType:
      try {
        value_float_ = std::stod(value);
      } catch (const std::exception&) {}
      break;

    case ParameterDataType::StringType:
      value_text_ = value;
      break;

    case ParameterDataType::ByteArrayType:
      value_array_.resize(value.size());
      memcpy(value_array_.data(),
             reinterpret_cast<const uint8_t*>(value.data()),
             value.size());
      break;

    default:
      break;
  }
}

template <>
void IParameter::SetValue(bool valid, const ByteArray& value) {
  std::scoped_lock lock(value_lock_);
  valid_ = valid;

  switch (data_type_) {
    case ParameterDataType::UnsignedType:
    case ParameterDataType::BooleanType:
      if (value.empty()) {
        value_uint_ = 0;
      } else {
        value_uint_ = value[0];
      }
      break;

    case ParameterDataType::EnumType:
    case ParameterDataType::SignedType:
      if (value.empty()) {
        value_int_ = 0;
      } else {
        value_int_ = value[0];
      }
      break;

    case ParameterDataType::FloatType:
      if (value.empty()) {
        value_float_ = 0;
      } else {
        value_float_ = value[0];
      }
      break;

    case ParameterDataType::StringType:
      value_text_.resize(value.size());
      memcpy(value_text_.data(), reinterpret_cast<const char*>(value.data()),
             value.size());
      break;

    case ParameterDataType::ByteArrayType:
      value_array_ = value;
      break;

    default:
      break;
  }
}

void IParameter::DataTypeAsString(const std::string& type) {
  IParameter temp;
  for (auto index = static_cast<int>(ParameterDataType::FloatType);
       index <= static_cast<int>(ParameterDataType::ByteArrayType);
       ++index) {
    temp.DataType(static_cast<ParameterDataType>(index));
    const auto type_string = temp.DataTypeAsString();
    if (util::string::IEquals(type, type_string)) {
      DataType(temp.DataType());
      return;
    }
  }
}

std::string IParameter::DataTypeAsString() const {
  switch (DataType()) {
    case ParameterDataType::FloatType:
       return "Float";

    case ParameterDataType::SignedType:
      return "Signed";

    case ParameterDataType::UnsignedType:
      return "Unsigned";

    case ParameterDataType::BooleanType:
      return "Boolean";

    case ParameterDataType::StringType:
      return "String";

    case ParameterDataType::EnumType:
      return "Enumerate";

    case ParameterDataType::ByteArrayType:
      return "Byte Array";

    default:
      break;
  }
  return {};
}

void IParameter::Valid(bool valid) {
  std::scoped_lock lock(value_lock_);
  valid_ = valid;
}

bool IParameter::Valid() const {
  std::scoped_lock lock(value_lock_);
  return valid_;
}

void IParameter::Init() {
  Valid(false);
}

void IParameter::Tick() {}

void IParameter::Exit() {
  Valid(false);
}

void IParameter::SaveXml(IXmlNode& root) const {
  auto& parameter_root = root.AddNode("Parameter");
  parameter_root.SetAttribute("name", name_);
  if (!identity_.empty()) {
    parameter_root.SetAttribute("id", identity_);
  }

  parameter_root.SetProperty("Name", name_);
  parameter_root.SetProperty("Unit", unit_);
  parameter_root.SetProperty("Description", description_);
  parameter_root.SetProperty("Device", device_);
  parameter_root.SetProperty("Signal", signal_);
  parameter_root.SetProperty("Identity", identity_);
  parameter_root.SetProperty("DisplayName", display_name_);
  parameter_root.SetProperty("DataType", DataTypeAsString());
  if (!enum_list_.empty()) {
    auto& enum_root = parameter_root.AddNode("EnumList");
    for (const auto& item : enum_list_) {
      auto& enum_node = enum_root.AddNode("Enum");
      enum_node.SetAttribute("id", item.first);
      enum_node.SetAttribute("value", item.second);
    }
  }
}

void IParameter::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  unit_ = root.Property<std::string>("Unit");
  description_ = root.Property<std::string>("Description");
  device_ = root.Property<std::string>("Device");
  signal_ = root.Property<std::string>("Signal");
  identity_ = root.Property<std::string>("Identity");
  display_name_ = root.Property<std::string>("DisplayName");
  DataTypeAsString( root.Property<std::string>("DataType"));
  const auto* enum_root = root.GetNode("EnumList");
  if (enum_root != nullptr) {
    enum_list_.clear();
    IXmlNode::ChildList list;
    enum_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Enum")) {
        continue;
      }
      const auto id = item->Attribute<int64_t>("id");
      const auto value = item->Attribute<std::string>("value");
      enum_list_.insert({id,value});
    }
  }
}


}  // namespace workflow