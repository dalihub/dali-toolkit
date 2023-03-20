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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/skinning-details.h>

// EXTERNAL INCLUDES
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/rendering/shader.h>

namespace Dali::Scene3D::Loader
{
const unsigned int Skinning::MAX_JOINTS = 64;

const char* Skinning::BONE_UNIFORM_NAME = "uBone";
const char* Skinning::JOINT_MATRIX      = "jointMatrix";
} // namespace Dali::Scene3D::Loader
