#ifndef DALI_TOOLKIT_PROGRESS_BAR_H
#define DALI_TOOLKIT_PROGRESS_BAR_H

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
class ProgressBar;
}

/**
 * @brief ProgressBar is a control to give the user an indication of the progress of an operation.
 *
 * Also progress value percentage is shown as text inside the progress bar.
 *
 * Signals
 * | %Signal Name      | Method                        |
 * |-------------------|-------------------------------|
 * | valueChanged      | @ref ValueChangedSignal()     |
 */
class DALI_IMPORT_API ProgressBar : public Control
{
public:

  // Properties

  /**
   * @brief The start and end property ranges for this control.
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1, ///< Start Index
    PROPERTY_END_INDEX =   PROPERTY_START_INDEX + 1000              ///< Reserve property indices
  };

  /**
   * @brief An enumeration of properties belonging to the ProgressBar class.
   */
  struct Property
  {
    enum
    {

      /**
       * @brief The progress value of progress bar, progress runs form 0 to 1.
       * @details Name "progressValue", type Property::FLOAT.
       * @note Optional. If not supplied, the default is 0.
       * @note Value should be between 0 to 1.
       * @note If Value is set to 0, progress bar will be set to beginning.
       * @note If Value is set to 1, progress bar will be set to end.
       * @note Any Value outside of the range is ignored.
       */
      PROGRESS_VALUE = PROPERTY_START_INDEX,

      /**
       * @brief The track Visual value of progress bar, it's a full progress area and it's shown behind PROGRESS_VISUAL.
       * @details Name "trackVisual", type Property::STRING if it is a url, map otherwise.
       * @note Optional. If not supplied, the default track visual will be shown.
       */
      TRACK_VISUAL,

      /**
       * @brief The progress Visual value of progress bar, size of the progress visual is changed based on PROGRESS_VALUE.
       * @details Name "progressVisual", type Property::STRING if it is a url, map otherwise.
       * @note Optional. If not supplied, the default progress visual will be shown.
       */
      PROGRESS_VISUAL,
    };
  };

public:

  /**
   * @brief Creates the ProgressBar control.
   * @return A handle to the ProgressBar control
   */
  static ProgressBar New();

  /**
   * @brief Creates an empty ProgressBar handle.
   */
  ProgressBar();

  /**
   * @brief Copy constructor.
   *
   * Creates another handle that points to the same real object.
   */
  ProgressBar( const ProgressBar& handle );

  /**
   * @brief Assignment operator.
   *
   * Changes this handle to point to another real object.
   */
  ProgressBar& operator=( const ProgressBar& handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ProgressBar();

  /**
   * @brief Downcast an Object handle to ProgressBar.
   *
   * If handle points to a ProgressBar the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ProgressBar or an uninitialized handle
   */
  static ProgressBar DownCast( BaseHandle handle );

public:  // Signals

  /**
   * @brief Value changed signal type.
   */
  typedef Signal< void ( ProgressBar, float ) > ValueChangedSignalType;

  /**
   * @brief Signal emitted when the ProgressBar value changes.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( ProgressBar progressBar, float value );
   * @endcode
   * @return The signal to connect to
   */
  ValueChangedSignalType& ValueChangedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation
   */
  DALI_INTERNAL ProgressBar(Internal::ProgressBar& implementation);

  /**
   * @brief Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor
   */
  explicit DALI_INTERNAL ProgressBar( Dali::Internal::CustomActor* internal );
  /// @endcond
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_PROGRESS_BAR_H
