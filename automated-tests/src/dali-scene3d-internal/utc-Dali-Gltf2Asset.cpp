/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include <dali-test-suite-utils.h>
#include <string>
#include "dali-scene3d/internal/loader/gltf2-asset.h"

using namespace Dali;
using namespace Dali::Scene3D::Loader;

int UtcDaliGltf2AssetComponentIsUnsigned(void)
{
  DALI_TEST_EQUAL(gltf2::Component::IsUnsigned(gltf2::Component::BYTE), false);
  DALI_TEST_EQUAL(gltf2::Component::IsUnsigned(gltf2::Component::UNSIGNED_BYTE), true);
  DALI_TEST_EQUAL(gltf2::Component::IsUnsigned(gltf2::Component::SHORT), false);
  DALI_TEST_EQUAL(gltf2::Component::IsUnsigned(gltf2::Component::UNSIGNED_SHORT), true);
  DALI_TEST_EQUAL(gltf2::Component::IsUnsigned(gltf2::Component::UNSIGNED_INT), true);
  DALI_TEST_EQUAL(gltf2::Component::IsUnsigned(gltf2::Component::FLOAT), false);
  DALI_TEST_EQUAL(gltf2::Component::IsUnsigned(gltf2::Component::INVALID), false);

  END_TEST;
}

int UtcDaliGltf2AssetComponentSize(void)
{
  DALI_TEST_EQUAL(gltf2::Component::Size(gltf2::Component::BYTE), 1u);
  DALI_TEST_EQUAL(gltf2::Component::Size(gltf2::Component::UNSIGNED_BYTE), 1u);
  DALI_TEST_EQUAL(gltf2::Component::Size(gltf2::Component::SHORT), 2u);
  DALI_TEST_EQUAL(gltf2::Component::Size(gltf2::Component::UNSIGNED_SHORT), 2u);
  DALI_TEST_EQUAL(gltf2::Component::Size(gltf2::Component::UNSIGNED_INT), 4u);
  DALI_TEST_EQUAL(gltf2::Component::Size(gltf2::Component::FLOAT), 4u);
  DALI_TEST_EQUAL(gltf2::Component::Size(gltf2::Component::INVALID), -1);

  END_TEST;
}

#define FROM_STRING_HELPER(x) FromString(#x, strlen(#x))
#define TARGET_FROM_STRING_HELPER(x) TargetFromString(#x, strlen(#x))
#define HASH_FROM_STRING_HELPER(x) HashFromString(#x, strlen(#x))

#define STRING_CHECK(type, x) DALI_TEST_EQUAL(gltf2::type::FROM_STRING_HELPER(x), gltf2::type::x)
#define TARGET_STRING_CHECK(type, x) DALI_TEST_EQUAL(gltf2::type::TARGET_FROM_STRING_HELPER(x), gltf2::type::x)
#define HASH_STRING_CHECK(type, x, y, z) DALI_TEST_EQUAL(gltf2::type::HASH_FROM_STRING_HELPER(x), gltf2::type::ToHash(gltf2::type::y, true, z))

int UtcDaliGltf2AssetAccessorType(void)
{
  STRING_CHECK(AccessorType, SCALAR);
  STRING_CHECK(AccessorType, VEC2);
  STRING_CHECK(AccessorType, VEC3);
  STRING_CHECK(AccessorType, VEC4);
  STRING_CHECK(AccessorType, MAT2);
  STRING_CHECK(AccessorType, MAT3);
  STRING_CHECK(AccessorType, MAT4);
  DALI_TEST_EQUAL(gltf2::AccessorType::FROM_STRING_HELPER(VEC88), gltf2::AccessorType::INVALID);

  END_TEST;
}

int UtcDaliGltf2AssetAlphaMode(void)
{
  STRING_CHECK(AlphaMode, OPAQUE);
  STRING_CHECK(AlphaMode, MASK);
  STRING_CHECK(AlphaMode, BLEND);
  DALI_TEST_EQUAL(gltf2::AlphaMode::FROM_STRING_HELPER(ALPHA_SCHMALPHA), gltf2::AlphaMode::INVALID);

  END_TEST;
}

