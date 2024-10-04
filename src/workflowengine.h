/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include "workflow/workflow.h"
#include <memory>
#include <vector>
namespace workflow {
using WorkflowList = std::vector<std::unique_ptr<Workflow>>;
class WorkflowEngine {
 public:
  [[nodiscard]] WorkflowList& Workflows() {return workflows_;}
  [[nodiscard]] const WorkflowList& Workflows() const {return workflows_;}
 private:
  WorkflowList workflows_;
};

}  // namespace workflow
