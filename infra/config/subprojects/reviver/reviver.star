# Copyright 2022 The Chromium Authors
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//lib/builders.star", "builder", "cpu", "defaults", "free_space", "os")
load("//lib/ci.star", "ci")
load("//lib/consoles.star", "consoles")
load("//lib/dimensions.star", "dimensions")
load("//lib/polymorphic.star", "polymorphic")

luci.bucket(
    name = "reviver",
    acls = [
        acl.entry(
            roles = acl.BUILDBUCKET_READER,
            groups = "all",
        ),
        acl.entry(
            roles = acl.BUILDBUCKET_TRIGGERER,
            # TODO(crbug/1346396) Switch this to something more sensible once
            # the builders are verified
            users = [
                "gbeaty@google.com",
                "reviver-builder@chops-service-accounts.iam.gserviceaccount.com",
            ],
        ),
        acl.entry(
            roles = acl.BUILDBUCKET_OWNER,
            groups = "project-chromium-admins",
        ),
    ],
)

consoles.list_view(
    name = "reviver",
)

defaults.set(
    bucket = "reviver",
    os = os.LINUX_DEFAULT,
    pool = ci.DEFAULT_POOL,
    list_view = "reviver",
    service_account = "reviver-builder@chops-service-accounts.iam.gserviceaccount.com",

    # TODO(crbug.com/1362440): remove this.
    omit_python2 = False,
)

polymorphic.launcher(
    name = "android-launcher",
    runner = "reviver/runner",
    target_builders = [
        "ci/android-nougat-x86-rel",
        "ci/android-pie-x86-rel",
        "ci/android-12-x64-rel",
    ],
    cores = 8,
    os = os.LINUX_DEFAULT,
    pool = ci.DEFAULT_POOL,
    # To avoid peak hours, we run it at 2 AM, 5 AM, 8 AM, 11AM, 2 PM UTC.
    schedule = "0 2,5,8,11,14 * * *",
)

polymorphic.launcher(
    name = "android-device-launcher",
    runner = "reviver/runner",
    target_builders = [
        "ci/android-pie-arm64-rel",
    ],
    os = os.LINUX_DEFAULT,
    pool = ci.DEFAULT_POOL,
    # To avoid peak hours, we run it at 5 AM, 8 AM, 11AM UTC.
    schedule = "0 5,8,11 * * *",
)

polymorphic.launcher(
    name = "linux-launcher",
    runner = "reviver/runner",
    target_builders = [
        polymorphic.target_builder(
            builder = "ci/Linux Builder",
            testers = [
                "ci/Linux Tests",
            ],
        ),
    ],
    # To avoid peak hours, we run it at 5~11 UTC, 21~27 PST.
    schedule = "0 5-11/3 * * *",
)

polymorphic.launcher(
    name = "win-launcher",
    runner = "reviver/runner",
    target_builders = [
        polymorphic.target_builder(
            builder = "ci/Win x64 Builder",
            dimensions = dimensions.dimensions(
                builderless = True,
                os = os.WINDOWS_DEFAULT,
                cpu = cpu.X86_64,
                free_space = free_space.standard,
            ),
            testers = [
                "ci/Win10 Tests x64",
            ],
        ),
    ],
    # To avoid peak hours, we run it at 5~11 UTC, 21~27 PST.
    schedule = "0 5-11/3 * * *",
)

polymorphic.launcher(
    name = "mac-launcher",
    runner = "reviver/runner",
    target_builders = [
        polymorphic.target_builder(
            builder = "ci/Mac Builder",
            dimensions = dimensions.dimensions(
                builderless = True,
                os = os.MAC_DEFAULT,
                cpu = cpu.X86_64,
                free_space = free_space.standard,
            ),
            testers = [
                "ci/Mac12 Tests",
            ],
        ),
    ],
    # To avoid peak hours, we run it at 5~11 UTC, 21~27 PST.
    schedule = "0 5-11/3 * * *",
)

# A coordinator of slightly aggressive scheduling with effectively unlimited
# test bot capacity for fuchsia.
polymorphic.launcher(
    name = "fuchsia-coordinator",
    runner = "reviver/runner",
    target_builders = [
        "ci/fuchsia-fyi-arm64-dbg",
        "ci/fuchsia-fyi-x64-asan",
        "ci/fuchsia-fyi-x64-dbg",
        "ci/fuchsia-x64-rel",
    ],
    os = os.LINUX_DEFAULT,
    pool = ci.DEFAULT_POOL,
    # Avoid peak hours.
    schedule = "0 2,4,6,8,10,12,14 * * *",
)

# A coordinator for lacros.
polymorphic.launcher(
    name = "lacros-coordinator",
    runner = "reviver/runner",
    target_builders = [
        polymorphic.target_builder(
            builder = "ci/linux-lacros-builder-rel",
            testers = [
                "ci/linux-lacros-tester-rel",
            ],
        ),
    ],
    os = os.LINUX_DEFAULT,
    pool = ci.DEFAULT_POOL,
    # To avoid peak hours, we run it from 8PM TO 4AM PST. It is
    # 3 AM to 11 AM UTC.
    schedule = "0 3,5,7,9 * * *",
)

builder(
    name = "runner",
    executable = "recipe:reviver/chromium/runner",
    auto_builder_dimension = False,
    builderless = 1,
    os = os.LINUX_DEFAULT,
    cpu = cpu.X86_64,
    ssd = False,
    pool = ci.DEFAULT_POOL,
    free_space = free_space.standard,
    # TODO(crbug/1346396) Remove this once the reviver service account has
    # necessary permissions
    service_account = ci.DEFAULT_SERVICE_ACCOUNT,
    execution_timeout = 6 * time.hour,
    resultdb_bigquery_exports = [
        resultdb.export_test_results(
            bq_table = "chrome-luci-data.chromium.reviver_test_results",
        ),
    ],
)
