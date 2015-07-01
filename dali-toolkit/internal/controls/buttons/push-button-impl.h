#ifndef __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__
#define __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/buttons/push-button.h>
#include "button-impl.h"

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * PushButton implementation class.
 *
 * \sa Dali::Toolkit::PushButton
 */
class PushButton : public Button
{
public:

  /**
   * Create a new PushButton.
   * @return A smart-pointer to the newly allocated PushButton.
   */
  static Dali::Toolkit::PushButton New();

protected:

  /**
   * Construct a new PushButton.
   */
  PushButton();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PushButton();

private: // From Button

  /**
   * @copydoc Toolkit::Internal::Button::OnButtonInitialize()
   */
  virtual void OnButtonInitialize();

  /**
   * @copydoc Toolkit::Internal::Button::OnLabelSet()
   */
  virtual void OnLabelSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnButtonImageSet()
   */
  virtual void OnButtonImageSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnSelectedImageSet()
   */
  virtual void OnSelectedImageSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnBackgroundImage()
   */
  virtual void OnBackgroundImageSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnSelectedBackgroundImageSet()
   */
  virtual void OnSelectedBackgroundImageSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnDisabledImageSet()
   */
  virtual void OnDisabledImageSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnDisabledSelectedImageSet()
   */
  virtual void OnDisabledSelectedImageSet();

  /**
   * @copydoc Toolkit::Internal::Button::OnDisabledBackgroundImageSet()
   */
  virtual void OnDisabledBackgroundImageSet();

  /**
   * @copydoc Toolkit::Internal::Button::PrepareForTranstionIn( Actor actor )
   */
  virtual void PrepareForTranstionIn( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::PrepareForTranstionOut( Actor actor )
   */
  virtual void PrepareForTranstionOut( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::OnTransitionIn( Actor actor )
   */
  virtual void OnTransitionIn( Actor actor );

  /**
   * @copydoc Toolkit::Internal::Button::OnTransitionOut( Actor actor )
   */
  virtual void OnTransitionOut( Actor actor );

private: // From Control

  /**
   * @copydoc CustomActorImpl::OnSizeSet( const Vector3& targetSize )
   */
  virtual void OnSizeSet( const Vector3& targetSize );

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Control::OnSetResizePolicy
   */
  virtual void OnSetResizePolicy( ResizePolicy::Type policy, Dimension::Type dimension );

private:

  /**
   * It adds the actor to the root actor and to the fade in animation.
   * @param[inout] actor The actor.
   * @param[in] opacity The opacity to fade to
   */
  void FadeImageTo( Actor actor, float opacity );

  /**
   * @brief Custom configuration for size negotiation
   */
  void ConfigureSizeNegotiation();

  /**
   * @brief Configure size negotiation for a given dimension
   *
   * @param[in] dimension The dimension to configure
   * @param[in] images The list of images to configure
   * @param[in] label The text label to configure
   */
  void ConfigureSizeNegotiationDimension( Dimension::Type dimension, const std::vector< Actor >& images, Actor& label );

private:

  // Undefined
  PushButton( const PushButton& );

  // Undefined
  PushButton& operator=( const PushButton& );

private:

  Vector3               mSize;                      ///< The button's size.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Toolkit::Internal::PushButton& GetImplementation( Toolkit::PushButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  Dali::RefObject& handle = button.GetImplementation();

  return static_cast<Toolkit::Internal::PushButton&>( handle );
}

inline const Toolkit::Internal::PushButton& GetImplementation( const Toolkit::PushButton& button )
{
  DALI_ASSERT_ALWAYS( button );

  const Dali::RefObject& handle = button.GetImplementation();

  return static_cast<const Toolkit::Internal::PushButton&>( handle );
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_PUSH_BUTTON_H__
