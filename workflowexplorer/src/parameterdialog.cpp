/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */


#include "parameterdialog.h"
#include "workflowexplorer.h"
#include <string>
#include <wx/choice.h>
#include <wx/valgen.h>
#include <util/unithelper.h>

namespace {

wxArrayString DataTypes() {
  wxArrayString temp;
  workflow::IParameter parameter;
  for (auto type = static_cast<int>(workflow::ParameterDataType::FloatType);
       type <= static_cast<int>(workflow::ParameterDataType::ByteArrayType);
       ++type) {
    parameter.DataType(static_cast<workflow::ParameterDataType>(type));
    const auto type_string = parameter.DataTypeAsString();
    temp.Add(wxString::FromUTF8(type_string));
  }
  temp.Sort();
  return temp;
}

wxArrayString Units() {
  wxArrayString temp;
  const auto& app = workflow::gui::wxGetApp();
  const auto& server = app.Server();
  const auto* container = server.GetParameterContainer();
  if (container != nullptr) {
    const auto used_list = container->Units();
    for (const auto& used : used_list) {
      temp.Add(wxString::FromUTF8(used));
    }
  }

  const auto unit_list = util::string::UnitHelper::UnitList();
  for (const auto& unit : unit_list) {
    temp.Add(wxString::FromUTF8(unit));
  }

  return temp;
}

wxArrayString Devices() {
  wxArrayString temp;
  const auto& app = workflow::gui::wxGetApp();
  const auto& server = app.Server();
  const auto* container = server.GetParameterContainer();
  if (container != nullptr) {
    const auto& used_list = container->Devices();
    for (const auto& used : used_list) {
      temp.Add(wxString::FromUTF8(used.first));
    }
  }
  return temp;
}

}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(ParameterDialog, wxDialog) //NOLINT
wxEND_EVENT_TABLE()

