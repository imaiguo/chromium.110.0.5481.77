# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

from __future__ import print_function
from __future__ import division
from __future__ import absolute_import

import mock
import six
import unittest

if six.PY2:
  from dashboard.api import api_auth
  from dashboard.api import bugs
  from dashboard.common import testing_common
  from dashboard.common import utils

  # TODO(https://crbug.com/1262292): Update after Python2 trybots retire.
  # pylint: disable=useless-object-inheritance
  class MockIssueTrackerService(object):
    """A fake version of IssueTrackerService that returns expected data."""

    def __init__(self, http=None):
      pass

    @classmethod
    # pylint: disable=keyword-arg-before-vararg
    def List(cls, project='chromium', *unused_args, **unused_kwargs):
      del project
      return {
          'items': [
              {
                  'id': 12345,
                  'summary': '5% regression in bot/suite/x at 10000:20000',
                  'state': 'open',
                  'status': 'New',
                  'author': {
                      'name': 'exam...@google.com'
                  },
              },
              {
                  'id': 13579,
                  'summary': '1% regression in bot/suite/y at 10000:20000',
                  'state': 'closed',
                  'status': 'WontFix',
                  'author': {
                      'name': 'exam...@google.com'
                  },
              },
          ]
      }

    @classmethod
    def GetIssue(cls, bug_id, project='chromium'):
      del bug_id
      del project
      return {
          'cc': [{
              'kind': 'monorail#issuePerson',
              'htmlLink': 'https://bugs.chromium.org/u/1253971105',
              'name': 'user@chromium.org',
          }, {
              'kind': 'monorail#issuePerson',
              'name': 'hello@world.org',
          }],
          'labels': [
              'Type-Bug',
              'Pri-3',
              'M-61',
          ],
          'owner': {
              'kind': 'monorail#issuePerson',
              'htmlLink': 'https://bugs.chromium.org/u/49586776',
              'name': 'owner@chromium.org',
          },
          'id': 737355,
          'author': {
              'kind': 'monorail#issuePerson',
              'htmlLink': 'https://bugs.chromium.org/u/49586776',
              'name': 'author@chromium.org',
          },
          'state': 'closed',
          'status': 'Fixed',
          'summary': 'The bug title',
          'components': [
              'Blink>ServiceWorker',
              'Foo>Bar',
          ],
          'published': '2017-06-28T01:26:53',
          'updated': '2018-03-01T16:16:22',
      }

    @classmethod
    def GetIssueComments(cls, _, project='chromium'):
      del project
      return [{
          'content': 'Comment one',
          'published': '2017-06-28T04:42:55',
          'author': 'comment-one-author@company.com',
      }, {
          'content': 'Comment two',
          'published': '2017-06-28T10:16:14',
          'author': 'author-two@chromium.org'
      }]

  class BugsTest(testing_common.TestCase):

    def setUp(self):
      # TODO(https://crbug.com/1262292): Change to super() after Python2 trybots retire.
      # pylint: disable=super-with-arguments
      super(BugsTest, self).setUp()
      self.SetUpApp([
          (r'/api/bugs/p/(.+)/(.+)', bugs.BugsWithProjectHandler),
          (r'/api/bugs/(.*)', bugs.BugsHandler),
      ])

      # Add a fake issue tracker service that we can get call values from.
      self.original_service = bugs.issue_tracker_service.IssueTrackerService
      bugs.issue_tracker_service = mock.MagicMock()
      self.service = MockIssueTrackerService
      bugs.issue_tracker_service.IssueTrackerService = self.service
      self.SetCurrentClientIdOAuth(api_auth.OAUTH_CLIENT_ID_ALLOWLIST[0])

    def tearDown(self):
      # TODO(https://crbug.com/1262292): Change to super() after Python2 trybots retire.
      # pylint: disable=super-with-arguments
      super(BugsTest, self).tearDown()
      bugs.issue_tracker_service.IssueTrackerService = self.original_service

    @mock.patch.object(utils, 'ServiceAccountHttp', mock.MagicMock())
    def testPost_WithValidBug_ShowsData(self):
      self.SetCurrentUserOAuth(testing_common.INTERNAL_USER)
      response = self.Post('/api/bugs/123456?include_comments=true')
      bug = self.GetJsonValue(response, 'bug')
      self.assertEqual('The bug title', bug.get('summary'))
      self.assertEqual(2, len(bug.get('cc')))
      self.assertEqual('hello@world.org', bug.get('cc')[1])
      self.assertEqual('Fixed', bug.get('status'))
      self.assertEqual('closed', bug.get('state'))
      self.assertEqual('author@chromium.org', bug.get('author'))
      self.assertEqual('owner@chromium.org', bug.get('owner'))
      self.assertEqual('2017-06-28T01:26:53', bug.get('published'))
      self.assertEqual('2018-03-01T16:16:22', bug.get('updated'))
      self.assertEqual('chromium', bug.get('projectId'))
      self.assertEqual(2, len(bug.get('comments')))
      self.assertEqual('Comment two', bug.get('comments')[1].get('content'))
      self.assertEqual('author-two@chromium.org',
                       bug.get('comments')[1].get('author'))

    @mock.patch.object(utils, 'ServiceAccountHttp', mock.MagicMock())
    def testPost_WithAlternateUrlWorksWithProjects(self):
      self.SetCurrentUserOAuth(testing_common.INTERNAL_USER)
      response = self.Post('/api/bugs/p/chromium/123456?include_comments=true')
      bug = self.GetJsonValue(response, 'bug')
      self.assertEqual('The bug title', bug.get('summary'))
      self.assertEqual(2, len(bug.get('cc')))
      self.assertEqual('hello@world.org', bug.get('cc')[1])
      self.assertEqual('Fixed', bug.get('status'))
      self.assertEqual('closed', bug.get('state'))
      self.assertEqual('author@chromium.org', bug.get('author'))
      self.assertEqual('owner@chromium.org', bug.get('owner'))
      self.assertEqual('2017-06-28T01:26:53', bug.get('published'))
      self.assertEqual('2018-03-01T16:16:22', bug.get('updated'))
      self.assertEqual('chromium', bug.get('projectId'))
      self.assertEqual(2, len(bug.get('comments')))
      self.assertEqual('Comment two', bug.get('comments')[1].get('content'))
      self.assertEqual('author-two@chromium.org',
                       bug.get('comments')[1].get('author'))

    @mock.patch.object(utils, 'ServiceAccountHttp', mock.MagicMock())
    def testPost_WithValidBugButNoComments(self):
      self.SetCurrentUserOAuth(testing_common.INTERNAL_USER)

      response = self.Post('/api/bugs/123456')
      bug = self.GetJsonValue(response, 'bug')
      self.assertNotIn('comments', bug)

    @mock.patch.object(utils, 'ServiceAccountHttp', mock.MagicMock())
    def testPost_Recent(self):
      self.SetCurrentUserOAuth(testing_common.INTERNAL_USER)
      self.assertEqual(MockIssueTrackerService.List()['items'],
                       self.GetJsonValue(self.Post('/api/bugs/recent'), 'bugs'))

    @mock.patch.object(utils, 'ServiceAccountHttp', mock.MagicMock())
    def testPost_WithInvalidBugIdParameter_ShowsError(self):
      self.SetCurrentUserOAuth(testing_common.INTERNAL_USER)
      response = self.Post('/api/bugs/foo', status=400)
      self.assertIn('Invalid bug ID \\"foo\\".', response.body)

    @mock.patch.object(utils, 'ServiceAccountHttp', mock.MagicMock())
    def testPost_NoAccess_ShowsError(self):
      self.SetCurrentUserOAuth(testing_common.EXTERNAL_USER)
      response = self.Post('/api/bugs/foo', status=403)
      self.assertIn('Access denied', response.body)

    def testPost_NoOauthUser(self):
      self.SetCurrentUserOAuth(None)
      self.Post('/api/bugs/12345', status=401)

    def testPost_BadOauthClientId(self):
      self.SetCurrentUserOAuth(testing_common.INTERNAL_USER)
      self.SetCurrentClientIdOAuth('invalid')
      self.Post('/api/bugs/12345', status=403)

  if __name__ == '__main__':
    unittest.main()
