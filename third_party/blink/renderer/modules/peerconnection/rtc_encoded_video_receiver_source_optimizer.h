// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_ENCODED_VIDEO_RECEIVER_SOURCE_OPTIMIZER_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_ENCODED_VIDEO_RECEIVER_SOURCE_OPTIMIZER_H_

#include "third_party/blink/renderer/core/streams/readable_stream_transferring_optimizer.h"
#include "third_party/blink/renderer/core/streams/underlying_source_base.h"
#include "third_party/blink/renderer/modules/peerconnection/rtc_encoded_video_underlying_source.h"

namespace blink {

class UnderlyingSourceBase;
class ScriptState;
class RTCEncodedVideoUnderlyingSource;

class MODULES_EXPORT RtcEncodedVideoReceiverSourceOptimizer
    : public ReadableStreamTransferringOptimizer {
 public:
  using UnderlyingSourceSetter = WTF::CrossThreadFunction<void(
      RTCEncodedVideoUnderlyingSource*,
      scoped_refptr<base::SingleThreadTaskRunner>)>;
  RtcEncodedVideoReceiverSourceOptimizer(
      UnderlyingSourceSetter,
      WTF::CrossThreadOnceClosure disconnect_callback);
  UnderlyingSourceBase* PerformInProcessOptimization(
      ScriptState* script_state) override;

 private:
  UnderlyingSourceSetter set_underlying_source_;
  WTF::CrossThreadOnceClosure disconnect_callback_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_PEERCONNECTION_RTC_ENCODED_VIDEO_RECEIVER_SOURCE_OPTIMIZER_H_
