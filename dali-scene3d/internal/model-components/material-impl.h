#ifndef DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_IMPL_H
#define DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_IMPL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/image-loader/async-image-loader.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-value.h>
#include <dali/public-api/object/property.h>
#include <dali/public-api/rendering/texture.h>
#include <set>
#include <utility>

// INTERNAL INCLUDES
#include <dali-scene3d/internal/model-components/material-modify-observer.h>
#include <dali-scene3d/public-api/loader/material-definition.h>
#include <dali-scene3d/public-api/loader/shader-definition.h>
#include <dali-scene3d/public-api/loader/shader-option.h>
#include <dali-scene3d/public-api/model-components/material.h>

namespace Dali
{
namespace Scene3D
{
namespace Internal
{
using MaterialPtr = IntrusivePtr<Material>;

/**
 * @brief This is the internal base class for Material of model.
 *
 * @SINCE_2_2.99
 */
class Material : public BaseObject, public ConnectionTracker
{
private:
  using ObserverContainer = std::vector<std::pair<MaterialModifyObserver*, bool>>;

public:
  /**
   * @brief Pair of texture handle and it's own scale factor. Texture handle can be empty
   */
  struct TextureInformation
  {
    bool IsReady()
    {
      return mLoadingTaskId == 0u;
    }

    std::string                         mUrl;
    Dali::Texture                       mTexture;
    Vector4                             mFactor{Vector4::ONE};
    Dali::Sampler                       mSampler;
    uint32_t                            mLoadingTaskId{0u};
    uint32_t                            mSemantic;
    Scene3D::Loader::ShaderOption::Type mShaderOptionType;
  };

  using TextureInformationContainer = std::vector<TextureInformation>;

public:
  // Creation & Destruction
  /**
   * @brief Create a new Material object.
   * @return A smart-pointer to the newly allocated Material.
   */
  static MaterialPtr New();

protected:
  /**
   * @brief Construct a new Material.
   */
  Material();

  /**
   * @brief Second-phase constructor.
   */
  void Initialize();

  /**
   * @brief Virtual destructor.
   */
  virtual ~Material();

public:
  /**
   * @copydoc Dali::Scene3D::Material::SetProperty()
   */
  void SetProperty(Dali::Property::Index index, Dali::Property::Value propertyValue);

  /**
   * @copydoc Dali::Scene3D::Material::GetProperty()
   */
  Dali::Property::Value GetProperty(Dali::Property::Index index) const;

  /**
   * @brief Sets a texture information for the material.
   *
   * @param[in] index The index of the texture to set.
   * @param[in] textureInformation The texture information to set.
   *
   * @note This function moves the value of textureInformation.
   */
  void SetTextureInformation(Scene3D::Material::TextureType index, TextureInformation&& textureInformation);

  /**
   * @brief Sets a texture for the material.
   *
   * @param[in] index The index of the texture to set.
   * @param[in] texture The texture to set.
   */
  void SetTexture(Scene3D::Material::TextureType index, Dali::Texture texture);

  /**
   * @brief Retrieves texture for the material.
   *
   * @param[in] index The index of the texture to get.
   *
   * @return The texture at the given index.
   */
  Dali::Texture GetTexture(Scene3D::Material::TextureType index);

  /**
   * @brief Retrieves the texture set for this material.
   *
   * @return The texture set for this material.
   */
  TextureSet GetTextureSet();

  /**
   * @brief Sets a sampler for the material.
   *
   * @param[in] index The index of the sampler to set.
   * @param[in] sampler The sampler to set.
   */
  void SetSampler(Scene3D::Material::TextureType index, Dali::Sampler sampler);

  /**
   * @brief Retrieves a sampler for the material.
   *
   * @param[in] index The index of the sampler to get.
   * @return The sampler at the given index.
   */
  Dali::Sampler GetSampler(Scene3D::Material::TextureType index);

  /**
   * @brief Retrieves Shader Option of this Material.
   *
   * @return Shader Option of this Material.
   */
  Scene3D::Loader::ShaderOption GetShaderOption() const;

public:
  /**
   * @brief Adds observer to this material.
   *
   * @param[in] observer Pointer of observer.
   */
  void AddObserver(MaterialModifyObserver* observer);

  /**
   * @brief Removes observer from this material.
   *
   * @param[in] observer Pointer of observer.
   */
  void RemoveObserver(MaterialModifyObserver* observer);

  /**
   * @brief Updates material data.
   */
  void UpdateMaterialData();

