/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "sysloginput.h"
#include <boost/program_options.hpp>
#include <vector>
#include <util/syslogmessage.h>
#include <util/stringutil.h>
#include <util/utilfactory.h>
#include "workflow/iworkflow.h"

#include "template_names.icc"

using namespace boost::program_options;
using namespace util::string;
using namespace util::syslog;
using SyslogList = std::vector<util::syslog::SyslogMessage>;

namespace workflow {
SyslogInput::SyslogInput() {
  Name(kSyslogInput.data());
  Template(kSyslogInput.data());
  Description("Server that receives syslog messages");
  std::ostringstream temp;
  temp << "--slot=" << data_slot_ << " ";
  temp << "--address=" << address_ << " ";
  temp << "--port=" << port_ << " ";
  temp << "--type=" << type_ << " ";

  Arguments(temp.str());
}

SyslogInput::SyslogInput(const IRunner& source)
    : IRunner(source) {
  Template(kSyslogInput.data());
  ParseArguments();
}

void SyslogInput::Init() {
  IRunner::Init();
  ParseArguments();
  if (IEquals(type_, "TCP")) {
    auto temp = util::UtilFactory::CreateSyslogServer(
        SyslogServerType::TcpServer);
    server_ = std::move(temp);
  } else {
    auto temp = util::UtilFactory::CreateSyslogServer(
        SyslogServerType::UdpServer);
    server_ = std::move(temp);
  }

  server_->Name(Name());
  server_->Port(port_);
  server_->Address(address_);
  server_->Start();
  auto* workflow = GetWorkflow();
  if (workflow != nullptr) {
    workflow->InitData<SyslogList>(data_slot_, nullptr);
    IsOk(true);
  } else {
    LastError("Workflow is not attached yet.");
    IsOk(false);
  }
}

void SyslogInput::Tick() {
  IRunner::Tick();
  auto* workflow = GetWorkflow();
  auto* syslog_list = workflow != nullptr ?
                          workflow->GetData<SyslogList>(data_slot_) :
                          nullptr;
  if (syslog_list == nullptr || !server_) {
    LastError("No syslog list found");
    IsOk(false);
    return;
  }
  syslog_list->clear();
  for (auto msg = server_->GetMsg(false); msg; msg = server_->GetMsg(false)) {
    // Insert message into the database
    syslog_list->emplace_back(*msg);
    msg.reset();
  }
}

void SyslogInput::Exit() {
  if (server_) {
    server_->Stop();
  }
  auto* workflow = GetWorkflow();
  if (workflow != nullptr) {
    workflow->ClearData(data_slot_);
  }
  IRunner::Exit();
}

void SyslogInput::ParseArguments() {
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
    desc.add_options() ("type,T",
                       value<std::string>(&type_),
                       "Type of server (UDP, TCP or TLS" );

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