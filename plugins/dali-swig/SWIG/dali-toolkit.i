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
%apply unsigned int& OUTPUT { unsigned int& volume };
%feature("director") Dali::Toolkit::ItemFactory;

%ignore *::AccessibilityManager(Internal::AccessibilityManager*);
%ignore *::Alignment(Internal::Alignment&);
%ignore *::Alignment(Dali::Internal::CustomActor*);
%ignore *::Button(Internal::Button&);
%ignore *::Button(Dali::Internal::CustomActor*);
%ignore *::CheckBoxButton(Internal::CheckBoxButton&);
%ignore *::CheckBoxButton(Dali::Internal::CustomActor*);
%ignore *::Control(Dali::Internal::CustomActor*);
%ignore *::ControlWrapper( Internal::ControlWrapper& );
%ignore *::ControlWrapper( Dali::Internal::CustomActor* );
%ignore *::FlexContainer(Internal::FlexContainer&);
%ignore *::FlexContainer(Dali::Internal::CustomActor*);
%ignore *::GaussianBlurView(Internal::GaussianBlurView&);
%ignore *::GaussianBlurView(Dali::Internal::CustomActor*);
%ignore *::ImageView(Internal::ImageView&);
%ignore *::ImageView(Dali::Internal::CustomActor*);
%ignore *::ItemView(Internal::ItemView&);
%ignore *::ItemView(Dali::Internal::CustomActor*);
%ignore *::Model3dView(Internal::Model3dView&);
%ignore *::Model3dView(Dali::Internal::CustomActor*);
%ignore *::PageTurnLandscapeView(Internal::PageTurnLandscapeView&);
%ignore *::PageTurnLandscapeView(Dali::Internal::CustomActor*);
%ignore *::PageTurnPortraitView(Internal::PageTurnPortraitView&);
%ignore *::PageTurnPortraitView(Dali::Internal::CustomActor*);
%ignore *::PageTurnView(Internal::PageTurnView&);
%ignore *::PageTurnView(Dali::Internal::CustomActor*);
%ignore *::Popup(Internal::Popup&);
%ignore *::Popup(Dali::Internal::CustomActor*);
%ignore *::ProgressBar(Internal::ProgressBar&);
%ignore *::ProgressBar(Dali::Internal::CustomActor*);
%ignore *::PushButton(Internal::PushButton&);
%ignore *::PushButton(Dali::Internal::CustomActor*);
%ignore *::RadioButton(Internal::RadioButton&);
%ignore *::RadioButton(Dali::Internal::CustomActor*);
%ignore *::Scrollable(Internal::Scrollable&);
%ignore *::Scrollable(Dali::Internal::CustomActor*);
%ignore *::ScrollBar(Internal::ScrollBar&);
%ignore *::ScrollBar(Dali::Internal::CustomActor*);
%ignore *::ScrollView(Internal::ScrollView&);
%ignore *::ScrollView(Dali::Internal::CustomActor*);
%ignore *::ScrollViewEffect(Internal::ScrollViewEffect*);
%ignore *::Slider(Internal::Slider&);
%ignore *::Slider(Dali::Internal::CustomActor*);
%ignore *::StyleManager(Internal::StyleManager*);
%ignore *::TableView(Internal::TableView&);
%ignore *::TableView(Dali::Internal::CustomActor*);
%ignore *::TextEditor(Internal::TextEditor&);
%ignore *::TextEditor(Dali::Internal::CustomActor*);
%ignore *::TextField(Internal::TextField&);
%ignore *::TextField(Dali::Internal::CustomActor*);
%ignore *::TextLabel(Internal::TextLabel&);
%ignore *::TextLabel(Dali::Internal::CustomActor*);
%ignore *::VideoView(Internal::VideoView&);
%ignore *::VideoView(Dali::Internal::CustomActor*);
%ignore *::VisualFactory(Dali::Internal::VisualFactory*);
%ignore *::Base(Dali::Internal::Visual::Base*);
%ignore *::GetExtension();
%ignore *::GetControlExtension();
%ignore *::ToggleButton(Internal::ToggleButton&);
%ignore *::ToggleButton(Dali::Internal::CustomActor*);

