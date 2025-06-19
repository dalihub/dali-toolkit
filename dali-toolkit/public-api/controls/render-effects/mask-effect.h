#ifndef DALI_TOOLKIT_MASK_EFFECT_H
#define DALI_TOOLKIT_MASK_EFFECT_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/render-effects/render-effect.h>
namespace Dali
{
namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class MaskEffectImpl;
} // namespace Internal DALI_INTERNAL

/**
 * @brief MaskEffect is a visual effect that masks owner control.
 * This class is a concrete class from RenderEffect interface.
 * Add this effect to a control, clear manually to deactivate.
 *
 * Toolkit::Control control = Toolkit::Control::New();
 * parent.Add(control);
 * control.SetRenderEffect(MaskEffect::New(maskControl)); // Activate
 * ...
 * control.ClearRenderEffect(); // Deactivate
 *
 * @SINCE_2_4.15
 */
class DALI_TOOLKIT_API MaskEffect : public RenderEffect
{
public:

  /**
   * @brief Enumeration for selecting how the mask source interprets pixel data.
   * @SINCE_2_4.15
   */
  enum MaskMode
  {
    ALPHA,     ///< Uses the alpha channel of the mask texture. (Default) @SINCE_2_4.15
    LUMINANCE, ///< Converts RGB to grayscale and uses the luminance as mask value. @SINCE_2_4.15
  };

  /**
   * @brief Creates an initialized MaskEffect with control, using default settings. The default settings are:
   *
   * maskMode = MaskMode::ALPHA
   * maskPosition = Vector2(0.f, 0.f)
   * maskScale = Vector2(1.f, 1.f)
   *
   * @param[in] maskControl The source Control to affect mask.
   *
   * @SINCE_2_4.15
   * @return A handle to a newly allocated Dali resource
   */
  static MaskEffect New(Toolkit::Control maskControl);

  /**
   * @brief Creates an initialized MaskEffect.
   *
   * @param[in] maskControl The source Control to affect mask.
   * @param[in] maskMode Defines pixel data type (alpha, luminance) used as the mask source.
   * @param[in] maskPosition The Position of mask source.
   * @param[in] maskScale The Scale of mask source.
   *
   * @SINCE_2_4.15
   * @return A handle to a newly allocated Dali resource
   */
  static MaskEffect New(Toolkit::Control maskControl, MaskMode maskMode, Vector2 maskPosition, Vector2 maskScale);

  /**
   * @brief Creates an uninitialized mask effect.
   * @SINCE_2_4.15
   */
  MaskEffect();

  /**
   * @brief Copy constructor.
   * @SINCE_2_4.15
   */
  MaskEffect(const MaskEffect& handle);

  /**
   * @brief Destructor
   * @SINCE_2_4.15
   */
  ~MaskEffect();

  /**
   * @brief Set whether the target should be rendered once(true) or every frame(false).
   * @param[in] targetMaskOnce If true, renders target once, else updates every frame.
   * @SINCE_2_4.25
   */
  void SetTargetMaskOnce(bool targetMaskOnce);

  /**
   * @brief Retrives whether the target should be rendered once(true) or every frame(false).
   * @SINCE_2_4.25
   */
  bool GetTargetMaskOnce() const;

  /**
   * @brief Set whether the source should be rendered once(true) or every frame(false).
   * @param[in] sourceMaskOnce If true, renders source once, else updates every frame.
   * @SINCE_2_4.25
   */
  void SetSourceMaskOnce(bool sourceMaskOnce);

  /**
   * @brief Retrives whether the source should be rendered once(true) or every frame(false).
   * @SINCE_2_4.25
   */
  bool GetSourceMaskOnce() const;

public: // Not intended for use by Application developers
  ///@cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @SINCE_2_4.15
   * @param[in] maskEffectImpl The mask effect internal implementation.
   */
  explicit DALI_INTERNAL MaskEffect(Internal::MaskEffectImpl* maskEffectImpl);
  ///@endcond
};
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_MASK_EFFECT_H
