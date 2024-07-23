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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/svg/svg-loader-observer.h>

namespace Dali::Toolkit::Internal
{
SvgLoaderObserver::~SvgLoaderObserver()
{
  if(!mLoadDestructionSignal.Empty())
  {
    mLoadDestructionSignal.Emit(this);
  }
  if(!mRasterizeDestructionSignal.Empty())
  {
    mRasterizeDestructionSignal.Emit(this);
  }
}
} // namespace Dali::Toolkit::Internal