%ignore *::Button::SetAnimationTime(float animationTime);
%ignore *::Button::SetAutoRepeating(bool);
%ignore *::Button::SetBackgroundImage(const std::string &);
%ignore *::Button::SetButtonImage(Dali::Image);
%ignore *::Button::SetDisabled(bool);
%ignore *::Button::SetDisabledBackgroundImage(const std::string &);
%ignore *::Button::SetDisabledImage(const std::string &);
%ignore *::Button::SetDisabledSelectedImage(const std::string &);
%ignore *::Button::SetInitialAutoRepeatingDelay(float);
%ignore *::Button::SetLabel(Dali::Actor);
%ignore *::Button::SetLabelText(const std::string &);
%ignore *::Button::SetNextAutoRepeatingDelay(float);
%ignore *::Button::SetSelected(bool);
%ignore *::Button::SetSelectedBackgroundImage(const std::string &);
%ignore *::Button::SetSelectedImage(const std::string &);
%ignore *::Button::SetSelectedImage(Dali::Image);
%ignore *::Button::SetTogglableButton(bool);
%ignore *::Button::SetUnselectedImage(const std::string &);
%ignore Dali::Toolkit::DevelVisual::Type;
%ignore Dali::Toolkit::DevelVisual::Property::Type;
%ignore Dali::Toolkit::AccessibilityManager::AccessibilityActionScrollSignalType;

