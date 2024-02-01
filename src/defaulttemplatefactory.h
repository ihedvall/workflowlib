/*
* Copyright 2024 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/irunnerfactory.h"

namespace workflow {

class DefaultTemplateFactory : public IRunnerFactory {
 public:
  DefaultTemplateFactory();
  static const IRunnerFactory& Instance();
  [[nodiscard]] std::unique_ptr<IRunner> CreateRunner(const IRunner& source) const override;
};

} // workflow