ParameterDialog::ParameterDialog(wxWindow *parent, IParameter& parameter)
    : wxDialog(parent, wxID_ANY, "Parameter Dialog" ,
               wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
      parameter_(parameter) {

  const auto new_parameter = parameter_.Name().empty();
  auto *name = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize,
                              new_parameter ? 0 : wxTE_READONLY,
                              wxTextValidator(wxFILTER_EMPTY, &name_));
  name->SetMinSize({30 * 10, -1});

  auto *display_name = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &display_name_));
  display_name->SetMinSize({30 * 10, -1});

  auto *identity = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                  wxDefaultPosition, wxDefaultSize, 0,
                                  wxTextValidator(wxFILTER_NONE, &identity_));
  identity->SetMinSize({30 * 10, -1});

  auto *desc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &description_));
  desc->SetMinSize({40 * 10, -1});

  auto *data_type = new wxChoice(this, wxID_ANY, wxDefaultPosition,
                                 wxDefaultSize, DataTypes(), 0,
                                 wxGenericValidator(&data_type_));
  data_type->SetMinSize({15 * 10, -1});

  auto *unit = new wxComboBox(this, wxID_ANY, unit_, wxDefaultPosition,
                                 wxDefaultSize, Units(), 0,
                                 wxGenericValidator(&unit_));
  unit->SetMinSize({15 * 10, -1});

  auto *device = new wxComboBox(this, wxID_ANY, device_, wxDefaultPosition,
                              wxDefaultSize, Devices(), 0,
                              wxGenericValidator(&device_));
  device->SetMinSize({15 * 10, -1});

  auto *signal = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxTextValidator(wxFILTER_NONE, &signal_));
  signal->SetMinSize({30 * 10, -1});

  auto *save_button_ = new wxButton(this, wxID_OK, wxGetStockLabel(wxID_SAVE, wxSTOCK_FOR_BUTTON));
  auto *cancel_button_ = new wxButton(this, wxID_CANCEL, wxGetStockLabel(wxID_CANCEL, wxSTOCK_FOR_BUTTON));

  auto *name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto *display_name_label = new wxStaticText(this, wxID_ANY, L"Display Name:");
  auto *identity_label = new wxStaticText(this, wxID_ANY, L"Identity:");
  auto *desc_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto *data_type_label = new wxStaticText(this, wxID_ANY, L"Data Type:");
  auto *unit_label = new wxStaticText(this, wxID_ANY, L"Unit:");
  auto *device_label = new wxStaticText(this, wxID_ANY, L"Device:");
  auto *signal_label = new wxStaticText(this, wxID_ANY, L"Signal:");

  int label_width = 100;
  label_width = std::max(label_width, name_label->GetBestSize().GetX());
  label_width = std::max(label_width, display_name_label->GetBestSize().GetX());
  label_width = std::max(label_width, identity_label->GetBestSize().GetX());
  label_width = std::max(label_width, desc_label->GetBestSize().GetX());
  label_width = std::max(label_width, data_type_label->GetBestSize().GetX());
  label_width = std::max(label_width, unit_label->GetBestSize().GetX());
  label_width = std::max(label_width, device_label->GetBestSize().GetX());
  label_width = std::max(label_width, signal_label->GetBestSize().GetX());

  name_label->SetMinSize({label_width, -1});
  display_name_label->SetMinSize({label_width, -1});
  identity_label->SetMinSize({label_width, -1});
  desc_label->SetMinSize({label_width, -1});
  data_type_label->SetMinSize({label_width, -1});
  unit_label->SetMinSize({label_width, -1});
  device_label->SetMinSize({label_width, -1});
  signal_label->SetMinSize({label_width, -1});

  auto *name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *display_name_sizer = new wxBoxSizer(wxHORIZONTAL);
  display_name_sizer->Add(display_name_label, 0,
                          wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  display_name_sizer->Add(display_name, 0,
                          wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *identity_sizer = new wxBoxSizer(wxHORIZONTAL);
  identity_sizer->Add(identity_label, 0,
                          wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  identity_sizer->Add(identity, 0,
                          wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *desc_sizer = new wxBoxSizer(wxHORIZONTAL);
  desc_sizer->Add(desc_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  desc_sizer->Add(desc, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *data_type_sizer = new wxBoxSizer(wxHORIZONTAL);
  data_type_sizer->Add(data_type_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  data_type_sizer->Add(data_type, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *unit_sizer = new wxBoxSizer(wxHORIZONTAL);
  unit_sizer->Add(unit_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  unit_sizer->Add(unit, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *device_sizer = new wxBoxSizer(wxHORIZONTAL);
  device_sizer->Add(device_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  device_sizer->Add(device, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *signal_sizer = new wxBoxSizer(wxHORIZONTAL);
  signal_sizer->Add(signal_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  signal_sizer->Add(signal, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button_);
  system_sizer->AddButton(cancel_button_);
  system_sizer->Realize();

  auto *id_box = new wxStaticBoxSizer(wxVERTICAL, this, L"Parameter Identification");
  id_box->Add(name_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  id_box->Add(display_name_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  id_box->Add(identity_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  id_box->Add(desc_sizer, 0, wxALIGN_LEFT | wxALL, 1);

  auto *cfg_box = new wxStaticBoxSizer(wxVERTICAL, this, L"Parameter Configuration");
  cfg_box->Add(data_type_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(unit_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(device_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(signal_sizer, 0, wxALIGN_LEFT | wxALL, 1);

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(id_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(cfg_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  if (new_parameter) {
    save_button_->SetDefault();
  } else {
    cancel_button_->SetDefault();
  }
}

bool ParameterDialog::TransferDataToWindow() {
  name_ = wxString::FromUTF8(parameter_.Name());
  display_name_ = wxString::FromUTF8(parameter_.Name());
  description_ = wxString::FromUTF8(parameter_.Description());
  data_type_ = wxString::FromUTF8(parameter_.DataTypeAsString());
  unit_ = wxString::FromUTF8(parameter_.Unit());
  device_ = wxString::FromUTF8(parameter_.Device());
  return wxWindowBase::TransferDataToWindow();
}

bool ParameterDialog::TransferDataFromWindow() {
  const auto ret = wxWindowBase::TransferDataFromWindow();
  if (!ret) {
    return false;
  }

  name_.Trim(true).Trim(false);
  display_name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);
  unit_.Trim(true).Trim(false);
  device_.Trim(true).Trim(false);

  parameter_.Name(name_.utf8_string());
  parameter_.DisplayName(display_name_.utf8_string());
  parameter_.Description(description_.utf8_string());
  parameter_.DataTypeAsString(data_type_.utf8_string());
  parameter_.Unit(unit_.utf8_string());
  parameter_.Device(device_.utf8_string());
  return true;
}

} // End namespace
