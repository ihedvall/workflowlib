/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "runsyslogschedule.h"
#include <boost/program_options.hpp>
#include <vector>
#include <util/syslogmessage.h>
#include "workflow/iworkflow.h"

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
  temp << "--input=" << in_slot_ << " ";
  temp << "--output" << out_slot_ << " ";
  temp << "--name=" << schedule_name_ << " ";

  Arguments(temp.str());
}

RunSyslogSchedule::RunSyslogSchedule(const IRunner& source)
    : IRunner(source) {
  Template(kRunSyslogSchedule.data());
  ParseArguments();
}

void RunSyslogSchedule::Init() {
  IRunner::Init();
  ParseArguments();
  IsOk(true);
}

void RunSyslogSchedule::Tick() {
  IRunner::Tick();
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

  auto* syslog_list = workflow->GetData<SyslogList>(in_slot_);
  if (syslog_list == nullptr) {
    LastError("No syslog list found");
    IsOk(false);
    return;
  }

  auto* remote_msg = remote->GetData<SyslogMessage>(out_slot_);
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
    desc.add_options() ("input,I",
                       value<size_t>(&in_slot_),
                       "Slot index for input data" );
    desc.add_options() ("output,O",
                       value<size_t>(&out_slot_),
                       "Slot index for output data" );
    desc.add_options() ("name,N",
                       value<std::string>(&schedule_name_),
                       "Name of schedule to run" );

    const auto arg_list = split_winmain(Arguments());
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