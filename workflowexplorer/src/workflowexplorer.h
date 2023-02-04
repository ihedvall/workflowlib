/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <string>
#include <map>
#include <memory>
#include <wx/wx.h>
#include "workflow/workflowserver.h"

namespace workflow::gui {

class WorkflowExplorer : public wxApp {
 public:

  bool OnInit() override;
  int OnExit() override;

  void OpenFile(const std::string& filename) const;
  WorkflowServer& Server() { return server_; }
  const WorkflowServer& Server() const { return server_; }

  std::string ConfigName() const;

  [[nodiscard]] bool IsModified() const;
  bool ReadConfigFile();
  bool SaveConfigFile();
 private:
  std::string notepad_; ///< Path to notepad.exe if it exist.
  std::string config_file_;
  WorkflowServer server_;
  WorkflowServer original_;

  void OnOpenLogFile(wxCommandEvent& event);
  void OnUpdateOpenLogFile(wxUpdateUIEvent& event);

  void OnConfigFile(wxCommandEvent& event);
  void OnUpdateSaveFile(wxUpdateUIEvent& event);
  void OnSaveFile(wxCommandEvent& event);
  void OnUpdateSaveAsFile(wxUpdateUIEvent& event);
  void OnSaveAsFile(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();
};

wxDECLARE_APP(WorkflowExplorer);
} // end namespace
