/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "scandirectorydata.h"
#include <boost/program_options.hpp>

#include <filesystem>
#include <sstream>
#include <util/idirectory.h>
#include "workflow/workflow.h"

using namespace boost::program_options;
using namespace std::filesystem;
using namespace util::log;

#include "template_names.icc"

namespace workflow {
ScanDirectoryData::ScanDirectoryData() {
  Name(kScanDirectory.data());
  Template(kScanDirectory.data());
  Description("Scanning workflow directory data");
  std::ostringstream temp;
  Arguments(temp.str());
}

ScanDirectoryData::ScanDirectoryData(const ITask& source)
    : ITask(source) {
  Template(kScanDirectory.data());
  ParseArguments();
}

void ScanDirectoryData::Init() {
  ITask::Init();
  ParseArguments();
}

void ScanDirectoryData::Tick() {
  ITask::Tick();

  // Check that the workflow is attached
  auto* workflow = GetWorkflow();
  if (workflow == nullptr) {
    LastError("No workflow connected");
    IsOk(false);
    return;
  }

  // Check that data exist in the workflow. If not create it
  try {
    auto* data = workflow->GetData<IDirectory>();
    if (data == nullptr) {
      LastError("Failed to get the directory data");
      IsOk(false);
      return;
    }

    const auto scan = data->ScanDirectory();
    if (!scan) {
      LastError(data->LastError());
      IsOk(false);
    } else {
      IsOk(true);
    }
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

    const auto arg_list = split_unix(Arguments());
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