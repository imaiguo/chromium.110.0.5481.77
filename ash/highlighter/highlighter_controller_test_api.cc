// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/highlighter/highlighter_controller_test_api.h"

#include "ash/fast_ink/fast_ink_points.h"
#include "ash/highlighter/highlighter_controller.h"
#include "ash/highlighter/highlighter_view.h"
#include "base/timer/timer.h"

namespace ash {

HighlighterControllerTestApi::HighlighterControllerTestApi(
    HighlighterController* instance)
    : instance_(instance) {
  AttachClient();
}

HighlighterControllerTestApi::~HighlighterControllerTestApi() {
  if (scoped_observation_)
    DetachClient();
  if (instance_->is_enabled())
    instance_->SetEnabled(false);
  instance_->DestroyPointerView();
}

void HighlighterControllerTestApi::AttachClient() {
  scoped_observation_ = std::make_unique<ScopedObservation>(this);
  scoped_observation_->Observe(instance_);
}

void HighlighterControllerTestApi::DetachClient() {
  scoped_observation_.reset();
  instance_->CallExitCallback();
}

void HighlighterControllerTestApi::SetEnabled(bool enabled) {
  instance_->UpdateEnabledState(
      enabled ? HighlighterEnabledState::kEnabled
              : HighlighterEnabledState::kDisabledBySessionComplete);
}

void HighlighterControllerTestApi::DestroyPointerView() {
  instance_->DestroyPointerView();
}

void HighlighterControllerTestApi::SimulateInterruptedStrokeTimeout() {
  if (!instance_->interrupted_stroke_timer_)
    return;
  instance_->interrupted_stroke_timer_->Stop();
  instance_->RecognizeGesture();
}

bool HighlighterControllerTestApi::IsShowingHighlighter() const {
  return !!instance_->highlighter_view_widget_;
}

bool HighlighterControllerTestApi::IsFadingAway() const {
  return IsShowingHighlighter() && instance_->GetHighlighterView()->animating();
}

bool HighlighterControllerTestApi::IsShowingSelectionResult() const {
  return !!instance_->result_view_widget_;
}

bool HighlighterControllerTestApi::IsWaitingToResumeStroke() const {
  return instance_->interrupted_stroke_timer_ &&
         instance_->interrupted_stroke_timer_->IsRunning();
}

const fast_ink::FastInkPoints& HighlighterControllerTestApi::points() const {
  return instance_->GetHighlighterView()->points_;
}

const fast_ink::FastInkPoints& HighlighterControllerTestApi::predicted_points()
    const {
  return instance_->GetHighlighterView()->predicted_points_;
}

bool HighlighterControllerTestApi::HandleEnabledStateChangedCalled() {
  return handle_enabled_state_changed_called_;
}

bool HighlighterControllerTestApi::HandleSelectionCalled() {
  return handle_selection_called_;
}

void HighlighterControllerTestApi::OnHighlighterSelectionRecognized(
    const gfx::Rect& rect) {
  handle_selection_called_ = true;
  selection_ = rect;
}

void HighlighterControllerTestApi::OnHighlighterEnabledChanged(
    HighlighterEnabledState state) {
  const bool enabled = (state == HighlighterEnabledState::kEnabled);
  handle_enabled_state_changed_called_ = true;
  enabled_ = enabled;
}

}  // namespace ash
