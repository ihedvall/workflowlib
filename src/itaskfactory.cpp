/*
* Copyright 2024 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/itaskfactory.h"

namespace workflow {

bool ITaskFactory::HasTemplate(const std::string &name) const {
  auto itr = template_list_.find(name);
  return itr != template_list_.cend();
}

const ITask *ITaskFactory::GetTemplate(const std::string &name) const {
  auto itr = template_list_.find(name);
  return itr != template_list_.cend() ? itr->second.get() : nullptr;
}

ITask *ITaskFactory::GetTemplate(const std::string &name) {
  auto itr = template_list_.find(name);
  return itr != template_list_.end() ? itr->second.get() : nullptr;
}


} // workflow