// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_WM_CORE_CURSOR_UTIL_H_
#define UI_WM_CORE_CURSOR_UTIL_H_

#include <vector>

#include "base/component_export.h"
#include "ui/display/display.h"

class SkBitmap;

namespace gfx {
class Point;
}

namespace wm {

// Scale and rotate the cursor's bitmap and hotpoint.
// |bitmap_in_out| and |hotpoint_in_out| are used as
// both input and output.
COMPONENT_EXPORT(UI_WM)
void ScaleAndRotateCursorBitmapAndHotpoint(float scale,
                                           display::Display::Rotation rotation,
                                           SkBitmap* bitmap_in_out,
                                           gfx::Point* hotpoint_in_out);

// Helpers for CursorLoader.
void GetImageCursorBitmap(int resource_id,
                          float scale,
                          display::Display::Rotation rotation,
                          gfx::Point* hotspot,
                          SkBitmap* bitmap);
void GetAnimatedCursorBitmaps(int resource_id,
                              float scale,
                              display::Display::Rotation rotation,
                              gfx::Point* hotspot,
                              std::vector<SkBitmap>* bitmaps);

}  // namespace wm

#endif  // UI_WM_CORE_CURSOR_UTIL_H_
