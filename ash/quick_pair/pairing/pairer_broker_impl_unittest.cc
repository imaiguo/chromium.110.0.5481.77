// Copyright 2022 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ash/quick_pair/pairing/pairer_broker_impl.h"

#include "ash/quick_pair/common/account_key_failure.h"
#include "ash/quick_pair/common/device.h"
#include "ash/quick_pair/common/fast_pair/fast_pair_metrics.h"
#include "ash/quick_pair/common/pair_failure.h"
#include "ash/quick_pair/common/protocol.h"
#include "ash/quick_pair/feature_status_tracker/fake_bluetooth_adapter.h"
#include "ash/quick_pair/pairing/fast_pair/fast_pair_pairer.h"
#include "ash/quick_pair/pairing/fast_pair/fast_pair_pairer_impl.h"
#include "ash/test/ash_test_base.h"
#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/memory/scoped_refptr.h"
#include "base/run_loop.h"
#include "base/test/metrics/histogram_tester.h"
#include "base/test/mock_callback.h"
#include "device/bluetooth/bluetooth_adapter.h"
#include "device/bluetooth/bluetooth_adapter_factory.h"
#include "device/bluetooth/test/mock_bluetooth_adapter.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

constexpr char kValidModelId[] = "718c17";
constexpr char kTestDeviceAddress[] = "test_address";
constexpr char kDeviceName[] = "test_device_name";
constexpr char kBluetoothCanonicalizedAddress[] = "0C:0E:4C:C8:05:08";
constexpr base::TimeDelta kCancelPairingRetryDelay = base::Seconds(1);

const char kFastPairRetryCountMetricName[] =
    "Bluetooth.ChromeOS.FastPair.PairRetry.Count";
constexpr char kInitializePairingProcessInitial[] =
    "FastPair.InitialPairing.Initialization";
constexpr char kInitializePairingProcessSubsequent[] =
    "FastPair.SubsequentPairing.Initialization";
constexpr char kInitializePairingProcessRetroactive[] =
    "FastPair.RetroactivePairing.Initialization";

constexpr char kProtocolPairingStepInitial[] =
    "FastPair.InitialPairing.Pairing";
constexpr char kProtocolPairingStepSubsequent[] =
    "FastPair.SubsequentPairing.Pairing";

class FakeFastPairPairer : public ash::quick_pair::FastPairPairer {
 public:
  FakeFastPairPairer(
      scoped_refptr<device::BluetoothAdapter> adapter,
      scoped_refptr<ash::quick_pair::Device> device,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
          handshake_complete_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
          paired_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>,
                              ash::quick_pair::PairFailure)>
          pair_failed_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>,
                              ash::quick_pair::AccountKeyFailure)>
          account_key_failure_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
          pairing_procedure_complete)
      : adapter_(adapter),
        device_(device),
        handshake_complete_callback_(std::move(handshake_complete_callback)),
        paired_callback_(std::move(paired_callback)),
        pair_failed_callback_(std::move(pair_failed_callback)),
        account_key_failure_callback_(std::move(account_key_failure_callback)),
        pairing_procedure_complete_(std::move(pairing_procedure_complete)) {}

  ~FakeFastPairPairer() override = default;

  void TriggerHandshakeCompleteCallback() {
    EXPECT_TRUE(handshake_complete_callback_);
    std::move(handshake_complete_callback_).Run(device_);
  }

  void TriggerPairedCallback() {
    EXPECT_TRUE(paired_callback_);
    std::move(paired_callback_).Run(device_);
  }

  void TriggerPairingProcedureCompleteCallback() {
    EXPECT_TRUE(pairing_procedure_complete_);
    std::move(pairing_procedure_complete_).Run(device_);
  }

  void TriggerAccountKeyFailureCallback(
      ash::quick_pair::AccountKeyFailure failure) {
    EXPECT_TRUE(account_key_failure_callback_);
    std::move(account_key_failure_callback_).Run(device_, failure);
  }

  void TriggerPairFailureCallback(ash::quick_pair::PairFailure failure) {
    EXPECT_TRUE(pair_failed_callback_);
    std::move(pair_failed_callback_).Run(device_, failure);
  }

 private:
  scoped_refptr<device::BluetoothAdapter> adapter_;
  scoped_refptr<ash::quick_pair::Device> device_;
  base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
      handshake_complete_callback_;
  base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
      paired_callback_;
  base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>,
                          ash::quick_pair::PairFailure)>
      pair_failed_callback_;
  base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>,
                          ash::quick_pair::AccountKeyFailure)>
      account_key_failure_callback_;
  base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
      pairing_procedure_complete_;
};

