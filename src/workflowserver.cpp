/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/workflowserver.h"
#include <algorithm>
#include <ranges>

using namespace util::xml;
namespace workflow {

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
    if (!workflow) {
      auto temp = std::make_unique<IWorkflow>(*workflow);
      workflow_list_.push_back(std::move(temp));
    }
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

  const auto& workflow_equal = std::ranges::equal(
      workflow_list_, server.workflow_list_,
      [] (const auto& workflow1, const auto& workflow2) {
        if (!workflow1 && !workflow2) return true;
        return workflow1 && workflow2 && (*workflow1 == *workflow2); });

  return workflow_equal;
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

void WorkflowServer::Init() {
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

  auto& workflow_root = engine_root.AddNode("WorkflowList");
  for (const auto& workflow : workflow_list_) {
    workflow->SaveXml(workflow_root);
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
      auto flow = std::make_unique<IWorkflow>();
      flow->ReadXml(*workflow);
      workflow_list_.emplace_back(std::move(flow));
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
}


}  // namespace workflow