/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/iworkflow.h"
#include <algorithm>
#include <ranges>

using namespace util::xml;

namespace workflow {

IWorkflow::IWorkflow(const IWorkflow& workflow)
: name_(workflow.name_),
  description_(workflow.description_),
  start_event_(workflow.start_event_) {
  for ( const auto& runner : runner_list_) {
    if (!runner) {
      continue;
    }
    auto temp = std::make_unique<IRunner>(*runner);
    runner_list_.push_back(std::move(temp));
  }
}

bool IWorkflow::operator==(const IWorkflow& workflow) const {
  if (name_ != workflow.name_) return false;
  if (description_ != workflow.description_) return false;
  if (start_event_ != workflow.start_event_) return false;
  const auto runner_equal =
      std::ranges::equal(runner_list_, workflow.runner_list_,
    [] (const auto& runner1, const auto& runner2) {
  if (!runner1 && !runner2) return true;
  return runner1 && runner2 && (*runner1 == *runner2);});
  return runner_equal;
}

void IWorkflow::OnStart() {
  start_ = true;
  start_condition_.notify_all();
}

void IWorkflow::AddRunner(std::unique_ptr<IRunner>& runner) {
  if (runner) {
    runner_list_.emplace_back(std::move(runner));
  }
}

void IWorkflow::SaveXml(IXmlNode& root) const {
  auto& workflow_root = root.AddNode("Workflow");
  workflow_root.SetAttribute("name", name_);

  workflow_root.SetProperty("Name", name_);
  workflow_root.SetProperty("Description", description_);
  workflow_root.SetProperty("StartEvent", start_event_);

  auto& runner_root = workflow_root.AddNode("RunnerList");
  for (const auto& runner : runner_list_) {
    runner->SaveXml(runner_root);
  }

}

void IWorkflow::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  description_ = root.Property<std::string>("Description");
  start_event_ = root.Property<std::string>("StartEvent");

  runner_list_.clear();
  const auto* runner_root = root.GetNode("RunnerList");
  if (runner_root != nullptr) {
    IXmlNode::ChildList list;
    runner_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Runner")) {
        continue;
      }
      auto runner = std::make_unique<IRunner>();
      runner->ReadXml(*item);
      runner_list_.push_back(std::move(runner));
    }
  }
}



}  // namespace workflow