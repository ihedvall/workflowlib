/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <wx/wx.h>
#include <wx/treelist.h>
#include <workflow/iworkflow.h>

namespace workflow::gui {

class WorkflowPanel: public wxPanel  {
 public:
  explicit WorkflowPanel(wxWindow *parent);

  void Update() override;

  void OnUpdateNewWorkflow(wxUpdateUIEvent& event);
  void OnUpdateSingleWorkflow(wxUpdateUIEvent& event);
  void OnUpdateMultipleWorkflow(wxUpdateUIEvent& event);
  void OnUpdateSingleRunner(wxUpdateUIEvent& event);
  void OnUpdateMultipleRunner(wxUpdateUIEvent& event);
  void OnNewWorkflow(wxCommandEvent& event);
  void OnEditWorkflow(wxCommandEvent& event);
  void OnCopyWorkflow(wxCommandEvent& event);
  void OnRenameWorkflow(wxCommandEvent& event);
  void OnDeleteWorkflow(wxCommandEvent& event);
  void OnNewRunner(wxCommandEvent& event);
  void OnEditRunner(wxCommandEvent& event);
  void OnCopyRunner(wxCommandEvent& event);
  void OnRenameRunner(wxCommandEvent& event);
  void OnDeleteRunner(wxCommandEvent& event);
  void OnRightClick(wxTreeListEvent& event);
  void OnUpWorkflow(wxCommandEvent& event);
  void OnDownWorkflow(wxCommandEvent& event);
  void OnUpRunner(wxCommandEvent& event);
  void OnDownRunner(wxCommandEvent& event);
  void OnDoubleClick(wxTreeListEvent& event);
 private:
  wxTreeListCtrl *list_ = nullptr;
  wxImageList image_list_;

  void RedrawWorkflowList();

  [[nodiscard]] IWorkflow* GetSelectedWorkflow();
  [[nodiscard]] IRunner* GetSelectedRunner();
  void SelectItem(const std::string& workflow, const std::string& runner);

  wxDECLARE_EVENT_TABLE();
};


}  // namespace workflow::gui
