/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "workflowexplorer.h"

#include <wx/config.h>
#include <wx/utils.h>
#include <wx/wx.h>

#include <algorithm>
#include <boost/locale.hpp>
#include <boost/process.hpp>
#include <filesystem>

#include "mainframe.h"
#include "util/logconfig.h"
#include "util/logstream.h"
#include "util/ixmlfile.h"
#include "workflowexplorerid.h"

using namespace util::log;
using namespace std::filesystem;
using namespace util::xml;

namespace workflow::gui {

wxIMPLEMENT_APP(WorkflowExplorer);

wxBEGIN_EVENT_TABLE(WorkflowExplorer, wxApp)
  EVT_UPDATE_UI(kIdOpenLogFile,WorkflowExplorer::OnUpdateOpenLogFile)
  EVT_MENU(kIdOpenLogFile, WorkflowExplorer::OnOpenLogFile)
  EVT_MENU(wxID_OPEN, WorkflowExplorer::OnConfigFile)
  EVT_UPDATE_UI(wxID_SAVE,WorkflowExplorer::OnUpdateSaveFile)
  EVT_MENU(wxID_SAVE, WorkflowExplorer::OnSaveFile)
  EVT_UPDATE_UI(wxID_SAVEAS,WorkflowExplorer::OnUpdateSaveAsFile)
  EVT_MENU(wxID_SAVEAS, WorkflowExplorer::OnSaveAsFile)
wxEND_EVENT_TABLE()


bool WorkflowExplorer::OnInit() {
  if (!wxApp::OnInit()) {
    return false;
  }
  ::wxInitAllImageHandlers();

  // Setup correct localization when formatting date and times
  boost::locale::generator gen;
  std::locale::global(gen(""));

  // Setup system basic configuration
  SetVendorDisplayName("Report Server");
  SetVendorName("ReportServer");

  SetAppDisplayName("Workflow Explorer");
  SetAppName("WorkflowExplorer");

  // Set up the log file.
  // The log file will be in c:/programdata/report_server/workflow_explorer.log

  auto& log_config = LogConfig::Instance();
  log_config.Type(LogType::LogToFile);
  log_config.SubDir("report_server/log");
  log_config.BaseName("workflow_explorer");
  log_config.CreateDefaultLogger();
  LOG_DEBUG() << "Log File created. Path: " << log_config.GetLogFile();

  // Fetch the latest config file
  auto* app_config = wxConfig::Get();
  wxString temp_file;
  app_config->Read("/General/ConfigFile",&temp_file, L"");
  config_file_ = temp_file.ToStdString();
  const auto cfg = ReadConfigFile();
  if (!cfg) {
    LOG_DEBUG() << "Failed to read the config file. File: " << config_file_;
  }

  // Find the path to the 'notepad.exe'
  notepad_ = util::log::FindNotepad();
  LOG_DEBUG() << "Notepad Path: " << notepad_;

  wxPoint start_pos;
  app_config->Read("/MainWin/X",&start_pos.x, wxDefaultPosition.x);
  app_config->Read("/MainWin/Y",&start_pos.y, wxDefaultPosition.x);
  wxSize start_size;
  app_config->Read("/MainWin/XWidth",&start_size.x, 1200);
  app_config->Read("/MainWin/YWidth",&start_size.y, 800);
  bool maximized = false;
  app_config->Read("/MainWin/Max",&maximized, maximized);

  auto* frame = new MainFrame(GetAppDisplayName(), start_pos, start_size, maximized);
  frame->Show(true);
  frame->Update();

  return true;
}

int WorkflowExplorer::OnExit() {
  LOG_DEBUG() << "Closing application";

  LOG_DEBUG() << "Saving configuration";

  auto& log_config = LogConfig::Instance();
  log_config.DeleteLogChain();

  return wxApp::OnExit();
}

void WorkflowExplorer::OnOpenLogFile(wxCommandEvent& event) {
  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  OpenFile(logfile);

}

void WorkflowExplorer::OnUpdateOpenLogFile(wxUpdateUIEvent &event) {
  if (notepad_.empty()) {
    event.Enable(false);
    return;
  }

  auto& log_config = LogConfig::Instance();
  std::string logfile = log_config.GetLogFile();
  try {
    std::filesystem::path p(logfile);
    const bool exist = std::filesystem::exists(p);
    event.Enable(exist);
  } catch (const std::exception&) {
    event.Enable(false);
  }
}

void WorkflowExplorer::OpenFile(const std::string& filename) const {
  if (!notepad_.empty()) {
    boost::process::spawn(notepad_, filename);
  }
}

void WorkflowExplorer::OnConfigFile(wxCommandEvent &event) {
  auto* app_config = wxConfig::Get();
  if (app_config == nullptr) {
    return;
  }
  wxString temp_file = app_config->Read("/General/ConfigFile");

  wxString default_dir;
  try {
    path filename(temp_file.ToStdString());
    path parent = filename.parent_path();
    if (exists(parent)) {
      default_dir = parent.string();
    }
  } catch (const std::exception&) {
  }

  auto* window = GetTopWindow(); // Note may return nullptr
  wxFileDialog dialog(window, ("Open Configuration File"), default_dir, "",
                      "Config Files (*.xml)|*.xml|All Files (*.*)|*.*",
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

  server_.Clear();
  config_file_ = dialog.GetPath().ToStdString();
  const auto read = ReadConfigFile();
  if (!read) {
    std::ostringstream msg;
    msg << "Failed to read in the configuration file." << std::endl;
    msg << config_file_ << std::endl;
    wxMessageBox(msg.str(), "Fail to Read Config File",
                 wxCENTRE | wxICON_ERROR,window);
  } else {
    temp_file = config_file_.c_str();
    app_config->Write("/General/ConfigFile",temp_file);
  }
  if (window != nullptr) {
    window->Update();
  }
}

bool WorkflowExplorer::ReadConfigFile() {
  wxBusyCursor wait;
  if (config_file_.empty()) {
    return true;
  }

  try {
    path filename(config_file_);
    if (!exists(filename)) {
      LOG_DEBUG() << "Config file doesn't exists. File: " << config_file_;
      config_file_.clear();
      return true;
    }
    auto xml_file = CreateXmlFile("Expat");
    xml_file->FileName(config_file_);
    const auto parse = xml_file->ParseFile();
    if (!parse) {
      LOG_DEBUG() << "Failed to parse the XML file. File: " << config_file_;
      config_file_.clear();
      return false;
    }
    const auto* root = xml_file->RootNode();
    if (root == nullptr) {
      LOG_DEBUG() << "No root node in XML file. File: " << config_file_;
      config_file_.clear();
      return false;
    }
    server_.ReadXml(*root);
    original_ = server_;
  } catch (const std::exception& err) {
    LOG_DEBUG() << "File system error. Error: " << err.what();
    config_file_.clear();
    return false;
  }
  return true;
}

std::string WorkflowExplorer::ConfigName() const {
  try {
    path config(config_file_);
    path filename(config.filename());
    return filename.string();
  } catch (const std::exception& err) {

  }
  return {};
}

void WorkflowExplorer::OnSaveFile(wxCommandEvent& event) {
  const auto save = SaveConfigFile();
  if (!save) {
    std::ostringstream msg;
    msg << "Failed to save the configuration file." << std::endl;
    msg << config_file_ << std::endl;
    wxMessageBox(msg.str(), "Fail to Save Config File",
                 wxCENTRE | wxICON_ERROR,GetTopWindow());
    config_file_.clear();
  }
}

bool WorkflowExplorer::SaveConfigFile() {
  wxBusyCursor wait;
  if (config_file_.empty()) {
    LOG_DEBUG() << "No config file selected";
    return false;
  }

  try {
    path filename(config_file_);

    // Check that parent dir exists. If not create the directory.
    const auto parent_dir = filename.parent_path();
    if (!exists(parent_dir)) {
      create_directories(parent_dir);
    }

    auto xml_file = CreateXmlFile("Expat");
    xml_file->FileName(config_file_);

    // Insert the root tag if file doesn't exist else parse existing file
    if (!exists(filename)) {
      xml_file->RootName("WorkflowExplorer");
    } else {
      const auto parse = xml_file->ParseFile();
      if (!parse) {
        LOG_DEBUG() << "Failed to parse the XML file. File: " << config_file_;
        return false;
      }
    }

    auto* root = xml_file->RootNode();
    if (root == nullptr) {
      LOG_DEBUG() << "No root node in XML file. File: " << config_file_;
      return false;
    }
    server_.SaveXml(*root);
    original_ = server_;
    const auto write = xml_file->WriteFile();
    if (!write) {
      LOG_DEBUG() << "Failed to write the file. File: " << config_file_;
      return false;
    }
    const auto backup = BackupFiles(config_file_, false);
    if (!backup) {
      LOG_ERROR() << "Failed to backup the config file. File: " << config_file_;
    }
  } catch (const std::exception& err) {
    LOG_DEBUG() << "File system error. Error: " << err.what();
    return false;
  }
  return true;
}

void WorkflowExplorer::OnUpdateSaveFile(wxUpdateUIEvent& event) {
  if (config_file_.empty()) {
    event.Enable(false);
    return;
  }
  try {
    path filename(config_file_);
    if (!exists(filename)) {
      event.Enable(false);
      return;
    }
  } catch (const std::exception& err) {
    event.Enable(false);
    return;
  }
  event.Enable(IsModified());
}

bool WorkflowExplorer::IsModified() const {
  return !(server_ == original_);
}

void WorkflowExplorer::OnUpdateSaveAsFile(wxUpdateUIEvent& event) {
  event.Enable(true);
}

void WorkflowExplorer::OnSaveAsFile(wxCommandEvent &event) {
  auto* app_config = wxConfig::Get();
  if (app_config == nullptr) {
    return;
  }
  wxString temp_file = app_config->Read("/General/ConfigFile");

  wxString default_dir;

  try {
    path filename(temp_file.ToStdString());
    path parent = filename.parent_path();
    if (exists(parent)) {
      default_dir = parent.string();
    }
  } catch (const std::exception&) {
  }

  auto* window = GetTopWindow(); // Note may return nullptr
  wxFileDialog dialog(window, "Save Configuration File", default_dir, "",
                      "Config Files (*.xml)|*.xml|All Files (*.*)|*.*",
                      wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  config_file_ = dialog.GetPath().ToStdString();
  const auto save = SaveConfigFile();
  if (!save) {
    std::ostringstream msg;
    msg << "Failed to save the configuration file." << std::endl;
    msg << config_file_ << std::endl;
    wxMessageBox(msg.str(), "Fail to Read Config File",
                 wxCENTRE | wxICON_ERROR,window);
    config_file_.clear();
  } else {
    temp_file = config_file_.c_str();
    app_config->Write("/General/ConfigFile",temp_file);
  }
  if (window != nullptr) {
    window->Update();
  }
}

}


