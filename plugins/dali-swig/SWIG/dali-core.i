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

%apply int& INOUT { int& integerValue };
%apply float& INOUT { float& floatValue };
%apply bool& INOUT { bool& boolValue };
%apply float& OUTPUT { float& localX };
%apply float& OUTPUT { float& localY };
%apply float& OUTPUT { float& viewportX };
%apply float& OUTPUT { float& viewportY };

#if defined(SWIGCSHARP)

%include arrays_csharp.i
%apply unsigned char INPUT[] { unsigned char* pixelBuffer}
%apply unsigned char INPUT[] { unsigned char* buffer}
%apply unsigned char INPUT[] { unsigned char* at}
%apply unsigned char INPUT[] { unsigned char* iterator}
%apply unsigned char INPUT[] { unsigned char* first}
//%apply unsigned char OUTPUT[] { unsigned char* GetBuffer}
//%apply unsigned char OUTPUT[] { unsigned char* Begin}
//%apply unsigned char OUTPUT[] { unsigned char* End}
%apply unsigned char OUTPUT[] { unsigned char* ValueOfIndex}
%apply unsigned short INPUT[] { unsigned short* indices}
%apply float INPUT[] { float* array }

#endif

%ignore *::Animation(Internal::Animation*);
%ignore *::Actor(Internal::Actor*);
%ignore *::BufferImage(Internal::BufferImage*);
%ignore *::CameraActor(Internal::CameraActor*);
%ignore *::CustomActor(Internal::CustomActor*);
%ignore *::EncodedBufferImage(Internal::EncodedBufferImage*);
%ignore *::FrameBuffer(Internal::FrameBuffer*);
%ignore *::FrameBufferImage(Internal::FrameBufferImage*);
%ignore *::Geometry(Internal::Geometry*);
%ignore *::Handle(Dali::Internal::Object*);
%ignore *::Image(Internal::Image*);
%ignore *::KeyFrames(Internal::KeyFrames*);
%ignore *::Layer(Internal::Layer*);
%ignore *::LinearConstrainer(Internal::LinearConstrainer*);
%ignore *::LongPressGestureDetector(Internal::LongPressGestureDetector*);
%ignore *::NativeImage(Internal::NativeImage*);
%ignore *::NinePatchImage(Internal::NinePatchImage*);
%ignore *::ObjectRegistry(Internal::ObjectRegistry*);
%ignore *::PanGestureDetector(Internal::PanGestureDetector*);
%ignore *::Path(Internal::Path*);
%ignore *::PathConstrainer(Internal::PathConstrainer*);
%ignore *::PinchGestureDetector(Internal::PinchGestureDetector*);
%ignore *::PixelData(Internal::PixelData*);
%ignore *::PropertyBuffer(Internal::PropertyBuffer*);
%ignore *::PropertyNotification(Internal::PropertyNotification*);
%ignore *::Renderer(Internal::Renderer*);
%ignore *::RenderTask(Internal::RenderTask*);
%ignore *::RenderTaskList(Internal::RenderTaskList*);
%ignore *::ResourceImage(Internal::ResourceImage*);
%ignore *::Sampler(Internal::Sampler*);
%ignore *::Shader(Internal::Shader*);
%ignore *::Stage(Internal::Stage*);
%ignore *::TapGestureDetector(Internal::TapGestureDetector*);
%ignore *::Texture(Internal::NewTexture*);
%ignore *::TextureSet(Internal::TextureSet*);
%ignore *::TouchData(Internal::TouchData*);
%ignore *::TypeInfo(Internal::TypeInfo*);
%ignore *::GetExtension();
%ignore *::Initialize(Internal::CustomActor&);
%ignore *::GetOwner() const;
%ignore *::ParentOrigin::DEFAULT;
%ignore *::AnchorPoint::DEFAULT;