%rename(View) Dali::Toolkit::Control;
%rename(ViewImpl) Dali::Toolkit::Internal::Control;
%rename(VisualBase) Dali::Toolkit::Visual::Base;
%rename(LoadImageSynchronously) Dali::Toolkit::SyncImageLoader::Load;
%rename(ControlOrientationType) Dali::Toolkit::ControlOrientation::Type;
%rename(DefaultItemLayoutType) Dali::Toolkit::DefaultItemLayout::Type;
%rename(DefaultItemLayoutProperty) Dali::Toolkit::DefaultItemLayoutProperty::Property;
%rename(NewItemLayout) Dali::Toolkit::DefaultItemLayout::New;
%rename(ContentDirectionType) Dali::Toolkit::FlexContainer::ContentDirection;
%rename(FlexDirectionType) Dali::Toolkit::FlexContainer::FlexDirection;
%rename(IluminationTypeEnum) Dali::Toolkit::Model3dView::IlluminationType;
%rename(DisplayStateType) Dali::Toolkit::Popup::DisplayState;
%rename(ContextualModeType) Dali::Toolkit::Popup::ContextualMode;
%rename(AnimationModeType) Dali::Toolkit::Popup::AnimationMode;
%rename(IndicatorHeightPolicyType) Dali::Toolkit::ScrollBar::IndicatorHeightPolicy;
%rename(ExceedPolicyType) Dali::Toolkit::TextField::ExceedPolicy;
%rename(ToolkitPropertyRange) Dali::Toolkit::PropertyRanges;
%rename(TooltipPositionType) Dali::Toolkit::Tooltip::Position::Type;
%rename(DirectionType) Dali::Toolkit::Direction::Type;
%rename(AlignType) Dali::Toolkit::Align::Type;
%rename(VisualType) Dali::Toolkit::Visual::Type;
%rename(VisualTransformPropertyType) Dali::Toolkit::DevelVisual::Transform::Property::Type;
%rename(VisualTransformPolicyType) Dali::Toolkit::DevelVisual::Transform::Policy::Type;
%rename(VISUAL_SHADER_VERTEX) Dali::Toolkit::Visual::Shader::Property::VERTEX_SHADER;
%rename(VISUAL_SHADER_FRAGMENT) Dali::Toolkit::Visual::Shader::Property::FRAGMENT_SHADER;
%rename(VISUAL_SHADER_SUBDIVIDE_GRID_X) Dali::Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_X;
%rename(VISUAL_SHADER_SUBDIVIDE_GRID_Y) Dali::Toolkit::Visual::Shader::Property::SUBDIVIDE_GRID_Y;
%rename(VISUAL_SHADER_HINTS) Dali::Toolkit::Visual::Shader::Property::HINTS;
%rename(BORDER_VISUAL_COLOR) Dali::Toolkit::BorderVisual::Property::COLOR;
%rename(BORDER_VISUAL_SIZE) Dali::Toolkit::BorderVisual::Property::SIZE;
%rename(BORDER_VISUAL_ANTI_ALIASING) Dali::Toolkit::BorderVisual::Property::ANTI_ALIASING;
%rename(COLOR_VISUAL_MIX_COLOR) Dali::Toolkit::ColorVisual::Property::MIX_COLOR;
%rename(GradientVisualUnitsType) Dali::Toolkit::GradientVisual::Units::Type;
%rename(GradientVisualSpreadMethodType) Dali::Toolkit::GradientVisual::SpreadMethod::Type;
%rename(GRADIENT_VISUAL_START_POSITION) Dali::Toolkit::GradientVisual::Property::START_POSITION;
%rename(GRADIENT_VISUAL_END_POSITION) Dali::Toolkit::GradientVisual::Property::END_POSITION;
%rename(GRADIENT_VISUAL_CENTER) Dali::Toolkit::GradientVisual::Property::CENTER;
%rename(GRADIENT_VISUAL_RADIUS) Dali::Toolkit::GradientVisual::Property::RADIUS;
%rename(GRADIENT_VISUAL_STOP_OFFSET) Dali::Toolkit::GradientVisual::Property::STOP_OFFSET;
%rename(GRADIENT_VISUAL_STOP_COLOR) Dali::Toolkit::GradientVisual::Property::STOP_COLOR;
%rename(GRADIENT_VISUAL_UNITS) Dali::Toolkit::GradientVisual::Property::UNITS;
%rename(GRADIENT_VISUAL_SPREAD_METHOD) Dali::Toolkit::GradientVisual::Property::SPREAD_METHOD;
%rename(MeshVisualShadingModeValue) Dali::Toolkit::MeshVisual::ShadingMode::Value;
%rename(MESH_VISUAL_OBJECT_URL) Dali::Toolkit::MeshVisual::Property::OBJECT_URL;
%rename(MESH_VISUAL_MATERIAL_URL) Dali::Toolkit::MeshVisual::Property::MATERIAL_URL;
%rename(MESH_VISUAL_TEXTURES_PATH) Dali::Toolkit::MeshVisual::Property::TEXTURES_PATH;
%rename(MESH_VISUAL_SHADING_MODE) Dali::Toolkit::MeshVisual::Property::SHADING_MODE;
%rename(MESH_VISUAL_USE_MIPMAPPING) Dali::Toolkit::MeshVisual::Property::USE_MIPMAPPING;
%rename(MESH_VISUAL_USE_SOFT_NORMALS) Dali::Toolkit::MeshVisual::Property::USE_SOFT_NORMALS;
%rename(MESH_VISUAL_LIGHT_POSITION) Dali::Toolkit::MeshVisual::Property::LIGHT_POSITION;
%rename(PrimitiveVisualShapeType) Dali::Toolkit::PrimitiveVisual::Shape::Type;
%rename(PRIMITIVE_VISUAL_SHAPE) Dali::Toolkit::PrimitiveVisual::Property::SHAPE;
%rename(PRIMITIVE_VISUAL_MIX_COLOR) Dali::Toolkit::PrimitiveVisual::Property::MIX_COLOR;
%rename(PRIMITIVE_VISUAL_SLICES) Dali::Toolkit::PrimitiveVisual::Property::SLICES;
%rename(PRIMITIVE_VISUAL_STACKS) Dali::Toolkit::PrimitiveVisual::Property::STACKS;
%rename(PRIMITIVE_VISUAL_SCALE_TOP_RADIUS) Dali::Toolkit::PrimitiveVisual::Property::SCALE_TOP_RADIUS;
%rename(PRIMITIVE_VISUAL_SCALE_BOTTOM_RADIUS) Dali::Toolkit::PrimitiveVisual::Property::SCALE_BOTTOM_RADIUS;
%rename(PRIMITIVE_VISUAL_SCALE_HEIGHT) Dali::Toolkit::PrimitiveVisual::Property::SCALE_HEIGHT;
%rename(PRIMITIVE_VISUAL_SCALE_RADIUS) Dali::Toolkit::PrimitiveVisual::Property::SCALE_RADIUS;
%rename(PRIMITIVE_VISUAL_SCALE_DIMENSIONS) Dali::Toolkit::PrimitiveVisual::Property::SCALE_DIMENSIONS;
%rename(PRIMITIVE_VISUAL_BEVEL_PERCENTAGE) Dali::Toolkit::PrimitiveVisual::Property::BEVEL_PERCENTAGE;
%rename(PRIMITIVE_VISUAL_BEVEL_SMOOTHNESS) Dali::Toolkit::PrimitiveVisual::Property::BEVEL_SMOOTHNESS;
%rename(PRIMITIVE_VISUAL_LIGHT_POSITION) Dali::Toolkit::PrimitiveVisual::Property::LIGHT_POSITION;
%rename(VISUAL_PROPERTY_TYPE) Dali::Toolkit::Visual::Property::TYPE;
%rename(VISUAL_PROPERTY_SHADER) Dali::Toolkit::Visual::Property::SHADER;
%rename(IMAGE_VISUAL_URL) Dali::Toolkit::ImageVisual::Property::URL;
%rename(IMAGE_VISUAL_FITTING_MODE) Dali::Toolkit::ImageVisual::Property::FITTING_MODE;
%rename(IMAGE_VISUAL_SAMPLING_MODE) Dali::Toolkit::ImageVisual::Property::SAMPLING_MODE;
%rename(IMAGE_VISUAL_DESIRED_WIDTH) Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH;
%rename(IMAGE_VISUAL_DESIRED_HEIGHT) Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT;
%rename(IMAGE_VISUAL_SYNCHRONOUS_LOADING) Dali::Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING;
%rename(IMAGE_VISUAL_BORDER_ONLY) Dali::Toolkit::ImageVisual::Property::BORDER_ONLY;
%rename(IMAGE_VISUAL_BATCHING_ENABLED) Dali::Toolkit::ImageVisual::Property::BATCHING_ENABLED;
%rename(IMAGE_VISUAL_PIXEL_AREA) Dali::Toolkit::ImageVisual::Property::PIXEL_AREA;
%rename(IMAGE_VISUAL_WRAP_MODE_U) Dali::Toolkit::ImageVisual::Property::WRAP_MODE_U;
%rename(IMAGE_VISUAL_WRAP_MODE_V) Dali::Toolkit::ImageVisual::Property::WRAP_MODE_V;
%rename(COLOR_VISUAL_MIX_COLOR) Dali::Toolkit::Color::Visual::Property::MIX_COLOR;
%rename(TEXT_VISUAL_TEXT) Dali::Toolkit::TextVisual::Property::TEXT;
%rename(TEXT_VISUAL_FONT_FAMILY) Dali::Toolkit::TextVisual::Property::FONT_FAMILY;
%rename(TEXT_VISUAL_FONT_STYLE) Dali::Toolkit::TextVisual::Property::FONT_STYLE;
%rename(TEXT_VISUAL_POINT_SIZE) Dali::Toolkit::TextVisual::Property::POINT_SIZE;
%rename(TEXT_VISUAL_MULTI_LINE) Dali::Toolkit::TextVisual::Property::MULTI_LINE;
%rename(TEXT_VISUAL_HORIZONTAL_ALIGNMENT) Dali::Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT;
%rename(TEXT_VISUAL_VERTICAL_ALIGNMENT) Dali::Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT;
%rename(TEXT_VISUAL_TEXT_COLOR) Dali::Toolkit::TextVisual::Property::TEXT_COLOR;
%rename(TEXT_VISUAL_ENABLE_MARKUP) Dali::Toolkit::TextVisual::Property::ENABLE_MARKUP;
%rename(TOOLTIP_CONTENT) Dali::Toolkit::Tooltip::Property::CONTENT;
%rename(TOOLTIP_LAYOUT) Dali::Toolkit::Tooltip::Property::LAYOUT;
%rename(TOOLTIP_WAIT_TIME) Dali::Toolkit::Tooltip::Property::WAIT_TIME;
%rename(TOOLTIP_BACKGROUND) Dali::Toolkit::Tooltip::Property::BACKGROUND;
%rename(TOOLTIP_TAIL) Dali::Toolkit::Tooltip::Property::TAIL;
%rename(TOOLTIP_POSITION) Dali::Toolkit::Tooltip::Property::POSITION;
%rename(TOOLTIP_HOVER_POINT_OFFSET) Dali::Toolkit::Tooltip::Property::HOVER_POINT_OFFSET;
%rename(TOOLTIP_MOVEMENT_THRESHOLD) Dali::Toolkit::Tooltip::Property::MOVEMENT_THRESHOLD;
%rename(TOOLTIP_DISAPPEAR_ON_MOVEMENT) Dali::Toolkit::Tooltip::Property::DISAPPEAR_ON_MOVEMENT;
%rename(TOOLTIP_CONTENT) Dali::Toolkit::Tooltip::Property::CONTENT;
%rename(TOOLTIP_BACKGROUND_VISUAL) Dali::Toolkit::Tooltip::Background::Property::VISUAL;
%rename(TOOLTIP_BACKGROUND_BORDER) Dali::Toolkit::Tooltip::Background::Property::BORDER;
%rename(TOOLTIP_TAIL_VISIBILITY) Dali::Toolkit::Tooltip::Tail::Property::VISIBILITY;
%rename(TOOLTIP_TAIL_ABOVE_VISUAL) Dali::Toolkit::Tooltip::Tail::Property::ABOVE_VISUAL;
%rename(TOOLTIP_TAIL_BELOW_VISUAL) Dali::Toolkit::Tooltip::Tail::Property::BELOW_VISUAL;

