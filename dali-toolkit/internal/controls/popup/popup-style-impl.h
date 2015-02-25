#ifndef __DALI_TOOLKIT_INTERNAL_POPUP_STYLE_H__
#define __DALI_TOOLKIT_INTERNAL_POPUP_STYLE_H__

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
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/ref-object.h>

#include <string>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class PopupStyle;

typedef IntrusivePtr<PopupStyle> PopupStylePtr;

/**
 * A PopupStyle describes the images, positions, sizes, and various other metrics
 * which define how the popup should look.
 */
class PopupStyle : public RefObject
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~PopupStyle();

public:

  Vector4 backingColor;                               ///< Color of backing layer (covers entire screen)
  std::string backgroundImage;                        ///< Background image path
  std::string buttonAreaImage;                        ///< This image is for the back ground area common for all the buttons in the popup
  Vector2 backgroundSize;                             ///< Background image size.
  Vector4 backgroundScale9Border;                     ///< Background scale-9 border settings.
  Vector4 backgroundOuterBorder;                      ///< Background outer border settings.
  Vector4 buttonArea9PatchBorder;                     ///< 9 patch border constants for buttonAreaImage
  float margin;                                       ///< Margin for all contents (body, title, button)
  float buttonSpacing;                                ///< Horizontal spacing for buttons.
  Vector3 buttonSize;                                 ///< Size of Buttons.
  Vector3 bottomSize;                                 ///< size of bottom button bar.
  std::string bottomBackgroundImage;                  ///< bottom background image path.
  std::string tailUpImage;                            ///< Tail Up-side image path.
  std::string tailDownImage;                          ///< Tail Down-side image path.
  std::string tailLeftImage;                          ///< Tail Left-side image path.
  std::string tailRightImage;                         ///< Tail Right-side image path.

protected:

  /**
   * Create a new PopupStyle; Only derived versions are instantiable.
   */
  PopupStyle();
};

class PopupStyleDefault;

typedef IntrusivePtr<PopupStyleDefault> PopupStyleDefaultPtr;

/**
 * This is the default popup style.
 */
class PopupStyleDefault : public PopupStyle
{
public:

  /**
   * Create a new PopupStyle
   */
  static PopupStyleDefaultPtr New();

  /**
   * Virtual destructor.
   */
  virtual ~PopupStyleDefault();

private:

protected:

  /**
   * Protected constructor; see also PopupStyleDefault::New()
   */
  PopupStyleDefault();
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_POPUP_STYLE_H__
