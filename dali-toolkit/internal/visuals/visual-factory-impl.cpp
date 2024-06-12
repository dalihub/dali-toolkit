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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/visual-factory-impl.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/scripting.h>
#include <dali/integration-api/adaptor-framework/adaptor.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/asset-manager/asset-manager.h>
#include <dali-toolkit/devel-api/styling/style-manager-devel.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-toolkit/internal/visuals/animated-gradient/animated-gradient-visual.h>
#include <dali-toolkit/internal/visuals/animated-image/animated-image-visual.h>
#include <dali-toolkit/internal/visuals/animated-vector-image/animated-vector-image-visual.h>
#include <dali-toolkit/internal/visuals/arc/arc-visual.h>
#include <dali-toolkit/internal/visuals/border/border-visual.h>
#include <dali-toolkit/internal/visuals/color/color-visual.h>
#include <dali-toolkit/internal/visuals/gradient/gradient-visual.h>
#include <dali-toolkit/internal/visuals/image/image-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/image/image-visual.h>
#include <dali-toolkit/internal/visuals/mesh/mesh-visual.h>
#include <dali-toolkit/internal/visuals/npatch/npatch-visual.h>
#include <dali-toolkit/internal/visuals/primitive/primitive-visual.h>
#include <dali-toolkit/internal/visuals/svg/svg-visual.h>
#include <dali-toolkit/internal/visuals/text/text-visual-shader-factory.h>
#include <dali-toolkit/internal/visuals/text/text-visual.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/visual-string-constants.h>
#include <dali-toolkit/internal/visuals/visual-url.h>
#include <dali-toolkit/internal/visuals/wireframe/wireframe-visual.h>
#include <dali-toolkit/public-api/visuals/image-visual-properties.h>
#include <dali-toolkit/public-api/visuals/text-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_CONTROL_VISUALS");
#endif

BaseHandle Create()
{
  BaseHandle handle = Toolkit::VisualFactory::Get();

  return handle;
}

DALI_TYPE_REGISTRATION_BEGIN_CREATE(Toolkit::VisualFactory, Dali::BaseHandle, Create, true)
DALI_TYPE_REGISTRATION_END()
const char* const BROKEN_IMAGE_FILE_NAME = "broken.png"; ///< The file name of the broken image.

static constexpr auto SHADER_TYPE_COUNT = 2u;

constexpr std::string_view VertexPredefines[SHADER_TYPE_COUNT]{
  "",                                     //VisualFactoryCache::COLOR_SHADER
  "#define IS_REQUIRED_ROUNDED_CORNER\n", //VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER
};
constexpr std::string_view FragmentPredefines[SHADER_TYPE_COUNT]{
  "",                                     //VisualFactoryCache::COLOR_SHADER
  "#define IS_REQUIRED_ROUNDED_CORNER\n", //VisualFactoryCache::COLOR_SHADER_ROUNDED_CORNER
};
constexpr VisualFactoryCache::ShaderType ShaderTypePredefines[SHADER_TYPE_COUNT]{
  VisualFactoryCache::ShaderType::COLOR_SHADER,
  VisualFactoryCache::ShaderType::COLOR_SHADER_ROUNDED_CORNER,
};

} // namespace

VisualFactory::VisualFactory(bool debugEnabled)
: mFactoryCache(),
  mImageVisualShaderFactory(),
  mTextVisualShaderFactory(),
  mSlotDelegate(this),
  mIdleCallback(nullptr),
  mDefaultCreationOptions(Toolkit::VisualFactory::CreationOptions::NONE),
  mDebugEnabled(debugEnabled),
  mPreMultiplyOnLoad(true),
  mPrecompiledShaderRequested(false)
{
}

VisualFactory::~VisualFactory()
{
  if(mIdleCallback && Adaptor::IsAvailable())
  {
    // Removes the callback from the callback manager in case the control is destroyed before the callback is executed.
    Adaptor::Get().RemoveIdle(mIdleCallback);
    mIdleCallback = nullptr;
  }
}

void VisualFactory::OnStyleChangedSignal(Toolkit::StyleManager styleManager, StyleChange::Type type)
{
  if(type == StyleChange::THEME_CHANGE)
  {
    SetBrokenImageUrl(styleManager);
  }
}

void VisualFactory::OnBrokenImageChangedSignal(Toolkit::StyleManager styleManager)
{
  SetBrokenImageUrl(styleManager);
}

