/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "eventdialog.h"
#include <wx/choice.h>
#include <wx/valgen.h>

namespace {

wxArrayString EventTypes() {
  wxArrayString temp;
  workflow::IEvent event;
  for (auto type = static_cast<int>(workflow::EventType::Internal);
       type <= static_cast<int>(workflow::EventType::Parameter);
       ++type) {
    event.Type(static_cast<workflow::EventType>(type));
    const auto type_string = event.DataTypeAsString();
    temp.Add(wxString::FromUTF8(type_string));
  }
  temp.Sort();
  return temp;
}

}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(EventDialog, wxDialog) //NOLINT
wxEND_EVENT_TABLE()

EventDialog::EventDialog(wxWindow *parent, IEvent& event)
: wxDialog(parent, wxID_ANY, "Workflow Dialog" ,
           wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
  event_(event) {

  const auto new_event = event_.Name().empty();
  auto *name = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                              wxDefaultSize, new_event ? 0 : wxTE_READONLY,
                              wxTextValidator(wxFILTER_EMPTY, &name_));
  name->SetMinSize({30 * 10, -1});

  auto *desc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &description_));
  desc->SetMinSize({40 * 10, -1});

  auto *type = new wxChoice(this, wxID_ANY, wxDefaultPosition,
                                 wxDefaultSize, EventTypes(), 0,
                                 wxGenericValidator(&type_));
  type->SetMinSize({15 * 10, -1});

  auto *save_button_ = new wxButton(this, wxID_OK,
                                    wxGetStockLabel(wxID_SAVE,
                                                    wxSTOCK_FOR_BUTTON));
  auto *cancel_button_ = new wxButton(this, wxID_CANCEL,
                                      wxGetStockLabel(wxID_CANCEL,
                                                      wxSTOCK_FOR_BUTTON));

  auto *name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto *desc_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto *type_label = new wxStaticText(this, wxID_ANY, L"Type:");

  int label_width = 100;
  label_width = std::max(label_width, name_label->GetBestSize().GetX());
  label_width = std::max(label_width, desc_label->GetBestSize().GetX());
  label_width = std::max(label_width, type_label->GetBestSize().GetX());

  name_label->SetMinSize({label_width, -1});
  desc_label->SetMinSize({label_width, -1});
  type_label->SetMinSize({label_width, -1});

  auto *name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *desc_sizer = new wxBoxSizer(wxHORIZONTAL);
  desc_sizer->Add(desc_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  desc_sizer->Add(desc, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *type_sizer = new wxBoxSizer(wxHORIZONTAL);
  type_sizer->Add(type_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  type_sizer->Add(type, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button_);
  system_sizer->AddButton(cancel_button_);
  system_sizer->Realize();

  auto *cfg_box = new wxStaticBoxSizer(wxVERTICAL, this,
                                       L"Event Configuration");
  cfg_box->Add(name_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(desc_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(type_sizer, 0, wxALIGN_LEFT | wxALL, 1);

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(cfg_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0,
                  wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  if (new_event) {
    save_button_->SetDefault();
  } else {
    cancel_button_->SetDefault();
  }

}

bool EventDialog::TransferDataToWindow() {
  name_ = wxString::FromUTF8(event_.Name());
  description_ = wxString::FromUTF8(event_.Description());
  type_ = wxString::FromUTF8(event_.EventTypeAsString());
  return wxWindowBase::TransferDataToWindow();
}

bool EventDialog::TransferDataFromWindow() {
  const auto ret = wxWindowBase::TransferDataFromWindow();
  if (!ret) {
    return false;
  }

  name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);

  event_.Name(name_.utf8_string());
  event_.Description(description_.utf8_string());
  event_.EventTypeAsString(type_.utf8_string());
  return true;
}

}  // namespace workflow::gui