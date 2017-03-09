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

%pragma(csharp) modulecode=%{
  public static readonly int VISUAL_PROPERTY_TRANSFORM = NDalicManualPINVOKE.Visual_Property_TRANSFORM_get();
  public static readonly int VISUAL_PROPERTY_PREMULTIPLIED_ALPHA = NDalicManualPINVOKE.Visual_Property_PREMULTIPLIED_ALPHA_get();
  public static readonly int VISUAL_PROPERTY_MIX_COLOR = NDalicManualPINVOKE.Visual_Property_MIX_COLOR_get();
%}

%typemap(cscode) Dali::Toolkit::Visual::Type %{
  ,
  TEXT
%}

%typemap(cscode) Dali::Actor::Property %{
  public static readonly int SIBLING_ORDER = NDalicManualPINVOKE.Actor_Property_SIBLING_ORDER_get();
  public static readonly int OPACITY = NDalicManualPINVOKE.Actor_Property_OPACITY_get();
  public static readonly int SCREEN_POSITION = NDalicManualPINVOKE.Actor_Property_SCREEN_POSITION_get();
  public static readonly int POSITION_USES_ANCHOR_POINT = NDalicManualPINVOKE.Actor_Property_POSITION_USES_ANCHOR_POINT_get();
%}

%typemap(cscode) Dali::Toolkit::Control::Property %{
  public static readonly int TOOLTIP = NDalicManualPINVOKE.View_Property_TOOLTIP_get();
  public static readonly int STATE = NDalicManualPINVOKE.View_Property_STATE_get();
  public static readonly int SUB_STATE = NDalicManualPINVOKE.View_Property_SUB_STATE_get();
%}

%typemap(cscode) Dali::Toolkit::ItemView::Property %{
  public static readonly int LAYOUT = NDalicManualPINVOKE.ItemView_Property_LAYOUT_get();
%}

%typemap(cscode) Dali::Toolkit::ItemView %{
  public Dali.Property.Array Layout
  {
    get
    {
      Dali.Property.Array temp = new Dali.Property.Array();
      GetProperty( ItemView.Property.LAYOUT).Get( temp );
      return temp;
    }
    set
    {
      SetProperty( ItemView.Property.LAYOUT, new Dali.Property.Value( value ) );
    }
  }
%}

%typemap(cscode) Dali::Toolkit::Button::Property %{
  public static readonly int UNSELECTED_VISUAL = NDalicManualPINVOKE.Button_Property_UNSELECTED_VISUAL_get();
  public static readonly int SELECTED_VISUAL = NDalicManualPINVOKE.Button_Property_SELECTED_VISUAL_get();
  public static readonly int DISABLED_SELECTED_VISUAL = NDalicManualPINVOKE.Button_Property_DISABLED_SELECTED_VISUAL_get();
  public static readonly int DISABLED_UNSELECTED_VISUAL = NDalicManualPINVOKE.Button_Property_DISABLED_UNSELECTED_VISUAL_get();
  public static readonly int UNSELECTED_BACKGROUND_VISUAL = NDalicManualPINVOKE.Button_Property_UNSELECTED_BACKGROUND_VISUAL_get();
  public static readonly int SELECTED_BACKGROUND_VISUAL = NDalicManualPINVOKE.Button_Property_SELECTED_BACKGROUND_VISUAL_get();
  public static readonly int DISABLED_UNSELECTED_BACKGROUND_VISUAL = NDalicManualPINVOKE.Button_Property_DISABLED_UNSELECTED_BACKGROUND_VISUAL_get();
  public static readonly int DISABLED_SELECTED_BACKGROUND_VISUAL = NDalicManualPINVOKE.Button_Property_DISABLED_SELECTED_BACKGROUND_VISUAL_get();
  public static readonly int LABEL_RELATIVE_ALIGNMENT = NDalicManualPINVOKE.Button_Property_LABEL_RELATIVE_ALIGNMENT_get();
  public static readonly int LABEL_PADDING = NDalicManualPINVOKE.Button_Property_LABEL_PADDING_get();
  public static readonly int VISUAL_PADDING = NDalicManualPINVOKE.Button_Property_VISUAL_PADDING_get();
%}

