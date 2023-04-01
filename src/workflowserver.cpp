/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/workflowserver.h"
#include <algorithm>
#include <ranges>
#include <vector>
#include <memory>
#include <util/stringutil.h>
#include "initdirectorydata.h"
#include "scandirectorydata.h"
#include "sysloginput.h"

#include "template_names.icc"

using namespace util::xml;
using namespace util::string;
namespace workflow {

WorkflowServer::WorkflowServer() {
 auto temp1 = std::make_unique<EventEngine>();
 event_engine_ = std::move(temp1);

 auto temp2 = std::make_unique<ParameterContainer>();
 parameter_container_ = std::move(temp2);

 WorkflowServer::CreateDefaultTemplates();
}

WorkflowServer::WorkflowServer(const WorkflowServer& server)
: name_(server.name_),
  description_(server.description_)
{
  if (server.parameter_container_) {
    parameter_container_ =
        std::make_unique<ParameterContainer>(*server.parameter_container_);
  }
  if (server.event_engine_) {
    event_engine_ = std::make_unique<EventEngine>(*server.event_engine_);
  }
  for (const auto& workflow : server.workflow_list_) {
    if (!workflow) {
      continue;
    }
    auto temp = std::make_unique<IWorkflow>(*workflow);
    workflow_list_.push_back(std::move(temp));
  }
  for (const auto& tmpl : server.template_list_) {
    if (!tmpl.second) continue;
    AddTemplate(*tmpl.second);
  }
}

WorkflowServer& WorkflowServer::operator=(const WorkflowServer& server) {
  if (this == &server) {
    return *this;
  }
  name_ = server.name_;
  description_ = server.description_;

  parameter_container_.reset();
  if (server.parameter_container_) {
    auto temp = std::make_unique<ParameterContainer>(
        *server.parameter_container_);
    parameter_container_ = std::move(temp);
  }

  event_engine_.reset();
  if (server.event_engine_) {
    auto temp = std::make_unique<EventEngine>(
        *server.event_engine_);
    event_engine_ = std::move(temp);
  }

  workflow_list_.clear();
  for (const auto& workflow : server.workflow_list_) {
    if (!workflow) continue;
    auto temp = std::make_unique<IWorkflow>(*workflow);
    workflow_list_.push_back(std::move(temp));
  }

  template_list_.clear();
  for (const auto& tmpl : server.template_list_) {
    if (!tmpl.second) continue;
    AddTemplate(*tmpl.second);
  }
  return *this;
}

bool WorkflowServer::operator==(const WorkflowServer& server) const {
  if (name_ != server.name_) return false;
  if (description_ != server.description_) return false;

  if (!parameter_container_ && server.parameter_container_) return false;
  if (parameter_container_ && !server.parameter_container_) return false;
  if (parameter_container_ && server.parameter_container_ ) {
    if (!(*parameter_container_ == *server.parameter_container_)) {
      return false;
    }
  }

  if (!event_engine_ && server.event_engine_) return false;
  if (event_engine_ && !server.event_engine_) return false;
  if (event_engine_ && server.event_engine_ ) {
    if (!(*event_engine_ == *server.event_engine_)) {
      return false;
    }
  }

  const auto workflow_equal = std::ranges::equal(
      workflow_list_, server.workflow_list_,
      [] (const auto& workflow1, const auto& workflow2) {
        if (!workflow1 && !workflow2) return true;
        return workflow1 && workflow2 && (*workflow1 == *workflow2);
      });
  if (!workflow_equal) {
    return false;
  }

  const auto template_equal = std::ranges::equal(
      template_list_, server.template_list_,
      [] (const auto& runner1, const auto& runner2) {
        if (!runner1.second && !runner2.second) return true;
        return runner1.second && runner2.second &&
               (*runner1.second == *runner2.second);
      });

  return template_equal;
}

void WorkflowServer::SetParameterContainer(
    std::unique_ptr<ParameterContainer>& parameters) {
  parameter_container_ = std::move(parameters);
}

ParameterContainer* WorkflowServer::GetParameterContainer() {
  return parameter_container_.get();
}

const ParameterContainer* WorkflowServer::GetParameterContainer() const {
  return parameter_container_.get();
}

void WorkflowServer::SetEventEngine(
    std::unique_ptr<EventEngine>&events) {
  event_engine_ = std::move(events);
}

EventEngine* WorkflowServer::GetEventEngine() {
  return event_engine_.get();
}

const EventEngine* WorkflowServer::GetEventEngine() const {
  return event_engine_.get();
}

void WorkflowServer::AddWorkflow(const IWorkflow& workflow) {
  auto temp = std::make_unique<IWorkflow>(workflow);
  auto itr = std::ranges::find_if(workflow_list_, [&] (const auto& item) {
     return item && util::string::IEquals(item->Name(), workflow.Name());
  });
  if (itr == workflow_list_.end()) {
    workflow_list_.emplace_back(std::move(temp));
  } else {
    *itr = std::move(temp);
  }
}

void WorkflowServer::DeleteWorkflow(const IWorkflow* workflow) {
  auto itr = std::ranges::find_if(workflow_list_, [&] (const auto& item) {
    return item.get() == workflow;
  });
  if (itr != workflow_list_.end()) {
    workflow_list_.erase(itr);
  }
}

const IWorkflow* WorkflowServer::GetWorkflow(const std::string& name) const {
  const auto itr = std::ranges::find_if(workflow_list_,
        [&] (const auto& workflow) {
        return workflow && IEquals(name, workflow->Name()); });
  return itr != workflow_list_.cend() ? itr->get() : nullptr;
}

IWorkflow* WorkflowServer::GetWorkflow(const std::string& name) {
  auto itr = std::ranges::find_if(workflow_list_,
                                        [&] (const auto& workflow) {
                                          return workflow && IEquals(name, workflow->Name()); });
  return itr != workflow_list_.end() ? itr->get() : nullptr;
}

void WorkflowServer::AddTemplate(const IRunner& temp) {
  auto runner = CreateRunner(temp);
  if (!runner) {
    return;
  }
  auto itr = template_list_.find(temp.Name());
  if (itr == template_list_.end()) {
    template_list_.emplace(runner->Name(), std::move(runner));
  } else if (!IEquals(temp.Name(), temp.Template()) ) {
    // Found a template with this name. If name and template_name is the same,
    // it is better to get the latest template instead of the old one
    itr->second = std::move(runner);
  }
}

void WorkflowServer::DeleteTemplate(const IRunner* temp) {
  auto itr = std::ranges::find_if(template_list_, [&] (const auto& item) {
    return item.second.get() == temp;
  });
  if (itr != template_list_.end()) {
    template_list_.erase(itr);
  }
}

const IRunner* WorkflowServer::GetTemplate(const std::string& name) const {
  const auto itr = std::ranges::find_if(template_list_, [&] (const auto& item) {
    return item.second && IEquals(name, item.second->Name()); });
  return itr != template_list_.cend() ? itr->second.get() : nullptr;
}

IRunner* WorkflowServer::GetTemplate(const std::string& name) {
  auto itr = std::ranges::find_if(template_list_, [&] (const auto& item) {
    return item.second && IEquals(name, item.second->Name()); });
  return itr != template_list_.end() ? itr->second.get() : nullptr;
}

void WorkflowServer::Init() {

  // Associate the event with its workflow
  for (auto& workflow : workflow_list_) {
    if (!workflow) {
      continue;
    }


    const auto& event_name = workflow->StartEvent();
    auto* event = event_engine_->GetEvent(event_name);
    if (event == nullptr) {
      continue;
    }
    event->AttachWorkflow(workflow.get());
  }

  if (parameter_container_) {
    parameter_container_->Init();
  }

  if (event_engine_) {
    event_engine_->Init();
  }
}

void WorkflowServer::Tick() {
  if (parameter_container_) {
    parameter_container_->Tick();
  }
  if (event_engine_) {
    event_engine_->Tick();
  }
}

void WorkflowServer::Exit() {
  if (parameter_container_) {
    parameter_container_->Exit();
  }

  if (event_engine_) {
    event_engine_->Exit();
    event_engine_->DetachWorkflows();
  }

  for (auto& workflow : workflow_list_) {
    if (!workflow) {
      continue;
    }
  }
}

void WorkflowServer::SaveXml(IXmlNode& root) const {
  // Remove old node if it exists
  const auto* old_root = root.GetNode("Engine");
  if (old_root != nullptr) {
    root.DeleteNode(old_root);
  }
  auto& engine_root = root.AddNode("Engine");
  engine_root.SetAttribute("name", name_);

  engine_root.SetProperty("Name", name_);
  engine_root.SetProperty("Description", description_);

  if (parameter_container_) {
    parameter_container_->SaveXml(engine_root);
  }

  if (event_engine_) {
    event_engine_->SaveXml(engine_root);
  }

  if (!workflow_list_.empty()) {
    auto& workflow_root = engine_root.AddNode("WorkflowList");
    for (const auto& workflow : workflow_list_) {
      if (!workflow) continue;
      workflow->SaveXml(workflow_root);
    }
  }
  if (!template_list_.empty()) {
    auto& template_root = engine_root.AddNode("TemplateList");
    for (const auto& runner : template_list_) {
      if (!runner.second) continue;
      runner.second->SaveXml(template_root);
    }
  }

}

void WorkflowServer::ReadXml(const IXmlNode& root) {
  const auto* engine_root = root.GetNode("Engine");
  if (engine_root == nullptr) {
    return;
  }

  name_ = engine_root->Property<std::string>("Name");
  description_ = engine_root->Property<std::string>("Description");
  if (parameter_container_) {
    parameter_container_->ReadXml(*engine_root);
  }
  if (event_engine_) {
    event_engine_->ReadXml(*engine_root);
  }

  workflow_list_.clear();
  const auto* workflow_root = engine_root->GetNode("WorkflowList");
  if (workflow_root != nullptr) {
    IXmlNode::ChildList list;
    workflow_root->GetChildList(list);
    for (const auto* workflow : list) {
      if (workflow == nullptr || !workflow->IsTagName("Workflow")) {
        continue;
      }
      auto flow = std::make_unique<IWorkflow>(this);
      flow->ReadXml(*workflow);
      workflow_list_.emplace_back(std::move(flow));
    }
  }

  // Do not clear template_list_ here.
  const auto* template_root = engine_root->GetNode("TemplateList");
  if (template_root != nullptr) {
    IXmlNode::ChildList list;
    template_root->GetChildList(list);
    for (const auto* item : list) {
      if (item == nullptr || !item->IsTagName("Runner")) {
        continue;
      }
      auto runner = std::make_unique<IRunner>();
      runner->ReadXml(*item);
      template_list_.emplace(runner->Name(), std::move(runner));
    }
  }
}

void WorkflowServer::Clear() {
  name_.clear();
  description_.clear();
  if (parameter_container_) {
    parameter_container_->Clear();
  }
  if (event_engine_) {
    event_engine_->Clear();
  }
  workflow_list_.clear();
  template_list_.clear();
}

void WorkflowServer::MoveUp(const IWorkflow* workflow) {
  if (workflow == nullptr || workflow_list_.size() <= 1) {
    return;
  }

  auto itr = std::ranges::find_if(workflow_list_, [&] (const auto& item) {
    return item && item.get() == workflow;
  });
  if (itr == workflow_list_.end() || itr == workflow_list_.begin()) {
    return;
  }
  auto prev = itr;
  --prev;
  auto temp = std::move(*prev);
  *prev = std::move(*itr);
  *itr = std::move(temp);
}

void WorkflowServer::MoveDown(const IWorkflow* workflow) {
  if (workflow == nullptr || workflow_list_.size() <= 1) {
    return;
  }

  auto itr = std::ranges::find_if(workflow_list_, [&] (const auto& item) {
    return item && item.get() == workflow;
  });
  if (itr == workflow_list_.end()) {
    return;
  }
  auto next = itr;
  ++next;
  if (next == workflow_list_.end()) {
    return;
  }
  auto temp = std::move(*next);
  *next = std::move(*itr);
  *itr = std::move(temp);
}

std::unique_ptr<IRunner> WorkflowServer::CreateRunner(const IRunner& source) {
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

void WorkflowServer::CreateDefaultTemplates() {
  std::array<std::unique_ptr<IRunner>,3> temp_list = {
    std::make_unique<InitDirectoryData>(),
    std::make_unique<ScanDirectoryData>(),
    std::make_unique<SyslogInput>() };

  for (auto& temp : temp_list) {
    if (template_list_.find(temp->Name()) == template_list_.end()) {
      template_list_.emplace(temp->Name(),std::move(temp));
    }
  }


}
}  // namespace workflow