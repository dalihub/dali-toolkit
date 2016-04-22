#ifndef __DALI_TOOLKIT_INTERNAL_MODEL3D_VIEW_H__
#define __DALI_TOOLKIT_INTERNAL_MODEL3D_VIEW_H__

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

// EXTERNAL INCLUDES
#include <dali/devel-api/rendering/renderer.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/control-impl.h>
#include <dali-toolkit/public-api/controls/model3d-view/model3d-view.h>
#include <dali-toolkit/internal/controls/model3d-view/obj-loader.h>

namespace Dali
{

namespace Toolkit
{

class Model3dView;

namespace Internal
{
/**
 * @brief Impl class for Model3dView.
 *
 * All the geometry loaded with the control is automatically centered and scaled to fit
 * the size of all the other controls. So the max is (0.5,0.5) and the min is (-0.5,-0.5)
*/
class Model3dView : public Control
{
public:

  /**
   * @brief Create a new Model3dView.
   *
   * @return A public handle to the newly allocated Model3dView.
   */
  static Toolkit::Model3dView New();

  // Properties

  /**
   * @brief Called when a property of an object of this type is set.
   *
   * @param[in] object The object whose property is set.
   * @param[in] index The property index.
   * @param[in] value The new property value.
   */
  static void SetProperty( BaseObject* object, Property::Index index, const Property::Value& value );

  /**
   * @brief Called to retrieve a property of an object of this type.
   *
   * @param[in] object The object whose property is to be retrieved.
   * @param[in] index The property index.
   * @return The current value of the property.
   */
  static Property::Value GetProperty( BaseObject* object, Property::Index index );

  /**
   * @copydoc Control::OnRelayout
   */
  virtual void OnRelayout( const Vector2& size, RelayoutContainer& container );

  /**
   * @brief Called to load both geometry (.obj) and material (.mtl) files
   *
   */
  void Load();

protected:

  /**
   * @brief Construct a new Model3dView.
   */
  Model3dView();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Model3dView();

private:

  /**
   * @copydoc Toolkit::Control::OnInitialize()
   */
  virtual void OnInitialize();

  /**
   * @copydoc CustomActorImpl::OnStageConnection()
   */
  virtual void OnStageConnection( int depth );

private:

  /**
   * @brief Load geometry (.obj) from file
   */
  void LoadGeometry();

  /**
   * @brief Load material (.mtl) from file
   */
  void LoadMaterial();

  /**
   * @brief Create Geometry class from the loaded geometry
   */
  void CreateGeometry();

  /**
   * @brief Create Material and Shader classes from the loaded material
   */
  void CreateMaterial();

  /**
   * @brief Load samplers and add them to Material
   */
  void LoadTextures();

  /**
   * @brief Set matrix to shader to orientate geometry
   */
  void UpdateView();

  /**
   * @brief Update shader uniforms
   */
  void UpdateShaderUniforms();


  ObjLoader mObjLoader;

  //Properties
  std::string mObjUrl;
  std::string mTextureSetUrl;
  std::string mImagesUrl;
  std::string mTexture0Url;
  std::string mTexture1Url;
  std::string mTexture2Url;
  Vector3 mLightPosition;
  float mCameraFOV;
  Toolkit::Model3dView::IlluminationType mIlluminationType;

  //Size
  Vector2 mControlSize;
  Vector3 mSceneCenter;
  Vector3 mSceneSize;

  //Render members
  Shader mShader;
  TextureSet mTextureSet;
  Geometry mMesh;
  Renderer mRenderer;
};

} // namespace Internal

// Helpers for public-api forwarding methods
inline Toolkit::Internal::Model3dView& GetImpl( Toolkit::Model3dView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<Toolkit::Internal::Model3dView&>(handle);
}

inline const Toolkit::Internal::Model3dView& GetImpl( const Toolkit::Model3dView& obj )
{
  DALI_ASSERT_ALWAYS(obj);
  const Dali::RefObject& handle = obj.GetImplementation();
  return static_cast<const Toolkit::Internal::Model3dView&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_MODEL_VIEW_H__
