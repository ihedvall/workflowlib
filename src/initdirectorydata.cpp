/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "initdirectorydata.h"
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

InitDirectoryData::InitDirectoryData() {
  Name(kInitDirectory.data());
  Template(kInitDirectory.data());
  Description("Initiate workflow directory data");
  std::ostringstream temp;
  temp << "--include-filter=" << include_filter_ << " ";
  temp << "--exclude-filter=" << exclude_filter_ << " ";
  temp << "--root-dir=" << root_dir_ << " ";
  Arguments(temp.str());
}

InitDirectoryData::InitDirectoryData(const IRunner& source)
: IRunner(source) {
  Template(kInitDirectory.data());
  ParseArguments();
}

void InitDirectoryData::Init() {
  IRunner::Init();
  ParseArguments();
}

void InitDirectoryData::Tick() {
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
    auto* data = workflow->GetData<IDirectory>();
    if (data == nullptr) {
      // The initializing of directory is placed here instead of in the Init()
      // due to network error
      IDirectory dir;
      dir.Directory(root_dir_);
      dir.StringToIncludeList(include_filter_);
      dir.StringToExcludeList(exclude_filter_);
      const auto create = workflow->InitData<IDirectory>(dir);
      if (!create) {
        LastError("Failed to init the directory data");
        IsOk(false);
      } else {
        IsOk(true);
      }
    } else {
      IsOk(true);
    }
  } catch (const std::exception& err) {
    std::ostringstream msg;
    msg << "Internal error. Error: " << err.what();
    IsOk(false);
  }

}

void InitDirectoryData::ParseArguments() {
  try {
    options_description desc("Available Arguments");
//    desc.add_options() ("help,H", "Help");
    desc.add_options() ("include-filter,I",
                       value<std::string>(&include_filter_),
                       "Include filter string list" );
    desc.add_options() ("exclude-filter,E",
                       value<std::string>(&exclude_filter_),
                      "Exclude filter" );
    desc.add_options() ("root-dir,R",
                       value<std::string>(&root_dir_),
                       "Root directory" );

    const auto arg_list = split_unix(Arguments());
    basic_command_line_parser parser(arg_list);
    parser.options(desc);
    const auto opt = parser.run();
    variables_map var_list;
    store(opt,var_list);
    notify(var_list);

    const path dir(root_dir_);
    if (root_dir_.empty()) {
      LastError("The root directory is not defined");
      IsOk(false);
    } else if (!exists(dir)) {
      std::ostringstream msg;
      msg << "The directory doesn't exist. Directory: " << root_dir_;
      LastError(msg.str());
      IsOk(false);
    } else {
      IsOk(true);
    }
  } catch( const std::exception& err) {
    std::ostringstream msg;
    msg << "Initialization error. Error: " << err.what();
    LastError(msg.str());
    IsOk(false);
    return;
  }
}


}  // namespace workflow