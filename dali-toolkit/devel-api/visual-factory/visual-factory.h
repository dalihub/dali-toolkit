#ifndef DALI_TOOLKIT_VISUAL_FACTORY_H
#define DALI_TOOLKIT_VISUAL_FACTORY_H

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
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/object/property-map.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>

namespace Dali
{
class Image;
struct Vector4;

namespace Toolkit
{
namespace Internal DALI_INTERNAL
{
class VisualFactory;
}

/**
 * @brief VisualFactory is a singleton object that provides and shares visuals between controls
 *
 * By setting environment variable 'DALI_DEBUG_RENDERING', a debug visual is used which renders a quad wireframe.
 *
 * The visual type is required in the property map for requesting a visual.
 *
 * | %Property Name           | Type              |
 * |--------------------------|-------------------|
 * | visualType               | INTEGER or STRING |
 * | shader                   | MAP               |
 */
class DALI_TOOLKIT_API VisualFactory : public BaseHandle
{
public:
  /**
   * @brief Create or retrieve VisualFactory singleton.
   *
   * @return A handle to the VisualFactory control.
   */
  static VisualFactory Get();

  /**
   * @brief Create a VisualFactory handle.
   *
   * Calling member functions with an uninitialised handle is not allowed.
   */
  VisualFactory();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~VisualFactory();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param[in] handle A reference to the copied handle.
   */
  VisualFactory(const VisualFactory& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] handle  A reference to the copied handle.
   * @return A reference to this.
   */
  VisualFactory& operator=(const VisualFactory& handle);

  /**
   * @brief Request the visual
   *
   * @param[in] propertyMap The map contains the properties required by the visual.
   *            The content of the map determines the type of visual that will be returned.
   * @return The handle to the created visual
   */
  Visual::Base CreateVisual(const Property::Map& propertyMap);

  /**
   * @brief Request the visual to render the given resource at the url.
   *
   * @param[in] url The URL to the resource to be rendered.
   * @param[in] size The width and height to fit the loaded image to.
   * @return The pointer pointing to the visual
   */
  Visual::Base CreateVisual(const std::string& url, ImageDimensions size);

  /**
   * @brief Enable or disable premultiplying alpha in images and image visuals.
   *
   * The default is to enable pre-multiplication on load.
   *
   * Applications that have assets with pre-multiplied alpha already applied should turn this option off.
   *
   * @param[in] preMultiply True if loaded images for image visuals should have alpha multiplied into the color
   * channels.
   */
  void SetPreMultiplyOnLoad(bool preMultiply);

  /**
   * @brief Get the setting for automatically pre-multiplying image visual images on load.
   *
   * @return True if loaded images have pre-multiplied alpha applied on load, false otherwise.
   */
  bool GetPreMultiplyOnLoad() const;

  /**
   * @brief Discard visual base. It will keep reference of visual until idle callback called.
   *
   * @param[in] visual Discarded visual base.
   */
  void DiscardVisual(Visual::Base visual);

  /**
   * @brief Compile the visual shader in advance. Afterwards,
   * when a visual using a new shader is requested, the pre-compiled shader is used.
   *
   * @note It is recommended that this method be called at the top of the application code.
   * @note Using precompiled shaders is helpful when the application is complex and uses
   * many different styles of visual options. On the other hand,if most visuals are the same
   * and the application is simple, it may use memory unnecessarily or slow down the application launching speed.
   */
  void UsePreCompiledShader();

private:
  explicit DALI_INTERNAL VisualFactory(Internal::VisualFactory* impl);
};

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VISUAL_FACTORY_H
