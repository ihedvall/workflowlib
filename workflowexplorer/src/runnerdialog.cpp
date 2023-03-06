/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "runnerdialog.h"
#include "workflowexplorer.h"
#include <wx/choice.h>
#include <wx/valgen.h>
#include <wx/combobox.h>
#include "workflowexplorerid.h"

namespace {

wxArrayString Types() {
  wxArrayString temp;
  workflow::IRunner runner;
  for (int type = static_cast<int>(workflow::RunnerType::InternalRunner);
       type <= static_cast<int>(workflow::RunnerType::PythonRunner); ++type) {
    runner.Type(static_cast<workflow::RunnerType>(type));
    temp.Add(wxString::FromUTF8(runner.TypeAsString()));
  }
  temp.Sort();
  return temp;
}

wxArrayString Templates() {
  const auto& app = workflow::gui::wxGetApp();
  const auto& server = app.Server();
  const auto& template_list = server.Templates();
  wxArrayString temp;
  for (const auto& itr : template_list) {
    temp.Add(wxString::FromUTF8(itr.first));
  }
  return temp;
}

}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(RunnerDialog, wxDialog) //NOLINT
    EVT_COMBOBOX(kIdRunnerName, RunnerDialog::OnNameChange)
wxEND_EVENT_TABLE()

RunnerDialog::RunnerDialog(wxWindow *parent, IRunner& runner)
: wxDialog(parent, wxID_ANY, "Runner Task Dialog" ,
               wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE),
      runner_(runner) {

  const auto new_runner = runner_.Name().empty();
  auto *name = new wxComboBox(this, kIdRunnerName, name_, wxDefaultPosition,
                              wxDefaultSize, Templates(),
                              new_runner ? wxCB_DROPDOWN : wxCB_READONLY,
                              wxTextValidator(wxFILTER_EMPTY, &name_));
  name->SetMinSize({30 * 10, -1});

  auto *desc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &description_));
  desc->SetMinSize({40 * 10, -1});

  auto *doc = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                              wxDefaultPosition, wxDefaultSize, 0,
                              wxTextValidator(wxFILTER_NONE, &documentation_));
  doc->SetMinSize({40 * 10, -1});

  auto *type = new wxChoice(this, wxID_ANY, wxDefaultPosition,
                               wxDefaultSize, Types(), 0,
                               wxGenericValidator(&type_));
  type->SetMinSize({15 * 10, -1});

  auto *arg = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                             wxDefaultPosition, wxDefaultSize, 0,
                             wxTextValidator(wxFILTER_NONE, &arguments_));
  arg->SetMinSize({40 * 10, -1});

  auto *save_button_ = new wxButton(this, wxID_OK,
                                    wxGetStockLabel(wxID_SAVE,
                                                    wxSTOCK_FOR_BUTTON));
  auto *cancel_button_ = new wxButton(this, wxID_CANCEL,
                                      wxGetStockLabel(wxID_CANCEL,
                                                      wxSTOCK_FOR_BUTTON));

  auto *name_label = new wxStaticText(this, wxID_ANY, L"Name:");
  auto *desc_label = new wxStaticText(this, wxID_ANY, L"Description:");
  auto *doc_label = new wxStaticText(this, wxID_ANY, L"Documentation:");
  auto *type_label = new wxStaticText(this, wxID_ANY, L"Type:");
  auto *arg_label = new wxStaticText(this, wxID_ANY, L"Arguments:");

  int label_width = 100;
  label_width = std::max(label_width, name_label->GetBestSize().GetX());
  label_width = std::max(label_width, desc_label->GetBestSize().GetX());
  label_width = std::max(label_width, doc_label->GetBestSize().GetX());
  label_width = std::max(label_width, type_label->GetBestSize().GetX());
  label_width = std::max(label_width, arg_label->GetBestSize().GetX());

  name_label->SetMinSize({label_width, -1});
  desc_label->SetMinSize({label_width, -1});
  doc_label->SetMinSize({label_width, -1});
  type_label->SetMinSize({label_width, -1});
  arg_label->SetMinSize({label_width, -1});

  auto *name_sizer = new wxBoxSizer(wxHORIZONTAL);
  name_sizer->Add(name_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  name_sizer->Add(name, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *desc_sizer = new wxBoxSizer(wxHORIZONTAL);
  desc_sizer->Add(desc_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  desc_sizer->Add(desc, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *doc_sizer = new wxBoxSizer(wxHORIZONTAL);
  doc_sizer->Add(doc_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  doc_sizer->Add(doc, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *type_sizer = new wxBoxSizer(wxHORIZONTAL);
  type_sizer->Add(type_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  type_sizer->Add(type, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *arg_sizer = new wxBoxSizer(wxHORIZONTAL);
  arg_sizer->Add(arg_label, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
  arg_sizer->Add(arg, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 5);

  auto *system_sizer = new wxStdDialogButtonSizer();
  system_sizer->AddButton(save_button_);
  system_sizer->AddButton(cancel_button_);
  system_sizer->Realize();

  auto *id_box = new wxStaticBoxSizer(wxVERTICAL, this,
                                       L"Runner Identification");
  id_box->Add(name_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  id_box->Add(desc_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  id_box->Add(doc_sizer, 0, wxALIGN_LEFT | wxALL, 1);

  auto *cfg_box = new wxStaticBoxSizer(wxVERTICAL, this,
                                       L"Runner Configuration");
  id_box->Add(type_sizer, 0, wxALIGN_LEFT | wxALL, 1);
  cfg_box->Add(arg_sizer, 0, wxALIGN_LEFT | wxALL, 1);


  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(id_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(cfg_box, 0, wxALIGN_LEFT | wxALL | wxEXPAND, 4);
  main_sizer->Add(system_sizer, 0,
                  wxALIGN_CENTER_HORIZONTAL | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  SetSizerAndFit(main_sizer);
  if (new_runner) {
    save_button_->SetDefault();
  } else {
    cancel_button_->SetDefault();
  }

}

bool RunnerDialog::TransferDataToWindow() {
  name_ = wxString::FromUTF8(runner_.Name());
  description_ = wxString::FromUTF8(runner_.Description());
  documentation_ = wxString::FromUTF8(runner_.Documentation());
  type_ = wxString::FromUTF8(runner_.TypeAsString());
  arguments_ = wxString::FromUTF8(runner_.Arguments());
  return wxWindowBase::TransferDataToWindow();
}

bool RunnerDialog::TransferDataFromWindow() {
  const auto ret = wxWindowBase::TransferDataFromWindow();
  if (!ret) {
    return false;
  }

  name_.Trim(true).Trim(false);
  description_.Trim(true).Trim(false);
  documentation_.Trim(true).Trim(false);
  arguments_.Trim(true).Trim(false);

  runner_.Name(name_.utf8_string());
  runner_.Description(description_.utf8_string());
  runner_.Documentation(documentation_.utf8_string());
  runner_.TypeAsString(type_.utf8_string());
  runner_.Arguments(arguments_.utf8_string());
  return true;
}

void RunnerDialog::OnNameChange(wxCommandEvent &event) {
  const auto template_name = event.GetString().ToStdString();
  auto& app = wxGetApp();
  auto& server = app.Server();
  auto* tmpl = server.GetTemplate(template_name);
  // If previous name was empty, import the template properties as well
  if (tmpl != nullptr && !template_name.empty()) {
    runner_ = *tmpl;
    TransferDataToWindow();
  }
}

}  // namespace workflow::gui