/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "scandirectorydata.h"
#include <boost/program_options.hpp>

#include <filesystem>
#include <sstream>
#include <util/idirectory.h>
#include "workflow/iworkflow.h"

using namespace boost::program_options;
using namespace std::filesystem;
using namespace util::log;

#include "template_names.icc"

namespace workflow {
ScanDirectoryData::ScanDirectoryData() {
  Name(kScanDirectory.data());
  Template(kScanDirectory.data());
  Description("Scanning workflow directory data");
}

ScanDirectoryData::ScanDirectoryData(const IRunner& source)
    : IRunner(source) {
  Template(kScanDirectory.data());
  ParseArguments();
}

void ScanDirectoryData::Init() {
  IRunner::Init();
  ParseArguments();
}

void ScanDirectoryData::Tick() {
  IRunner::Tick();

  // Check that the workflow is attached
  auto* workflow = GetWorkflow();
  if (workflow == nullptr) {
    LastError("No workflow connected");
    IsOk(false);
    return;
  }

  // Check that data exist in the workflow. If not create it
  try {
    auto* data = workflow->GetData<IDirectory>(data_slot_);
    if (data == nullptr) {
      LastError("Failed to get the directory data");
      IsOk(false);
      return;
    }

    const auto scan = data->ScanDirectory();
    if (!scan) {
      LastError(data->LastError());
      IsOk(false);
    }
    IsOk(true);
  } catch (const std::exception& err) {
    std::ostringstream msg;
    msg << "Internal error. Error: " << err.what();
    IsOk(false);
  }

}

void ScanDirectoryData::ParseArguments() {
  try {
    options_description desc("Available Arguments");
    //    desc.add_options() ("help,H", "Help");
    desc.add_options() ("slot,S",
                       value<size_t>(&data_slot_),
                       "Slot index for data" );

    const auto arg_list = split_winmain(Arguments());
    basic_command_line_parser parser(arg_list);
    parser.options(desc);
    const auto opt = parser.run();
    variables_map var_list;
    store(opt,var_list);
    notify(var_list);
  } catch( const std::exception& err) {
    std::ostringstream msg;
    msg << "Initialization error. Error: " << err.what();
    LastError(msg.str());
    IsOk(false);
    return;
  }
}

}  // namespace workflow