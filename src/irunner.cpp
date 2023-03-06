/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/irunner.h"
#include <util/stringutil.h>
#include <algorithm>
#include <ranges>
#include "workflow/iworkflow.h"
#include "initdirectorydata.h"
#include "scandirectorydata.h"
#include "sysloginput.h"

#include "template_names.icc"

using namespace util::xml;
using namespace util::string;

namespace workflow {
IRunner::IRunner(const IRunner& source)
: name_(source.name_),
  description_(source.description_),
  documentation_(source.documentation_),
  arguments_(source.arguments_),
  type_(source.type_),
  template_(source.template_),
  period_(source.period_),
  parameter_list_(source.parameter_list_) {
}

bool IRunner::operator==(const IRunner& runner) const {
  if (name_ != runner.name_) return false;
  if (description_ != runner.description_) return false;
  if (documentation_ != runner.documentation_) return false;
  if (arguments_ != runner.arguments_) return false;
  if (type_ != runner.type_) return false;
  if (template_ != runner.template_) return false;
  if (period_ != runner.period_) return false;
  const auto list_equal =
      std::ranges::equal(parameter_list_,runner.parameter_list_,
      [] (const auto* parameter1, const auto* parameter2) {
        if (parameter1 == nullptr && parameter2 == nullptr) return true;
        return parameter1 != nullptr && parameter2 != nullptr &&
          parameter1->Name() == parameter2->Name();
      });
  return list_equal;
}
void IRunner::TypeAsString(const std::string& type) {
  IRunner temp;
  for (auto index = static_cast<int>(RunnerType::InternalRunner);
       index <= static_cast<int>(RunnerType::PythonRunner);
       ++index) {
    temp.Type(static_cast<RunnerType>(index));
    const auto type_string = temp.TypeAsString();
    if (util::string::IEquals(type, type_string)) {
      Type(temp.Type());
      return;
    }
  }
}

std::string IRunner::TypeAsString() const {
  switch (Type()) {
    case RunnerType::InternalRunner:
      return "Internal";

    case RunnerType::DllRunner:
      return "DLL";

    case RunnerType::ExeRunner:
      return "Executable";

    case RunnerType::LuaRunner:
      return "Lua";

    case RunnerType::PythonRunner:
      return "Python";

    default:
      break;
  }
  return {};
}

void IRunner::Init() {
  is_ok_ = true;
}

void IRunner::Tick() {}
void IRunner::Exit() {}

void IRunner::SaveXml(IXmlNode& root) const {
  auto& runner_root = root.AddNode("Runner");
  runner_root.SetAttribute("name", name_);

  runner_root.SetProperty("Name", name_);
  runner_root.SetProperty("Description", description_);
  runner_root.SetProperty("Documentation", documentation_);
  runner_root.SetProperty("Arguments", arguments_);
  runner_root.SetProperty("Type", TypeAsString());
  runner_root.SetProperty("Template", template_);
  runner_root.SetProperty("Period", period_);
}

void IRunner::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  description_ = root.Property<std::string>("Description");
  documentation_ = root.Property<std::string>("Documentation");
  arguments_ = root.Property<std::string>("Arguments");
  TypeAsString(root.Property<std::string>("Type"));
  template_ = root.Property<std::string>("Template");
  period_ = root.Property<double>("Period");
}

void IRunner::AttachWorkflow(IWorkflow* workflow) {
  workflow_ = workflow;
}

std::unique_ptr<IRunner> IRunner::Create(const IRunner& source) {
  std::unique_ptr<IRunner> runner;
  const auto& template_name = source.Template();
  if (IEquals(template_name, kInitDirectory.data())) {
    auto temp = std::make_unique<InitDirectoryData>(source);
    runner = std::move(temp);
  } else if (IEquals(template_name, kScanDirectory.data())) {
    auto temp = std::make_unique<ScanDirectoryData>(source);
    runner = std::move(temp);
  } else if (IEquals(template_name, kSyslogInput.data())) {
    auto temp = std::make_unique<SyslogInput>(source);
    runner = std::move(temp);
  } else {
    runner = std::make_unique<IRunner>(source);
  }
  return runner;
}

}  // namespace workflow