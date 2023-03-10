# Copyright 2018 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

import json
import six

from dashboard.api import api_request_handler
from dashboard.common import utils
from dashboard.models import report_template


# pylint: disable=abstract-method
class ReportTemplateHandler(api_request_handler.ApiRequestHandler):

  def _CheckUser(self):
    self._CheckIsInternalUser()

  def Post(self, *args, **kwargs):
    del args, kwargs  # Unused.
    utils.LogObsoleteHandlerUsage(self, 'POST')
    template = json.loads(self.request.get('template'))
    name = self.request.get('name', None)
    owners = self.request.get('owners', None)
    if template is None or name is None or owners is None:
      raise api_request_handler.BadRequestError

    owners = owners.split(',')
    template_id = self.request.get('id', None)
    if template_id is not None:
      try:
        template_id = int(template_id)
      except ValueError as e:
        six.raise_from(api_request_handler.BadRequestError, e)
    try:
      report_template.PutTemplate(template_id, name, owners, template)
    except ValueError as e:
      six.raise_from(api_request_handler.BadRequestError, e)
    return report_template.List()
