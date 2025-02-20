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
#include <dali-toolkit/internal/text/async-text/async-text-loader.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-loader-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
AsyncTextLoader::AsyncTextLoader()
{
}

AsyncTextLoader::~AsyncTextLoader()
{
}

AsyncTextLoader::AsyncTextLoader(Internal::AsyncTextLoader* implementation)
: BaseHandle(implementation)
{
}

void AsyncTextLoader::ClearModule()
{
  GetImplementation(*this).ClearModule();
}

void AsyncTextLoader::SetCustomFontDirectories(const TextAbstraction::FontPathList& customFontDirectories)
{
  GetImplementation(*this).SetCustomFontDirectories(customFontDirectories);
}

void AsyncTextLoader::SetModuleClearNeeded(bool clear)
{
  GetImplementation(*this).SetModuleClearNeeded(clear);
}

bool AsyncTextLoader::IsModuleClearNeeded()
{
  return GetImplementation(*this).IsModuleClearNeeded();
}

AsyncTextLoader AsyncTextLoader::New()
{
  auto asyncTextLoaderImpl = new Internal::AsyncTextLoader();

  return AsyncTextLoader(asyncTextLoaderImpl);
}

AsyncTextRenderInfo AsyncTextLoader::RenderText(AsyncTextParameters& parameters)
{
  return GetImplementation(*this).RenderText(parameters);
}

AsyncTextRenderInfo AsyncTextLoader::RenderTextFit(AsyncTextParameters& parameters)
{
  return GetImplementation(*this).RenderTextFit(parameters);
}

AsyncTextRenderInfo AsyncTextLoader::RenderAutoScroll(AsyncTextParameters& parameters)
{
  return GetImplementation(*this).RenderAutoScroll(parameters);
}

AsyncTextRenderInfo AsyncTextLoader::GetNaturalSize(AsyncTextParameters& parameters)
{
  return GetImplementation(*this).GetNaturalSize(parameters);
}

AsyncTextRenderInfo AsyncTextLoader::GetHeightForWidth(AsyncTextParameters& parameters)
{
  return GetImplementation(*this).GetHeightForWidth(parameters);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
