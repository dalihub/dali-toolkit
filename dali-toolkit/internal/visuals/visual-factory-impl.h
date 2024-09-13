#ifndef DALI_TOOLKIT_VISUAL_FACTORY_IMPL_H
#define DALI_TOOLKIT_VISUAL_FACTORY_IMPL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <dali/integration-api/adaptor-framework/shader-precompiler.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/base-object.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/internal/visuals/visual-base-impl.h>
#include <dali-toolkit/public-api/styling/style-manager.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
class VisualFactoryCache;
class ImageVisualShaderFactory;
class TextVisualShaderFactory;
class ColorVisualShaderFactory;

/**
 * @copydoc Toolkit::VisualFactory
 */
class VisualFactory : public BaseObject, public ConnectionTracker
{
public:
  /**
   * @brief Constructor
   *
   * @param[in] debugEnabled If true, use debug renderer to replace all the concrete renderer.
   */
  VisualFactory(bool debugEnabled);

  /**
   * @brief StyleChanged callback
   *
   * @param[in] styleManager Handle for style manager.
   * @param[in] type Style change type.
   */
  void OnStyleChangedSignal(Toolkit::StyleManager styleManager, StyleChange::Type type);

  /**
   * @brief BrokenImageChanged callback
   *
   * @param[in] styleManager Handle for style manager.
   */
  void OnBrokenImageChangedSignal(Toolkit::StyleManager styleManager);

  /**
   * @copydoc Toolkit::VisualFactory::CreateVisual( const Property::Map& )
   */
  Toolkit::Visual::Base CreateVisual(const Property::Map& propertyMap);

  /**
   * @copydoc Toolkit::VisualFactory::CreateVisual( const Property::Map&, Toolkit::VisualFactory::CreationOptions )
   */
  Toolkit::Visual::Base CreateVisual(const Property::Map& propertyMap, Toolkit::VisualFactory::CreationOptions creationOptions);

  /**
   * @copydoc Toolkit::VisualFactory::CreateVisual( const std::string&, ImageDimensions )
   */
  Toolkit::Visual::Base CreateVisual(const std::string& image, ImageDimensions size);

  /**
   * @copydoc Toolkit::VisualFactory::CreateVisual( const std::string&, ImageDimensions, Toolkit::VisualFactory::CreationOptions )
   */
  Toolkit::Visual::Base CreateVisual(const std::string& image, ImageDimensions size, Toolkit::VisualFactory::CreationOptions creationOptions);

  /**
   * @copydoc Toolkit::VisualFactory::SetPreMultiplyOnLoad()
   */
  void SetPreMultiplyOnLoad(bool preMultiply);

  /**
   * @copydoc Toolkit::VisualFactory::GetPreMultiplyOnLoad()
   */
  bool GetPreMultiplyOnLoad() const;

  /**
   * @copydoc Toolkit::VisualFactory::SetDefaultCreationOptions( Toolkit::VisualFactory::CreationOptions )
   */
  void SetDefaultCreationOptions(Toolkit::VisualFactory::CreationOptions creationOptions);

  /**
   * @copydoc Toolkit::VisualFactory::GetDefaultCreationOptions()
   */
  Toolkit::VisualFactory::CreationOptions GetDefaultCreationOptions() const;

  /**
   * @copydoc Toolkit::VisualFactory::DiscardVisual()
   */
  void DiscardVisual(Toolkit::Visual::Base visual);

  /**
   * @copydoc Toolkit::VisualFactory::UsePreCompiledShader()
   */
  void UsePreCompiledShader();

  /**
   * @return the reference to texture manager
   */
  Internal::TextureManager& GetTextureManager();

  /**
   * @return the reference to svg loader
   */
  Internal::SvgLoader& GetSvgLoader();

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~VisualFactory() override;

private:
  /**
   * @brief Set the Broken Image url
   * @param[in] styleManager The instance of StyleManager
   */
  void SetBrokenImageUrl(Toolkit::StyleManager& styleManager);

  /**
   * Get the factory cache, creating it if necessary.
   */
  Internal::VisualFactoryCache& GetFactoryCache();

