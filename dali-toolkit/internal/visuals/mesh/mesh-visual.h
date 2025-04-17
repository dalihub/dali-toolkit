#ifndef DALI_TOOLKIT_INTERNAL_MESH_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_MESH_VISUAL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <string.h>
#include <fstream>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/model3d-view/obj-loader.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/public-api/visuals/mesh-visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class MeshVisual;
typedef IntrusivePtr<MeshVisual> MeshVisualPtr;

/**
 * The visual which renders a 3D object to the control's quad
 *
 * The following Property::Map keys are required to create a MeshVisual
 *
 * | %Property Name  | Type        | Representing                                                          |
 * |-----------------|-------------|-----------------------------------------------------------------------|
 * | objectUrl       | STRING      | A URL to the .obj file                                                |
 * | materialUrl     | STRING      | A URL to the .mtl file                                                |
 * | texturesPath    | STRING      | A URL of the path to the texture images                               |
 * | shadingMode     | STRING      | An enum of shading modes                                              |
 * | useMipmapping   | BOOLEAN     | If true, use mipmaps for textures. Default true.                      |
 * | useSoftNormals  | BOOLEAN     | If true, average normals at points for smooth textures. Default true. |
 * | lightPosition   | VECTOR3     | The position (on stage) of the light                                  |
 */
class MeshVisual : public Visual::Base
{
public:
  /**
   * @brief Create a new mesh visual.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] properties A Property::Map containing settings for this visual
   * @return A smart-pointer to the newly allocated visual.
   */
  static MeshVisualPtr New(VisualFactoryCache& factoryCache, const Property::Map& properties);

public: // from Visual
  /**
   * @copydoc Visual::Base::CreatePropertyMap
   */
  void DoCreatePropertyMap(Property::Map& map) const override;

  /**
   * @copydoc Visual::Base::CreateInstancePropertyMap
   */
  void DoCreateInstancePropertyMap(Property::Map& map) const override;

protected:
  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  MeshVisual(VisualFactoryCache& factoryCache);

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~MeshVisual();

  /**
   * @copydoc Visual::Base::OnInitialize
   */
  void OnInitialize() override;

  /**
   * @copydoc Visual::Base::DoSetProperties
   */
  void DoSetProperties(const Property::Map& propertyMap) override;

  /**
   * @copydoc Visual::Base::DoSetOnScene
   */
  void DoSetOnScene(Actor& actor) override;

  /**
   * @copydoc Visual::Base::OnSetTransform
   */
  void OnSetTransform() override;

  /**
   * @copydoc Visual::Base::UpdateShader
   */
  void UpdateShader() override;

private:
  /**
   * @brief Provide an empty geometry for the visual to use.
   * @details For use in error cases where the initialisation has failed for varying reasons.
   */
  void SupplyEmptyGeometry();

  /**
   * @brief Create a shader for the object to use.
   */
  void CreateShader();

  /**
   * @brief Update shader related info, uniforms, etc. for the new shader.
   */
  void UpdateShaderUniforms();

  /**
   * @brief Use the object URL stored in the mesh visual to load and create the geometry of the object.
   * @return Boolean of success of operation.
   */
  bool CreateGeometry();

  /**
   * @brief Use the object URL stored in the visual to load the geometry of the object.
   * @return Boolean of success of operation.
   */
  bool LoadGeometry();

  /**
   * @brief Use the material URL stored in the mesh visual to load the material of the object.
   * @return Boolean of success of operation.
   */
  bool LoadMaterial();

  /**
   * @brief Use the image and texture URL components to load the different types of texture.
   * @return Boolean of success of operation. Returns false if any texture fails to load from a url.
   */
  bool LoadTextures();

  /**
   * Helper method to set individual values by index key.
   * @param[in] index The index key of the value
   * @param[in] value The value
   */
  void DoSetProperty(Property::Index index, const Property::Value& value);

private:
  // Undefined
  MeshVisual(const MeshVisual& meshVisual);

  // Undefined
  MeshVisual& operator=(const MeshVisual& meshVisual);

private:
  std::string mObjectUrl;
  std::string mMaterialUrl;

  std::string mDiffuseTextureUrl;
  std::string mNormalTextureUrl;
  std::string mGlossTextureUrl;
  std::string mTexturesPath;

  Shader     mShader;
  Geometry   mGeometry;
  TextureSet mTextureSet;

  ObjLoader mObjLoader;
  Vector3   mSceneCenter;
  Vector3   mSceneSize;

  Vector3                                 mLightPosition;
  Toolkit::MeshVisual::ShadingMode::Value mShadingMode;

  bool mUseTexture;
  bool mUseMipmapping;
  bool mUseSoftNormals;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_MESH_VISUAL_H */
