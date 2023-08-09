#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_RENDERER_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_RENDERER_H
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

#include <memory>

#include <dali-toolkit/public-api/particle-system/particle-emitter.h>
#include <dali-toolkit/public-api/particle-system/particle-renderer.h>
#include <dali-toolkit/public-api/particle-system/particle.h>

#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/rendering/renderer.h>
#include <dali/public-api/rendering/vertex-buffer.h>
#include <dali/public-api/signals/render-callback.h>

namespace Dali::Graphics
{
class Controller;
class Shader;
} // namespace Dali::Graphics

namespace Dali::Toolkit::ParticleSystem
{
class ParticleEmitter;
}

namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleEmitter;
class ParticleRenderer;
class ParticleList;

class ParticleRenderer : public Dali::BaseObject
{
public:
  ParticleRenderer();

  /**
   * Create shader
   */
  void CreateShader();

  void SetTexture(const Dali::Texture& texture);

  void SetBlendingMode(BlendingMode blendingMode);

  BlendingMode GetBlendingMode() const;

  bool Initialize();

  void PrepareToDie();

  void SetEmitter(ParticleSystem::Internal::ParticleEmitter* emitter);

  void UpdateParticlesTask(Internal::ParticleList& list,
                           uint32_t                particleStartIndex,
                           uint32_t                particleCount,
                           uint8_t*                dst);

  [[nodiscard]] Renderer GetRenderer() const;

  uint32_t OnStreamBufferUpdate(void* data, size_t size);

  bool mUsingStreamDivisor{true}; ///< If attribute divisor is supported, it's going to be used

  Internal::ParticleEmitter* mEmitter{nullptr}; ///< Emitter implementation that uses the renderer

  Dali::Texture      mTexture;
  Dali::TextureSet   mTextureSet;
  Dali::Renderer     mRenderer;
  Dali::Shader       mShader;
  Dali::Geometry     mGeometry;
  Dali::VertexBuffer mVertexBuffer;
  Dali::VertexBuffer mStreamBuffer;
  BlendingMode       mBlendingMode{BlendingMode::DEFAULT};

  std::unique_ptr<Dali::VertexBufferUpdateCallback> mStreamBufferUpdateCallback;

  bool mInitialized{false};
};
} // namespace Dali::Toolkit::ParticleSystem::Internal
namespace Dali
{
inline Toolkit::ParticleSystem::Internal::ParticleRenderer& GetImplementation(Toolkit::ParticleSystem::ParticleRenderer& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleRenderer handle is empty");

  BaseObject& handle = source.GetBaseObject();

  return static_cast<Toolkit::ParticleSystem::Internal::ParticleRenderer&>(handle);
}

inline const Toolkit::ParticleSystem::Internal::ParticleRenderer& GetImplementation(const Toolkit::ParticleSystem::ParticleRenderer& source)
{
  DALI_ASSERT_ALWAYS(source && "ParticleRenderer handle is empty");

  const BaseObject& handle = source.GetBaseObject();

  return static_cast<const Toolkit::ParticleSystem::Internal::ParticleRenderer&>(handle);
}

} // namespace Dali

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_INTERNAL_PARTICLE_RENDERER_H