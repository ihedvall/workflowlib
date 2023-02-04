/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <wx/defs.h>
namespace workflow::gui {
constexpr wxWindowID kIdOpenLogFile = 1;
constexpr wxWindowID kIdNotebook = 2;


constexpr wxWindowID kIdNewParameter = 10;
constexpr wxWindowID kIdEditParameter = 11;
constexpr wxWindowID kIdCopyParameter = 12;
constexpr wxWindowID kIdRenameParameter = 13;
constexpr wxWindowID kIdDeleteParameter = 14;
constexpr wxWindowID kIdParameterList = 15;

constexpr wxWindowID kIdNewWorkflow = 20;
constexpr wxWindowID kIdEditWorkflow = 21;
constexpr wxWindowID kIdCopyWorkflow = 22;
constexpr wxWindowID kIdRenameWorkflow = 23;
constexpr wxWindowID kIdDeleteWorkflow = 24;
constexpr wxWindowID kIdWorkflowList = 25;

constexpr wxWindowID kIdNewEvent = 30;
constexpr wxWindowID kIdEditEvent = 31;
constexpr wxWindowID kIdCopyEvent = 32;
constexpr wxWindowID kIdRenameEvent = 33;
constexpr wxWindowID kIdDeleteEvent = 34;
constexpr wxWindowID kIdEventList = 35;
};