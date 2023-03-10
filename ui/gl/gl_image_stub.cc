// Copyright 2012 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_image_stub.h"

#include <GL/gl.h>

#include "base/notreached.h"
#include "ui/gfx/gpu_fence.h"

namespace gl {

GLImageStub::GLImageStub() {}

GLImageStub::~GLImageStub() {}

gfx::Size GLImageStub::GetSize() {
  return gfx::Size(1, 1);
}

unsigned GLImageStub::GetInternalFormat() { return GL_RGBA; }

unsigned GLImageStub::GetDataType() {
  return GL_UNSIGNED_BYTE;
}

GLImageStub::BindOrCopy GLImageStub::ShouldBindOrCopy() {
  return BIND;
}

bool GLImageStub::BindTexImage(unsigned target) { return true; }

bool GLImageStub::CopyTexImage(unsigned target) {
  NOTREACHED();
  return false;
}

bool GLImageStub::CopyTexSubImage(unsigned target,
                                  const gfx::Point& offset,
                                  const gfx::Rect& rect) {
  return false;
}

}  // namespace gl
