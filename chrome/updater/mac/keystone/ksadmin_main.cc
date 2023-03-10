// Copyright 2021 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/updater/mac/keystone/ksadmin.h"

int main(int argc, const char* argv[]) {
  return updater::KSAdminAppMain(argc, argv);
}
