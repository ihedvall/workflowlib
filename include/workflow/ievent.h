/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <string>
#include <atomic>
#include <memory>
#include <vector>
#include <thread>

#include "workflow/irunner.h"
#include <util/ixmlnode.h>

namespace workflow {

class IWorkflow;


/**
 * @enum EventType
 *
 * @brief The EventType enum class represents the different types of events.
 *
 * EventType represents the various types of events that can occur within a system.
 * Each event type is represented by a unique enumerator value.
 *
 * The EventType enum class provides the following options:
 *
 * - Init: The initialization event.
 * - Exit: The exit event.
 * - Cyclic: The cyclic event.
 * - Periodic: The periodic event.
 * - Parameter: The parameter event.
 */
enum class EventType {
  Init,
  Exit,
  Cyclic,
  Periodic,
  Parameter,
};

/**
 * @class IEvent
 *
 * @brief The IEvent class represents an event in a workflow system.
 *
 * IEvent is the base class for all types of events that can occur within a workflow system.
 * It provides basic functionality and properties that every event should have.
 *
 * Each event has a name, description, type, period, parameter, and attached workflows.
 */
class IEvent {
 public:
  IEvent() = default; ///< Default constructor
  virtual ~IEvent();  ///< Destructor
  IEvent(const IEvent& event); ///< Default copy constructor
  [[nodiscard]] bool operator == (const IEvent& event) const; ///< Compares 2 events

  /**
   * @brief Set the name of the event.
   *
   * This function sets the name of the event. The name is used in presentation
   * and the name should be unique within a workflow system
   *
   * @param name The name of the event.
   *
   * @returns None.
   */
  void Name(const std::string& name) { name_ = name;}
  /**
    * @brief Get the name of the event.
    *
    * This function returns the name of the event.
    * The name is used in presentation and should be unique within a workflow system.
    *
    * @returns The name of the event.
    */
  [[nodiscard]] const std::string& Name() const {return name_;}

  /**
   * @brief Set the description of the event.
   *
   * This function sets the description of the event. The description provides user information
   * about the event and its purpose.
   *
   * @param description The description of the event.
   *
   * @returns None.
   */
  void Description(const std::string& description) { description_ = description;}

  /**
    * @brief Get the description of the event.
    *
    * This function returns the description of the event. The description provides user information
    * about the event and its purpose.
    *
    * @returns The description of the event.
    */
  [[nodiscard]] const std::string& Description() const {return description_;}

  /**
   * @brief Set the type of the event.
   *
   * This function sets the type of the event. The type determines the behavior of the event
   * and how it should be handled within the workflow system.
   *
   * The EventType::Init and EventType::Exit events are only used at the beginning and at the
   * shutdown of a workflow system.
   *
   * The EventType::Periodic and the EventType::Cyclic generates events with a specified
   * period (in milli-seconds). The difference is that the cyclic event just sleeps for the
   * specific period while the periodic generate an event each period (frequency).
   *
   * Use cyclic event as it is safer if just a delay is needed. The periodic event generate
   * an event with a fixed frequency and normalized. A periodic event with 1000 ms period,
   * will generate an event each second as close to the second switch ("hh::mm:ss:000) as possible.
   *
   * The EventTYpe::Parameter is triggered by a parameter in the workflow parameter area.
   * The parameter should be a so-called event parameter (value: 0 or 1).
   *
   * @param type The type of the event.
   *
   * @returns None.
   */
  void Type(EventType type) { type_ = type;}
  /**
   * @brief Get the type of the event.
   *
   * This function returns the type of the event.
   * The type determines the behavior of the event and how it should be handled within the workflow system.
   *
   * The EventType::Init and EventType::Exit events are only used at the beginning and at the shutdown of a workflow system.
   *
   * The EventType::Periodic and the EventType::Cyclic generates events with a specified period (in milli-seconds).
   * The difference is that the cyclic event just sleeps for the specific period while the periodic generate an event each period (frequency).
   *
   * Use cyclic event if just a delay is needed. The periodic event generates an event with a fixed frequency and normalized.
   * A periodic event with a 1000 ms period will generate an event each second as close to the second switch ("hh::mm:ss:000) as possible.
   *
   * @return The type of the event.
   */
  [[nodiscard]] EventType Type() const {return type_;}
  void EventTypeAsString(const std::string& type);
  [[nodiscard]] std::string EventTypeAsString() const;

  /**
    * @brief Set the period in milliseconds.
    *
    * This function sets the period of the event in milli-seconds.
    * The period defines the interval at which the event should occur.
    *
    * @param period The period of the event in milliseconds.
    *
    * @returns None.
    */
  void Period(uint64_t period) { period_ = period;}
  /**
   * @brief Get the period in milliseconds.
   *
   * This function returns the period of the event in milliseconds.
   * The period defines the interval at which the event should occur.
   *
   * @returns The period of the event in milliseconds.
   */
  [[nodiscard]] uint64_t Period() const {return period_;}

  void Parameter(const std::string& parameter) {parameter_ = parameter;}
  [[nodiscard]] const std::string& Parameter() const {return parameter_;}

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  void AttachWorkflow(IWorkflow* workflow);
  void DetachWorkflows();

 protected:

 private:
  std::string name_;
  std::string description_;
  std::string parameter_;
  uint64_t period_ = 1000; ///< Period in ms
  EventType type_ = EventType::Cyclic;
  std::vector<IWorkflow*> workflow_list_;

  std::thread working_thread_;
  std::atomic<bool> stop_thread_ = true;
  uint64_t step_time_ = 0;
  int64_t next_time_ = 0;

  void PeriodicTask();
  void CyclicTask();
};

}  // namespace workflow
