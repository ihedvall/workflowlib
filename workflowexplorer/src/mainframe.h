/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/statusbr.h>

namespace workflow::gui {
class MainFrame : public wxFrame {
 public:
  MainFrame(const wxString& title, const wxPoint& start_pos, const wxSize& start_size, bool maximized);
  void Update() override;


 private:
  wxImageList image_list_;
  wxNotebook* notebook_ = nullptr;

  void OnExit(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnPageChange(wxBookCtrlEvent& event);

  void OnUpdateWorkflow(wxUpdateUIEvent &event);
  void OnWorkflow(wxCommandEvent& event);

  void OnUpdateEvent(wxUpdateUIEvent &event);
  void OnEvent(wxCommandEvent& event);

  void OnUpdateParameter(wxUpdateUIEvent &event);
  void OnParameter(wxCommandEvent& event);

  void OnUpdateTemplate(wxUpdateUIEvent &event);
  void OnTemplate(wxCommandEvent& event);
 wxDECLARE_EVENT_TABLE();
};
}

