#ifndef DALI_SCENE_LOADER_DLI_LOADER_H
#define DALI_SCENE_LOADER_DLI_LOADER_H
/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/api.h"
#include "dali-scene-loader/public-api/animation-definition.h"
#include "dali-scene-loader/public-api/customization.h"
#include "dali-scene-loader/public-api/string-callback.h"
#include "dali-scene-loader/public-api/index.h"

// EXTERNAL INCLUDES
#include "dali/public-api/common/vector-wrapper.h"

namespace Dali
{
namespace SceneLoader
{
typedef std::pair<std::string, std::string> Metadata;

// Forward declarations
struct LoadResult;
struct CameraParameters;
struct LightParameters;
struct TextParameters;

class ResourceBundle;
struct NodeDefinition;
class SceneDefinition;

class DALI_SCENE_LOADER_API DliLoader
{
public:
  using ConvertFontCode = void(*)(const std::string& code, std::string& fontFamily, std::string& slant, std::string& weight, float& size);
  using ConvertColorCode = Vector4(*)(const std::string& code);

  using CategoryProcessor = std::function<void(Property::Array&& categoryData, StringCallback onError)>;
  using CategoryProcessorVector = std::vector<std::pair<std::string /*name*/, CategoryProcessor>>;

  using NodeProcessor = std::function<void(const NodeDefinition& nodeDef,
    Property::Map&& nodeData, StringCallback onError)>;

  using AnimationProcessor = std::function<void(const AnimationDefinition& animDef,
    Property::Map&& animData, StringCallback onError)>;

  struct InputParams
  {
    /**
     * @brief The absolute path of animation binaries referenced in the .dli.
     */
    std::string mAnimationsPath;

    /**
     * @brief Provides a facility to determine a color from a code instead of RGB(A) values.
     */
    ConvertColorCode mConvertColorCode;

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
    NodeProcessor mNodePropertyProcessor;

    /**
     * @brief Provides an extension point to animations. If provided, this function will be
     *  called with each JSON element and fully processed definition, of an animation.
     */
    AnimationProcessor mAnimationPropertyProcessor;
  };

  struct LoadParams
  {
    InputParams const& input;
    LoadResult& output;
  };

  DliLoader();
  ~DliLoader();

  /**
   * @brief Sets the callback that messages from non-fatal errors get posted to.
   *  Uses DefaultErrorCallback by default.
   */
  void SetErrorCallback(StringCallback onError);

  /**
   * @brief Attempts to load and parse a .dli document into a DOM tree.
   * @return Whether the operation was successful.
   */
  bool LoadScene(const std::string& uri, LoadParams& params);

  /**
   * @return The error string describing how the parse has failed, if any.
   */
  std::string GetParseError() const;

private:
  struct Impl;
  const std::unique_ptr<Impl> mImpl;
};

}
}

#endif // DALI_SCENE_LOADER_DLI_LOADER_H
