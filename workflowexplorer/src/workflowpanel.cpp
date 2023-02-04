/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "workflowpanel.h"
#include "workflowexplorerid.h"
#include <wx/treelist.h>
#include <wx/withimages.h>

// #include "workflowdialog.h"
#include "workflowexplorer.h"

namespace {

constexpr int kTestCloseBmp = 0;
constexpr int kTestOpenBmp = 1;
constexpr int kFileCloseBmp = 2;
constexpr int kFileOpenBmp = 3;
constexpr int kMeasCloseBmp = 4;
constexpr int kMeasOpenBmp = 5;

}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(WorkflowPanel, wxPanel)
    EVT_UPDATE_UI(kIdNewWorkflow, WorkflowPanel::OnUpdateNewWorkflow)
    EVT_MENU(kIdNewWorkflow, WorkflowPanel::OnNewWorkflow)
wxEND_EVENT_TABLE()

WorkflowPanel::WorkflowPanel(wxWindow *parent)
: wxPanel(parent),
  image_list_(16, 16, false, 6) {
  image_list_.Add(wxBitmap("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  list_ = new wxTreeListCtrl(this, kIdParameterList, wxDefaultPosition,
                             {800, 600}, wxTL_SINGLE);
  list_->SetImageList(&image_list_);

  list_->AppendColumn(L"Workflow/Runner");
   list_->AppendColumn(L"Description");

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(list_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetSizerAndFit(main_sizer);
}

void WorkflowPanel::RedrawWorkflowList() {
  if (list_ == nullptr) {
    return;
  }
  list_->DeleteAllItems();
  auto root_item = list_->GetRootItem();
  auto& app = wxGetApp();
  auto& server = app.Server();
  const auto& workflow_list = server.Workflows();

  list_->AppendItem(root_item, "Needs Update",
                    wxWithImages::NO_IMAGE,wxWithImages::NO_IMAGE,
                    nullptr);

}

void WorkflowPanel::Update() {
  wxWindow::Update();
  RedrawWorkflowList();
}

void WorkflowPanel::OnUpdateNewWorkflow(wxUpdateUIEvent &event) {
  event.Enable(list_ != nullptr);
}

void WorkflowPanel::OnNewWorkflow(wxCommandEvent &event) {
  if (list_ == nullptr) {
    return;
  }
  IWorkflow workflow;
  auto& app = wxGetApp();
/*
  WorkflowDialog dialog(this, workflow);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
*/
}


}  // namespace workflow::gui