%csconstvalue("PropertyRanges.PROPERTY_REGISTRATION_START_INDEX") PROPERTY_START_INDEX;
%csconstvalue("View.PropertyRange.PROPERTY_START_INDEX+1000") PROPERTY_END_INDEX;
%csconstvalue("PropertyRanges.CHILD_PROPERTY_REGISTRATION_START_INDEX") CHILD_PROPERTY_START_INDEX;
%csconstvalue("PropertyRanges.CHILD_PROPERTY_REGISTRATION_START_INDEX+1000") CHILD_PROPERTY_END_INDEX;
%csconstvalue("PropertyRanges.ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX") ANIMATABLE_PROPERTY_START_INDEX;
%csconstvalue("PropertyRanges.ANIMATABLE_PROPERTY_REGISTRATION_START_INDEX+1000") ANIMATABLE_PROPERTY_END_INDEX;
%csconstvalue("PropertyRanges.CORE_PROPERTY_MAX_INDEX+1") VISUAL_PROPERTY_BASE_START_INDEX;
%csconstvalue("0") CONTROL_BEHAVIOUR_NONE;
%csconstvalue("0") CONTROL_BEHAVIOUR_DEFAULT;
%csconstvalue("1 << 0") DISABLE_SIZE_NEGOTIATION;
%csconstvalue("1 << 1") REQUIRES_TOUCH_EVENTS;
%csconstvalue("1 << 2") REQUIRES_HOVER_EVENTS;
%csconstvalue("1 << 3") REQUIRES_WHEEL_EVENTS;
%csconstvalue("1 << 4") REQUIRES_STYLE_CHANGE_SIGNALS;
%csconstvalue("1 << 5") REQUIRES_KEYBOARD_NAVIGATION_SUPPORT;
%csconstvalue("1 << 6") DISABLE_STYLE_CHANGE_SIGNALS;

