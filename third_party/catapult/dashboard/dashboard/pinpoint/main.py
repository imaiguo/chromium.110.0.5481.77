# Copyright 2022 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""Pinpoint Service (Python 3)

"""
from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

import six

if six.PY3:
  import logging
  import google.cloud.logging
  from dashboard.common import datastore_hooks

  google.cloud.logging.Client().setup_logging(log_level=logging.DEBUG)
  logging.getLogger("urllib3").setLevel(logging.INFO)
  datastore_hooks.InstallHooks()

from dashboard.pinpoint import dispatcher

APP = dispatcher.APP
