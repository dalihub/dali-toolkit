#ifndef __DALI_TOOLKIT_INTERNAL_IMAGE_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_IMAGE_VIEW_H__

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
#include <dali/devel-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/image-view/image-view.h>

namespace Dali
{

namespace Toolkit
{

class ImageView;

namespace Internal
{
class ImageView : public Control
{
 protected:

  /**
   * Construct a new ImageView.
   */
  ImageView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageView();

public:
  /**
   * Create a new ImageView.
   * @return A smart-pointer to the newly allocated ImageView.
   */
  static Toolkit::ImageView New();

  /**
   * @copydoc Dali::Toolkit::SetImage( Image image )
   */
  void SetImage( Image image );

  /**
   * @copydoc Dali::Toolkit::Image GetImage() const
   */
  Image GetImage() const;

  // Properties
  /**
   * Called when a property of an object of this type is set.
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * Called to retrieve a property of an object of this type.
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index propertyIndex );

private: // From Control

  /**
   * @copydoc Toolkit::Control::OnRelayout()
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @copydoc Toolkit::Control::OnStageConnect()
   */
  virtual void OnStageConnection( int depth );

  /**
   * @copydoc Toolkit::Control::GetNaturalSize
   */
  virtual Vector3 GetNaturalSize();

  /**
   * @copydoc Toolkit::Control::GetHeightForWidth()
   */
  virtual float GetHeightForWidth( float width );

  /**
   * @copydoc Toolkit::Control::GetWidthForHeight()
   */
  virtual float GetWidthForHeight( float height );

private:
  /**
   * Attaches mImage member to the renderer, creating the renderers, samplers, meshes and materials if needed
   *
   * @pre mImage has been initialised
   */
  void AttachImage();

private:

  Sampler mSampler;
  Material mMaterial;
  Geometry mMesh;
  Renderer mRenderer;
  Image mImage;
  std::string mImageUrl;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Toolkit::Internal::ImageView& GetImpl( Toolkit::ImageView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::ImageView&>(handle);
}

inline const Toolkit::Internal::ImageView& GetImpl( const Toolkit::ImageView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::ImageView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_IMAGE_VIEW_H__