%typemap(csclassmodifiers) Dali::Toolkit::Alignment "internal class"

typedef unsigned int ItemId;
typedef std::vector<ItemId> ItemIdContainer;
typedef std::pair<ItemId, Actor> Item;
typedef std::vector<Item> ItemContainer;
typedef Dali::IntrusivePtr<Dali::Toolkit::Ruler> RulerPtr;

%include <dali-toolkit/public-api/toolkit-property-index-ranges.h>

%include <dali-toolkit/public-api/visuals/visual-properties.h>
%include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
%include <dali-toolkit/public-api/visuals/border-visual-properties.h>
%include <dali-toolkit/public-api/visuals/color-visual-properties.h>
%include <dali-toolkit/public-api/visuals/gradient-visual-properties.h>
%include <dali-toolkit/public-api/visuals/image-visual-properties.h>
%include <dali-toolkit/public-api/visuals/mesh-visual-properties.h>
%include <dali-toolkit/public-api/visuals/primitive-visual-properties.h>
%include <dali-toolkit/devel-api/visuals/text-visual-properties.h>

%include <dali-toolkit/devel-api/builder/builder.h>

%include <dali-toolkit/devel-api/visual-factory/transition-data.h>

%include <dali-toolkit/devel-api/controls/tooltip/tooltip-properties.h>

