# Copyright 2019 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

from google.appengine.ext import ndb

from dashboard import associate_alerts
from dashboard.api import api_request_handler
from dashboard.common import utils


# pylint: disable=abstract-method
class ExistingBugHandler(api_request_handler.ApiRequestHandler):

  def _CheckUser(self):
    if not utils.IsValidSheriffUser():
      raise api_request_handler.ForbiddenError()

  def Post(self, *args, **kwargs):
    utils.LogObsoleteHandlerUsage(self, 'POST')
    del args, kwargs  # Unused.
    keys = self.request.get_all('key')
    bug_id = int(self.request.get('bug'))
    project_id = self.request.get('project_id', 'chromium')
    alerts = ndb.get_multi([ndb.Key(urlsafe=k) for k in keys])
    associate_alerts.AssociateAlerts(bug_id, project_id, alerts)
    return {}
