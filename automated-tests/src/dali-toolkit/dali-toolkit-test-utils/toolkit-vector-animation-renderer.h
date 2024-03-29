#ifndef DALI_TOOLKIT_TEST_VECTOR_ANIMATION_RENDERER_H
#define DALI_TOOLKIT_TEST_VECTOR_ANIMATION_RENDERER_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

namespace Test
{
namespace VectorAnimationRenderer
{
#define VECTOR_ANIMATION_TOTAL_FRAME_NUMBER 5
#define VECTOR_ANIMATION_MARKER_NAME_1 "marker1"
#define VECTOR_ANIMATION_MARKER_NAME_2 "marker2"
#define VECTOR_ANIMATION_MARKER_START_FRAME_1 0
#define VECTOR_ANIMATION_MARKER_END_FRAME_1 2
#define VECTOR_ANIMATION_MARKER_START_FRAME_2 2
#define VECTOR_ANIMATION_MARKER_END_FRAME_2 3

void     DelayRendering(uint32_t delay);
uint32_t GetDroppedFrames();
void     UseNativeImageTexture(bool useNativeImage);

} // namespace VectorAnimationRenderer
} // namespace Test

#endif // DALI_TOOLKIT_TEST_VECTOR_ANIMATION_RENDERER_H
