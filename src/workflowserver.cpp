/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/workflowserver.h"

namespace workflow {
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
}  // namespace workflow