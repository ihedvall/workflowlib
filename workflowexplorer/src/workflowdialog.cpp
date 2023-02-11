/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "workflowdialog.h"
#include "workflowexplorer.h"
#include <wx/choice.h>
#include <wx/valgen.h>

namespace {

wxArrayString Events() {
  wxArrayString temp;
  const auto& app = workflow::gui::wxGetApp();
  const auto& server = app.Server();
  const auto* engine = server.GetEventEngine();
  if (engine != nullptr) {
    const auto& list = engine->Events();
    for (const auto& itr : list) {
      const auto* event = itr.second.get();
      if (event == nullptr) {
        continue;
      }
      temp.Add(wxString::FromUTF8(event->Name()));
    }
  }
  temp.Sort();
  return temp;
}

}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(WorkflowDialog, wxDialog) //NOLINT
wxEND_EVENT_TABLE()

WorkflowDialog::WorkflowDialog(wxWindow *parent, IWorkflow& workflow)
: wxDialog(parent, wxID_ANY, "Workflow Dialog" ,
       wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
workflow_(workflow) {

  const auto new_workflow = workflow_.Name().empty();
  auto *name = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, new_workflow ? 0 : wxTE_READONLY,
                              wxTextValidator(wxFILTER_EMPTY, &name_));
  name->SetMinSize({30 * 10, -1});

  auto *desc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &description_));
  desc->SetMinSize({40 * 10, -1});

  auto *event = new wxComboBox(this, wxID_ANY, event_, wxDefaultPosition,
                              wxDefaultSize, Events(), 0,
                              wxGenericValidator(&event_));
  event->SetMinSize({15 * 10, -1});

  auto *save_button_ = new wxButton(this, wxID_OK,
                                    wxGetStockLabel(wxID_SAVE,
                                                    wxSTOCK_FOR_BUTTON));
  auto *cancel_button_ = new wxButton(this, wxID_CANCEL,
                                      wxGetStockLabel(wxID_CANCEL,
                                                      wxSTOCK_FOR_BUTTON));

  auto *name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto *desc_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto *event_label = new wxStaticText(this, wxID_ANY, L"Event:");

  int label_width = 100;
  label_width = std::max(label_width, name_label->GetBestSize().GetX());
  label_width = std::max(label_width, desc_label->GetBestSize().GetX());
  label_width = std::max(label_width, event_label->GetBestSize().GetX());

  name_label->SetMinSize({label_width, -1});
  desc_label->SetMinSize({label_width, -1});
  event_label->SetMinSize({label_width, -1});

  auto *name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *desc_sizer = new wxBoxSizer(wxHORIZONTAL);
  desc_sizer->Add(desc_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  desc_sizer->Add(desc, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *event_sizer = new wxBoxSizer(wxHORIZONTAL);
  event_sizer->Add(event_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  event_sizer->Add(event, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button_);
  system_sizer->AddButton(cancel_button_);
  system_sizer->Realize();

  auto *cfg_box = new wxStaticBoxSizer(wxVERTICAL, this,
                                       L"Workflow Configuration");
  cfg_box->Add(name_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(desc_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(event_sizer, 0, wxALIGN_LEFT | wxALL, 1);

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(cfg_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0,
                  wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  if (new_workflow) {
    save_button_->SetDefault();
  } else {
    cancel_button_->SetDefault();
  }

}

bool WorkflowDialog::TransferDataToWindow() {
  name_ = wxString::FromUTF8(workflow_.Name());
  description_ = wxString::FromUTF8(workflow_.Description());
  event_ = wxString::FromUTF8(workflow_.StartEvent());
  return wxWindowBase::TransferDataToWindow();
}

bool WorkflowDialog::TransferDataFromWindow() {
  const auto ret = wxWindowBase::TransferDataFromWindow();
  if (!ret) {
    return false;
  }

  name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);
  event_.Trim(true).Trim(false);

  workflow_.Name(name_.utf8_string());
  workflow_.Description(description_.utf8_string());
  workflow_.StartEvent(event_.utf8_string());
  return true;
}

}  // namespace workflow::gui