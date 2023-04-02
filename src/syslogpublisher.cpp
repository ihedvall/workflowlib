/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "syslogpublisher.h"
#include <boost/program_options.hpp>
#include <vector>
#include <util/syslogmessage.h>
#include <util/utilfactory.h>
#include "workflow/iworkflow.h"

#include "template_names.icc"

using namespace boost::program_options;

using namespace util::syslog;
namespace workflow {

SyslogPublisher::SyslogPublisher() {
  Name(kSyslogPublisher.data());
  Template(kSyslogPublisher.data());
  Description("TCP server that publish syslog messages");
  std::ostringstream temp;
  temp << "--slot=" << data_slot_ << " ";
  temp << "--address=" << address_ << " ";
  temp << "--port=" << port_ << " ";

  Arguments(temp.str());
}

SyslogPublisher::SyslogPublisher(const IRunner& source)
    : IRunner(source) {
  Template(kSyslogPublisher.data());
  ParseArguments();
}

void SyslogPublisher::Init() {
  IRunner::Init();
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
    const auto* data = workflow->GetData<SyslogMessage>(data_slot_);
    if (data == nullptr) {
      workflow->InitData<SyslogMessage>(data_slot_, nullptr);
    }
    IsOk(true);
  } else {
    LastError("Workflow is not attached yet.");
    IsOk(false);
  }
}

void SyslogPublisher::Tick() {
  IRunner::Tick();
  auto* workflow = GetWorkflow();
  if (workflow == nullptr) {
    return;
  }
  const auto* msg = workflow->GetData<SyslogMessage>(data_slot_);

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
    workflow->ClearData(data_slot_);
  }
  IRunner::Exit();
}

void SyslogPublisher::ParseArguments() {
  try {
    options_description desc("Available Arguments");
    desc.add_options() ("slot,S",
                       value<size_t>(&data_slot_),
                       "Slot index for data" );
    desc.add_options() ("address,A",
                       value<std::string>(&address_),
                       "Server address defines local or global access" );
    desc.add_options() ("port,P",
                       value<uint16_t>(&port_),
                       "Server IP port" );

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