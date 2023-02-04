/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "parameterpanel.h"
#include "workflowexplorerid.h"
#include <wx/treelist.h>
#include <wx/withimages.h>

#include "parameterdialog.h"
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

wxBEGIN_EVENT_TABLE(ParameterPanel, wxPanel)
    EVT_UPDATE_UI(kIdNewParameter, ParameterPanel::OnUpdateNewParameter)
    EVT_MENU(kIdNewParameter, ParameterPanel::OnNewParameter)
wxEND_EVENT_TABLE()

ParameterPanel::ParameterPanel(wxWindow *parent) :
    wxPanel(parent),
    image_list_(16, 16, false, 6) {
  image_list_.Add(wxBitmap("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  list_ = new wxTreeListCtrl(this, kIdParameterList, wxDefaultPosition,
                             {800, 600}, wxTL_SINGLE);
  list_->SetImageList(&image_list_);

  list_->AppendColumn(L"Name");
  list_->AppendColumn(L"Value");
  list_->AppendColumn(L"Unit");

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(list_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetSizerAndFit(main_sizer);
}

void ParameterPanel::RedrawParameterList() {
  if (list_ == nullptr) {
    return;
  }
  list_->DeleteAllItems();
  auto root_item = list_->GetRootItem();
  list_->AppendItem(root_item, "Needs Update",
                    wxWithImages::NO_IMAGE,wxWithImages::NO_IMAGE,
                    nullptr);

}

void ParameterPanel::Update() {
  wxWindow::Update();
  RedrawParameterList();
}

void ParameterPanel::OnUpdateNewParameter(wxUpdateUIEvent &event) {
  event.Enable(list_ != nullptr);
}

void ParameterPanel::OnNewParameter(wxCommandEvent &event) {
  if (list_ == nullptr) {
    return;
  }
  IParameter parameter;
  auto& app = wxGetApp();

  ParameterDialog dialog(this, parameter);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

}

}