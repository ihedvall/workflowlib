/*
* Copyright 2024 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/itaskfactory.h"

namespace workflow {

class DefaultTemplateFactory : public ITaskFactory {
 public:
  DefaultTemplateFactory();
  static const ITaskFactory& Instance();
  [[nodiscard]] std::unique_ptr<ITask> CreateRunner(const ITask& source) const override;
};

} // workflow
