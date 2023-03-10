# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

from dashboard.api import api_request_handler
from dashboard.common import utils
from dashboard.models import graph_data


# pylint: disable=abstract-method
class ListTimeseriesHandler(api_request_handler.ApiRequestHandler):
  """API handler for listing timeseries for a benchmark."""

  def _CheckUser(self):
    self._CheckIsLoggedIn()

  def Post(self, *args, **kwargs):
    """Returns list in response to API requests.

    Argument:
      benchmark: name of the benchmark to list tests for

    Outputs:
      JSON list of monitored timeseries for the benchmark, see README.md.
    """
    utils.LogObsoleteHandlerUsage(self, 'POST')
    del kwargs  # Unused.
    benchmark = args[0]
    sheriff_name = self.request.get('sheriff', 'Chromium Perf Sheriff')
    query = graph_data.TestMetadata.query()
    query = query.filter(graph_data.TestMetadata.suite_name == benchmark)
    query = query.filter(graph_data.TestMetadata.has_rows == True)
    query = query.filter(graph_data.TestMetadata.deprecated == False)
    if sheriff_name and sheriff_name != 'all':
      print(sheriff_name, 'xxxxxxxxxxxxxxxxxxxxxxxx')
      raise api_request_handler.BadRequestError(
          'Not supporting sheriff name anymore. Use `all` instead.')

    keys = query.fetch(keys_only=True)
    return [utils.TestPath(key) for key in keys]
