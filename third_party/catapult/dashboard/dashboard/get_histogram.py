# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""URL endpoint for getting a histogram."""
from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

import json

from google.appengine.ext import ndb

from dashboard.common import request_handler
from dashboard.common import utils


class GetHistogramHandler(request_handler.RequestHandler):
  """URL endpoint to get histogramby guid."""

  def post(self):
    """Fetches a histogram by guid.

    Request parameters:
      guid: GUID of requested histogram.

    Outputs:
      JSON serialized Histogram.
    """
    utils.LogObsoleteHandlerUsage(self, 'POST')
    guid = self.request.get('guid')
    if not guid:
      self.ReportError('Missing "guid" parameter', status=400)
      return

    histogram_key = ndb.Key('Histogram', guid)
    try:
      histogram_entity = histogram_key.get()
    except AssertionError:
      # Thrown if accessing internal_only as an external user.
      self.ReportError('Histogram "%s" not found' % guid, status=400)
      return

    if not histogram_entity:
      self.ReportError('Histogram "%s" not found' % guid, status=400)
      return

    self.response.out.write(json.dumps(histogram_entity.data))
