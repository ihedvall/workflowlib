/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/workflow.h"
#include <algorithm>
#include <util/stringutil.h>
#include <workflow/workflowserver.h>

using namespace util::xml;
using namespace util::string;
namespace workflow {

Workflow::Workflow(WorkflowServer* server)
  : server_(server) {
}

Workflow::Workflow(const Workflow& workflow)
: name_(workflow.name_),
  description_(workflow.description_),
  start_event_(workflow.start_event_),
  server_(workflow.server_) {
  for ( const auto& runner : workflow.task_list_) {
    if (!runner) {
      continue;
    }
    auto temp = std::make_unique<ITask>(*runner);
    task_list_.push_back(std::move(temp));
  }
}

Workflow& Workflow::operator=(const Workflow& workflow) {
  if (this == & workflow) {
    return *this;
  }
  name_ = workflow.name_;
  description_ = workflow.description_;
  start_event_ = workflow.start_event_;
  task_list_.clear();
  for (const auto& task : workflow.task_list_) {
    if (!task) {
      continue;
    }
    auto temp = std::make_unique<ITask>(*task);
    task_list_.push_back(std::move(temp));
  }
  return *this;
}

bool Workflow::operator==(const Workflow& workflow) const {
  if (name_ != workflow.name_) return false;
  if (description_ != workflow.description_) return false;
  if (start_event_ != workflow.start_event_) return false;
  const auto task_equal =
      std::ranges::equal(task_list_, workflow.task_list_,
    [] (const auto& task1, const auto& task2) {
  if (!task1 && !task2) return true;
  return task1 && task2 && (*task1 == *task2);});
  return task_equal;
}

void Workflow::OnStart() {
  start_ = true;
  start_condition_.notify_all();
}

void Workflow::AddTask(const ITask& task) {
  auto temp = server_ != nullptr ? server_->CreateRunner(task) :
                                  std::make_unique<ITask>(task);
  task_list_.emplace_back(std::move(temp));
}

void Workflow::DeleteTask(const ITask* task) {
  auto itr = std::ranges::find_if(task_list_, [&] (const auto& item) {
    return item && item.get() == task;
  });
  if (itr != task_list_.end()) {
    task_list_.erase(itr);
  }
}

void Workflow::SaveXml(IXmlNode& root) const {
  auto& workflow_root = root.AddNode("Workflow");
  workflow_root.SetAttribute("name", name_);

  workflow_root.SetProperty("Name", name_);
  workflow_root.SetProperty("Description", description_);
  workflow_root.SetProperty("StartEvent", start_event_);

  auto& task_root = workflow_root.AddNode("TaskList");
  for (const auto& runner : task_list_) {
    runner->SaveXml(task_root);
  }

}

void Workflow::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  description_ = root.Property<std::string>("Description");
  start_event_ = root.Property<std::string>("StartEvent");

  task_list_.clear();
  // Check for old name runners
  const auto* runner_root = root.GetNode("RunnerList");
  if (runner_root != nullptr) {
    IXmlNode::ChildList list;
    runner_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Runner")) {
        continue;
      }
      auto runner = std::make_unique<ITask>();
      runner->ReadXml(*item);
      task_list_.push_back(std::move(runner));
    }
  }
  // Now check for new tasks
  const auto* task_root = root.GetNode("TaskList");
  if (task_root != nullptr) {
    IXmlNode::ChildList list;
    task_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Task")) {
        continue;
      }
      auto task = std::make_unique<ITask>();
      task->ReadXml(*item);
      task_list_.push_back(std::move(task));
    }
  }

}

const ITask* Workflow::GetTask(const std::string& name) const {
  const auto itr = std::ranges::find_if(task_list_, [&] (const auto& task) {
    return task && IEquals(name, task->Name());
  });
  return itr == task_list_.cend() ? nullptr : itr->get();
}

ITask* Workflow::GetTask(const std::string& name) {
  auto itr = std::ranges::find_if(task_list_, [&] (const auto& task) {
    return task && IEquals(name, task->Name());
  });
  return itr == task_list_.end() ? nullptr : itr->get();
}

void Workflow::MoveUp(const ITask* task) {
  if (task == nullptr || task_list_.size() <= 1) {
    return;
  }

  auto itr = std::ranges::find_if(task_list_, [&] (const auto& item) {
    return item && item.get() == task;
  });
  if (itr == task_list_.end() || itr == task_list_.begin()) {
    return;
  }
  auto prev = itr;
  --prev;
  auto temp = std::move(*prev);
  *prev = std::move(*itr);
  *itr = std::move(temp);
}

void Workflow::MoveDown(const ITask* task) {
  if (task == nullptr || task_list_.size() <= 1) {
    return;
  }

  auto itr = std::ranges::find_if(task_list_, [&] (const auto& item) {
    return item && item.get() == task;
  });
  if (itr == task_list_.end()) {
    return;
  }
  auto next = itr;
  ++next;
  if (next == task_list_.end()) {
    return;
  }
  auto temp = std::move(*next);
  *next = std::move(*itr);
  *itr = std::move(temp);
}
void Workflow::Init() {
  // Attach the runner to the workflow, so it can access workflow data
  for (const auto& itr : task_list_) {
    if (!itr) continue;
    itr->AttachWorkflow(this);
  }
}

void Workflow::Tick() {
  for (const auto& itr : task_list_) {
    if (!itr) continue;
    itr->Tick();
  }
}

void Workflow::Exit() {
  for (const auto& itr : task_list_) {
    if (!itr) continue;
    itr->AttachWorkflow(nullptr);
  }
}

void Workflow::ClearData() {
  data_.reset();
}

Workflow* Workflow::GetWorkflow(const std::string& schedule_name) {
  return server_ != nullptr ? server_->GetWorkflow(schedule_name) : nullptr;
}

const ITask* Workflow::GetTaskByTemplateName(
    const std::string& name) const {
  const auto itr = std::ranges::find_if(task_list_, [&] (const auto& task) {
    return task && IEquals(name, task->Template());
  });
  if (itr != task_list_.cend()) {
    return itr->get();
  }
  return nullptr;
}

}  // namespace workflow