Toolkit::Visual::Base VisualFactory::CreateVisual(const Property::Map& propertyMap)
{
  return CreateVisual(propertyMap, mDefaultCreationOptions);
}

Toolkit::Visual::Base VisualFactory::CreateVisual(const Property::Map& propertyMap, Toolkit::VisualFactory::CreationOptions creationOptions)
{
  Visual::BasePtr visualPtr;

  Property::Value*           typeValue  = propertyMap.Find(Toolkit::Visual::Property::TYPE, VISUAL_TYPE);
  Toolkit::DevelVisual::Type visualType = Toolkit::DevelVisual::IMAGE; // Default to IMAGE type.
  if(typeValue)
  {
    Scripting::GetEnumerationProperty(*typeValue, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT, visualType);
  }

  switch(visualType)
  {
    case Toolkit::Visual::BORDER:
    {
      visualPtr = BorderVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Toolkit::Visual::COLOR:
    {
      visualPtr = ColorVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Toolkit::Visual::GRADIENT:
    {
      visualPtr = GradientVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Toolkit::Visual::IMAGE:
    case Toolkit::Visual::ANIMATED_IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find(Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue)
      {
        if(imageURLValue->Get(imageUrl))
        {
          if(!imageUrl.empty())
          {
            VisualUrl visualUrl(imageUrl);

            switch(visualUrl.GetType())
            {
              case VisualUrl::N_PATCH:
              {
                visualPtr = NPatchVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap);
                break;
              }
              case VisualUrl::TVG:
              case VisualUrl::SVG:
              {
                visualPtr = SvgVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap);
                break;
              }
              case VisualUrl::JSON:
              {
                visualPtr = AnimatedVectorImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap);
                break;
              }
              case VisualUrl::GIF:
              case VisualUrl::WEBP:
              {
                if(visualType == Toolkit::DevelVisual::ANIMATED_IMAGE || !(creationOptions & Toolkit::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY))
                {
                  visualPtr = AnimatedImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap);
                  break;
                }
                DALI_FALLTHROUGH;
              }
              case VisualUrl::REGULAR_IMAGE:
              {
                visualPtr = ImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, propertyMap);
                break;
              }
            }
          }
        }
        else
        {
          Property::Array* array = imageURLValue->GetArray();
          if(array && array->Count() > 0)
          {
            visualPtr = AnimatedImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), *array, propertyMap);
          }
        }
      }
      break;
    }

    case Toolkit::Visual::MESH:
    {
      visualPtr = MeshVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Toolkit::Visual::PRIMITIVE:
    {
      visualPtr = PrimitiveVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Toolkit::Visual::WIREFRAME:
    {
      visualPtr = WireframeVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Toolkit::Visual::TEXT:
    {
      visualPtr = TextVisual::New(GetFactoryCache(), GetTextVisualShaderFactory(), propertyMap);
      break;
    }

    case Toolkit::Visual::N_PATCH:
    {
      Property::Value* imageURLValue = propertyMap.Find(Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue && imageURLValue->Get(imageUrl))
      {
        if(!imageUrl.empty())
        {
          visualPtr = NPatchVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap);
        }
      }
      break;
    }

    case Toolkit::Visual::SVG:
    {
      Property::Value* imageURLValue = propertyMap.Find(Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue && imageURLValue->Get(imageUrl))
      {
        if(!imageUrl.empty())
        {
          visualPtr = SvgVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap);
        }
      }
      break;
    }

    case Toolkit::DevelVisual::ANIMATED_GRADIENT:
    {
      visualPtr = AnimatedGradientVisual::New(GetFactoryCache(), propertyMap);
      break;
    }

    case Toolkit::DevelVisual::ANIMATED_VECTOR_IMAGE:
    {
      Property::Value* imageURLValue = propertyMap.Find(Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME);
      std::string      imageUrl;
      if(imageURLValue && imageURLValue->Get(imageUrl))
      {
        if(!imageUrl.empty())
        {
          visualPtr = AnimatedVectorImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), imageUrl, propertyMap);
        }
      }
      break;
    }

    case Toolkit::DevelVisual::ARC:
    {
      visualPtr = ArcVisual::New(GetFactoryCache(), propertyMap);
      break;
    }
  }

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "VisualFactory::CreateVisual( VisualType:%s %s%s)\n", Scripting::GetEnumerationName<Toolkit::DevelVisual::Type>(visualType, VISUAL_TYPE_TABLE, VISUAL_TYPE_TABLE_COUNT), (visualType == Toolkit::DevelVisual::IMAGE) ? "url:" : "", (visualType == Toolkit::DevelVisual::IMAGE) ? (([&]() {
                  // Return URL if present in PropertyMap else return "not found message"
                  Property::Value* imageURLValue = propertyMap.Find(Toolkit::ImageVisual::Property::URL, IMAGE_URL_NAME);
                  return (imageURLValue) ? imageURLValue->Get<std::string>().c_str() : "url not found in PropertyMap";
                })())
                                                                                                                                                                                                                                                                                                                            : "");

  if(!visualPtr)
  {
    DALI_LOG_ERROR("VisualType unknown\n");
  }

  if(mDebugEnabled && visualType != Toolkit::DevelVisual::WIREFRAME)
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New(GetFactoryCache(), visualPtr, propertyMap);
  }

  return Toolkit::Visual::Base(visualPtr.Get());
}

