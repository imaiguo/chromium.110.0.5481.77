// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/app_list/model/app_list_model_observer.h"

#include "base/check.h"

namespace ash {

AppListModelObserver::~AppListModelObserver() {
  CHECK(!IsInObserverList());
}

}  // namespace ash
