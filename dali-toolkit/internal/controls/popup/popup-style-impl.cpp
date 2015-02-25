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
#include <dali-toolkit/internal/controls/popup/popup-style-impl.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace // unnamed namespace
{
// Popup style default
const Vector4 DEFAULT_BACKING_COLOR = Vector4(0.0f, 0.0f, 0.0f, 0.5f);
const float DEFAULT_MARGIN = 20.0f;          //From Tizen GUI UX
const float DEFAULT_BUTTON_SPACING = 20.0f;  //From Tizen GUI UX
const Vector3 DEFAULT_BUTTON_SIZE(275.0f, 74.0f, 0.0f);
const char* DEFAULT_BACKGROUND_IMAGE_PATH = DALI_IMAGE_DIR "00_popup_bg.png";
const char* DEFAULT_BUTTON_AREA_IMAGE_PATH = DALI_IMAGE_DIR "00_popup_button_bg.png";
const char* DEFAULT_TAIL_UP_IMAGE_PATH = DALI_IMAGE_DIR "popup_tail_up.png";
const char* DEFAULT_TAIL_DOWN_IMAGE_PATH = DALI_IMAGE_DIR "popup_tail_down.png";
const char* DEFAULT_TAIL_LEFT_IMAGE_PATH = DALI_IMAGE_DIR "popup_tail_left.png";
const char* DEFAULT_TAIL_RIGHT_IMAGE_PATH = DALI_IMAGE_DIR "popup_tail_right.png";
const Vector3 DEFAULT_BOTTOM_SIZE(620.0f,96.0f,0.0f);
const Vector2 DEFAULT_BACKGROUND_SIZE(620.0f, 236.0f);
const Vector4 DEFAULT_BACKGROUND_STYLE_9_BORDER( 25.0f, 25.0f, 26.0f, 50.0f );
const Vector4 DEFAULT_BACKGROUND_OUTER_BORDER( 0.0f, 0.0f, 0.0f, 0.0f );
const Vector4 DEFAULT_BUTTON_AREA_9_PATCH_BORDER( 13.0f, 8.0f, 13.0f, 8.0f );
}

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

///////////////////////////////////////////////////////////////////////////////
// Popup Style (base class)
///////////////////////////////////////////////////////////////////////////////

PopupStyle::PopupStyle()
: backingColor(),
  backgroundImage(),
  buttonAreaImage(),
  backgroundSize(),
  backgroundScale9Border(),
  backgroundOuterBorder(),
  buttonArea9PatchBorder(),
  margin(0.0f),
  buttonSpacing(0.0f),
  buttonSize(),
  tailUpImage(),
  tailDownImage(),
  tailLeftImage(),
  tailRightImage()
{
}

PopupStyle::~PopupStyle()
{
}

///////////////////////////////////////////////////////////////////////////////
// Popup style: Default
///////////////////////////////////////////////////////////////////////////////

PopupStyleDefault::PopupStyleDefault()
{
  backingColor = DEFAULT_BACKING_COLOR;
  backgroundImage = DEFAULT_BACKGROUND_IMAGE_PATH;
  buttonAreaImage = DEFAULT_BUTTON_AREA_IMAGE_PATH;
  margin = DEFAULT_MARGIN;
  buttonSpacing = DEFAULT_BUTTON_SPACING;
  buttonSize = DEFAULT_BUTTON_SIZE;
  tailUpImage = DEFAULT_TAIL_UP_IMAGE_PATH;
  tailDownImage = DEFAULT_TAIL_DOWN_IMAGE_PATH;
  tailLeftImage = DEFAULT_TAIL_LEFT_IMAGE_PATH;
  tailRightImage = DEFAULT_TAIL_RIGHT_IMAGE_PATH;
  backgroundSize = DEFAULT_BACKGROUND_SIZE;
  backgroundScale9Border = DEFAULT_BACKGROUND_STYLE_9_BORDER;
  backgroundOuterBorder = DEFAULT_BACKGROUND_OUTER_BORDER;
  buttonArea9PatchBorder = DEFAULT_BUTTON_AREA_9_PATCH_BORDER;
  bottomSize = DEFAULT_BOTTOM_SIZE;
}

PopupStyleDefaultPtr PopupStyleDefault::New()
{
  return PopupStyleDefaultPtr(new PopupStyleDefault());
}

PopupStyleDefault::~PopupStyleDefault()
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
