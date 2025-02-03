/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/workflowserver.h"
#include <algorithm>
#include <vector>
#include <memory>
#include <sstream>
#include <util/stringutil.h>
#include "defaulttemplatefactory.h"

using namespace util::xml;
using namespace util::string;
namespace workflow {

WorkflowServer::WorkflowServer() {
 auto temp1 = std::make_unique<EventEngine>();
 event_engine_ = std::move(temp1);

 auto temp2 = std::make_unique<ParameterContainer>();
 parameter_container_ = std::move(temp2);

 const auto& default_runner = DefaultTemplateFactory::Instance();
 factory_list_.emplace_back(&default_runner);
}

bool WorkflowServer::operator==(const WorkflowServer& server) const {
  if (name_ != server.name_) return false;
  if (description_ != server.description_) return false;
  if (property_list_ != server.property_list_) return false;

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
/*
  const auto template_equal = std::ranges::equal(
      template_list_, server.template_list_,
      [] (const auto& runner1, const auto& runner2) {
        if (!runner1.second && !runner2.second) return true;
        return runner1.second && runner2.second &&
               (*runner1.second == *runner2.second);
      });
*/
  return true;
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

void WorkflowServer::AddWorkflow(const Workflow& workflow) {
  auto temp = std::make_unique<Workflow>(workflow);
  auto itr = std::ranges::find_if(workflow_list_, [&] (const auto& item) {
     return item && util::string::IEquals(item->Name(), workflow.Name());
  });
  if (itr == workflow_list_.end()) {
    workflow_list_.emplace_back(std::move(temp));
  } else {
    *itr = std::move(temp);
  }
}

void WorkflowServer::DeleteWorkflow(const Workflow* workflow) {
  auto itr = std::ranges::find_if(workflow_list_, [&] (const auto& item) {
    return item.get() == workflow;
  });
  if (itr != workflow_list_.end()) {
    workflow_list_.erase(itr);
  }
}

const Workflow* WorkflowServer::GetWorkflow(const std::string& name) const {
  const auto itr = std::ranges::find_if(workflow_list_,
        [&] (const auto& workflow) {
        return workflow && IEquals(name, workflow->Name()); });
  return itr != workflow_list_.cend() ? itr->get() : nullptr;
}

Workflow* WorkflowServer::GetWorkflow(const std::string& name) {
  auto itr = std::ranges::find_if(workflow_list_,
                                        [&] (const auto& workflow) {
                                          return workflow && IEquals(name, workflow->Name()); });
  return itr != workflow_list_.end() ? itr->get() : nullptr;
}


std::map<std::string, const ITask*> WorkflowServer::Templates() const {
  std::map<std::string, const ITask*> template_list;
  for ( const auto* factory : factory_list_) {
    if ( factory == nullptr) {
      continue;
    }
    const auto& list = factory->Templates();
    for ( const auto& itr : list) {
      template_list.emplace(itr.first, itr.second.get());
    }
  }
  return template_list;
}

const ITask* WorkflowServer::GetTemplate(const std::string& name) const {
  for ( const auto* factory : factory_list_) {
    if (factory->HasTemplate(name)) {
      return factory->GetTemplate(name);
    }
  }
  return nullptr;
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
  // Save application properties that have been modified or added.
  // Application properties cannot be removed.
  if (!property_list_.empty()) {
    auto& application_root = root.AddUniqueNode("Application");
    for (const auto& prop : property_list_) {
      auto key = prop.first;
      const auto& value = prop.second;
      const auto sep = key.find('/');
      std::string grp;
      if (sep != std::string::npos) {
        grp = key.substr(0,sep);
        key = key.substr(sep + 1);
      }
      if (grp.empty()) {
        application_root.SetProperty(key, value);
      } else if (!key.empty()){
        auto& group = application_root.AddUniqueNode(grp);
        group.SetProperty(key, value);
      }
    }
  }

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
}

void WorkflowServer::ReadXml(const IXmlNode& root) {
  const auto* application_root = root.GetNode("Application");
  if (application_root != nullptr) {
    property_list_.clear();
    IXmlNode::ChildList group_list;
    application_root->GetChildList(group_list);
    for (const auto* group : group_list) {
      if (group == nullptr) {
        continue;
      }
      std::ostringstream key;
      key << group->TagName();
      IXmlNode::ChildList key_list;
      group->GetChildList(key_list);
      if (key_list.empty()) {
        // No more items. Create property
        auto value = group->Value<std::string>();
        if (value.empty()) {
          value = group->Attribute<std::string>("value");
        }
        property_list_.emplace(key.str(), value);
      } else {
        IXmlNode::ChildList value_list;
        group->GetChildList(value_list);
        for (const auto* value_tag : value_list) {
          if (value_tag == nullptr) {
            continue;
          }
          key << "/" << value_tag->TagName();
          auto value = value_tag->Value<std::string>();
          if (value.empty()) {
            value = value_tag->Attribute<std::string>("value");
          }
          property_list_.emplace(key.str(), value);
        }
      }
    }
  }

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
      auto flow = std::make_unique<Workflow>(this);
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

void WorkflowServer::MoveUp(const Workflow* workflow) {
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

void WorkflowServer::MoveDown(const Workflow* workflow) {
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

std::unique_ptr<ITask> WorkflowServer::CreateRunner(const ITask &templ) const {
  const std::string& template_name = templ.Template();
  for (const auto* factory : factory_list_) {
    if (factory == nullptr) {
      continue;
    }
    if (factory->HasTemplate(template_name)) {
      return factory->CreateTask(templ);
    }
  }
  return {};
}

void WorkflowServer::AddTaskFactory(const ITaskFactory &factory) {
  // First check if factory already added
  const std::string& name = factory.Name();
  for (const auto* factory1 : factory_list_) {
    if (factory1 == nullptr) {
      continue;
    }
    if (factory1->Name() == name) {
      return;
    }
  }
  factory_list_.emplace_back(&factory);
}

const std::vector<const ITaskFactory *> &WorkflowServer::Factories() const {
  return factory_list_;
}

template <>
std::string WorkflowServer::GetApplicationProperty(
    const std::string& key) const {
  const auto itr = property_list_.find(key);
  return itr != property_list_.cend() ? itr->second : std::string();
}

template <>
bool WorkflowServer::GetApplicationProperty(
    const std::string& key) const {
  bool value = false;
  if (const auto itr = property_list_.find(key); itr != property_list_.cend()) {
    if (const auto& text = itr->second; !text.empty()) {
      switch (text[0]) {
        case 't':
        case 'T':
        case 'Y':
        case 'y':
        case '1':
          value = true;
          break;
        default:
          break;
      }
    }
  }
  return value;
}

}  // namespace workflow