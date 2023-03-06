/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/iworkflow.h"
#include <algorithm>
#include <ranges>
#include <util/stringutil.h>

using namespace util::xml;
using namespace util::string;
namespace workflow {

IWorkflow::IWorkflow(const IWorkflow& workflow)
: name_(workflow.name_),
  description_(workflow.description_),
  start_event_(workflow.start_event_) {
  for ( const auto& runner : workflow.runner_list_) {
    if (!runner) {
      continue;
    }
    auto temp = std::make_unique<IRunner>(*runner);
    runner_list_.push_back(std::move(temp));
  }
}

IWorkflow& IWorkflow::operator=(const IWorkflow& workflow) {
  if (this == & workflow) {
    return *this;
  }
  name_ = workflow.name_;
  description_ = workflow.description_;
  start_event_ = workflow.start_event_;
  runner_list_.clear();
  for (const auto& runner : workflow.runner_list_) {
    if (!runner) {
      continue;
    }
    auto temp = std::make_unique<IRunner>(*runner);
    runner_list_.push_back(std::move(temp));
  }
  return *this;
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

void IWorkflow::AddRunner(const IRunner& runner) {
  auto temp = IRunner::Create(runner);
  runner_list_.emplace_back(std::move(temp));
}

void IWorkflow::DeleteRunner(const IRunner* runner) {
  auto itr = std::ranges::find_if(runner_list_, [&] (const auto& item) {
    return item && item.get() == runner;
  });
  if (itr != runner_list_.end()) {
    runner_list_.erase(itr);
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

const IRunner* IWorkflow::GetRunner(const std::string& name) const {
  const auto itr = std::ranges::find_if(runner_list_, [&] (const auto& runner) {
    return runner && IEquals(name, runner->Name());
  });
  return itr == runner_list_.cend() ? nullptr : itr->get();
}

IRunner* IWorkflow::GetRunner(const std::string& name) {
  auto itr = std::ranges::find_if(runner_list_, [&] (const auto& runner) {
    return runner && IEquals(name, runner->Name());
  });
  return itr == runner_list_.end() ? nullptr : itr->get();
}

void IWorkflow::MoveUp(const IRunner* runner) {
  if (runner == nullptr || runner_list_.size() <= 1) {
    return;
  }

  auto itr = std::ranges::find_if(runner_list_, [&] (const auto& item) {
    return item && item.get() == runner;
  });
  if (itr == runner_list_.end() || itr == runner_list_.begin()) {
    return;
  }
  auto prev = itr;
  --prev;
  auto temp = std::move(*prev);
  *prev = std::move(*itr);
  *itr = std::move(temp);
}

void IWorkflow::MoveDown(const IRunner* runner) {
  if (runner == nullptr || runner_list_.size() <= 1) {
    return;
  }

  auto itr = std::ranges::find_if(runner_list_, [&] (const auto& item) {
    return item && item.get() == runner;
  });
  if (itr == runner_list_.end()) {
    return;
  }
  auto next = itr;
  ++next;
  if (next == runner_list_.end()) {
    return;
  }
  auto temp = std::move(*next);
  *next = std::move(*itr);
  *itr = std::move(temp);
}
void IWorkflow::Init() {
  // Attach the runner to the workflow, so it can access workflow data
  for (const auto& itr : runner_list_) {
    if (!itr) continue;
    itr->AttachWorkflow(this);
  }
}

void IWorkflow::Tick() {
  for (const auto& itr : runner_list_) {
    if (!itr) continue;
    itr->Tick();
  }
}

void IWorkflow::Exit() {
  for (const auto& itr : runner_list_) {
    if (!itr) continue;
    itr->AttachWorkflow(nullptr);
  }
}

void IWorkflow::ClearData(size_t index) {
  if (index < data_list_.size()) {
    data_list_[index].reset();
  }
}

}  // namespace workflow