%include <dali-toolkit/public-api/controls/control-impl.h>
%include <dali-toolkit/public-api/controls/control.h>

%include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

%include <dali-toolkit/public-api/controls/alignment/alignment.h>
%include <dali-toolkit/public-api/controls/buttons/button.h>
%include <dali-toolkit/public-api/controls/buttons/check-box-button.h>
%include <dali-toolkit/public-api/controls/buttons/push-button.h>
%include <dali-toolkit/public-api/controls/buttons/radio-button.h>
%include <dali-toolkit/public-api/controls/flex-container/flex-container.h>
%include <dali-toolkit/public-api/controls/image-view/image-view.h>
%include <dali-toolkit/public-api/controls/model3d-view/model3d-view.h>
%include <dali-toolkit/public-api/controls/scroll-bar/scroll-bar.h>
%include <dali-toolkit/public-api/controls/scrollable/scrollable.h>
%include <dali-toolkit/public-api/enums.h>
%include <dali-toolkit/public-api/controls/scrollable/item-view/item-layout.h>
%include <dali-toolkit/public-api/controls/scrollable/item-view/default-item-layout.h>
%include <dali-toolkit/devel-api/controls/scrollable/item-view/default-item-layout-property.h>
%include <dali-toolkit/public-api/controls/scrollable/item-view/item-factory.h>
%include <dali-toolkit/public-api/controls/scrollable/item-view/item-view-declarations.h>
%include <dali-toolkit/public-api/controls/scrollable/item-view/item-view.h>
%include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-constraints.h>
%include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-effect.h>
%include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view-page-path-effect.h>
%include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>
%include <dali-toolkit/public-api/controls/table-view/table-view.h>
%include <dali-toolkit/public-api/text/rendering-backend.h>
%include <dali-toolkit/public-api/controls/text-controls/text-editor.h>
%include <dali-toolkit/public-api/controls/text-controls/text-field.h>
%include <dali-toolkit/public-api/controls/text-controls/text-label.h>
%include <dali-toolkit/public-api/accessibility-manager/accessibility-manager.h>
%include <dali-toolkit/public-api/styling/style-manager.h>
%include <dali-toolkit/public-api/controls/slider/slider.h>
%include <dali-toolkit/public-api/controls/video-view/video-view.h>

