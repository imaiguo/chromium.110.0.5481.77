// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/video_capture/public/cpp/mock_video_capture_service.h"
#include "build/chromeos_buildflags.h"

#include "mojo/public/cpp/bindings/pending_receiver.h"

namespace video_capture {

MockVideoCaptureService::MockVideoCaptureService() {}

MockVideoCaptureService::~MockVideoCaptureService() = default;

void MockVideoCaptureService::ConnectToDeviceFactory(
    mojo::PendingReceiver<video_capture::mojom::DeviceFactory> receiver) {
  DoConnectToDeviceFactory(std::move(receiver));
}

void MockVideoCaptureService::ConnectToVideoSourceProvider(
    mojo::PendingReceiver<video_capture::mojom::VideoSourceProvider> receiver) {
  DoConnectToVideoSourceProvider(std::move(receiver));
}

#if BUILDFLAG(IS_CHROMEOS_ASH)
void MockVideoCaptureService::InjectGpuDependencies(
    mojo::PendingRemote<video_capture::mojom::AcceleratorFactory>
        accelerator_factory) {
  DoInjectGpuDependencies(std::move(accelerator_factory));
}
#endif  // BUILDFLAG(IS_CHROMEOS_ASH)

}  // namespace video_capture
