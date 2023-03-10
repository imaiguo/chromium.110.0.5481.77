// Copyright 2022 The Tint Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "src/tint/type/test_helper.h"
#include "src/tint/type/texture.h"

namespace tint::type {
namespace {

using PointerTest = TestHelper;

TEST_F(PointerTest, Creation) {
    auto* a = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);
    auto* b = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);
    auto* c = create<Pointer>(create<F32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);
    auto* d = create<Pointer>(create<I32>(), ast::AddressSpace::kPrivate, ast::Access::kReadWrite);
    auto* e = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kRead);

    EXPECT_TRUE(a->StoreType()->Is<I32>());
    EXPECT_EQ(a->AddressSpace(), ast::AddressSpace::kStorage);
    EXPECT_EQ(a->Access(), ast::Access::kReadWrite);

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_NE(a, d);
    EXPECT_NE(a, e);
}

TEST_F(PointerTest, Hash) {
    auto* a = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);
    auto* b = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);

    EXPECT_EQ(a->Hash(), b->Hash());
}

TEST_F(PointerTest, Equals) {
    auto* a = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);
    auto* b = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);
    auto* c = create<Pointer>(create<F32>(), ast::AddressSpace::kStorage, ast::Access::kReadWrite);
    auto* d = create<Pointer>(create<I32>(), ast::AddressSpace::kPrivate, ast::Access::kReadWrite);
    auto* e = create<Pointer>(create<I32>(), ast::AddressSpace::kStorage, ast::Access::kRead);

    EXPECT_TRUE(a->Equals(*b));
    EXPECT_FALSE(a->Equals(*c));
    EXPECT_FALSE(a->Equals(*d));
    EXPECT_FALSE(a->Equals(*e));
    EXPECT_FALSE(a->Equals(Void{}));
}

TEST_F(PointerTest, FriendlyName) {
    auto* r = create<Pointer>(create<I32>(), ast::AddressSpace::kNone, ast::Access::kRead);
    EXPECT_EQ(r->FriendlyName(Symbols()), "ptr<i32, read>");
}

TEST_F(PointerTest, FriendlyNameWithAddressSpace) {
    auto* r = create<Pointer>(create<I32>(), ast::AddressSpace::kWorkgroup, ast::Access::kRead);
    EXPECT_EQ(r->FriendlyName(Symbols()), "ptr<workgroup, i32, read>");
}

}  // namespace
}  // namespace tint::type
