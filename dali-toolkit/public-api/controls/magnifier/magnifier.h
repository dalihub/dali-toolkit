#ifndef __DALI_TOOLKIT_MAGNIFIER_H__
#define __DALI_TOOLKIT_MAGNIFIER_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Magnifier;
}

/**
 * Magnifier control is used to apply a magnify effect to content on the stage.
 *
 * This is done by rendering the contents of a SourceActor at a given source position
 * to the stage as a separate overlay. In addition to the contents, an optional frame
 * is displayed around the magnified contents.
 */
class DALI_IMPORT_API Magnifier : public Control
{
public:

  // Custom properties

  static const std::string SOURCE_POSITION_PROPERTY_NAME;                   ///< Property, name "source-position",              type Vector3

public:

  /**
   * Creates an empty Magnifier handle
   */
  Magnifier();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   * @param handle to copy from
   */
  Magnifier( const Magnifier& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  Magnifier& operator=( const Magnifier& handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Magnifier();

  /**
   * Create the Poup control
   * @return A handle to the Magnifier control.
   */
  static Magnifier New();

  /**
   * Downcast an Object handle to Magnifier. If handle points to an Magnifier the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Magnifier or an uninitialized handle
   */
  static Magnifier DownCast( BaseHandle handle );

public:

  /**
   * Set the actors to be rendered in magnifier.
   * @param[in] actor This actor and its children will be rendered.
   */
  void SetSourceActor(Actor actor);

  /**
   * Set the source camera position to render in magnifier
   * @param[in] position The target position from which to render source.
   */
  void SetSourcePosition(Vector3 position);

  /**
   * Returns whether the frame is visible or not.
   * @return true if frame is visible, false if not.
   */
  bool GetFrameVisibility() const;

  /**
   * Sets whether the frame part of the magnifier should be visible
   * or not.
   * @param[in] visible true to display frame, false to hide frame.
   */
  void SetFrameVisibility(bool visible);

  /**
   * Get the magnification factor of the magnifier
   * The larger the value the larger the contents magnified.
   * A value of 1.0f indications 1x magnification.
   * @return Magnification factor is returned
   */
  float GetMagnificationFactor() const;

  /**
   * Set the magnification factor of the magnifier
   * The larger the value the larger the contents magnified.
   * A value of 1.0f indications 1x magnification.
   * @param[in] value Magnification factor.
   */
  void SetMagnificationFactor(float value);

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Magnifier(Internal::Magnifier& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL Magnifier(Dali::Internal::CustomActor* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_MAGNIFIER_H__
