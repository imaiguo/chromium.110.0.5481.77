# Copyright 2015 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

from dashboard.common import layered_cache
from dashboard.common import request_handler

import six

from flask import make_response


def LayeredCacheDeleteExpiredGet():
  """This get handler is called from cron.

  It deletes only expired CachedPickledString entities from the datastore.
  """
  layered_cache.DeleteAllExpiredEntities()
  return make_response('')


if six.PY2:

  class LayeredCacheDeleteExpiredHandler(request_handler.RequestHandler):
    """URL endpoint for a cron job to delete expired entities from datastore."""

    def get(self):
      """This get handler is called from cron.

      It deletes only expired CachedPickledString entities from the datastore.
      """
      layered_cache.DeleteAllExpiredEntities()
