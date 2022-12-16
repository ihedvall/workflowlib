/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <string>
#include "workflow/parametercontainer.h"
#include "workflow/eventengine.h"
#include "workflow/iworkflow.h"

namespace workflow {

using WorkflowList = std::vector<std::unique_ptr<IWorkflow>>;

class WorkflowServer {
 public:
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

  

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

 private:
  std::string name_;
  std::string description_;
  std::unique_ptr<ParameterContainer> parameter_container_;
  std::unique_ptr<EventEngine> event_engine_;
  WorkflowList workflow_list_;
};

}  // namespace workflow
