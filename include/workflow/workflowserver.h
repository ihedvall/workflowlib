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
#include "workflow/iworkflow.h"
#include "workflow/irunner.h"
#include "workflow/irunnerfactory.h"

#include <util/ixmlnode.h>
#include <util/stringutil.h>

namespace workflow {

using WorkflowList = std::vector<std::unique_ptr<IWorkflow>>;
using RunnerFactoryList = std::vector<const IRunnerFactory*>;

using PropertyList = std::map<std::string, std::string,
      util::string::IgnoreCase>;

class WorkflowServer {
 public:
  WorkflowServer();
  virtual ~WorkflowServer() = default;

  WorkflowServer(const WorkflowServer& server);
  WorkflowServer& operator = (const WorkflowServer& server);
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
  void AddWorkflow(const IWorkflow& workflow);
  void DeleteWorkflow(const IWorkflow* workflow);
  [[nodiscard]] const IWorkflow* GetWorkflow(const std::string& name) const;
  [[nodiscard]] IWorkflow* GetWorkflow(const std::string& name);

  void MoveUp(const IWorkflow* workflow);
  void MoveDown(const IWorkflow* workflow);

  [[nodiscard]] const std::vector<const IRunnerFactory*>& Factories() const;;
  [[nodiscard]] std::map<std::string, const IRunner*> Templates() const;
  [[nodiscard]] const IRunner* GetTemplate(const std::string& name) const;
  [[nodiscard]] std::unique_ptr<IRunner> CreateRunner(const IRunner& templ) const;

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

  void AddRunnerFactory(const IRunnerFactory& factory);

 private:
  std::string name_;
  std::string description_;
  std::unique_ptr<ParameterContainer> parameter_container_;
  std::unique_ptr<EventEngine> event_engine_;
  WorkflowList workflow_list_;
  RunnerFactoryList factory_list_; ///< List of available runner factories
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
