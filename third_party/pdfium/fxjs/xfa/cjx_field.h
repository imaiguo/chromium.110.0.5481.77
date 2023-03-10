// Copyright 2017 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_FIELD_H_
#define FXJS_XFA_CJX_FIELD_H_

#include "fxjs/xfa/cjx_container.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Field;

class CJX_Field final : public CJX_Container {
 public:
  CONSTRUCT_VIA_MAKE_GARBAGE_COLLECTED;
  ~CJX_Field() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(addItem);
  JSE_METHOD(boundItem);
  JSE_METHOD(clearItems);
  JSE_METHOD(deleteItem);
  JSE_METHOD(execCalculate);
  JSE_METHOD(execEvent);
  JSE_METHOD(execInitialize);
  JSE_METHOD(execValidate);
  JSE_METHOD(getDisplayItem);
  JSE_METHOD(getItemState);
  JSE_METHOD(getSaveItem);
  JSE_METHOD(setItemState);

  JSE_PROP(defaultValue); /* {default} */
  JSE_PROP(editValue);
  JSE_PROP(formatMessage);
  JSE_PROP(formattedValue);
  JSE_PROP(length);
  JSE_PROP(parentSubform);
  JSE_PROP(rawValue);
  JSE_PROP(selectedIndex);

 private:
  explicit CJX_Field(CXFA_Field* field);

  using Type__ = CJX_Field;
  using ParentType__ = CJX_Container;

  static const TypeTag static_type__ = TypeTag::Field;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_FIELD_H_