  /**
   * Get the image visual shader factory, creating it if necessary.
   */
  ImageVisualShaderFactory& GetImageVisualShaderFactory();

  /**
   * Get the text visual shader factory, creating it if necessary.
   */
  TextVisualShaderFactory& GetTextVisualShaderFactory();

  /**
   * Get the color visual shader factory, creating it if necessary.
   */
  ColorVisualShaderFactory& GetColorVisualShaderFactory();

  /**
   * @brief Callbacks called for clear discarded visuals.
   */
  void OnDiscardCallback();

  /**
   * @brief Register idle callback for discard visuals if need.
   */
  void RegisterDiscardCallback();

  /**
   * @brief Callbacks called when application is terminated.
   */
  void OnApplicationTerminated();

  VisualFactory(const VisualFactory&) = delete;

  VisualFactory& operator=(const VisualFactory& rhs) = delete;

private:
  std::unique_ptr<VisualFactoryCache>       mFactoryCache;
  std::unique_ptr<ImageVisualShaderFactory> mImageVisualShaderFactory;
  std::unique_ptr<TextVisualShaderFactory>  mTextVisualShaderFactory;
  std::unique_ptr<ColorVisualShaderFactory> mColorVisualShaderFactory;
  SlotDelegate<VisualFactory>               mSlotDelegate;
  CallbackBase*                             mIdleCallback;
  using DiscardedVisualContainer = std::vector<Toolkit::Visual::Base>;
  DiscardedVisualContainer mDiscardedVisuals{};

  Toolkit::VisualFactory::CreationOptions mDefaultCreationOptions : 2;

  bool mDebugEnabled : 1;
  bool mPreMultiplyOnLoad : 1; ///< Local store for this flag
  bool mPrecompiledShaderRequested : 1;
};

/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType0 The type of first argument passed to the CreateVisual()
 * @tparam ParameterType1 The type of second argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the visual will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param0 First template based argument passed to the visual factory
 * @param[in] param1 Second template based argument passed to the visual factory
 */
template<class ParameterType0, class ParameterType1>
void InitializeVisual(Actor& actor, Toolkit::Visual::Base& visual, ParameterType0& param0, ParameterType1& param1)
{
  if(actor)
  {
    Toolkit::GetImplementation(visual).SetOffScene(actor);
  }
  visual = Toolkit::VisualFactory::Get().CreateVisual(param0, param1);
  if(visual && actor && actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::GetImplementation(visual).SetOnScene(actor);
  }
}

/**
 * @brief Template to allow discard old visual, get new one and set it on stage if possible
 *
 * @tparam ParameterType The type of argument passed to the CreateVisual()
 * @SINCE_1_0.39
 * @param[in] actor Actor for which the visual will be replaced
 * @param[in,out] visual The visual to be replaced
 * @param[in] param Template based argument passed to the visual factory
 */
template<class ParameterType>
void InitializeVisual(Actor& actor, Toolkit::Visual::Base& visual, ParameterType& param)
{
  if(actor && visual)
  {
    Toolkit::GetImplementation(visual).SetOffScene(actor);
  }
  visual = Toolkit::VisualFactory::Get().CreateVisual(param);
  if(visual && actor && actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE))
  {
    Toolkit::GetImplementation(visual).SetOnScene(actor);
  }
}

} // namespace Internal

inline const Internal::VisualFactory& GetImplementation(const Toolkit::VisualFactory& factory)
{
  DALI_ASSERT_ALWAYS(factory && "VisualFactory handle is empty");

  const BaseObject& handle = factory.GetBaseObject();

  return static_cast<const Internal::VisualFactory&>(handle);
}

inline Internal::VisualFactory& GetImplementation(Toolkit::VisualFactory& factory)
{
  DALI_ASSERT_ALWAYS(factory && "VisualFactory handle is empty");

  BaseObject& handle = factory.GetBaseObject();

  return static_cast<Internal::VisualFactory&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_VISUAL_FACTORY_IMPL_H */
