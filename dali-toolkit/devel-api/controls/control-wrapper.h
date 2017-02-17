#ifndef DALI_TOOLKIT_CONTROL_WRAPPER_H
#define DALI_TOOLKIT_CONTROL_WRAPPER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ControlWrapper;
}

/**
 * @brief ControlWrapper is a base class for custom UI controls developed in managed code (i.e. C#.NET).
 *
 * The implementation of the ControlWrapper must be supplied; see Internal::ControlWrapper for more details.
 */
class DALI_IMPORT_API ControlWrapper : public Control
{

public:

  /**
   * @brief Create a new instance of a ControlWrapper.
   *
   * @param[in] typeName The name of the type that is registered with this control
   * @param[in] implementation The implementation of this control
   *
   * @return A handle to a new ControlWrapper.
   */
  static ControlWrapper New( const std::string& typeName, Internal::ControlWrapper& implementation );

  /**
   * @brief Creates an empty ControlWrapper handle.
   */
  ControlWrapper();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ControlWrapper();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object
   * @param[in] handle Handle to the copied object
   */
  ControlWrapper( const ControlWrapper& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object
   * @param[in] handle Handle to the object
   * @return A reference to this
   */
  ControlWrapper& operator=( const ControlWrapper& handle );

  /**
   * @brief Downcast an Object handle to ControlWrapper.
   *
   * If handle points to a ControlWrapper the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ControlWrapper or an uninitialized handle
   */
  static ControlWrapper DownCast( BaseHandle handle );

public: // Not intended for application developers

  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @param[in]  implementation  The Control implementation.
   */
  ControlWrapper( Internal::ControlWrapper& implementation );

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   *
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit ControlWrapper( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_CONTROL_WRAPPER_H
