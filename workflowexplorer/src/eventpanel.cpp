/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "eventpanel.h"
#include "workflowexplorerid.h"
#include <wx/treelist.h>
#include <wx/withimages.h>

#include "eventdialog.h"
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

wxBEGIN_EVENT_TABLE(EventPanel, wxPanel)
    EVT_UPDATE_UI(kIdNewEvent, EventPanel::OnUpdateNewEvent)
    EVT_MENU(kIdNewEvent, EventPanel::OnNewEvent)
wxEND_EVENT_TABLE()

EventPanel::EventPanel(wxWindow *parent)
: wxPanel(parent),
  image_list_(16, 16, false, 6) {
    image_list_.Add(wxBitmap("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
    list_ = new wxTreeListCtrl(this, kIdEventList, wxDefaultPosition,
                               {800, 600}, wxTL_SINGLE);
    list_->SetImageList(&image_list_);

    list_->AppendColumn(L"Name");
    list_->AppendColumn(L"Description");
    list_->AppendColumn(L"Event");

    auto *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(list_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
    SetSizerAndFit(main_sizer);
  }

  void EventPanel::RedrawEventList() {
    if (list_ == nullptr) {
      return;
    }
    list_->DeleteAllItems();
    auto root_item = list_->GetRootItem();
    list_->AppendItem(root_item, "Needs Update",
                      wxWithImages::NO_IMAGE,wxWithImages::NO_IMAGE,
                      nullptr);

  }

  void EventPanel::Update() {
    wxWindow::Update();
    RedrawEventList();
  }

  void EventPanel::OnUpdateNewEvent(wxUpdateUIEvent &event) {
    event.Enable(list_ != nullptr);
  }

  void EventPanel::OnNewEvent(wxCommandEvent &event) {
    if (list_ == nullptr) {
      return;
    }
    IEvent new_event;
    auto& app = wxGetApp();

    EventDialog dialog(this, new_event);
    const auto ret = dialog.ShowModal();
    if (ret != wxID_OK) {
      return;
    }

  }

}  // namespace workflow::gui