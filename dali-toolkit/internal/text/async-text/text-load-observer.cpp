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
#include <dali-toolkit/internal/text/async-text/text-load-observer.h>

// INTERNAL INCLUDES

namespace Dali
{
namespace Toolkit
{
TextLoadObserver::TextInformation::TextInformation(Text::AsyncTextRenderInfo renderInfo, Text::AsyncTextParameters parameters)
: renderInfo(renderInfo),
  parameters(parameters)
{
}

TextLoadObserver::TextLoadObserver()
: mDestructionSignalConnect(0)
{
}

TextLoadObserver::~TextLoadObserver()
{
  if(!mDestructionSignal.Empty())
  {
    mDestructionSignal.Emit(this);
  }
}

TextLoadObserver::DestructionSignalType& TextLoadObserver::DestructionSignal()
{
  return mDestructionSignal;
}

void TextLoadObserver::ConnectDestructionSignal()
{
  mDestructionSignalConnect++;
}

bool TextLoadObserver::DisconnectDestructionSignal()
{
  mDestructionSignalConnect--;
  if(mDestructionSignalConnect < 0)
  {
    mDestructionSignalConnect = 0;
  }
  return !mDestructionSignalConnect;
}

} // namespace Toolkit

} // namespace Dali
