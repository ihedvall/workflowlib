/*
* Copyright 2024 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "defaulttemplatefactory.h"

#include <array>
#include <memory>

#include <util/stringutil.h>
#include <boost/dll/alias.hpp>

#include "initdirectorydata.h"
#include "scandirectorydata.h"
#include "sysloginput.h"
#include "syslogpublisher.h"
#include "runsyslogschedule.h"

#include "template_names.icc"

using namespace util::string;

BOOST_DLL_ALIAS(workflow::DefaultTemplateFactory::Instance, GetRunnerFactory)

namespace workflow {

const ITaskFactory &DefaultTemplateFactory::Instance() {
  static DefaultTemplateFactory default_template_factory;
  return default_template_factory;
}

DefaultTemplateFactory::DefaultTemplateFactory() {
  name_ = "Default";
  description_ = "Default runner templates";

  std::array<std::unique_ptr<ITask>, 5> temp_list = {
      std::make_unique<InitDirectoryData>(),
      std::make_unique<ScanDirectoryData>(),
      std::make_unique<SyslogInput>(),
      std::make_unique<SyslogPublisher>(),
      std::make_unique<RunSyslogSchedule>()
  };

  for (auto& temp : temp_list) {
    if (template_list_.find(temp->Name()) == template_list_.end()) {
      template_list_.emplace(temp->Name(),std::move(temp));
    }
  }
}

std::unique_ptr<ITask> DefaultTemplateFactory::CreateRunner(const ITask &source) const {
  std::unique_ptr<ITask> runner;
  const auto& template_name = source.Template();
  if (IEquals(template_name, kInitDirectory.data())) {
    auto temp = std::make_unique<InitDirectoryData>(source);
    runner = std::move(temp);
  } else if (IEquals(template_name, kScanDirectory.data())) {
    auto temp = std::make_unique<ScanDirectoryData>(source);
    runner = std::move(temp);
  } else if (IEquals(template_name, kSyslogInput.data())) {
    auto temp = std::make_unique<SyslogInput>(source);
    runner = std::move(temp);
  } else if (IEquals(template_name, kSyslogPublisher.data())) {
    auto temp = std::make_unique<SyslogPublisher>(source);
    runner = std::move(temp);
  } else if (IEquals(template_name, kRunSyslogSchedule.data())) {
    auto temp = std::make_unique<RunSyslogSchedule>(source);
    runner = std::move(temp);
  } else {
    runner = std::make_unique<ITask>(source);
  }
  return runner;
}

} // workflow

