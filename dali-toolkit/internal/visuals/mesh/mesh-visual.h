#ifndef DALI_TOOLKIT_INTERNAL_MESH_VISUAL_H
#define DALI_TOOLKIT_INTERNAL_MESH_VISUAL_H

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

// EXTERNAL INCLUDES
#include <fstream>
#include <string.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/visuals/visual-impl.h>
#include <dali-toolkit/internal/controls/model3d-view/obj-loader.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The renderer which renders a 3D object to the control's quad
 *
 * The following Property::Map keys are required to create a MeshRender
 *
 * | %Property Name  | Type        | Representing                                                          |
 * |-----------------|-------------|-----------------------------------------------------------------------|
 * | objectUrl       | STRING      | A URL to the .obj file                                                |
 * | materialUrl     | STRING      | A URL to the .mtl file                                                |
 * | texturesPath    | STRING      | A URL of the path to the texture images                               |
 * | shaderType      | STRING      | An enum of shader types                                               |
 * | useMipmapping   | BOOLEAN     | If true, use mipmaps for textures. Default true.                      |
 * | useSoftNormals  | BOOLEAN     | If true, average normals at points for smooth textures. Default true. |
 * | lightPosition   | VECTOR3     | The position (on stage) of the light                                  |
 */
class MeshVisual: public Visual
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   */
  MeshVisual( VisualFactoryCache& factoryCache );

  /**
   * @brief A reference counted object may only be deleted by calling Unreference().
   */
  virtual ~MeshVisual();

public:  // from Visual

  /**
   * @copydoc Visual::SetSize
   */
  virtual void SetSize( const Vector2& size );

  /**
   * @copydoc Visual::SetClipRect
   */
  virtual void SetClipRect( const Rect<int>& clipRect );

  /**
   * @copydoc Visual::SetOffset
   */
  virtual void SetOffset( const Vector2& offset );

  /**
   * @copydoc Visual::CreatePropertyMap
   */
  virtual void DoCreatePropertyMap( Property::Map& map ) const;

protected:

  /**
   * @copydoc Visual::DoInitialize
   */
  virtual void DoInitialize( Actor& actor, const Property::Map& propertyMap );

  /**
   * @copydoc Visual::DoSetOnStage
   */
  virtual void DoSetOnStage( Actor& actor );

public:

  /**
   * Declare whether a texture map should be used for the object, if it's present. Defaults to true.
   * @param[in] useTexture boolean declaration.
   */
  void SetUseTexture( bool useTexture );

  /**
   * Declare whether a normal map should be used for the object, if it's present. Defaults to true.
   * @param[in] useNormalMap boolean declaration.
   */
  void SetUseNormalMap( bool useNormalMap );

private:

  //Corresponds to the shader that will be used by the mesh renderer.
  enum ShaderType
  {
    TEXTURELESS,
    DIFFUSE_TEXTURE,
    ALL_TEXTURES
  };

  /**
   * @brief Provide an empty geometry for the renderer to use.
   * @details For use in error cases where the initialisation has failed for varying reasons.
   */
  void SupplyEmptyGeometry();

  /**
   * @brief Initialize the renderer with the geometry and shader from the cache, if not available, create and save to the cache for sharing.
   */
  void InitializeRenderer();

  /**
   * @brief Create a shader for the object to use.
   */
  void CreateShader();

  /**
   * @brief Update shader related info, uniforms, etc. for the new shader.
   */
  void UpdateShaderUniforms();

  /**
   * @brief Use the object URL stored in the renderer to load and create the geometry of the object.
   * @return Boolean of success of operation.
   */
  bool CreateGeometry();

  /**
   * @brief Use the object URL stored in the renderer to load the geometry of the object.
   * @return Boolean of success of operation.
   */
  bool LoadGeometry();

  /**
   * @brief Use the material URL stored in the renderer to load the material of the object.
   * @return Boolean of success of operation.
   */
  bool LoadMaterial();

  /**
   * @brief Use the image and texture URL components to load the different types of texture.
   * @return Boolean of success of operation. Returns false if any texture fails to load from a url.
   */
  bool LoadTextures();

private:

  // Undefined
  MeshVisual( const MeshVisual& meshRenderer );

  // Undefined
  MeshVisual& operator=( const MeshVisual& meshRenderer );

private:

  std::string mObjectUrl;
  std::string mMaterialUrl;

  std::string mDiffuseTextureUrl;
  std::string mNormalTextureUrl;
  std::string mGlossTextureUrl;
  std::string mTexturesPath;

  Shader mShader;
  Geometry mGeometry;
  TextureSet mTextureSet;

  ObjLoader mObjLoader;
  Vector3 mSceneCenter;
  Vector3 mSceneSize;

  Vector3 mLightPosition;
  ShaderType mShaderType;

  bool mUseTexture;
  bool mUseMipmapping;
  bool mUseSoftNormals;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_INTERNAL_MESH_VISUAL_H */
