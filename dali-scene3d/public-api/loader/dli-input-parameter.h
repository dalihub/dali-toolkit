#ifndef DALI_SCENE3D_LOADER_DLI_INPUT_PARAMETER_H
#define DALI_SCENE3D_LOADER_DLI_INPUT_PARAMETER_H
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
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>
#include <dali-scene3d/public-api/loader/model-loader.h>

namespace Dali::Scene3D::Loader
{
class DliInputParameter : public ModelLoader::InputParameter
{
public:
  using ConvertFontCode         = void (*)(const std::string& code, std::string& fontFamily, std::string& slant, std::string& weight, float& size);
  using ConvertColorCode        = Vector4 (*)(const std::string& code);
  using CategoryProcessor       = std::function<void(Property::Array&& categoryData, StringCallback onError)>;
  using CategoryProcessorVector = std::vector<std::pair<std::string /*name*/, CategoryProcessor>>;
  using NodeProcessor           = std::function<void(const Dali::Scene3D::Loader::NodeDefinition& nodeDef,
                                           Property::Map&&                              nodeData,
                                           StringCallback                               onError)>;
  using AnimationProcessor      = std::function<void(const AnimationDefinition& animDef,
                                                Property::Map&&            animData,
                                                StringCallback             onError)>;

public:
  /**
   * @brief The absolute path of animation binaries referenced in the .dli.
   */
  std::string mAnimationsPath;

  /**
   * @brief Provides a facility to determine a color from a code instead of RGB(A) values.
   */
  ConvertColorCode mConvertColorCode{nullptr};

  /**
   * @brief A collection of handlers, mapped to the names of the top level (i.e. below
   *  root) element, whom they will attempt to process. This will take place before
   *  the parsing of scene Nodes and Animations, but after skeletons, environment, mesh,
   *  shader and material resources.
   */
  CategoryProcessorVector mPreNodeCategoryProcessors;

  /**
   * @brief A collection of handlers, mapped to the names of the top level (i.e. below
   *  root) element, whom they will attempt to process. This will take place after
   *  the parsing of the scene Nodes and Animations.
   */
  CategoryProcessorVector mPostNodeCategoryProcessors;

  /**
   * @brief Provides an extension point to nodes. If provided, this function will be
   *  called with each JSON element and definition, of a scene node.
   * @note Constraints rely on ID resolution (from .dli to scene definition), which
   *  takes place after the parsing of the nodes; therefore AT THIS POINT the node
   *  IDs seen in constraints will still be the .dli IDs - NOT to be relied on for
   *  indexing into mScene.
   */
  NodeProcessor mNodePropertyProcessor{nullptr};

  /**
   * @brief Provides an extension point to animations. If provided, this function will be
   *  called with each JSON element and fully processed definition, of an animation.
   */
  AnimationProcessor mAnimationPropertyProcessor;
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_DLI_INPUT_PARAMETER_H
