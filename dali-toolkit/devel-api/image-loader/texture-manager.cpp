/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali-toolkit/devel-api/image-loader/texture-manager.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace TextureManager
{
std::string AddTexture(Texture& texture)
{
  TextureSet set = TextureSet::New();
  set.SetTexture(0u, texture);
  return AddTexture(set);
}

std::string AddTexture(TextureSet& textureSet)
{
  auto  visualFactory = Toolkit::VisualFactory::Get();
  auto& textureMgr    = GetImplementation(visualFactory).GetTextureManager();
  return textureMgr.AddExternalTexture(textureSet);
}

std::string AddTexture(const Dali::EncodedImageBuffer& encodedImageBuffer)
{
  auto  visualFactory = Toolkit::VisualFactory::Get();
  auto& textureMgr    = GetImplementation(visualFactory).GetTextureManager();
  return textureMgr.AddExternalTexture(encodedImageBuffer);
}

TextureSet RemoveTexture(const std::string& textureUrl)
{
  auto  visualFactory = Toolkit::VisualFactory::Get();
  auto& textureMgr    = GetImplementation(visualFactory).GetTextureManager();
  return textureMgr.RemoveExternalTexture(textureUrl);
}

} // namespace TextureManager

} // namespace Toolkit

} // namespace Dali
