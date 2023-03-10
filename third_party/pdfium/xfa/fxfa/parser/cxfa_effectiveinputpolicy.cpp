// Copyright 2017 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_effectiveinputpolicy.h"

#include "fxjs/xfa/cjx_node.h"
#include "xfa/fxfa/parser/cxfa_document.h"

namespace {

const CXFA_Node::AttributeData kEffectiveInputPolicyAttributeData[] = {
    {XFA_Attribute::Id, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Use, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Usehref, XFA_AttributeType::CData, nullptr},
};

}  // namespace

CXFA_EffectiveInputPolicy::CXFA_EffectiveInputPolicy(CXFA_Document* doc,
                                                     XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET::kConnectionSet,
                XFA_ObjectType::Node,
                XFA_Element::EffectiveInputPolicy,
                {},
                kEffectiveInputPolicyAttributeData,
                cppgc::MakeGarbageCollected<CJX_Node>(
                    doc->GetHeap()->GetAllocationHandle(),
                    this)) {}

CXFA_EffectiveInputPolicy::~CXFA_EffectiveInputPolicy() = default;
