#ifndef DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_RENDERER_H
#define DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_RENDERER_H
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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/particle-system/particle-types.h>

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/signals/callback.h>
#include <memory>

namespace Dali
{
class Texture;
}
namespace Dali::Toolkit::ParticleSystem::Internal
{
class ParticleRenderer;
}

namespace Dali::Toolkit::ParticleSystem
{
/**
 * @brief Blending modes for particle renderer
 */
enum class DALI_TOOLKIT_API BlendingMode
{
  ADDITIVE, ///< Additive blending mode (default)
  DEFAULT = ADDITIVE,
  SCREEN ///< SCREEN mode, Advanced blending support required
};

class DALI_TOOLKIT_API ParticleRenderer : public Dali::BaseHandle
{
public:
  ParticleRenderer() = default;

  /**
   * @brief Creates new instance of basic 2D renderer
   *
   * @return Valid instance of renderer
   */
  static ParticleRenderer New();

  /**
   * @brief Sets blending mode for the renderer
   *
   * @param[in] blendingMode Valid particle blending mode to be used
   */
  void SetBlendingMode(BlendingMode blendingMode);

  /**
   * @brief Sets blending mode for the renderer
   *
   * @return Current blending mode
   */
  BlendingMode GetBlendingMode() const;

  /**
   * @brief Sets renderable as a 2D texture (sprites)
   *
   * @param[in] texture Valid texture
   */
  void SetTexture(const Dali::Texture& texture);

  /**
   * @brief Downcasts a handle to ParticleRenderer handle.
   *
   * If handle points to an ParticleRenderer object, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a ParticleRenderer object or an uninitialized handle
   */
  static ParticleRenderer DownCast(BaseHandle handle);

private:
  /// @cond internal
  /**
   * @brief This constructor is used by ParticleRenderer::New() methods.
   *
   * @param [in] impl A pointer to a newly allocated implementation
   */
  ParticleRenderer(Internal::ParticleRenderer* impl);
  /// @endcond
};
} // namespace Dali::Toolkit::ParticleSystem

#endif // DALI_TOOLKIT_PARTICLE_SYSTEM_PARTICLE_RENDERER_H