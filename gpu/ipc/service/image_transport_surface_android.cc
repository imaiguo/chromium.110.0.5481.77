// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/ipc/service/image_transport_surface.h"

#include <utility>

#include "base/feature_list.h"
#include "base/logging.h"
#include "base/task/single_thread_task_runner.h"
#include "gpu/command_buffer/service/feature_info.h"
#include "gpu/config/gpu_finch_features.h"
#include "gpu/ipc/common/gpu_surface_lookup.h"
#include "gpu/ipc/service/pass_through_image_transport_surface.h"
#include "ui/gl/android/scoped_a_native_window.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_surface_egl_surface_control.h"
#include "ui/gl/gl_surface_stub.h"

namespace gpu {

// static
scoped_refptr<gl::Presenter> ImageTransportSurface::CreatePresenter(
    gl::GLDisplay* display,
    base::WeakPtr<ImageTransportSurfaceDelegate> delegate,
    SurfaceHandle surface_handle,
    gl::GLSurfaceFormat format) {
  if (gl::GetGLImplementation() == gl::kGLImplementationMockGL ||
      gl::GetGLImplementation() == gl::kGLImplementationStubGL)
    return nullptr;

  if (!delegate ||
      !delegate->GetFeatureInfo()->feature_flags().android_surface_control) {
    return nullptr;
  }

  DCHECK(GpuSurfaceLookup::GetInstance());
  DCHECK_NE(surface_handle, kNullSurfaceHandle);
  // On Android, the surface_handle is the id of the surface in the
  // GpuSurfaceTracker/GpuSurfaceLookup
  bool can_be_used_with_surface_control = false;
  gl::ScopedJavaSurface scoped_java_surface =
      GpuSurfaceLookup::GetInstance()->AcquireJavaSurface(
          surface_handle, &can_be_used_with_surface_control);
  gl::ScopedANativeWindow window(scoped_java_surface);
  if (!window) {
    LOG(WARNING) << "Failed to acquire ANativeWindow";
    return nullptr;
  }

  if (!can_be_used_with_surface_control) {
    return nullptr;
  }

  scoped_refptr<gl::Presenter> surface = new gl::GLSurfaceEGLSurfaceControl(
      display->GetAs<gl::GLDisplayEGL>(), std::move(window),
      base::SingleThreadTaskRunner::GetCurrentDefault());

  bool initialize_success = surface->Initialize(format);
  if (!initialize_success)
    return nullptr;

  return surface;
}

// static
scoped_refptr<gl::GLSurface> ImageTransportSurface::CreateNativeGLSurface(
    gl::GLDisplay* display,
    base::WeakPtr<ImageTransportSurfaceDelegate> delegate,
    SurfaceHandle surface_handle,
    gl::GLSurfaceFormat format) {
  if (gl::GetGLImplementation() == gl::kGLImplementationMockGL ||
      gl::GetGLImplementation() == gl::kGLImplementationStubGL)
    return new gl::GLSurfaceStub;
  DCHECK(GpuSurfaceLookup::GetInstance());
  DCHECK_NE(surface_handle, kNullSurfaceHandle);
  // On Android, the surface_handle is the id of the surface in the
  // GpuSurfaceTracker/GpuSurfaceLookup
  bool can_be_used_with_surface_control = false;
  gl::ScopedJavaSurface scoped_java_surface =
      GpuSurfaceLookup::GetInstance()->AcquireJavaSurface(
          surface_handle, &can_be_used_with_surface_control);
  gl::ScopedANativeWindow window(scoped_java_surface);
  if (!window) {
    LOG(WARNING) << "Failed to acquire ANativeWindow";
    return nullptr;
  }

  scoped_refptr<gl::GLSurface> surface = new gl::NativeViewGLSurfaceEGL(
      display->GetAs<gl::GLDisplayEGL>(), std::move(window), nullptr);

  bool initialize_success = surface->Initialize(format);
  if (!initialize_success)
    return scoped_refptr<gl::GLSurface>();

  return scoped_refptr<gl::GLSurface>(
      new PassThroughImageTransportSurface(delegate, surface.get(), false));
}

}  // namespace gpu