class FakeFastPairPairerFactory
    : public ash::quick_pair::FastPairPairerImpl::Factory {
 public:
  std::unique_ptr<ash::quick_pair::FastPairPairer> CreateInstance(
      scoped_refptr<device::BluetoothAdapter> adapter,
      scoped_refptr<ash::quick_pair::Device> device,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
          handshake_complete_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
          paired_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>,
                              ash::quick_pair::PairFailure)>
          pair_failed_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>,
                              ash::quick_pair::AccountKeyFailure)>
          account_key_failure_callback,
      base::OnceCallback<void(scoped_refptr<ash::quick_pair::Device>)>
          pairing_procedure_complete) override {
    auto fake_fast_pair_pairer = std::make_unique<FakeFastPairPairer>(
        std::move(adapter), std::move(device),
        std::move(handshake_complete_callback), std::move(paired_callback),
        std::move(pair_failed_callback),
        std::move(account_key_failure_callback),
        std::move(pairing_procedure_complete));
    fake_fast_pair_pairer_ = fake_fast_pair_pairer.get();
    return fake_fast_pair_pairer;
  }

  ~FakeFastPairPairerFactory() override = default;

  FakeFastPairPairer* fake_fast_pair_pairer() { return fake_fast_pair_pairer_; }

 protected:
  FakeFastPairPairer* fake_fast_pair_pairer_ = nullptr;
};

}  // namespace

namespace ash {
namespace quick_pair {

class PairerBrokerImplTest : public AshTestBase, public PairerBroker::Observer {
 public:
  PairerBrokerImplTest()
      : AshTestBase(base::test::TaskEnvironment::TimeSource::MOCK_TIME) {}

  void SetUp() override {
    AshTestBase::SetUp();

    adapter_ = base::MakeRefCounted<FakeBluetoothAdapter>();

    device::BluetoothAdapterFactory::SetAdapterForTesting(adapter_);

    fast_pair_pairer_factory_ = std::make_unique<FakeFastPairPairerFactory>();
    FastPairPairerImpl::Factory::SetFactoryForTesting(
        fast_pair_pairer_factory_.get());

    pairer_broker_ = std::make_unique<PairerBrokerImpl>();
    pairer_broker_->AddObserver(this);
  }

  void TearDown() override {
    pairer_broker_->RemoveObserver(this);
    pairer_broker_.reset();
    AshTestBase::TearDown();
  }

  void OnDevicePaired(scoped_refptr<Device> device) override {
    ++device_paired_count_;
  }

  void OnPairFailure(scoped_refptr<Device> device,
                     PairFailure failure) override {
    ++pair_failure_count_;
  }

  void OnAccountKeyWrite(scoped_refptr<Device> device,
                         absl::optional<AccountKeyFailure> error) override {
    ++account_key_write_count_;
  }

  void OnPairingStart(scoped_refptr<Device> device) override {
    pairing_started_ = true;
  }

  void OnHandshakeComplete(scoped_refptr<Device> device) override {
    handshake_complete_ = true;
  }

  void OnPairingComplete(scoped_refptr<Device> device) override {
    device_pair_complete_ = true;
  }

 protected:
  int device_paired_count_ = 0;
  int pair_failure_count_ = 0;
  int account_key_write_count_ = 0;
  bool pairing_started_ = false;
  bool handshake_complete_ = false;
  bool device_pair_complete_ = false;

  base::HistogramTester histogram_tester_;
  scoped_refptr<FakeBluetoothAdapter> adapter_;
  device::MockBluetoothDevice* mock_bluetooth_device_ptr_ = nullptr;
  std::unique_ptr<FakeFastPairPairerFactory> fast_pair_pairer_factory_;

  std::unique_ptr<PairerBroker> pairer_broker_;
};

TEST_F(PairerBrokerImplTest, PairDevice_Initial) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairInitial);
  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()->TriggerPairedCallback();

  EXPECT_TRUE(pairer_broker_->IsPairing());
  EXPECT_EQ(device_paired_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 1);

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairingProcedureCompleteCallback();
  EXPECT_FALSE(pairer_broker_->IsPairing());
}

TEST_F(PairerBrokerImplTest, PairDevice_Subsequent) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairSubsequent);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairing_started_);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerHandshakeCompleteCallback();
  EXPECT_TRUE(handshake_complete_);

  fast_pair_pairer_factory_->fake_fast_pair_pairer()->TriggerPairedCallback();

  EXPECT_TRUE(pairer_broker_->IsPairing());
  EXPECT_EQ(device_paired_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 1);

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairingProcedureCompleteCallback();
  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_TRUE(device_pair_complete_);
}

TEST_F(PairerBrokerImplTest, PairDevice_Retroactive) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairRetroactive);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()->TriggerPairedCallback();

  EXPECT_TRUE(pairer_broker_->IsPairing());
  EXPECT_EQ(device_paired_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 1);

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairingProcedureCompleteCallback();
  EXPECT_FALSE(pairer_broker_->IsPairing());
}

TEST_F(PairerBrokerImplTest, AlreadyPairingDevice_Initial) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairInitial);

  pairer_broker_->PairDevice(device);
  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()->TriggerPairedCallback();

  EXPECT_TRUE(pairer_broker_->IsPairing());
  EXPECT_EQ(device_paired_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 1);
  EXPECT_EQ(histogram_tester_.GetBucketCount(
                kInitializePairingProcessInitial,
                FastPairInitializePairingProcessEvent::kAlreadyPairingFailure),
            1);
}

