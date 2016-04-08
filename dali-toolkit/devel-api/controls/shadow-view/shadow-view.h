#ifndef __DALI_TOOLKIT_SHADOW_VIEW_H__
#define __DALI_TOOLKIT_SHADOW_VIEW_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

/**
 * ShadowView implementation class
 */
class ShadowView;

} // namespace Internal

/**
 *
 * ShadowView is a class for applying shadows to objects present in the view.
 *
 * Basic idea:-
 *
 * 1) The ShadowView object will render all its child actors offscreen from the light's point of view projected on to the shadow plane in a seperate render task.\n
 * 2) The ShadowView object then blurs the result of step 1), using a two pass separated Gaussian blur.\n
 * 3) The ShadowView object gets rendered automatically in the default render task along with it's children.
 *
 * Fundamentally, the ShadowView is simply an Actor in the normal actor tree that affects all of its children. It should be added to your Actor tree and manipulated in the
 * normal way. It can be considered a 'portal' in the sense that all child actors are clipped to the ShadowView actor bounds.
 *
 * LIMITATIONS:
 * The ShadowView is intended to provide simple planar projection shadows, Which means it needs a flat plane to cast shadows. So Shadows can't be cast on other objects.
 *
 * ************\n
 * NB: It is essential to remove the ShadowView from the stage and also to call Deactivate() on it when you are not using it. This will ensure that resources are freed and
 * rendering stops.\n
 * ************\n
 *
 * Usage example:-
 *
 *  @code
 *  // initialise\n
 *  ShadowView shadowView = ShadowView::New();
 *
 *  // create and add some visible actors to the ShadowView, all these child actors will therefore cast a shadow.
 *  Image image = Image::New(...);
 *  ImageActor imageActor = ImageActor::New(image);
 *  imageActor.SetParentOrigin( ParentOrigin::CENTER );
 *  imageActor.SetAnchorPoint( AnchorPoint::CENTER );
 *  shadowView.Add(imageActor);\n Add the renderable actor to the shadow view
 *
 *  ImageActor shadowPlane = ImageActor::New(); //This will be the shadow plane
 *  shadowPlane.SetParentOrigin( ParentOrigin::CENTER );
 *  shadowPlane.SetAnchorPoint( AnchorPoint::CENTER );
 *  shadowPlane.SetSize(700.0f, 700.0f);
 *  shadowPlane.SetPosition( Vector3(0.0f, 0.0f, -30.0f) ); //Just behind the image actor.
 *  shadowPlane.SetShadowPlane(ShadowPlane);
 *
 *  Actor pointLight = Actor::New(); // This will be the light source
 *  pointLight.SetPosition(300.0f, 250.0f, 600.0f);
 *  Stage::GetCurrent().Add(pointLight);
 *  shadowView.SetPointLight(pointLight);
 *
 *  // Start rendering the ShadowView
 *  Stage::GetCurrent().Add(ShadowPlane);
 *  shadowView.Activate();
 *  ...
 *
 *  // animate the strength of the blur - this can fade between no blur and full blur. See GetBlurStrengthPropertyIndex().
 *  Animation blurAnimation = Animation::New( ... );
 *  blurAnimation.AnimateTo( Property( shadowView, shadowView.GetBlurStrengthPropertyIndex() ), ... );
 *  blurAnimation.Play();
 *
 *  ...
 *  // Stop rendering the ShadowView
 *  Stage::GetCurrent().Remove(shadowView);
 *  shadowView.Deactivate();
 *  @endcode
 */
class DALI_IMPORT_API ShadowView : public Control
{
public:

  /**
   * Create an uninitialized ShadowView; this can be initialized with ShadowView::New()
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ShadowView();

  /**
   * Copy constructor. Creates another handle that points to the same real object
   */
  ShadowView(const ShadowView& handle);

  /**
   * Assignment operator. Changes this handle to point to another real object
   */
  ShadowView& operator=(const ShadowView& view);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ShadowView();

  /**
   * Downcast an Object handle to ShadowView. If handle points to a ShadowView the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a ShadowView or an uninitialized handle
   */
  static ShadowView DownCast( BaseHandle handle );

  /**
  * Create an initialized ShadowView. Add children and call SetShadowPlane to make shadows visible\n
  * @return A handle to a newly allocated Dali resource
  */
  static ShadowView New();

  /**
   * Create an initialized ShadowView. Add children and call SetShadowPlane to make shadows visible\n
   * @param[in] downsampleWidthScale The width scale factor applied during the blur process, scaling the size of the source image to the size of the final blurred image output.
   * Useful for downsampling - trades visual quality for processing speed. A value of 1.0f results in no scaling applied.
   * @param[in] downsampleHeightScale The height scale factor applied during the blur process, scaling the size of the source image to the size of the final blurred image output.
   * Useful for downsampling - trades visual quality for processing speed. A value of 1.0f results in no scaling applied.
   * @return A handle to a newly allocated Dali resource
   */
  static ShadowView New(float downsampleWidthScale, float downsampleHeightScale);

  /**
   * Set the Shadow Plane Background for the shadow effect.
   *
   * @param[in] shadowPlaneBackground An actor representing the shadow
   * plane. The position of the actor represents the origin of the
   * plane, and the orientation of the actor represents the direction
   * of the plane normal. Make the plane sufficiently large if the shadows are
   * clipped.
   */
  void SetShadowPlaneBackground(Actor shadowPlaneBackground);

  /**
   * Set the Point Light for the shadow effect. This is usually NOT a renderable actor.
   * The orientation of the actor is not considered for the shadow calculation.
   * @param[in] pointLight An actor representing the location of the
   * directionless light source that casts the shadow.
   */
  void SetPointLight(Actor pointLight);

  /**
   * Set the field of view of the point light source. This will be used by an additional
   * internal camera to look at the scene form the light source. If you notice any aritifacts
   * when the light position is near to the object, Increase the field of view.
   * @param[in] fieldOfView  New field of view in radians, Typical values are  Math::PI / 4.0f,
   *  Math::PI / 2.0f
   */
  void SetPointLightFieldOfView(float fieldOfView);

  /**
   * Set shadow color.
   * @param[in] color The shadow color
   */
  void SetShadowColor(Vector4 color);

  /**
   * Start rendering the ShadowView. Must be called after you Add() it to the stage.
   * @pre This Actor has been added to the stage.
   */
  void Activate();

  /**
   * Stop rendering the ShadowView. Must be called after you Remove() it from the stage.
   * @pre This Actor has been removed from the stage.
   */
  void Deactivate();

  /**
   * Get the property index that controls the strength of the blur applied to the shadow. Useful for animating this property.
   * This property represents a value in the range [0.0 - 1.0] where 0.0 is no blur and 1.0 is full blur. Default 0.2.
   * @return The property index that can be used with e.g. AnimateTo( ... )
   */
  Dali::Property::Index GetBlurStrengthPropertyIndex() const;

  /**
   * Get the property index that controls the color of the shadow. Useful for animating this property.
   * This property represents a value in the Vector4 format. Default color value is Vector4(0.2f, 0.2f, 0.2f, 0.8f) (i.e grey color).
   * @return The property index that can be used with e.g. AnimateTo( ... )
   */
  Dali::Property::Index GetShadowColorPropertyIndex() const;


public:

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The UI Control implementation.
   */
  DALI_INTERNAL ShadowView( Internal::ShadowView& implementation );

  /**
   * Allows the creation of this UI Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL ShadowView( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_SHADOW_VIEW_H__
