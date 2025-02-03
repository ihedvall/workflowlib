/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <vector>
#include <map>
#include <string>

#include "workflow/parametercontainer.h"
#include "workflow/eventengine.h"
#include "workflow/workflow.h"
#include "workflow/itask.h"
#include "workflow/itaskfactory.h"

#include <util/ixmlnode.h>
#include <util/stringutil.h>

namespace workflow {

using WorkflowList = std::vector<std::unique_ptr<Workflow>>;
using TaskFactoryList = std::vector<const ITaskFactory*>;

using PropertyList = std::map<std::string, std::string,
      util::string::IgnoreCase>;

class WorkflowServer {
 public:
  WorkflowServer();
  virtual ~WorkflowServer() = default;

  WorkflowServer(const WorkflowServer& server) = delete;
  WorkflowServer& operator = (const WorkflowServer& server) = delete;

  [[nodiscard]] bool operator == (const WorkflowServer& server) const;

  void Name(const std::string& name) {name_ = name;}
  [[nodiscard]] const std::string& Name() const {return name_;}

  void Description(const std::string& desc) {description_ = desc;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  void SetParameterContainer(std::unique_ptr<ParameterContainer>& parameters);
  [[nodiscard]] ParameterContainer* GetParameterContainer();
  [[nodiscard]] const ParameterContainer* GetParameterContainer() const;

  void SetEventEngine(std::unique_ptr<EventEngine>& events);
  [[nodiscard]] EventEngine* GetEventEngine();
  [[nodiscard]] const EventEngine* GetEventEngine() const;

  [[nodiscard]] WorkflowList& Workflows() {return workflow_list_;}
  [[nodiscard]] const WorkflowList& Workflows() const {return workflow_list_;}
  void AddWorkflow(const Workflow& workflow);
  void DeleteWorkflow(const Workflow* workflow);
  [[nodiscard]] const Workflow* GetWorkflow(const std::string& name) const;
  [[nodiscard]] Workflow* GetWorkflow(const std::string& name);

  void MoveUp(const Workflow* workflow);
  void MoveDown(const Workflow* workflow);

  [[nodiscard]] const std::vector<const ITaskFactory*>& Factories() const;;
  [[nodiscard]] std::map<std::string, const ITask*> Templates() const;
  [[nodiscard]] const ITask* GetTemplate(const std::string& name) const;
  [[nodiscard]] std::unique_ptr<ITask> CreateRunner(const ITask& templ) const;

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  virtual void ReadXml(const util::xml::IXmlNode& root);
  virtual void SaveXml(util::xml::IXmlNode& root) const;

  void Clear();

  [[nodiscard]] const PropertyList& ApplicationProperties() const {
    return property_list_;
  }
  [[nodiscard]] PropertyList& ApplicationProperties() {
    return property_list_;
  }

  template <typename T>
  [[nodiscard]] T GetApplicationProperty(const std::string& key) const;

  void SetApplicationProperty(
      const std::string& key, const std::string& value) {
    if (auto itr = property_list_.find(key); itr != property_list_.end()) {
      itr->second = value;
    } else {
      property_list_.emplace(key,value);
    }
  }

  void AddTaskFactory(const ITaskFactory& factory);

 private:
  std::string name_;
  std::string description_;
  std::unique_ptr<ParameterContainer> parameter_container_;
  std::unique_ptr<EventEngine> event_engine_;
  WorkflowList workflow_list_;
  TaskFactoryList factory_list_; ///< List of available task factories
  PropertyList property_list_; ///< Application tag properties
};

template <typename T>
T WorkflowServer::GetApplicationProperty(const std::string& key) const {
  T value {};
  const auto itr = property_list_.find(key);
  try {
    if (itr != property_list_.cend()) {
      std::istringstream conv(itr->second);
      conv >> value;
    }
  } catch (const std::exception& ) {
  }
  return value;
}

template <>
std::string WorkflowServer::GetApplicationProperty(
    const std::string& key) const;


template <>
bool WorkflowServer::GetApplicationProperty(
    const std::string& key) const;
}  // namespace workflow
