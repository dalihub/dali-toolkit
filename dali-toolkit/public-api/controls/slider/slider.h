#ifndef __DALI_TOOLKIT_SLIDER_H__
#define __DALI_TOOLKIT_SLIDER_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Slider;
}

/**
 * Slider is a control to enable sliding an indicator between two values
 */
class Slider : public Control
{
public:

  //Signal Names
  static const char* const SIGNAL_VALUE_CHANGED;
  static const char* const SIGNAL_MARK;

  // Properties
  static const std::string LOWER_BOUND_PROPERTY_NAME;           ///< Property, name "lower-bound",       type FLOAT
  static const std::string UPPER_BOUND_PROPERTY_NAME;           ///< Property, name "upper-bound",       type FLOAT
  static const std::string VALUE_PROPERTY_NAME;                 ///< Property, name "value",             type FLOAT

  static const std::string HIT_REGION_PROPERTY_NAME;            ///< Property, name "hit-region",        type VECTOR2
  static const std::string BACKING_REGION_PROPERTY_NAME;        ///< Property, name "backing-region",    type VECTOR2
  static const std::string HANDLE_REGION_PROPERTY_NAME;         ///< Property, name "handle-region",     type VECTOR2

  static const std::string BACKING_IMAGE_NAME_PROPERTY_NAME;    ///< Property, name "backing-image-name",  type std::string
  static const std::string HANDLE_IMAGE_NAME_PROPERTY_NAME;     ///< Property, name "handle-image-name",   type std::string
  static const std::string PROGRESS_IMAGE_NAME_PROPERTY_NAME;   ///< Property, name "progress-image-name", type std::string
  static const std::string POPUP_IMAGE_NAME_PROPERTY_NAME;      ///< Property, name "popup-image-name",    type std::string
  static const std::string POPUP_ARROW_IMAGE_NAME_PROPERTY_NAME; ///< Property, name "popup-arrow-image-name", type std::string

  static const std::string BACKING_SCALE9_BORDER_PROPERTY_NAME;  ///< Property, name "backing-scale9-border",  type VECTOR4
  static const std::string PROGRESS_SCALE9_BORDER_PROPERTY_NAME; ///< Property, name "progress-scale9-border", type VECTOR4
  static const std::string POPUP_SCALE9_BORDER_PROPERTY_NAME;    ///< Property, name "popup-scale9-border",    type VECTOR4

  static const std::string DISABLE_COLOR_PROPERTY_NAME;          ///< Property, name "disable-color",    type VECTOR4
  static const std::string POPUP_TEXT_COLOR_PROPERTY_NAME;       ///< Property, name "popup-text-color", type VECTOR4

  static const std::string VALUE_PRECISION_PROPERTY_NAME;        ///< Property, name "value-precision",  type INT

  static const std::string SHOW_POPUP_PROPERTY_NAME;             ///< Property, name "show-popup",       type BOOLEAN
  static const std::string SHOW_VALUE_PROPERTY_NAME;             ///< Property, name "show-value",       type BOOLEAN

  static const std::string ENABLED_PROPERTY_NAME;                ///< Property, name "enabled",          type BOOLEAN

  static const std::string MARKS_PROPERTY_NAME;                  ///< Property, name "marks",            type Property::Array<float>
  static const std::string SNAP_TO_MARKS_PROPERTY_NAME;          ///< Property, name "snap-to-marks",    type BOOLEAN
  static const std::string MARK_TOLERANCE_PROPERTY_NAME;         ///< Property, name "mark-tolerance",   type FLOAT

public:

  /**
   * Create the Slider control
   * @return A handle to the Slider control.
   */
  static Slider New();

  /**
   * Creates an empty Slider handle
   */
  Slider();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   */
  Slider( const Slider& handle );

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  Slider& operator=( const Slider& handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes typically do not contain member data.
   */
  virtual ~Slider();

  /**
   * Downcast an Object handle to Slider. If handle points to a Slider the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Slider or an uninitialized handle
   */
  static Slider DownCast( BaseHandle handle );

public:

  // Signals

  // Value changed
  typedef SignalV2< bool ( Slider, float ) > ValueChangedSignalType;
  typedef SignalV2< bool ( Slider, int ) > MarkSignalType;

  /**
   * Signal emitted when the slider value changes
   */
  ValueChangedSignalType& ValueChangedSignal();

  /**
   * Signal emitted when the slider handle reaches a mark
   */
  MarkSignalType& MarkSignal();

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  Slider(Internal::Slider& implementation);

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  Slider( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SLIDER_H__