%rename(ParentOriginTop) Dali::ParentOrigin::TOP;
%rename(ParentOriginBottom) Dali::ParentOrigin::BOTTOM;
%rename(ParentOriginLeft) Dali::ParentOrigin::LEFT;
%rename(ParentOriginRight) Dali::ParentOrigin::RIGHT;
%rename(ParentOriginMiddle) Dali::ParentOrigin::MIDDLE;
%rename(ParentOriginTopLeft) Dali::ParentOrigin::TOP_LEFT;
%rename(ParentOriginTopCenter) Dali::ParentOrigin::TOP_CENTER;
%rename(ParentOriginTopRight) Dali::ParentOrigin::TOP_RIGHT;
%rename(ParentOriginCenterLeft) Dali::ParentOrigin::CENTER_LEFT;
%rename(ParentOriginCenter) Dali::ParentOrigin::CENTER;
%rename(ParentOriginCenterRight) Dali::ParentOrigin::CENTER_RIGHT;
%rename(ParentOriginBottomLeft) Dali::ParentOrigin::BOTTOM_LEFT;
%rename(ParentOriginBottomCenter) Dali::ParentOrigin::BOTTOM_CENTER;
%rename(ParentOriginBottomRight) Dali::ParentOrigin::BOTTOM_RIGHT;
%rename(AnchorPointTop) Dali::AnchorPoint::TOP;
%rename(AnchorPointBottom) Dali::AnchorPoint::BOTTOM;
%rename(AnchorPointLeft) Dali::AnchorPoint::LEFT;
%rename(AnchorPointRight) Dali::AnchorPoint::RIGHT;
%rename(AnchorPointMiddle) Dali::AnchorPoint::MIDDLE;
%rename(AnchorPointTopLeft) Dali::AnchorPoint::TOP_LEFT;
%rename(AnchorPointTopCenter) Dali::AnchorPoint::TOP_CENTER;
%rename(AnchorPointTopRight) Dali::AnchorPoint::TOP_RIGHT;
%rename(AnchorPointCenterLeft) Dali::AnchorPoint::CENTER_LEFT;
%rename(AnchorPointCenter) Dali::AnchorPoint::CENTER;
%rename(AnchorPointCenterRight) Dali::AnchorPoint::CENTER_RIGHT;
%rename(AnchorPointBottomLeft) Dali::AnchorPoint::BOTTOM_LEFT;
%rename(AnchorPointBottomCenter) Dali::AnchorPoint::BOTTOM_CENTER;
%rename(AnchorPointBottomRight) Dali::AnchorPoint::BOTTOM_RIGHT;
%rename(ConvertToFloat) operator float;
%rename(DimensionType) Dali::Dimension::Type;
%rename(ResizePolicyType) Dali::ResizePolicy::Type;
%rename(ResizePolicyDefault) Dali::ResizePolicy::DEFAULT;
%rename(SizeScalePolicyType) Dali::SizeScalePolicy::Type;
%rename(HorizontalAlignmentType) Dali::HorizontalAlignment::Type;
%rename(VerticalAlignmentType) Dali::VerticalAlignment::Type;
%rename(ClippingModeType) Dali::ClippingMode::Type;
%rename(FilterModeType) Dali::FilterMode::Type;
%rename(WrapModeType) Dali::WrapMode::Type;
%rename(PixelFormat) Dali::Pixel::Format;
%rename(TextureType) Dali::TextureType::Type;
%rename(FaceCullingModeType) Dali::FaceCullingMode::Type;
%rename(BlendModeType) Dali::BlendMode::Type;
%rename(BlendEquationType) Dali::BlendEquation::Type;
%rename(BlendFactorType) Dali::BlendFactor::Type;
%rename(DepthWriteModeType) Dali::DepthWriteMode::Type;
%rename(DepthTestModeType) Dali::DepthTestMode::Type;
%rename(DepthFunctionType) Dali::DepthFunction::Type;
%rename(RenderModeType) Dali::RenderMode::Type;
%rename(StencilFunctionType) Dali::StencilFunction::Type;
%rename(StencilModeType) Dali::StencilMode::Type;
%rename(StencilOperationType) Dali::StencilOperation::Type;
%rename(DrawModeType) Dali::DrawMode::Type;
%rename(PointStateType) Dali::PointState::Type;
%rename(FittingModeType) Dali::FittingMode::Type;
%rename(FittingModeDefault) Dali::FittingMode::DEFAULT;
%rename(SamplingModeType) Dali::SamplingMode::Type;
%rename(RenderBufferFormat) Dali::RenderBuffer::Format;
%rename(BlendingModeType) Dali::BlendingMode::Type;
%rename(BlendingFactorType) Dali::BlendingFactor::Type;
%rename(BlendingEquationType) Dali::BlendingEquation::Type;
%rename(CameraType) Dali::Camera::Type;
%rename(LayerBehavior) Dali::Layer::Behavior;

