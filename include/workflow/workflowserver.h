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
#include <util/ixmlnode.h>
#include <util/stringutil.h>

namespace workflow {

using WorkflowList = std::vector<std::unique_ptr<IWorkflow>>;
using TemplateList = std::map<std::string, std::unique_ptr<IRunner>,
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

  [[nodiscard]] TemplateList& Templates() {return template_list_;}
  [[nodiscard]] const TemplateList& Templates() const {return template_list_;}
  void AddTemplate(const IRunner& temp);
  void DeleteTemplate(const IRunner* temp);
  [[nodiscard]] const IRunner* GetTemplate(const std::string& name) const;
  [[nodiscard]] IRunner* GetTemplate(const std::string& name);

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  virtual void ReadXml(const util::xml::IXmlNode& root);
  virtual void SaveXml(util::xml::IXmlNode& root) const;

  void Clear();

  [[nodiscard]] virtual std::unique_ptr<IRunner> CreateRunner(
      const IRunner& source);
  virtual void CreateDefaultTemplates();
 private:
  std::string name_;
  std::string description_;
  std::unique_ptr<ParameterContainer> parameter_container_;
  std::unique_ptr<EventEngine> event_engine_;
  WorkflowList workflow_list_;
  TemplateList template_list_;
};

}  // namespace workflow
