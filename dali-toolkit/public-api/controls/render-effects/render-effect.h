#ifndef DALI_TOOLKIT_RENDER_EFFECT_H
#define DALI_TOOLKIT_RENDER_EFFECT_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/render-tasks/render-task.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{
namespace Toolkit
{
class Control;
namespace Internal DALI_INTERNAL
{
class Control;
class RenderEffectImpl;
} // namespace DALI_INTERNAL

/**
 * @brief
 * RenderEffect is an interface for visual effects.
 * You do not ::New() an interface.
 *
 * Each effect has a single owner Control.
 *
 * Used internal at:
 * Toolkit::Control::SetRenderEffect(Toolkit::RenderEffect effect);
 * Toolkit::Control::ClearRenderEffect();
 *
 * @SINCE_2_3.28
 */
class DALI_TOOLKIT_API RenderEffect : public BaseHandle
{
public:
  /**
   * @brief Creates an uninitialized effect.
   * @SINCE_2_3.28
   */
  RenderEffect() = default;

  /**
   * @brief Copy constructor. Creates another handle that points to the same real object.
   * @SINCE_2_3.28
   */
  RenderEffect(const RenderEffect& handle);

  /**
   * @brief Destructor.
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_2_3.28
   */
  ~RenderEffect();

public: // Not intended for Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_2_3.28
   * @param[in]  renderEffectImpl The UI Control implementation.
   */
  explicit DALI_INTERNAL RenderEffect(Internal::RenderEffectImpl* renderEffectImpl);
  ///@endcond
};

} // namespace Toolkit
} // namespace Dali

#endif //DALI_TOOLKIT_RENDER_EFFECT_H