  /**
   * @brief Sets uniform value to the Renderer.
   *
   * @param[in] renderer Renderer object.
   */
  void SetRendererUniform(Dali::Renderer renderer);

  /**
   * @brief Retrieves shadow map texture offset.
   *
   * @return shadow map texture offset.
   */
  uint32_t GetShadowMapTextureOffset();

  /**
   * @brief Retrieves specular image based light texture offset.
   *
   * @return Specular image based light texture offset.
   */
  uint32_t GetSpecularImageBasedLightTextureOffset();

  /**
   * @brief Retrieves diffuse image based light texture offset.
   *
   * @return Diffuse image based light texture offset.
   */
  uint32_t GetDiffuseImageBasedLightTextureOffset();

  /**
   * @brief Retrieves image based light scale factor name.
   *
   * @return Image based light scale factor name.
   */
  std::string_view GetImageBasedLightScaleFactorName();

  /**
   * @brief Retrieves image based light max lod uniform name.
   *
   * @return Image based light max lod uniform name.
   */
  std::string_view GetImageBasedLightMaxLodUniformName();

  /**
   * @brief Checks if resource is ready.
   *
   * @return True if resource is ready, false otherwise.
   */
  bool IsResourceReady();

  /**
   * @brief Resets dirty flag of this material.
   */
  void ResetFlag();

private:
  /**
   * @brief Checks modify flag and send observers the material changeness.
   * It will clean up modify flag
   */
  void NotifyObserver();

  /**
   * @brief Requests loading an image from a URL and store it in TextureInformation.
   *
   * @param[in] textureInformation TextureInformation object to store loaded texture information.
   * @param[in] url URL of the image to load.
   */
  void RequestTextureLoad(TextureInformation& textureInformation, const std::string& url);

  /**
   * @brief Called when loading requested by RequestTextureLoad is complete.
   *
   * @param[in] loadedTaskId ID of the loaded texture.
   * @param[in] pixelData PixelData of the loaded texture.
   */
  void TextureLoadComplete(uint32_t loadedTaskId, PixelData pixelData);

  /**
   * @brief Called when all requested textures are loaded.
   */
  void ResourcesLoadComplete();

  /**
   * @brief Updates the material using each attribute of this material and send a notification to the ModelPrimitive class.
   */
  void Apply();

private:
  // Delete copy & move operator
  Material(const Material&)                = delete;
  Material(Material&&)                     = delete;
  Material& operator=(const Material& rhs) = delete;
  Material& operator=(Material&& rhs)      = delete;

private:
  ObserverContainer mObservers{}; ///< List of observers who need to be notified after some properties are changed.

  TextureInformationContainer     mTextureInformations;
  Dali::Toolkit::AsyncImageLoader mAsyncImageLoader;

  std::string                            mName;                                                   ///< Material name
  Dali::Scene3D::Material::AlphaModeType mAlphaMode   = Scene3D::Material::AlphaModeType::OPAQUE; ///< Alpha mode
  float                                  mAlphaCutoff = 0.5f;                                     ///< Alpha cutoff value
  bool                                   mDoubleSided = false;                                    ///< Whether to render both sides
  float                                  mIor         = -1.0f;                                    ///< Index of refraction (TODO: Magic number)
  MaterialModifyObserver::ModifyFlag     mModifyFlag  = MaterialModifyObserver::ModifyFlag::NONE; ///< Modified dirty flags

  Scene3D::Loader::ShaderOption        mShaderOption;
  uint32_t                             mMaterialFlag  = std::numeric_limits<uint32_t>::max();
  Scene3D::Loader::RendererState::Type mRendererState = Scene3D::Loader::RendererState::NONE;

  bool mIsOpaque = true;
  bool mIsMask   = false;
  bool mObserverNotifying; ///< True if observe is notify now. If then, we should not change the mObservers.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Material& GetImplementation(Dali::Scene3D::Material& material)
{
  DALI_ASSERT_ALWAYS(material && "Material handle is empty");

  BaseObject& handle = material.GetBaseObject();

  return static_cast<Internal::Material&>(handle);
}

inline const Internal::Material& GetImplementation(const Dali::Scene3D::Material& material)
{
  DALI_ASSERT_ALWAYS(material && "Material handle is empty");

  const BaseObject& handle = material.GetBaseObject();

  return static_cast<const Internal::Material&>(handle);
}

} // namespace Scene3D

} // namespace Dali

#endif // DALI_SCENE3D_MODEL_COMPONENTS_MATERIAL_IMPL_H
