/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/ievent.h"
#include <util/stringutil.h>
#include "workflow/iworkflow.h"

using namespace util::xml;

namespace workflow {

IEvent::~IEvent() {
  runner_.reset();
}

IEvent::IEvent(const IEvent& event)
:  name_(event.name_),
   description_(event.description_),
   type_(event.type_),
   parameter_(event.parameter_),
   period_(event.period_)
{
  if (event.runner_) {
    runner_ = std::make_unique<IRunner>(*event.runner_);
  }
}

bool IEvent::operator==(const IEvent& event) const {
  if (name_ != event.name_) return false;
  if (description_ != event.description_) return false;
  if (type_ != event.type_) return false;
  if (parameter_ != event.parameter_) return false;
  if (period_ != event.period_) return false;
  return true;
}

void IEvent::AddRunner(std::unique_ptr<IRunner>& runner) {
  runner_ = std::move(runner);
}

void IEvent::EventTypeAsString(const std::string& type) {
  IEvent temp;
  for (auto index = static_cast<int>(EventType::Internal);
       index <= static_cast<int>(EventType::Parameter);
       ++index) {
    temp.Type(static_cast<EventType>(index));
    const auto type_string = temp.EventTypeAsString();
    if (util::string::IEquals(type, type_string)) {
      Type(temp.Type());
      return;
    }
  }
}

std::string IEvent::EventTypeAsString() const {
  switch (Type()) {
    case EventType::Internal:
      return "Internal";

    case EventType::External:
      return "External";

    case EventType::Periodic:
      return "Periodic";

    case EventType::Parameter:
      return "Parameter";

    default:
      break;
  }
  return {};
}

void IEvent::SaveXml(IXmlNode& root) const {
  auto& event_root = root.AddNode("Event");
  event_root.SetAttribute("name", name_);
  event_root.SetProperty("Name", name_);
  event_root.SetProperty("Description", description_);
  event_root.SetProperty("Type", EventTypeAsString());
  event_root.SetProperty("Parameter", parameter_);
  event_root.SetProperty("Period", period_);
}

void IEvent::ReadXml(const IXmlNode& root) {
  name_ = root.Property<std::string>("Name");
  description_ = root.Property<std::string>("Description");
  EventTypeAsString( root.Property<std::string>("Type"));
  parameter_ = root.Property<std::string>("Parameter");
  period_ = root.Property<uint64_t>("Period", 1000);
}

void IEvent::Init() {}

void IEvent::Tick() {
  for (auto* workflow : workflow_list_) {
    if (workflow != nullptr) {
      workflow->Tick();
    }
  }
};

void IEvent::Exit() {
}

void IEvent::AttachWorkflow(IWorkflow* workflow){
  if (workflow != nullptr) {
    workflow_list_.emplace_back(workflow);
  }
}

void IEvent::DetachWorkflows() {
  workflow_list_.clear();
}

}  // namespace workflow