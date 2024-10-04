/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "runsyslogschedule.h"
#include <boost/program_options.hpp>
#include <vector>
#include <util/syslogmessage.h>
#include "workflow/workflow.h"

#include "template_names.icc"

using namespace boost::program_options;
using namespace util::string;
using namespace util::syslog;
using SyslogList = std::vector<util::syslog::SyslogMessage>;

namespace workflow {

RunSyslogSchedule::RunSyslogSchedule() {
  Name(kRunSyslogSchedule.data());
  Template(kRunSyslogSchedule.data());
  Description("Task that forward syslog message to another schedule");
  std::ostringstream temp;
  temp << "--name=" << schedule_name_ << " ";

  Arguments(temp.str());
}

RunSyslogSchedule::RunSyslogSchedule(const ITask& source)
    : ITask(source) {
  Template(kRunSyslogSchedule.data());
  ParseArguments();
}

void RunSyslogSchedule::Init() {
  ITask::Init();
  ParseArguments();
  IsOk(true);
}

void RunSyslogSchedule::Tick() {
  ITask::Tick();
  auto* workflow = GetWorkflow();
  if (workflow == nullptr) {
    LastError("No workflow found");
    IsOk(false);
    return;
  }

  auto* remote = workflow->GetWorkflow(schedule_name_);
  if (remote == nullptr) {
    LastError("No remote schedule found");
    IsOk(false);
    return;
  }

  auto* syslog_list = workflow->GetData<SyslogList>();
  if (syslog_list == nullptr) {
    LastError("No syslog list found");
    IsOk(false);
    return;
  }

  auto* remote_msg = remote->GetData<SyslogMessage>();
  if (remote_msg == nullptr) {
    LastError("No remote data found");
    IsOk(false);
    return;
  }

  for (const auto& msg : *syslog_list) {
    *remote_msg = msg;
    remote->Tick();
  }
}

void RunSyslogSchedule::ParseArguments() {
  try {
    options_description desc("Available Arguments");
    desc.add_options() ("name,N",
                       value<std::string>(&schedule_name_),
                       "Name of schedule to run" );

    const auto arg_list = split_unix(Arguments());
    basic_command_line_parser parser(arg_list);
    parser.options(desc);

    const auto opt = parser.run();
    variables_map var_list;
    store(opt,var_list);
    notify(var_list);
    IsOk(true);
  } catch( const std::exception& err) {
    std::ostringstream msg;
    msg << "Initialization error. Error: " << err.what();
    LastError(msg.str());
    IsOk(false);
  }
}

}  // namespace workflow