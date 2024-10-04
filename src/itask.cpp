/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/itask.h"
#include <util/stringutil.h>
#include <algorithm>
#include "workflow/workflow.h"

using namespace util::xml;
using namespace util::string;

namespace workflow {
ITask::ITask(const ITask& source)
: name_(source.name_),
  description_(source.description_),
  documentation_(source.documentation_),
  arguments_(source.arguments_),
  type_(source.type_),
  template_(source.template_),
  period_(source.period_),
  parameter_list_(source.parameter_list_) {
}

bool ITask::operator==(const ITask& runner) const {
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
void ITask::TypeAsString(const std::string& type) {
  ITask temp;
  for (auto index = static_cast<int>(TaskType::InternalTask);
       index <= static_cast<int>(TaskType::PythonTask);
       ++index) {
    temp.Type(static_cast<TaskType>(index));
    const auto type_string = temp.TypeAsString();
    if (util::string::IEquals(type, type_string)) {
      Type(temp.Type());
      return;
    }
  }
}

std::string ITask::TypeAsString() const {
  switch (Type()) {
    case TaskType::InternalTask:
      return "Internal";

    case TaskType::DllTask:
      return "DLL";

    case TaskType::ExeTask:
      return "Executable";

    case TaskType::LuaTask:
      return "Lua";

    case TaskType::PythonTask:
      return "Python";

    default:
      break;
  }
  return {};
}

void ITask::Init() {
  is_ok_ = true;
}

void ITask::Tick() {}
void ITask::Exit() {}

void ITask::SaveXml(IXmlNode& root) const {
  auto& runner_root = root.AddNode("Task");
  runner_root.SetAttribute("name", name_);

  runner_root.SetProperty("Name", name_);
  runner_root.SetProperty("Description", description_);
  runner_root.SetProperty("Documentation", documentation_);
  runner_root.SetProperty("Arguments", arguments_);
  runner_root.SetProperty("Type", TypeAsString());
  runner_root.SetProperty("Template", template_);
  runner_root.SetProperty("Period", period_);
}

void ITask::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  description_ = root.Property<std::string>("Description");
  documentation_ = root.Property<std::string>("Documentation");
  arguments_ = root.Property<std::string>("Arguments");
  TypeAsString(root.Property<std::string>("Type"));
  template_ = root.Property<std::string>("Template");
  period_ = root.Property<double>("Period");
}

void ITask::AttachWorkflow(Workflow* workflow) {
  workflow_ = workflow;
}

const ITask* ITask::GetTaskByTemplateName(const std::string& name) const {
  return workflow_ != nullptr ? workflow_->GetTaskByTemplateName(name) :
    nullptr;
}

}  // namespace workflow