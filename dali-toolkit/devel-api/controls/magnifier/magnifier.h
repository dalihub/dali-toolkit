#ifndef __DALI_TOOLKIT_MAGNIFIER_H__
#define __DALI_TOOLKIT_MAGNIFIER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000,             ///< Reserve property indices

    ANIMATABLE_PROPERTY_START_INDEX = ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX,
    ANIMATABLE_PROPERTY_END_INDEX =   ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX + 1000              ///< Reserve animatable property indices
  };

  /**
   * @brief An enumeration of properties belonging to the Magnifier class.
   */
  struct Property
  {
    enum
    {
      // Event side properties
      FRAME_VISIBILITY = PROPERTY_START_INDEX,           ///< name "frameVisibility",       Whether a frame is visible or not,         type boolean
      MAGNIFICATION_FACTOR,                              ///< name "magnificationFactor",   Larger value means greater magnification,  type float

      // Animatable properties
      SOURCE_POSITION = ANIMATABLE_PROPERTY_START_INDEX, ///< name "sourcePosition",  The position of the source,  type Vector3
    };
  };

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
