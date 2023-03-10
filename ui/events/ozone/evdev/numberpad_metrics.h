// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_NUMBERPAD_METRICS_H_
#define UI_EVENTS_OZONE_EVDEV_NUMBERPAD_METRICS_H_

#include "base/component_export.h"
#include "base/no_destructor.h"
#include "chromeos/ash/components/feature_usage/feature_usage_metrics.h"
#include "ui/events/devices/input_device.h"

namespace ui {

class NumberpadMetricsTest;

// Combine delegate callbacks and FeatureUsageMetrics state for each metric.
class COMPONENT_EXPORT(EVDEV) NumberpadMetricsDelegate final
    : public ash::feature_usage::FeatureUsageMetrics::Delegate {
 public:
  explicit NumberpadMetricsDelegate(const std::string& feature_name);
  NumberpadMetricsDelegate(const NumberpadMetricsDelegate&) = delete;
  NumberpadMetricsDelegate& operator=(const NumberpadMetricsDelegate&) = delete;
  ~NumberpadMetricsDelegate() final;

  void RecordUsage(bool success);
  void SetState(bool now_eligible, bool now_enabled);
  void SetState(bool now_eligible) { SetState(now_eligible, now_eligible); }

  // ash::feature_usage::NumberpadMetricsDelegate::Delegate:
  bool IsEligible() const final;
  bool IsEnabled() const final;

 private:
  bool eligible_ = false;
  bool enabled_ = false;
  ash::feature_usage::FeatureUsageMetrics metrics_;
};

// A class that records number-pad related metrics.
class COMPONENT_EXPORT(EVDEV) NumberpadMetricsRecorder {
 public:
  // Names for feature_usage_metric features generated by this module.
  static constexpr char kFeatureDynamicActivations[] =
      "NumberpadDynamicActivations";
  static constexpr char kFeatureDynamicCancellations[] =
      "NumberpadDynamicCancellations";
  static constexpr char kFeatureDynamicEnterKeystrokes[] =
      "NumberpadDynamicEnterKeystrokes";
  static constexpr char kFeatureDynamicNonEnterKeystrokes[] =
      "NumberpadDynamicNonEnterKeystrokes";
  static constexpr char kFeatureInternalEnterKeystrokes[] =
      "NumberpadInternalEnterKeystrokes";
  static constexpr char kFeatureInternalNonEnterKeystrokes[] =
      "NumberpadInternalNonEnterKeystrokes";
  static constexpr char kFeatureExternalEnterKeystrokes[] =
      "NumberpadExternalEnterKeystrokes";
  static constexpr char kFeatureExternalNonEnterKeystrokes[] =
      "NumberpadExternalNonEnterKeystrokes";

  void AddDevice(const ui::InputDevice& input_device);
  void RemoveDevice(const ui::InputDevice& input_device);

  // Call to process an individual key; should only be called for
  // keys on devices that are numberpads (it does not check this
  // itself).
  void ProcessKey(unsigned int key, bool down, const InputDevice& input_device);

  static NumberpadMetricsRecorder* GetInstance();
  static void ClearInstance();

 private:
  friend class ::ui::NumberpadMetricsTest;
  friend base::NoDestructor<NumberpadMetricsRecorder>;

  NumberpadMetricsRecorder();
  NumberpadMetricsRecorder(const NumberpadMetricsRecorder&) = delete;
  NumberpadMetricsRecorder& operator=(const NumberpadMetricsRecorder&) = delete;
  ~NumberpadMetricsRecorder();

  void UpdateDeviceState();
  void ToggleDynamicNumlockState();
  void SetDynamicNumlockStateOn();

  // Track what kinds of devices are present
  unsigned int dynamic_numpad_present_ = 0;
  unsigned int internal_numpad_present_ = 0;
  unsigned int external_numpad_present_ = 0;

  // Track knowledge about the dynamic numberpad to enable
  // deductions about its current state.
  bool dynamic_numlock_state_ = false;
  bool dynamic_numlock_state_known_ = false;
  bool any_dynamic_numlock_state_flips_ = false;
  bool dynamic_numlock_state_guess_ = false;
  unsigned int dynamic_numlock_state_flips_ = 0;
  bool dynamic_numpad_used_ = false;

  NumberpadMetricsDelegate dynamic_activations_metrics_delegate_{
      kFeatureDynamicActivations};
  NumberpadMetricsDelegate dynamic_cancellations_metrics_delegate_{
      kFeatureDynamicCancellations};
  NumberpadMetricsDelegate dynamic_enter_keystrokes_metrics_delegate_{
      kFeatureDynamicEnterKeystrokes};
  NumberpadMetricsDelegate dynamic_non_enter_keystrokes_metrics_delegate_{
      kFeatureDynamicNonEnterKeystrokes};

  NumberpadMetricsDelegate internal_enter_keystrokes_metrics_delegate_{
      kFeatureInternalEnterKeystrokes};
  NumberpadMetricsDelegate internal_non_enter_keystrokes_metrics_delegate_{
      kFeatureInternalNonEnterKeystrokes};

  NumberpadMetricsDelegate external_enter_keystrokes_metrics_delegate_{
      kFeatureExternalEnterKeystrokes};
  NumberpadMetricsDelegate external_non_enter_keystrokes_metrics_delegate_{
      kFeatureExternalNonEnterKeystrokes};
};

}  // namespace ui

#endif  // UI_EVENTS_OZONE_EVDEV_NUMBERPAD_METRICS_H_
