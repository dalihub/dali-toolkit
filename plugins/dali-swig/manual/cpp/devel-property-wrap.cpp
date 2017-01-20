/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#ifndef CSHARP_DEVEL_PROPERTY
#define CSHARP_DEVEL_PROPERTY
#endif

#include "common.h"

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/controls/scrollable/item-view/item-view-devel.h>
#include <dali-toolkit/devel-api/controls/buttons/button-devel.h>

#ifdef __cplusplus
extern "C" {
#endif

SWIGEXPORT int SWIGSTDCALL CSharp_Actor_Property_BATCH_PARENT_get() {
  int jresult ;
  int result;

  result = (int)Dali::DevelActor::Property::BATCH_PARENT;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Actor_Property_SIBLING_ORDER_get() {
  int jresult ;
  int result;

  result = (int)Dali::DevelActor::Property::SIBLING_ORDER;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Renderer_Property_BATCHING_ENABLED_get() {
  int jresult ;
  int result;

  result = (int)Dali::DevelRenderer::Property::BATCHING_ENABLED;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_View_Property_TOOLTIP_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelControl::Property::TOOLTIP;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_ItemView_Property_LAYOUT_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelItemView::Property::LAYOUT;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_UNSELECTED_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::UNSELECTED_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_SELECTED_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::SELECTED_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_DISABLED_SELECTED_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::DISABLED_SELECTED_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_DISABLED_UNSELECTED_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::DISABLED_UNSELECTED_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_UNSELECTED_BACKGROUND_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::UNSELECTED_BACKGROUND_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_SELECTED_BACKGROUND_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::SELECTED_BACKGROUND_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_DISABLED_UNSELECTED_BACKGROUND_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::DISABLED_UNSELECTED_BACKGROUND_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_DISABLED_SELECTED_BACKGROUND_VISUAL_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::DISABLED_SELECTED_BACKGROUND_VISUAL;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_LABEL_RELATIVE_ALIGNMENT_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::LABEL_RELATIVE_ALIGNMENT;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_LABEL_PADDING_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::LABEL_PADDING;
  jresult = (int)result;
  return jresult;
}

SWIGEXPORT int SWIGSTDCALL CSharp_Button_Property_VISUAL_PADDING_get() {
  int jresult ;
  int result;

  result = (int)Dali::Toolkit::DevelButton::Property::VISUAL_PADDING;
  jresult = (int)result;
  return jresult;
}

#ifdef __cplusplus
}
#endif

