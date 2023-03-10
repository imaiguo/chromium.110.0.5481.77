// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMEOS_ASH_COMPONENTS_DBUS_EASY_UNLOCK_FAKE_EASY_UNLOCK_CLIENT_H_
#define CHROMEOS_ASH_COMPONENTS_DBUS_EASY_UNLOCK_FAKE_EASY_UNLOCK_CLIENT_H_

#include <string>

#include "base/component_export.h"
#include "chromeos/ash/components/dbus/easy_unlock/easy_unlock_client.h"

namespace ash {

// A fake implementation of EasyUnlockClient.
class COMPONENT_EXPORT(ASH_DBUS_EASY_UNLOCK) FakeEasyUnlockClient
    : public EasyUnlockClient {
 public:
  // Tests if the provided keys belong to the same (fake) EC P256 key pair
  // generated by |this|.
  static bool IsEcP256KeyPair(const std::string& private_key,
                              const std::string& public_key);

  FakeEasyUnlockClient();

  FakeEasyUnlockClient(const FakeEasyUnlockClient&) = delete;
  FakeEasyUnlockClient& operator=(const FakeEasyUnlockClient&) = delete;

  ~FakeEasyUnlockClient() override;

  // EasyUnlockClient overrides
  void Init(dbus::Bus* bus) override;
  void GenerateEcP256KeyPair(KeyPairCallback callback) override;
  void WrapPublicKey(const std::string& key_algorithm,
                     const std::string& public_key,
                     DataCallback callback) override;
  void PerformECDHKeyAgreement(const std::string& private_key,
                               const std::string& public_key,
                               DataCallback callback) override;
  void CreateSecureMessage(const std::string& payload,
                           const CreateSecureMessageOptions& options,
                           DataCallback callback) override;
  void UnwrapSecureMessage(const std::string& message,
                           const UnwrapSecureMessageOptions& options,
                           DataCallback callback) override;

 private:
  int generated_keys_count_;
};

}  // namespace ash

#endif  // CHROMEOS_ASH_COMPONENTS_DBUS_EASY_UNLOCK_FAKE_EASY_UNLOCK_CLIENT_H_
