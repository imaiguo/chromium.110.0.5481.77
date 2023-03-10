// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_WALLPAPER_WALLPAPER_UTILS_WALLPAPER_COLOR_CALCULATOR_H_
#define ASH_WALLPAPER_WALLPAPER_UTILS_WALLPAPER_COLOR_CALCULATOR_H_

#include "ash/ash_export.h"
#include "ash/wallpaper/wallpaper_utils/wallpaper_calculated_colors.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/time/time.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/image/image_skia.h"

namespace base {
class TaskRunner;
}

namespace color_utils {
struct ColorProfile;
}

namespace ash {

class WallpaperColorCalculatorObserver;

// Calculates colors based on a wallpaper image.
class ASH_EXPORT WallpaperColorCalculator {
 public:
  // |image|, |color_profiles| are the input parameters to the color calculation
  // that is executed on the |task_runner|.
  WallpaperColorCalculator(
      const gfx::ImageSkia& image,
      const std::vector<color_utils::ColorProfile>& color_profiles,
      scoped_refptr<base::TaskRunner> task_runner);

  WallpaperColorCalculator(const WallpaperColorCalculator&) = delete;
  WallpaperColorCalculator& operator=(const WallpaperColorCalculator&) = delete;

  ~WallpaperColorCalculator();

  void AddObserver(WallpaperColorCalculatorObserver* observer);

  void RemoveObserver(WallpaperColorCalculatorObserver* observer);

  // Initiates the calculation and returns false if the calculation fails to be
  // initiated. Observers may be notified synchronously or asynchronously.
  // Callers should be aware that this will make |image_| read-only.
  [[nodiscard]] bool StartCalculation();

  WallpaperCalculatedColors const get_calculated_colors() {
    return calculated_colors_;
  }

  void set_calculated_colors_for_test(
      const WallpaperCalculatedColors& calculated_colors) {
    calculated_colors_ = calculated_colors;
  }

  // Explicitly sets the |task_runner_| for testing.
  void SetTaskRunnerForTest(scoped_refptr<base::TaskRunner> task_runner);

 private:
  // Handles asynchronous calculation results. |async_start_time| is used to
  // record duration metrics.
  void OnAsyncCalculationComplete(
      base::TimeTicks async_start_time,
      const WallpaperCalculatedColors& calculated_colors);

  // Notifies observers that a color calulation has completed. Called on the
  // same thread that constructed |this|.
  void NotifyCalculationComplete(
      const WallpaperCalculatedColors& calculated_colors);

  // The result of the color calculation.
  WallpaperCalculatedColors calculated_colors_;

  // The image to calculate colors from.
  gfx::ImageSkia image_;

  // The color profiles used to calculate colors.
  std::vector<color_utils::ColorProfile> color_profiles_;

  // The task runner to run the calculation on.
  scoped_refptr<base::TaskRunner> task_runner_;

  base::ObserverList<WallpaperColorCalculatorObserver>::Unchecked observers_;

  base::WeakPtrFactory<WallpaperColorCalculator> weak_ptr_factory_{this};
};

}  // namespace ash

#endif  // ASH_WALLPAPER_WALLPAPER_UTILS_WALLPAPER_COLOR_CALCULATOR_H_
