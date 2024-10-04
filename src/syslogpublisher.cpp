/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "syslogpublisher.h"
#include <boost/program_options.hpp>
#include <vector>
#include <util/syslogmessage.h>
#include <util/utilfactory.h>
#include "workflow/workflow.h"

#include "template_names.icc"

using namespace boost::program_options;

using namespace util::syslog;
namespace workflow {

SyslogPublisher::SyslogPublisher() {
  Name(kSyslogPublisher.data());
  Template(kSyslogPublisher.data());
  Description("TCP server that publish syslog messages");
  std::ostringstream temp;
  temp << "--address=" << address_ << " ";
  temp << "--port=" << port_ << " ";

  Arguments(temp.str());
}

SyslogPublisher::SyslogPublisher(const ITask& source)
    : ITask(source) {
  Template(kSyslogPublisher.data());
  ParseArguments();
}

void SyslogPublisher::Init() {
  ITask::Init();
  ParseArguments();

  auto temp = util::UtilFactory::CreateSyslogServer(
        SyslogServerType::TcpPublisher);
  server_ = std::move(temp);

  server_->Name(Name());
  server_->Port(port_);
  server_->Address(address_);
  server_->Start();

  auto* workflow = GetWorkflow();
  if (workflow != nullptr) {
    const SyslogMessage empty_msg;
    workflow->InitData(empty_msg);
    IsOk(true);
  } else {
    LastError("Workflow is not attached yet.");
    IsOk(false);
  }
}

void SyslogPublisher::Tick() {
  ITask::Tick();
  auto* workflow = GetWorkflow();
  if (workflow == nullptr) {
    return;
  }
  const auto* msg = workflow->GetData<SyslogMessage>();

  if (msg == nullptr || !server_) {
    LastError("No syslog message found");
    IsOk(false);
    return;
  }
  server_->AddMsg(*msg);
}

void SyslogPublisher::Exit() {
  if (server_) {
    server_->Stop();
  }
  auto* workflow = GetWorkflow();
  if (workflow != nullptr) {
    workflow->ClearData();
  }
  ITask::Exit();
}

void SyslogPublisher::ParseArguments() {
  try {
    options_description desc("Available Arguments");
    desc.add_options() ("address,A",
                       value<std::string>(&address_),
                       "Server address defines local or global access" );
    desc.add_options() ("port,P",
                       value<uint16_t>(&port_),
                       "Server IP port" );

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