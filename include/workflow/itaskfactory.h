/*
* Copyright 2024 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include <map>
#include <string>
#include <memory>

#include <util/stringutil.h>
#include <boost/config.hpp>
#include "workflow/itask.h"


namespace workflow {

using TemplateList = std::map<std::string, std::unique_ptr<ITask>, util::string::IgnoreCase>;

class BOOST_SYMBOL_VISIBLE ITaskFactory {
 public:
  ITaskFactory() = default;
  virtual ~ITaskFactory() = default;

  [[nodiscard]] const std::string& Name() const {return name_;}
  [[nodiscard]] const std::string& Description() const {return description_;}

  [[nodiscard]] bool HasTemplate(const std::string& name) const;

  [[nodiscard]] const ITask* GetTemplate(const std::string& name) const;
  [[nodiscard]] ITask* GetTemplate(const std::string& name);

  [[nodiscard]] virtual std::unique_ptr<ITask> CreateRunner(const ITask& source) const = 0;

  [[nodiscard]] TemplateList& Templates() {return template_list_;}
  [[nodiscard]] const TemplateList& Templates() const {return template_list_;}

 protected:
  std::string name_;
  std::string description_;
  TemplateList template_list_;
};

} // workflow