int UtcDaliGltf2AssetAttribute(void)
{
  TARGET_STRING_CHECK(Attribute, POSITION);
  TARGET_STRING_CHECK(Attribute, NORMAL);
  TARGET_STRING_CHECK(Attribute, TANGENT);
  HASH_STRING_CHECK(Attribute, TEXCOORD_0, TEXCOORD_N, 0);
  HASH_STRING_CHECK(Attribute, TEXCOORD_1, TEXCOORD_N, 1);
  HASH_STRING_CHECK(Attribute, COLOR_0, COLOR_N, 0);
  HASH_STRING_CHECK(Attribute, COLOR_1, COLOR_N, 1);
  HASH_STRING_CHECK(Attribute, JOINTS_0, JOINTS_N, 0);
  HASH_STRING_CHECK(Attribute, JOINTS_1, JOINTS_N, 1);
  HASH_STRING_CHECK(Attribute, JOINTS_2, JOINTS_N, 2);
  HASH_STRING_CHECK(Attribute, WEIGHTS_0, WEIGHTS_N, 0);
  HASH_STRING_CHECK(Attribute, WEIGHTS_1, WEIGHTS_N, 1);
  HASH_STRING_CHECK(Attribute, WEIGHTS_2, WEIGHTS_N, 2);
  DALI_TEST_EQUAL(gltf2::Attribute::TARGET_FROM_STRING_HELPER(VISCOSITY), gltf2::Attribute::INVALID);

  END_TEST;
}

int UtcDaliGltf2AssetAnimationSamplerInterpolation(void)
{
  STRING_CHECK(Animation::Sampler::Interpolation, STEP);
  STRING_CHECK(Animation::Sampler::Interpolation, LINEAR);
  STRING_CHECK(Animation::Sampler::Interpolation, CUBICSPLINE);
  DALI_TEST_EQUAL(gltf2::Animation::Sampler::Interpolation::FROM_STRING_HELPER(EASE_IN_OUT), gltf2::Animation::Sampler::Interpolation::INVALID);

  END_TEST;
}

int UtcDaliGltf2AssetAnimationChannelTarget(void)
{
  STRING_CHECK(Animation::Channel::Target, TRANSLATION);
  STRING_CHECK(Animation::Channel::Target, ROTATION);
  STRING_CHECK(Animation::Channel::Target, SCALE);
  STRING_CHECK(Animation::Channel::Target, WEIGHTS);
  DALI_TEST_EQUAL(gltf2::Animation::Channel::Target::FROM_STRING_HELPER(FLUFFINESS), gltf2::Animation::Channel::Target::INVALID);

  END_TEST;
}

int UtcDaliGltf2AssetAccessorSparse(void)
{
  gltf2::Accessor acc;
  DALI_TEST_CHECK(!acc.mSparse);

  std::vector<gltf2::BufferView> bufferViews;

  gltf2::Accessor::Sparse sparse{256u};
  sparse.mIndices.mBufferView    = gltf2::Ref<gltf2::BufferView>(bufferViews, 5u);
  sparse.mIndices.mComponentType = gltf2::Component::FLOAT;
  sparse.mValues.mBufferView     = gltf2::Ref<gltf2::BufferView>(bufferViews, 284u);
  sparse.mValues.mByteOffset     = 16532;
  acc.SetSparse(sparse);

  DALI_TEST_EQUAL(acc.mSparse->mCount, sparse.mCount);
  DALI_TEST_EQUAL(acc.mSparse->mIndices.mBufferView, sparse.mIndices.mBufferView);
  DALI_TEST_EQUAL(acc.mSparse->mIndices.mByteOffset, sparse.mIndices.mByteOffset);
  DALI_TEST_EQUAL(acc.mSparse->mIndices.mComponentType, sparse.mIndices.mComponentType);
  DALI_TEST_EQUAL(acc.mSparse->mValues.mBufferView, sparse.mValues.mBufferView);
  DALI_TEST_EQUAL(acc.mSparse->mValues.mByteOffset, sparse.mValues.mByteOffset);

  END_TEST;
}
