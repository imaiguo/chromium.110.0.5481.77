// Copyright 2017 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/parser/cxfa_pcl.h"

#include "fxjs/xfa/cjx_node.h"
#include "xfa/fxfa/parser/cxfa_document.h"

namespace {

const CXFA_Node::PropertyData kPclPropertyData[] = {
    {XFA_Element::FontInfo, 1, {}},   {XFA_Element::Jog, 1, {}},
    {XFA_Element::Xdc, 1, {}},        {XFA_Element::BatchOutput, 1, {}},
    {XFA_Element::PageOffset, 1, {}}, {XFA_Element::OutputBin, 1, {}},
    {XFA_Element::Staple, 1, {}},     {XFA_Element::MediumInfo, 1, {}},
};

const CXFA_Node::AttributeData kPclAttributeData[] = {
    {XFA_Attribute::Name, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Desc, XFA_AttributeType::CData, nullptr},
    {XFA_Attribute::Lock, XFA_AttributeType::Integer, (void*)0},
};

}  // namespace

CXFA_Pcl::CXFA_Pcl(CXFA_Document* doc, XFA_PacketType packet)
    : CXFA_Node(doc,
                packet,
                XFA_XDPPACKET::kConfig,
                XFA_ObjectType::Node,
                XFA_Element::Pcl,
                kPclPropertyData,
                kPclAttributeData,
                cppgc::MakeGarbageCollected<CJX_Node>(
                    doc->GetHeap()->GetAllocationHandle(),
                    this)) {}

CXFA_Pcl::~CXFA_Pcl() = default;