Toolkit::Visual::Base VisualFactory::CreateVisual(const std::string& url, ImageDimensions size)
{
  return CreateVisual(url, size, mDefaultCreationOptions);
}

Toolkit::Visual::Base VisualFactory::CreateVisual(const std::string& url, ImageDimensions size, Toolkit::VisualFactory::CreationOptions creationOptions)
{
  Visual::BasePtr visualPtr;

  if(!url.empty())
  {
    // first resolve url type to know which visual to create
    VisualUrl visualUrl(url);
    switch(visualUrl.GetType())
    {
      case VisualUrl::N_PATCH:
      {
        visualPtr = NPatchVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl);
        break;
      }
      case VisualUrl::TVG:
      case VisualUrl::SVG:
      {
        visualPtr = SvgVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
        break;
      }
      case VisualUrl::JSON:
      {
        visualPtr = AnimatedVectorImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
        break;
      }
      case VisualUrl::GIF:
      case VisualUrl::WEBP:
      {
        if(!(creationOptions & Toolkit::VisualFactory::CreationOptions::IMAGE_VISUAL_LOAD_STATIC_IMAGES_ONLY))
        {
          visualPtr = AnimatedImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
          break;
        }
        DALI_FALLTHROUGH;
      }
      case VisualUrl::REGULAR_IMAGE:
      {
        visualPtr = ImageVisual::New(GetFactoryCache(), GetImageVisualShaderFactory(), visualUrl, size);
        break;
      }
    }
  }

  if(mDebugEnabled)
  {
    //Create a WireframeVisual if we have debug enabled
    visualPtr = WireframeVisual::New(GetFactoryCache(), visualPtr);
  }

  return Toolkit::Visual::Base(visualPtr.Get());
}

void VisualFactory::SetPreMultiplyOnLoad(bool preMultiply)
{
  if(mPreMultiplyOnLoad != preMultiply)
  {
    GetFactoryCache().SetPreMultiplyOnLoad(preMultiply);
  }
  mPreMultiplyOnLoad = preMultiply;
}

bool VisualFactory::GetPreMultiplyOnLoad() const
{
  return mPreMultiplyOnLoad;
}

void VisualFactory::SetDefaultCreationOptions(Toolkit::VisualFactory::CreationOptions creationOptions)
{
  mDefaultCreationOptions = creationOptions;
}

Toolkit::VisualFactory::CreationOptions VisualFactory::GetDefaultCreationOptions() const
{
  return mDefaultCreationOptions;
}

void VisualFactory::DiscardVisual(Toolkit::Visual::Base visual)
{
  mDiscardedVisuals.emplace_back(visual);

  RegisterDiscardCallback();
}

void VisualFactory::UsePreCompiledShader()
{
  if(mPrecompiledShaderRequested)
  {
    return;
  }
  mPrecompiledShaderRequested = true;

  ShaderPreCompiler::Get().Enable();

  // Get image shader
  std::vector<RawShaderData> rawShaderList;
  RawShaderData              imageShaderData;
  GetImageVisualShaderFactory().GetPreCompiledShader(imageShaderData);
  rawShaderList.push_back(imageShaderData);

  // Get text shader
  RawShaderData textShaderData;
  GetTextVisualShaderFactory().GetPreCompiledShader(textShaderData);
  rawShaderList.push_back(textShaderData);

  // Get color shader
  RawShaderData colorShaderData;
  GetPreCompiledShader(colorShaderData);
  rawShaderList.push_back(colorShaderData);

  // Save all shader
  ShaderPreCompiler::Get().SavePreCompileShaderList(rawShaderList);
}

