// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_VIEWS_CROSTINI_CROSTINI_UPDATE_FILESYSTEM_VIEW_H_
#define CHROME_BROWSER_UI_VIEWS_CROSTINI_CROSTINI_UPDATE_FILESYSTEM_VIEW_H_

#include "ui/base/metadata/metadata_header_macros.h"
#include "ui/views/bubble/bubble_dialog_delegate_view.h"

class Profile;

namespace crostini {
void SetCrostiniUpdateFilesystemSkipDelayForTesting(bool should_skip);
}  // namespace crostini

// Provides a warning to the user that an upgrade is occurring and Crostini
// start will take longer than usual.
class CrostiniUpdateFilesystemView : public views::BubbleDialogDelegateView {
 public:
  METADATA_HEADER(CrostiniUpdateFilesystemView);

  static void Show(Profile* profile);

  static CrostiniUpdateFilesystemView* GetActiveViewForTesting();

 private:
  CrostiniUpdateFilesystemView();
  ~CrostiniUpdateFilesystemView() override;
};

#endif  // CHROME_BROWSER_UI_VIEWS_CROSTINI_CROSTINI_UPDATE_FILESYSTEM_VIEW_H_