typedef std::pair<std::string, Dali::Property::Value> StringValuePair;
typedef std::vector<Dali::TouchPoint> TouchPointContainer;
typedef std::pair< Dali::Radian, Dali::Radian > AngleThresholdPair;

%include <dali/public-api/object/ref-object.h>
%include <dali/public-api/object/any.h>

%include <dali/public-api/common/intrusive-ptr.h>

%include <dali/public-api/math/vector2.h>
%include <dali/public-api/math/vector3.h>
%include <dali/public-api/math/vector4.h>
%include <dali/public-api/math/rect.h>
%include <dali/public-api/math/uint-16-pair.h>
%include <dali/public-api/math/degree.h>
%include <dali/public-api/math/radian.h>
%include <dali/public-api/math/quaternion.h>
%include <dali/public-api/math/matrix.h>
%include <dali/public-api/math/matrix3.h>
%include <dali/public-api/math/random.h>
%include <dali/public-api/math/angle-axis.h>
%include <dali/public-api/math/viewport.h>

%include <dali/public-api/object/property-index-ranges.h>
%include <dali/public-api/object/property.h>
%include <dali/public-api/object/property-array.h>
%include <dali/public-api/object/property-map.h>
%include <dali/public-api/object/property-types.h>
%include <dali/public-api/object/property-value.h>
%include <dali/public-api/object/base-object.h>
%include <dali/public-api/object/base-handle.h>

%include <dali/public-api/signals/connection-tracker-interface.h>
%include <dali/public-api/signals/signal-slot-observers.h>
%include <dali/public-api/signals/connection-tracker.h>
%include <dali/public-api/signals/dali-signal.h>

%include <dali/public-api/object/object-registry.h>
%include <dali/public-api/object/property-conditions.h>
%include <dali/public-api/object/property-notification-declarations.h>
%include <dali/public-api/object/property-notification.h>
%include <dali/public-api/object/handle.h>
%include <dali/public-api/object/type-info.h>

%include <dali/public-api/common/constants.h>
%include <dali/public-api/actors/actor-enumerations.h>
%include <dali/public-api/common/dali-vector.h>
%include <dali/public-api/common/vector-wrapper.h>

%include <dali/public-api/images/image.h>
%include <dali/public-api/actors/sampling.h>
%include <dali/public-api/images/pixel.h>
%include <dali/public-api/images/pixel-data.h>

%include <dali/public-api/rendering/texture.h>
%include <dali/public-api/rendering/sampler.h>
%include <dali/public-api/rendering/texture-set.h>
%include <dali/public-api/rendering/property-buffer.h>
%include <dali/public-api/rendering/geometry.h>
%include <dali/public-api/rendering/shader.h>
%include <dali/public-api/rendering/renderer.h>
%include <dali/public-api/actors/draw-mode.h>
%include <dali/public-api/common/loading-state.h>
%include <dali/public-api/rendering/frame-buffer.h>
%include <dali/public-api/render-tasks/render-task-list.h>
%include <dali/public-api/render-tasks/render-task.h>

%include <dali/public-api/events/touch-point.h>
%include <dali/public-api/events/point-state.h>
%include <dali/public-api/events/touch-data.h>
%include <dali/public-api/events/gesture-detector.h>
%include <dali/public-api/events/gesture.h>
%include <dali/public-api/events/hover-event.h>
%include <dali/public-api/events/key-event.h>
%include <dali/public-api/events/long-press-gesture-detector.h>
%include <dali/public-api/events/long-press-gesture.h>
%include <dali/public-api/events/wheel-event.h>

%include <dali/public-api/actors/actor.h>
%include <dali/public-api/actors/layer.h>
%include <dali/public-api/common/stage.h>
%include <dali/public-api/size-negotiation/relayout-container.h>
%include <dali/public-api/actors/custom-actor-impl.h>
%include <dali/public-api/actors/custom-actor.h>

