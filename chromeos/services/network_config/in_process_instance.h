// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_SERVICES_NETWORK_CONFIG_IN_PROCESS_INSTANCE_H_
#define CHROMEOS_SERVICES_NETWORK_CONFIG_IN_PROCESS_INSTANCE_H_

#include "base/component_export.h"
#include "chromeos/services/network_config/public/mojom/cros_network_config.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"

namespace chromeos {
namespace network_config {

COMPONENT_EXPORT(IN_PROCESS_NETWORK_CONFIG)
void BindToInProcessInstance(
    mojo::PendingReceiver<mojom::CrosNetworkConfig> receiver);

COMPONENT_EXPORT(IN_PROCESS_NETWORK_CONFIG)
void OverrideInProcessInstanceForTesting(
    mojom::CrosNetworkConfig* network_config);

}  // namespace network_config
}  // namespace chromeos

// TODO(https://crbug.com/1164001): remove when moved to ash.
namespace ash {
namespace network_config {
using ::chromeos::network_config::BindToInProcessInstance;
}  // namespace network_config
}  // namespace ash

#endif  // CHROMEOS_SERVICES_NETWORK_CONFIG_IN_PROCESS_INSTANCE_H_