Internal::TextureManager& VisualFactory::GetTextureManager()
{
  return GetFactoryCache().GetTextureManager();
}

void VisualFactory::SetBrokenImageUrl(Toolkit::StyleManager& styleManager)
{
  const std::string        imageDirPath   = AssetManager::GetDaliImagePath();
  std::string              brokenImageUrl = imageDirPath + BROKEN_IMAGE_FILE_NAME;
  std::vector<std::string> customBrokenImageUrlList;

  if(styleManager)
  {
    customBrokenImageUrlList       = Toolkit::DevelStyleManager::GetBrokenImageUrlList(styleManager);
    const auto brokenImageUrlValue = Toolkit::DevelStyleManager::GetConfigurations(styleManager).Find("brokenImageUrl", Property::Type::STRING);
    if(brokenImageUrlValue)
    {
      brokenImageUrlValue->Get(brokenImageUrl);
    }
  }

  // Add default image
  mFactoryCache->SetBrokenImageUrl(brokenImageUrl, customBrokenImageUrlList);
}

void VisualFactory::GetPreCompiledShader(RawShaderData& shaders)
{
  std::vector<std::string_view> vertexPrefix;
  std::vector<std::string_view> fragmentPrefix;
  std::vector<std::string_view> shaderName;
  int                           shaderCount = 0;
  shaders.shaderCount                       = 0;
  for(uint32_t i = 0u; i < SHADER_TYPE_COUNT; ++i)
  {
    vertexPrefix.push_back(VertexPredefines[i]);
    fragmentPrefix.push_back(FragmentPredefines[i]);
    shaderName.push_back(Scripting::GetLinearEnumerationName<VisualFactoryCache::ShaderType>(ShaderTypePredefines[i], VISUAL_SHADER_TYPE_TABLE, VISUAL_SHADER_TYPE_TABLE_COUNT));
    shaderCount++;
  }

  shaders.vertexPrefix   = vertexPrefix;
  shaders.fragmentPrefix = fragmentPrefix;
  shaders.shaderName     = shaderName;
  shaders.vertexShader   = SHADER_COLOR_VISUAL_SHADER_VERT;
  shaders.fragmentShader = SHADER_COLOR_VISUAL_SHADER_FRAG;
  shaders.shaderCount    = shaderCount;
}

Internal::VisualFactoryCache& VisualFactory::GetFactoryCache()
{
  if(!mFactoryCache)
  {
    mFactoryCache                      = std::unique_ptr<VisualFactoryCache>(new VisualFactoryCache(mPreMultiplyOnLoad));
    Toolkit::StyleManager styleManager = Toolkit::StyleManager::Get();
    if(styleManager)
    {
      styleManager.StyleChangedSignal().Connect(mSlotDelegate, &VisualFactory::OnStyleChangedSignal);
      Toolkit::DevelStyleManager::BrokenImageChangedSignal(styleManager).Connect(mSlotDelegate, &VisualFactory::OnBrokenImageChangedSignal);
    }
    SetBrokenImageUrl(styleManager);
  }

  return *mFactoryCache;
}

ImageVisualShaderFactory& VisualFactory::GetImageVisualShaderFactory()
{
  if(!mImageVisualShaderFactory)
  {
    mImageVisualShaderFactory = std::unique_ptr<ImageVisualShaderFactory>(new ImageVisualShaderFactory());
  }
  return *mImageVisualShaderFactory;
}

TextVisualShaderFactory& VisualFactory::GetTextVisualShaderFactory()
{
  if(!mTextVisualShaderFactory)
  {
    mTextVisualShaderFactory = std::unique_ptr<TextVisualShaderFactory>(new TextVisualShaderFactory());
  }
  return *mTextVisualShaderFactory;
}

void VisualFactory::OnDiscardCallback()
{
  mIdleCallback = nullptr;

  // Discard visual now.
  mDiscardedVisuals.clear();
}

void VisualFactory::RegisterDiscardCallback()
{
  if(!mIdleCallback && Adaptor::IsAvailable())
  {
    // The callback manager takes the ownership of the callback object.
    mIdleCallback = MakeCallback(this, &VisualFactory::OnDiscardCallback);

    Adaptor& adaptor = Adaptor::Get();

    if(DALI_UNLIKELY(!adaptor.AddIdle(mIdleCallback, false)))
    {
      DALI_LOG_ERROR("Fail to add idle callback for visual factory. Call it synchronously.\n");
      OnDiscardCallback();
    }
  }
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
