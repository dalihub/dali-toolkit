#ifndef DALI_TOOLKIT_IMAGE_ATLAS_MANAGER_H
#define DALI_TOOLKIT_IMAGE_ATLAS_MANAGER_H

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
 */

// EXTERNAL INCLUDES
#include <string>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/rendering/texture-set.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/image-atlas/image-atlas.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The manager for automatic image atlasing. Owned by RendererFactory
 */
class ImageAtlasManager : public RefObject
{
public:
  typedef std::vector< Toolkit::ImageAtlas > AtlasContainer;
  typedef std::vector< TextureSet > TextureSetContainer;

public:

  /**
   * Construtor
   *
   */
  ImageAtlasManager();

  /**
   * @brief Add an image to the atlas.
   *
   * @note To make the atlasing efficient, an valid size should be provided.
   *       If size is not provided, then the image file will be opened to read the actual size for loading.
   *
   * SamplingMode::BOX_THEN_LINEAR is used to sampling pixels from the input image while fitting it to desired size.
   *
   * @param [out] textureRect The texture area of the resource image in the atlas.
   * @param [in] url The URL of the resource image file to use.
   * @param [in] size The width and height to fit the loaded image to.
   * @param [in] fittingMode The method used to fit the shape of the image before loading to the shape defined by the size parameter.
   * @param [in] orientationCorrection Reorient the image to respect any orientation metadata in its header.
   * @return The texture set containing the image.
   */
  TextureSet Add( Vector4& textureRect,
                const std::string& url,
                ImageDimensions size = ImageDimensions(),
                FittingMode::Type fittingMode = FittingMode::DEFAULT,
                bool orientationCorrection = true );

  /**
   * @brief Add a pixel buffer to the atlas
   *
   * @param [out] textureRect The texture area of the resource image in the atlas.
   * @param [in] pixelData The pixel data.
   * @return The texture set containing the image.
   */
  TextureSet Add( Vector4& textureRect,
                  PixelData pixelData );

  /**
   * Remove the image at the given rectangle from the texture set.
   *
   * @param [in] textureSet The texture set containing the atlas image.
   * @param [in] textureRect The texture area to be removed.
   */
  void Remove( TextureSet textureSet, const Vector4& textureRect );

  /**
   * @brief Set the broken image which is used to replace the image if loading fails.
   *
   * @param[in] brokenImageUrl The url of the broken image.
   */
  void SetBrokenImage( const std::string& brokenImageUrl );

  /**
   * @brief Get shader
   */
  Shader GetShader() const;

private:

  /**
   * @brief Create a new atlas.
   *
   * This method is called when the newly added image or pixel buffer cannot fit into the current atlas list.
   */
  void CreateNewAtlas();

protected:

  /**
   * Destructor
   */
  virtual ~ImageAtlasManager();

  /**
   * Undefined copy constructor.
   */
  ImageAtlasManager(const ImageAtlasManager&);

  /**
   * Undefined assignment operator.
   */
  ImageAtlasManager& operator=(const ImageAtlasManager& rhs);


private:

  AtlasContainer    mAtlasList;
  TextureSetContainer mTextureSetList;
  std::string       mBrokenImageUrl;

};

} // name Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_ATLAS_MANAGER_H
