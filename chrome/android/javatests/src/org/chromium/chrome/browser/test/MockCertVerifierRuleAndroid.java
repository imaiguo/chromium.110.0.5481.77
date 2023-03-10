// Copyright 2018 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.test;

import org.junit.rules.ExternalResource;

import org.chromium.content_public.browser.test.NativeLibraryTestUtils;

/** JUnit test rule which enables tests to force certificate verification results. */
public class MockCertVerifierRuleAndroid extends ExternalResource {

    private long mNativePtr;

    // Certificate verification result to force.
    private int mResult;

    public MockCertVerifierRuleAndroid(int result) {
        mResult = result;
    }

    @Override
    protected void before() {
        NativeLibraryTestUtils.loadNativeLibraryNoBrowserProcess();
        mNativePtr = nativeInit();
        nativeSetResult(mNativePtr, mResult);
        nativeSetUp(mNativePtr);
    }

    public void setResult(int result) {
        mResult = result;
        if (mNativePtr != 0) {
            nativeSetResult(mNativePtr, result);
        }
    }

    @Override
    protected void after() {
        nativeTearDown(mNativePtr);
        mNativePtr = 0;
    }

    private static native long nativeInit();
    private native void nativeSetUp(long nativeMockCertVerifierRuleAndroid);
    private native void nativeSetResult(long nativeMockCertVerifierRuleAndroid, int result);
    private native void nativeTearDown(long nativeMockCertVerifierRuleAndroid);
}