%include <dali-toolkit/devel-api/controls/popup/popup.h>
%include <dali-toolkit/devel-api/controls/progress-bar/progress-bar.h>
%include <dali-toolkit/devel-api/controls/gaussian-blur-view/gaussian-blur-view.h>
%include <dali-toolkit/devel-api/controls/page-turn-view/page-factory.h>
%include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-view.h>
%include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-landscape-view.h>
%include <dali-toolkit/devel-api/controls/page-turn-view/page-turn-portrait-view.h>
%include <dali-toolkit/devel-api/controls/buttons/toggle-button.h>

%include <dali-toolkit/devel-api/align-enums.h>

%include <dali-toolkit/devel-api/visual-factory/visual-base.h>
%include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

%include <dali-toolkit/public-api/image-loader/async-image-loader.h>
%include <dali-toolkit/public-api/image-loader/sync-image-loader.h>
%include <dali-toolkit/public-api/text/rendering-backend.h>

%template(ItemIdContainer) std::vector<unsigned int>;
%template(Item) std::pair<unsigned int, Dali::Actor>;
%template(ItemContainer) std::vector<std::pair<unsigned int, Dali::Actor>>;
%template(ActorContainer) std::vector<Dali::Actor>;
%template(AccessibilityActionSignal) Dali::Signal<bool(Dali::Toolkit::AccessibilityManager&)>;
%template(AccessibilityFocusOvershotSignal) Dali::Signal<void(Dali::Actor, Dali::Toolkit::AccessibilityManager::FocusOvershotDirection)>;
%template(FocusChangedSignal) Dali::Signal<void(Dali::Actor, Dali::Actor)>;
%template(FocusGroupChangedSignal) Dali::Signal<void(Dali::Actor, bool)>;
%template(StyleChangedSignal) Dali::Signal<void(Dali::Toolkit::StyleManager, Dali::StyleChange::Type)>;
%template(ButtonSignal) Dali::Signal<bool(Dali::Toolkit::Button)>;
%template(GaussianBlurViewSignal) Dali::Signal<void (Dali::Toolkit::GaussianBlurView)>;
%template(PageTurnSignal) Dali::Signal<void(Dali::Toolkit::PageTurnView, unsigned int, bool)>;
%template(PagePanSignal) Dali::Signal<void(Dali::Toolkit::PageTurnView)>;
%template(ProgressBarValueChangedSignal) Dali::Signal<void(Dali::Toolkit::ProgressBar, float, float)>;
%template(ScrollViewSnapStartedSignal) Dali::Signal< void(const Dali::Toolkit::ScrollView::SnapEvent&)>;
%template(ScrollableSignal) Dali::Signal< void(const Dali::Vector2&)>;
%template(TextEditorSignal) Dali::Signal<void(Dali::Toolkit::TextEditor)>;
%template(TextFieldSignal) Dali::Signal<void(Dali::Toolkit::TextField)>;
%template(ControlKeySignal) Dali::Signal<bool(Dali::Toolkit::Control, const Dali::KeyEvent&)>;
%template(KeyInputFocusSignal) Dali::Signal<void(Dali::Toolkit::Control)>;
%template(VideoViewSignal) Dali::Signal<void(Dali::Toolkit::VideoView&)>;
%template(SliderValueChangedSignal) Dali::Signal<bool(Dali::Toolkit::Slider, float)>;
%template(SliderMarkReachedSignal) Dali::Signal<bool(Dali::Toolkit::Slider, int)>;
%template(RulerPtr) Dali::IntrusivePtr<Dali::Toolkit::Ruler>;