TEST_F(PairerBrokerImplTest, AlreadyPairingDevice_Subsequent) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairSubsequent);

  pairer_broker_->PairDevice(device);
  pairer_broker_->PairDevice(device);
  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()->TriggerPairedCallback();

  EXPECT_TRUE(pairer_broker_->IsPairing());
  EXPECT_EQ(device_paired_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 1);
  EXPECT_EQ(histogram_tester_.GetBucketCount(
                kInitializePairingProcessSubsequent,
                FastPairInitializePairingProcessEvent::kAlreadyPairingFailure),
            1);
}

TEST_F(PairerBrokerImplTest, AlreadyPairingDevice_Retroactive) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairRetroactive);

  pairer_broker_->PairDevice(device);
  pairer_broker_->PairDevice(device);
  base::RunLoop().RunUntilIdle();
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()->TriggerPairedCallback();
  base::RunLoop().RunUntilIdle();

  EXPECT_TRUE(pairer_broker_->IsPairing());
  EXPECT_EQ(device_paired_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 1);
  EXPECT_EQ(histogram_tester_.GetBucketCount(
                kInitializePairingProcessRetroactive,
                FastPairInitializePairingProcessEvent::kAlreadyPairingFailure),
            1);
}

TEST_F(PairerBrokerImplTest, PairAfterCancelPairing) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairInitial);
  device->set_classic_address(kBluetoothCanonicalizedAddress);

  // Add a matching mock device to the bluetooth adapter with the
  // same address to mock the relationship between Device and
  // device::BluetoothDevice.
  std::unique_ptr<testing::NiceMock<device::MockBluetoothDevice>>
      mock_bluetooth_device_ =
          std::make_unique<testing::NiceMock<device::MockBluetoothDevice>>(
              adapter_.get(), 0, kDeviceName, kBluetoothCanonicalizedAddress,
              true, false);
  mock_bluetooth_device_ptr_ = mock_bluetooth_device_.get();
  adapter_->AddMockDevice(std::move(mock_bluetooth_device_));

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());
  EXPECT_CALL(*mock_bluetooth_device_ptr_, IsPaired())
      .WillOnce(testing::Return(false));

  // Attempt to pair with a failure.
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);

  // Fast forward |kCancelPairingDelay| seconds to allow the retry callback to
  // be called.
  task_environment()->FastForwardBy(kCancelPairingRetryDelay);

  // Now allow the pairing to succeed.
  fast_pair_pairer_factory_->fake_fast_pair_pairer()->TriggerPairedCallback();

  EXPECT_EQ(device_paired_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 1);
}

TEST_F(PairerBrokerImplTest, PairDeviceFailureMax_Initial) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairInitial);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);

  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_EQ(pair_failure_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  histogram_tester_.ExpectTotalCount(kProtocolPairingStepInitial, 1);
}

TEST_F(PairerBrokerImplTest, PairDeviceFailureMax_Subsequent) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairSubsequent);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);

  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_EQ(pair_failure_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  histogram_tester_.ExpectTotalCount(kProtocolPairingStepSubsequent, 1);
}

TEST_F(PairerBrokerImplTest, PairDeviceFailureMax_Retroactive) {
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairRetroactive);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);
  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerPairFailureCallback(
          PairFailure::kPasskeyCharacteristicNotifySession);

  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_EQ(pair_failure_count_, 1);
  histogram_tester_.ExpectTotalCount(kFastPairRetryCountMetricName, 0);
}

TEST_F(PairerBrokerImplTest, AccountKeyFailure_Initial) {
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairInitial);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerAccountKeyFailureCallback(
          AccountKeyFailure::kAccountKeyCharacteristicDiscovery);

  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_EQ(account_key_write_count_, 1);
}

TEST_F(PairerBrokerImplTest, AccountKeyFailure_Subsequent) {
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairSubsequent);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerAccountKeyFailureCallback(
          AccountKeyFailure::kAccountKeyCharacteristicDiscovery);

  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_EQ(account_key_write_count_, 1);
}

TEST_F(PairerBrokerImplTest, AccountKeyFailure_Retroactive) {
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairRetroactive);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  fast_pair_pairer_factory_->fake_fast_pair_pairer()
      ->TriggerAccountKeyFailureCallback(
          AccountKeyFailure::kAccountKeyCharacteristicDiscovery);

  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_EQ(account_key_write_count_, 1);
}

TEST_F(PairerBrokerImplTest, StopPairing) {
  auto device = base::MakeRefCounted<Device>(kValidModelId, kTestDeviceAddress,
                                             Protocol::kFastPairInitial);

  pairer_broker_->PairDevice(device);
  EXPECT_TRUE(pairer_broker_->IsPairing());

  // Stop Pairing mid pair.
  pairer_broker_->StopPairing();
  EXPECT_FALSE(pairer_broker_->IsPairing());
  EXPECT_EQ(pair_failure_count_, 0);

  // Stop Pairing when we are not pairing should cause no issues.
  pairer_broker_->StopPairing();
  EXPECT_FALSE(pairer_broker_->IsPairing());
}

}  // namespace quick_pair
}  // namespace ash