%include <dali/public-api/events/pan-gesture-detector.h>
%include <dali/public-api/events/pan-gesture.h>
%include <dali/public-api/events/pinch-gesture-detector.h>
%include <dali/public-api/events/pinch-gesture.h>
%include <dali/public-api/events/tap-gesture-detector.h>
%include <dali/public-api/events/tap-gesture.h>
%include <dali/public-api/events/touch-event.h>

%include <dali/public-api/animation/alpha-function.h>
%include <dali/public-api/animation/key-frames.h>
%include <dali/public-api/animation/path.h>
%include <dali/public-api/animation/time-period.h>
%include <dali/public-api/animation/animation.h>
%include <dali/public-api/animation/linear-constrainer.h>
%include <dali/devel-api/animation/path-constrainer.h>

%include <dali/public-api/common/view-mode.h>
%include <dali/public-api/images/image-operations.h>
%include <dali/public-api/images/buffer-image.h>
%include <dali/public-api/images/encoded-buffer-image.h>
%include <dali/public-api/images/native-image.h>
%include <dali/public-api/images/native-image-interface.h>
%include <dali/public-api/images/resource-image.h>
%include <dali/public-api/images/nine-patch-image.h>
%include <dali/public-api/images/frame-buffer-image.h>

%include <dali/public-api/actors/blending.h>
%include <dali/public-api/actors/camera-actor.h>

%template(StringValuePair) std::pair<std::string, Dali::Property::Value>;
%template(TouchPointContainer) std::vector<Dali::TouchPoint>;
%template(RectDouble) Dali::Rect<double>;
%template(RectInteger)  Dali::Rect<int>;
%template(RectUnsignedInteger)  Dali::Rect<unsigned int>;
%template(RectFloat)  Dali::Rect<float>;
%template(VectorInteger) Dali::Vector<int>;
%template(VectorFloat) Dali::Vector<float>;
%template(VectorUnsignedChar) Dali::Vector<unsigned char>;
%template(VectorUint16Pair) Dali::Vector<Dali::Uint16Pair>;
%template(VoidSignal) Dali::Signal<void()>;
%template(BoolSignal) Dali::Signal<bool()>;
%template(FloatSignal) Dali::Signal<void(float)>;
%template(ObjectCreatedSignal) Dali::Signal<void(Dali::BaseHandle)>;
%template(ObjectDestroyedSignal) Dali::Signal<void(const Dali::RefObject*)>;
%template(PropertyNotifySignal) Dali::Signal<void(Dali::PropertyNotification&)>;
%template(ImageSignal) Dali::Signal<void(Dali::Image)>;
%template(RenderTaskSignal) Dali::Signal<void(Dali::RenderTask&)>;
%template(LongPressGestureDetectedSignal) Dali::Signal<void (Dali::Actor, const Dali::LongPressGesture&)>;
%template(ActorTouchEventSignal) Dali::Signal<bool (Dali::Actor, const Dali::TouchEvent&)>;
%template(ActorTouchDataSignal) Dali::Signal<bool (Dali::Actor, const Dali::TouchData&)>;
%template(ActorHoverEventSignal) Dali::Signal<bool (Dali::Actor, const Dali::HoverEvent&)>;
%template(ActorWheelEventSignal) Dali::Signal<bool (Dali::Actor, const Dali::WheelEvent&)>;
%template(ActorSignal) Dali::Signal<void (Dali::Actor)>;
%template(KeyEventSignal) Dali::Signal<void (const Dali::KeyEvent&)>;
%template(TouchEventSignal) Dali::Signal<void (const Dali::TouchEvent&)>;
%template(TouchSignal) Dali::Signal<void (const Dali::TouchData&)>;
%template(StageWheelEventSignal) Dali::Signal<void (const Dali::WheelEvent&)>;
%template(AngleThresholdPair) std::pair<Dali::Radian, Dali::Radian>;
%template(PanGestureDetectedSignal) Dali::Signal<void (Dali::Actor, const Dali::PanGesture&)>;
%template(PinchGestureDetectedSignal) Dali::Signal<void (Dali::Actor, const Dali::PinchGesture&)>;
%template(TapGestureDetectedSignal) Dali::Signal<void (Dali::Actor, const Dali::TapGesture&)>;
%template(AnimationSignal) Dali::Signal<void(Dali::Animation&)>;
%template(ResourceImageSignal) Dali::Signal<void(Dali::ResourceImage)>;
