/*
* Copyright 2024 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/irunnerfactory.h"

namespace workflow {

bool IRunnerFactory::HasTemplate(const std::string &name) const {
  auto itr = template_list_.find(name);
  return itr != template_list_.cend();
}

const IRunner *IRunnerFactory::GetTemplate(const std::string &name) const {
  auto itr = template_list_.find(name);
  return itr != template_list_.cend() ? itr->second.get() : nullptr;
}

IRunner *IRunnerFactory::GetTemplate(const std::string &name) {
  auto itr = template_list_.find(name);
  return itr != template_list_.end() ? itr->second.get() : nullptr;
}


} // workflow