/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/internal/loader/gltf2-asset.h>
#include <dali-scene3d/internal/loader/json-reader.h>
#include <dali-scene3d/public-api/loader/load-scene-metadata.h>
#include <dali-scene3d/public-api/loader/utils.h>

namespace js = json;
namespace gt = gltf2;

namespace Dali::Scene3D::Loader
{
namespace
{
struct ImageData
{
  struct SamplingMode
  {
    enum Type
    {
      INVALID          = -1,
      BOX              = Dali::SamplingMode::BOX,
      NEAREST          = Dali::SamplingMode::NEAREST,
      LINEAR           = Dali::SamplingMode::LINEAR,
      BOX_THEN_NEAREST = Dali::SamplingMode::BOX_THEN_NEAREST,
      BOX_THEN_LINEAR  = Dali::SamplingMode::BOX_THEN_LINEAR,
      NO_FILTER        = Dali::SamplingMode::NO_FILTER,
      DONT_CARE        = Dali::SamplingMode::DONT_CARE
    };

    static Type FromString(const char* s, size_t len);
  };

  std::string mImageUri{};  ///< The URI of the image
  uint16_t    mMinWidth{};  ///< The minimum width of the image
  uint16_t    mMinHeight{}; ///< The mimimum height of the image

  ImageData::SamplingMode::Type mSamplingMode{ImageData::SamplingMode::BOX_THEN_LINEAR}; ///< The sampling mode used to resize the image.
};

const std::map<std::string_view, ImageData::SamplingMode::Type>& GetStringSamplingModeTable()
{
  static const std::map<std::string_view, ImageData::SamplingMode::Type> SAMPLING_MODE_TYPES{
    ENUM_STRING_MAPPING(ImageData::SamplingMode, BOX),
    ENUM_STRING_MAPPING(ImageData::SamplingMode, NEAREST),
    ENUM_STRING_MAPPING(ImageData::SamplingMode, LINEAR),
    ENUM_STRING_MAPPING(ImageData::SamplingMode, BOX_THEN_NEAREST),
    ENUM_STRING_MAPPING(ImageData::SamplingMode, BOX_THEN_LINEAR),
    ENUM_STRING_MAPPING(ImageData::SamplingMode, NO_FILTER),
    ENUM_STRING_MAPPING(ImageData::SamplingMode, DONT_CARE),
  };
  return SAMPLING_MODE_TYPES;
}

ENUM_TYPE_FROM_STRING(ImageData::SamplingMode, GetStringSamplingModeTable())

struct MetaData
{
  std::vector<ImageData> mImageData;
};

const js::Reader<ImageData>& GetImageMetaDataReader()
{
  static const auto IMAGE_METADATA_READER = std::move(js::Reader<ImageData>()
                                                         .Register(*js::MakeProperty("uri", js::Read::String, &ImageData::mImageUri))
                                                         .Register(*js::MakeProperty("minWidth", js::Read::Number, &ImageData::mMinWidth))
                                                         .Register(*js::MakeProperty("minHeight", js::Read::Number, &ImageData::mMinHeight))
                                                         .Register(*js::MakeProperty("samplingMode", gt::ReadStringEnum<ImageData::SamplingMode>, &ImageData::mSamplingMode)));
  return IMAGE_METADATA_READER;
}

const js::Reader<MetaData>& GetMetaDataReader()
{
  static const auto METADATA_READER = std::move(js::Reader<MetaData>()
                                                   .Register(*js::MakeProperty("images", js::Read::Array<ImageData, js::ObjectReader<ImageData>::Read>, &MetaData::mImageData)));
  return METADATA_READER;
}
} // namespace

void LoadSceneMetadata(const std::string& url, SceneMetadata& sceneMetadata)
{
  bool failed = false;
  auto js     = LoadTextFile(url.c_str(), &failed);
  if(failed)
  {
    return;
  }

  json::unique_ptr root(json_parse(js.c_str(), js.size()));
  if(!root)
  {
    throw std::runtime_error("Failed to parse " + url);
  }
  auto& rootObj = js::Cast<json_object_s>(*root);

  static bool setObjectReaders = true;
  if(setObjectReaders)
  {
    js::SetObjectReader(GetImageMetaDataReader());

    setObjectReaders = false;
  }

  MetaData metaData;
  GetMetaDataReader().Read(rootObj, metaData);

  sceneMetadata.mImageMetadata.reserve(metaData.mImageData.size() + metaData.mImageData.size());
  for(auto&& data : metaData.mImageData)
  {
    if(data.mSamplingMode == ImageData::SamplingMode::INVALID)
    {
      throw std::runtime_error("Sampling mode type unsupported.");
    }
    else
    {
      sceneMetadata.mImageMetadata.insert({data.mImageUri, ImageMetadata{ImageDimensions(data.mMinWidth, data.mMinHeight), static_cast<Dali::SamplingMode::Type>(data.mSamplingMode)}});
    }
  }
}

} // namespace Dali::Scene3D::Loader
