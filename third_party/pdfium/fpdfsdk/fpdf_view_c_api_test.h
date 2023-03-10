// Copyright 2015 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FPDFSDK_FPDF_VIEW_C_API_TEST_H_
#define FPDFSDK_FPDF_VIEW_C_API_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif

// Function to call from gtest harness to ensure linker resolution. Returns
// 1 on success or 0 on error.
int CheckPDFiumCApi();

#ifdef __cplusplus
}
#endif

#endif  // FPDFSDK_FPDF_VIEW_C_API_TEST_H_
