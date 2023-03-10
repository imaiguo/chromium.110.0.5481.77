# Copyright 2019 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

import json
import six

if six.PY2:
  from dashboard.api import api_auth
  from dashboard.api import nudge_alert
  from dashboard.common import testing_common
  from dashboard.models import anomaly
  from dashboard.models import graph_data

  class NudgeAlertTest(testing_common.TestCase):

    def setUp(self):
      # TODO(https://crbug.com/1262292): Change to super() after Python2 trybots retire.
      # pylint: disable=super-with-arguments
      super(NudgeAlertTest, self).setUp()
      self.SetUpApp([('/api/nudge_alert', nudge_alert.NudgeAlertHandler)])
      self.SetCurrentClientIdOAuth(api_auth.OAUTH_CLIENT_ID_ALLOWLIST[0])
      self.SetCurrentUserOAuth(None)
      testing_common.SetSheriffDomains(['example.com'])

    def _Post(self, **params):
      return json.loads(self.Post('/api/nudge_alert', params).body)

    def testInvalidUser(self):
      self.Post('/api/nudge_alert', status=403)

    def testSuccess(self):
      self.SetCurrentUserOAuth(testing_common.INTERNAL_USER)
      path = 'm/b/s/m/c'
      test = graph_data.TestMetadata(
          has_rows=True,
          id=path,
          improvement_direction=anomaly.DOWN,
          units='units')
      test.put()
      key = anomaly.Anomaly(
          test=test.key, start_revision=1, end_revision=1).put()
      graph_data.Row(id=1, parent=test.key, value=1).put()
      response = self._Post(
          key=key.urlsafe(), new_start_revision=3, new_end_revision=4)
      self.assertEqual({}, response)
      self.assertEqual(3, key.get().start_revision)
      self.assertEqual(4, key.get().end_revision)
