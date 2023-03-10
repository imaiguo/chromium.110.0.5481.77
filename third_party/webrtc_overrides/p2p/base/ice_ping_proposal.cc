#include "third_party/webrtc_overrides/p2p/base/ice_ping_proposal.h"

#include "third_party/webrtc/p2p/base/ice_controller_interface.h"
#include "third_party/webrtc_overrides/p2p/base/ice_proposal.h"

namespace blink {

IcePingProposal::IcePingProposal(
    const cricket::IceControllerInterface::PingResult& ping_result,
    bool reply_expected)
    : IceProposal(reply_expected),
      connection_(ping_result.connection),
      recheck_delay_ms_(ping_result.recheck_delay_ms) {}

}  // namespace blink
