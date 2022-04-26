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

// CLASS HEADER
#include "dali-scene-loader/public-api/dli-loader.h"

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <memory>
#include "dali-toolkit/devel-api/builder/json-parser.h"
#include "dali/devel-api/common/map-wrapper.h"
#include "dali/integration-api/debug.h"
#include "dali/public-api/object/property-array.h"

// INTERNAL INCLUDES
#include "dali-scene-loader/internal/json-util.h"
#include "dali-scene-loader/public-api/alpha-function-helper.h"
#include "dali-scene-loader/public-api/animation-definition.h"
#include "dali-scene-loader/public-api/blend-shape-details.h"
#include "dali-scene-loader/public-api/camera-parameters.h"
#include "dali-scene-loader/public-api/ktx-loader.h"
#include "dali-scene-loader/public-api/light-parameters.h"
#include "dali-scene-loader/public-api/load-result.h"
#include "dali-scene-loader/public-api/parse-renderer-state.h"
#include "dali-scene-loader/public-api/scene-definition.h"
#include "dali-scene-loader/public-api/skinning-details.h"
#include "dali-scene-loader/public-api/utils.h"

#define DLI_0_1_COMPATIBILITY

namespace Dali
{
using namespace Toolkit;

namespace SceneLoader
{
namespace rs = RendererState;

namespace
{
const std::string NODES         = "nodes";
const std::string SCENES        = "scenes";
const std::string NODE          = "node";
const std::string URI           = "uri";
const std::string URL           = "url";
const std::string CUSTOMIZATION = "customization";
const std::string HINTS         = "hints";
const std::string NAME("name");
const std::string BLEND_SHAPE_HEADER("blendShapeHeader");
const std::string BLEND_SHAPES("blendShapes");
const std::string BLEND_SHAPE_VERSION_1_0("1.0");
const std::string BLEND_SHAPE_VERSION_2_0("2.0");
const std::string VERSION("version");

const char* const SHADOW_MAP_SIZE   = "shadowMapSize";
const char* const ORTHOGRAPHIC_SIZE = "orthographicSize";
const char* const PIXEL_UNITS       = "px";

const char SLASH = '/';

void ReadModelTransform(const TreeNode* node, Quaternion& orientation, Vector3& translation, Vector3& scale)
{
  float num[16u] = {.0f};

  if(ReadVector(node->GetChild("matrix"), num, 16u))
  {
    Matrix mat(num);
    mat.GetTransformComponents(translation, orientation, scale);
  }
  else
  {
    if(ReadVector(node->GetChild("angle"), num, 3u))
    {
      orientation = Quaternion(Radian(Degree(num[0u])), Radian(Degree(num[1u])), Radian(Degree(num[2u])));
    }

    if(ReadVector(node->GetChild("position"), num, 3u))
    {
      translation = Vector3(num);
    }
  }
}

bool ReadAttribBlob(const TreeNode* node, MeshDefinition::Blob& buffer)
{
  return ReadBlob(node, buffer.mOffset, buffer.mLength);
}

bool ReadAttribAccessor(const TreeNode* node, MeshDefinition::Accessor& accessor)
{
  return ReadBlob(node, accessor.mBlob.mOffset, accessor.mBlob.mLength);
}

bool ReadColorCode(const TreeNode* node, Vector4& color, DliLoader::ConvertColorCode convertColorCode)
{
  if(!node || !convertColorCode)
  {
    return false;
  }

  color = convertColorCode(node->GetString());

  return true;
}

bool ReadColorCodeOrColor(const TreeNode* node, Vector4& color, DliLoader::ConvertColorCode convertColorCode)
{
  return ReadColorCode(node->GetChild("colorCode"), color, convertColorCode) ||
         ReadColor(node->GetChild("color"), color);
}

RendererState::Type ReadRendererState(const TreeNode& tnRendererState)
{
  if(tnRendererState.GetType() == TreeNode::INTEGER)
  {
    return static_cast<RendererState::Type>(tnRendererState.GetInteger());
  }
  else if(tnRendererState.GetType() == TreeNode::STRING)
  {
    return RendererState::Parse(tnRendererState.GetString());
  }
  else
  {
    return -1;
  }
}

///@brief Reads arc properties.
void ReadArcField(const TreeNode* eArc, ArcNode& arc)
{
  ReadBool(eArc->GetChild("antiAliasing"), arc.mAntiAliasing);
  ReadInt(eArc->GetChild("arcCaps"), arc.mArcCaps);
  ReadFloat(eArc->GetChild("radius"), arc.mRadius);

  arc.mStartAngleDegrees = .0f;
  ReadFloat(eArc->GetChild("startAngle"), arc.mStartAngleDegrees);

  arc.mEndAngleDegrees = .0f;
  ReadFloat(eArc->GetChild("endAngle"), arc.mEndAngleDegrees);
}

const TreeNode* GetNthChild(const TreeNode* node, uint32_t index)
{
  uint32_t i = 0;
  for(TreeNode::ConstIterator it = (*node).CBegin(); it != (*node).CEnd(); ++it, ++i)
  {
    if(i == index)
    {
      return &((*it).second);
    }
  }
  return NULL;
}

const TreeNode* RequireChild(const TreeNode* node, const std::string& childName)
{
  auto child = node->GetChild(childName);
  if(!child)
  {
    ExceptionFlinger flinger(ASSERT_LOCATION);
    flinger << "Failed to find child node '" << childName << "'";
    if(auto nodeName = node->GetName())
    {
      flinger << " on '" << nodeName << "'";
    }
    flinger << ".";
  }
  return child;
}

void ParseProperties(const Toolkit::TreeNode& node, Property::Array& array);

void ParseProperties(const Toolkit::TreeNode& node, Property::Map& map)
{
  DALI_ASSERT_DEBUG(node.GetType() == TreeNode::OBJECT);
  for(auto i0 = node.CBegin(), i1 = node.CEnd(); i0 != i1; ++i0)
  {
    auto kv = *i0;
    switch(kv.second.GetType())
    {
      case TreeNode::ARRAY:
      {
        Property::Array array;
        ParseProperties(kv.second, array);
        map.Insert(kv.first, array);
        break;
      }

      case TreeNode::OBJECT:
      {
        Property::Map innerMap;
        ParseProperties(kv.second, innerMap);
        map.Insert(kv.first, innerMap);
        break;
      }

      case TreeNode::STRING:
      {
        map.Insert(kv.first, kv.second.GetString());
        break;
      }

      case TreeNode::INTEGER:
      {
        map.Insert(kv.first, kv.second.GetInteger());
        break;
      }

      case TreeNode::BOOLEAN:
      {
        map.Insert(kv.first, kv.second.GetBoolean());
        break;
      }

      case TreeNode::FLOAT:
      {
        map.Insert(kv.first, kv.second.GetFloat());
        break;
      }

      case TreeNode::IS_NULL:
      {
        break;
      }
    }
  }
}

void ParseProperties(const Toolkit::TreeNode& node, Property::Array& array)
{
  DALI_ASSERT_DEBUG(node.GetType() == TreeNode::ARRAY);
  for(auto i0 = node.CBegin(), i1 = node.CEnd(); i0 != i1; ++i0)
  {
    auto kv = *i0;
    switch(kv.second.GetType())
    {
      case TreeNode::ARRAY:
      {
        Property::Array innerArray;
        ParseProperties(kv.second, innerArray);
        array.PushBack(innerArray);
        break;
      }

      case TreeNode::OBJECT:
      {
        Property::Map map;
        ParseProperties(kv.second, map);
        array.PushBack(map);
        break;
      }

      case TreeNode::STRING:
      {
        array.PushBack(kv.second.GetString());
        break;
      }

      case TreeNode::INTEGER:
      {
        array.PushBack(kv.second.GetInteger());
        break;
      }

      case TreeNode::BOOLEAN:
      {
        array.PushBack(kv.second.GetBoolean());
        break;
      }

      case TreeNode::FLOAT:
      {
        array.PushBack(kv.second.GetFloat());
        break;
      }

      case TreeNode::IS_NULL:
      {
        break;
      }
    }
  }
}

} //namespace

struct DliLoader::Impl
{
  StringCallback      mOnError = DefaultErrorCallback;
  Toolkit::JsonParser mParser;

  void ParseScene(LoadParams& params);

private:
  std::map<Index, Matrix> mInverseBindMatrices;

  /**
   * @brief Due to .dli nodes being processed in depth-first traversal with orphans being
   *  ignored, features that rely on node indices (which is more compact and closer to
   *  glTF) require a mapping from .dli node indices to those in the resulting SceneDefinition.
   *  The index mapper is responsible for maintaing this mapping, and resolving node IDs
   *  once the processing of the nodes has finished.
   * @note The resolution requires the whole scene graph to finish parsing, therefore any
   *  node extensions relying on node IDs will see the dli ID in their processor.
   */
  struct IIndexMapper
  {
    /**
     * @brief Attempts to create a mapping from a node's @a dli index to its @a scene
     *  index.
     * @return Whether the operation was successful.
     */
    virtual bool Map(Index iDli, Index iScene) = 0;

    /**
     * @return The scene index for the node's @a dli index.
     */
    virtual Index Resolve(Index iDli) = 0;
  };

  /**
   * @brief Traverses the DOM tree created by LoadDocument() in an attempt to create
   *  an intermediate representation of resources and nodes.
   */
  void ParseSceneInternal(Index iScene, const Toolkit::TreeNode* tnScenes, const Toolkit::TreeNode* tnNodes, LoadParams& params);

  void ParseSkeletons(const Toolkit::TreeNode* skeletons, SceneDefinition& scene, ResourceBundle& resources);
  void ParseEnvironments(const Toolkit::TreeNode* environments, ResourceBundle& resources);
  void ParseMaterials(const Toolkit::TreeNode* materials, ConvertColorCode convertColorCode, ResourceBundle& resources);

  void ParseNodes(const Toolkit::TreeNode* nodes, Index index, LoadParams& params);
  void ParseNodesInternal(const Toolkit::TreeNode* nodes, Index index, std::vector<Index>& inOutParentStack, LoadParams& params, IIndexMapper& indexMapper);

  void ParseAnimations(const Toolkit::TreeNode* animations, LoadParams& params);
  void ParseAnimationGroups(const Toolkit::TreeNode* animationGroups, LoadParams& params);

  void ParseShaders(const Toolkit::TreeNode* shaders, ResourceBundle& resources);
  void ParseMeshes(const Toolkit::TreeNode* meshes, ResourceBundle& resources);

  void GetCameraParameters(std::vector<CameraParameters>& cameras) const;
  void GetLightParameters(std::vector<LightParameters>& lights) const;
};

DliLoader::DliLoader()
: mImpl{new Impl}
{
}

DliLoader::~DliLoader() = default;

void DliLoader::SetErrorCallback(StringCallback onError)
{
  mImpl->mOnError = onError;
}

bool DliLoader::LoadScene(const std::string& uri, LoadParams& params)
{
  std::string daliBuffer = LoadTextFile(uri.c_str());

  auto& parser = mImpl->mParser;
  parser       = JsonParser::New();
  if(!parser.Parse(daliBuffer))
  {
    return false;
  }

  mImpl->ParseScene(params);
  return true;
}

std::string DliLoader::GetParseError() const
{
  std::stringstream stream;

  auto& parser = mImpl->mParser;
  if(parser.ParseError())
  {
    stream << "position: " << parser.GetErrorPosition() << ", line: " << parser.GetErrorLineNumber() << ", column: " << parser.GetErrorColumn() << ", description: " << parser.GetErrorDescription() << ".";
  }

  return stream.str();
}

void DliLoader::Impl::ParseScene(LoadParams& params)
{
  auto& input  = params.input;
  auto& output = params.output;

  // get index of root node.
  auto docRoot = mParser.GetRoot();

  // Process resources first - these are shared
  if(auto environments = docRoot->GetChild("environment"))
  {
    ParseEnvironments(environments, output.mResources); // NOTE: must precede parsing of materials
  }

  if(auto meshes = docRoot->GetChild("meshes"))
  {
    ParseMeshes(meshes, output.mResources);
  }

  if(auto shaders = docRoot->GetChild("shaders"))
  {
    ParseShaders(shaders, output.mResources);
  }

  if(auto materials = docRoot->GetChild("materials"))
  {
    ParseMaterials(materials, input.mConvertColorCode, output.mResources);
  }

  for(auto& c : input.mPreNodeCategoryProcessors)
  {
    if(auto node = docRoot->GetChild(c.first))
    {
      Property::Array array;
      ParseProperties(*node, array);
      c.second(std::move(array), mOnError);
    }
  }

  // Process scenes
  Index iScene = 0; // default scene
  ReadIndex(docRoot->GetChild("scene"), iScene);

  auto tnScenes = RequireChild(docRoot, "scenes");
  auto tnNodes  = RequireChild(docRoot, "nodes");
  ParseSceneInternal(iScene, tnScenes, tnNodes, params);

  ParseSkeletons(docRoot->GetChild("skeletons"), output.mScene, output.mResources);

  output.mScene.EnsureUniqueSkinningShaderInstances(output.mResources);
  output.mScene.EnsureUniqueBlendShapeShaderInstances(output.mResources);

  // Ger cameras and lights
  GetCameraParameters(output.mCameraParameters);
  GetLightParameters(output.mLightParameters);

  // Post-node processors and animations last
  for(auto& c : input.mPostNodeCategoryProcessors)
  {
    if(auto node = docRoot->GetChild(c.first))
    {
      Property::Array array;
      ParseProperties(*node, array);
      c.second(std::move(array), mOnError);
    }
  }

  if(auto animations = docRoot->GetChild("animations"))
  {
    ParseAnimations(animations, params);
  }

  if(!output.mAnimationDefinitions.empty())
  {
    if(auto animationGroups = docRoot->GetChild("animationGroups"))
    {
      ParseAnimationGroups(animationGroups, params);
    }
  }
}

void DliLoader::Impl::ParseSceneInternal(Index iScene, const Toolkit::TreeNode* tnScenes, const Toolkit::TreeNode* tnNodes, LoadParams& params)
{
  auto getSceneRootIdx = [tnScenes, tnNodes](Index iScene) {
    auto tn = GetNthChild(tnScenes, iScene); // now a "scene" object
    if(!tn)
    {
      ExceptionFlinger(ASSERT_LOCATION) << iScene << " is out of bounds access into " << SCENES << ".";
    }

    tn = RequireChild(tn, NODES); // now a "nodes" array
    if(tn->GetType() != TreeNode::ARRAY)
    {
      ExceptionFlinger(ASSERT_LOCATION) << SCENES << "[" << iScene << "]." << NODES << " has an invalid type; array required.";
    }

    if(tn->Size() < 1)
    {
      ExceptionFlinger(ASSERT_LOCATION) << SCENES << "[" << iScene << "]." << NODES << " must define a node id.";
    }

    tn = GetNthChild(tn, 0); // now the first element of the array
    Index iRootNode;
    if(!ReadIndex(tn, iRootNode))
    {
      ExceptionFlinger(ASSERT_LOCATION) << SCENES << "[" << iScene << "]." << NODES << " has an invalid value for root node index: '" << iRootNode << "'.";
    }

    if(iRootNode >= tnNodes->Size())
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Root node index << " << iRootNode << " of scene " << iScene << " is out of bounds.";
    }

    tn = GetNthChild(tnNodes, iRootNode); // now a "node" object
    if(tn->GetType() != TreeNode::OBJECT)
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Root node of scene " << iScene << " is of invalid JSON type; object required";
    }

    return iRootNode;
  };

  Index iRootNode = getSceneRootIdx(iScene);
  ParseNodes(tnNodes, iRootNode, params);

  auto& scene = params.output.mScene;
  scene.AddRootNode(0);

  for(Index i = 0; i < iScene; ++i)
  {
    Index       iRootNode = getSceneRootIdx(i);
    const Index iRoot     = scene.GetNodeCount();
    ParseNodes(tnNodes, iRootNode, params);
    scene.AddRootNode(iRoot);
  }

  auto numScenes = tnScenes->Size();
  for(Index i = iScene + 1; i < numScenes; ++i)
  {
    Index       iRootNode = getSceneRootIdx(i);
    const Index iRoot     = scene.GetNodeCount();
    ParseNodes(tnNodes, iRootNode, params);
    scene.AddRootNode(iRoot);
  }
}

void DliLoader::Impl::ParseSkeletons(const TreeNode* skeletons, SceneDefinition& scene, ResourceBundle& resources)
{
  if(skeletons)
  {
    auto iStart = skeletons->CBegin();
    for(auto i0 = iStart, i1 = skeletons->CEnd(); i0 != i1; ++i0)
    {
      auto&       node = (*i0).second;
      std::string skeletonRootName;
      if(ReadString(node.GetChild(NODE), skeletonRootName))
      {
        SkeletonDefinition skeleton;
        if(!scene.FindNode(skeletonRootName, &skeleton.mRootNodeIdx))
        {
          ExceptionFlinger(ASSERT_LOCATION) << FormatString("Skeleton %d: node '%s' not defined.", resources.mSkeletons.size(), skeletonRootName.c_str());
        }

        uint32_t                   jointCount = 0;
        std::function<void(Index)> visitFn;
        auto&                      ibms = mInverseBindMatrices;
        visitFn                         = [&](Index id) {
          auto node = scene.GetNode(id);
          jointCount += ibms.find(id) != ibms.end();

          for(auto i : node->mChildren)
          {
            visitFn(i);
          }
        };
        visitFn(skeleton.mRootNodeIdx);

        if(jointCount > Skinning::MAX_JOINTS)
        {
          mOnError(FormatString("Skeleton %d: joint count exceeds supported limit.", resources.mSkeletons.size()));
          jointCount = Skinning::MAX_JOINTS;
        }

        skeleton.mJoints.reserve(jointCount);

        visitFn = [&](Index id) {
          auto iFind = ibms.find(id);
          if(iFind != ibms.end() && skeleton.mJoints.size() < Skinning::MAX_JOINTS)
          {
            skeleton.mJoints.push_back({id, iFind->second});
          }

          auto node = scene.GetNode(id);
          for(auto i : node->mChildren)
          {
            visitFn(i);
          }
        };
        visitFn(skeleton.mRootNodeIdx);

        resources.mSkeletons.push_back(std::move(skeleton));
      }
      else
      {
        ExceptionFlinger(ASSERT_LOCATION) << "skeleton " << std::distance(iStart, i0) << ": Missing required attribute '" << NODE << "'.";
      }
    }
  }
}

void DliLoader::Impl::ParseEnvironments(const TreeNode* environments, ResourceBundle& resources)
{
  Matrix cubeOrientation(Matrix::IDENTITY);

  for(auto i0 = environments->CBegin(), i1 = environments->CEnd(); i0 != i1; ++i0)
  {
    auto& node = (*i0).second;

    EnvironmentDefinition envDef;
    ReadString(node.GetChild("cubeSpecular"), envDef.mSpecularMapPath);
    ReadString(node.GetChild("cubeDiffuse"), envDef.mDiffuseMapPath);
    ToUnixFileSeparators(envDef.mSpecularMapPath);
    ToUnixFileSeparators(envDef.mDiffuseMapPath);
    envDef.mIblIntensity = 1.0f;
    ReadFloat(node.GetChild("iblIntensity"), envDef.mIblIntensity);
    if(ReadVector(node.GetChild("cubeInitialOrientation"), cubeOrientation.AsFloat(), 16u))
    {
      envDef.mCubeOrientation = Quaternion(cubeOrientation);
    }

    resources.mEnvironmentMaps.emplace_back(std::move(envDef), EnvironmentDefinition::Textures());
  }

  // NOTE: guarantees environmentMaps to have an empty environment.
  if(resources.mEnvironmentMaps.empty())
  {
    resources.mEnvironmentMaps.emplace_back(EnvironmentDefinition(), EnvironmentDefinition::Textures());
  }
}

void DliLoader::Impl::ParseShaders(const TreeNode* shaders, ResourceBundle& resources)
{
  uint32_t iShader = 0;
  for(auto i0 = shaders->CBegin(), i1 = shaders->CEnd(); i0 != i1; ++i0, ++iShader)
  {
    auto&            node = (*i0).second;
    ShaderDefinition shaderDef;
    ReadStringVector(node.GetChild("defines"), shaderDef.mDefines);

    // Read shader hints. Possible values are:
    //                         Don't define for No hints.
    // "OUTPUT_IS_TRANSPARENT" Might generate transparent alpha from opaque inputs.
    //     "MODIFIES_GEOMETRY" Might change position of vertices, this option disables any culling optimizations.

    ReadStringVector(node.GetChild(HINTS), shaderDef.mHints);

    if(ReadString(node.GetChild("vertex"), shaderDef.mVertexShaderPath) &&
       ReadString(node.GetChild("fragment"), shaderDef.mFragmentShaderPath))
    {
      ToUnixFileSeparators(shaderDef.mVertexShaderPath);
      ToUnixFileSeparators(shaderDef.mFragmentShaderPath);

      for(TreeNode::ConstIterator j0 = node.CBegin(), j1 = node.CEnd(); j0 != j1; ++j0)
      {
        const TreeNode::KeyNodePair& keyValue = *j0;
        const std::string&           key      = keyValue.first;
        const TreeNode&              value    = keyValue.second;

        Property::Value uniformValue;
        if(key.compare("vertex") == 0 || key.compare("fragment") == 0 || key.compare("defines") == 0 || key.compare(HINTS) == 0)
        {
          continue;
        }
        else if(key.compare("rendererState") == 0)
        {
          shaderDef.mRendererState = ReadRendererState(keyValue.second);
        }
        else if(value.GetType() == TreeNode::INTEGER || value.GetType() == TreeNode::FLOAT)
        {
          float f = 0.f;
          ReadFloat(&value, f);
          uniformValue = f;
        }
        else if(value.GetType() == TreeNode::BOOLEAN)
        {
          DALI_LOG_WARNING("\"bool\" uniforms are handled as floats in shader");
          bool value = false;
          if(ReadBool(&keyValue.second, value))
          {
            uniformValue = value ? 1.0f : 0.0f;
          }
        }
        else
          switch(auto size = GetNumericalArraySize(&value))
          {
            case 16:
            {
              Matrix m;
              ReadVector(&value, m.AsFloat(), size);
              uniformValue = m;
              break;
            }

            case 9:
            {
              Matrix3 m;
              ReadVector(&value, m.AsFloat(), size);
              uniformValue = m;
              break;
            }

            case 4:
            {
              Vector4 v;
              ReadVector(&value, v.AsFloat(), size);
              uniformValue = v;
              break;
            }

            case 3:
            {
              Vector3 v;
              ReadVector(&value, v.AsFloat(), size);
              uniformValue = v;
              break;
            }

            case 2:
            {
              Vector2 v;
              ReadVector(&value, v.AsFloat(), size);
              uniformValue = v;
              break;
            }

            default:
              mOnError(FormatString(
                "shader %d: Ignoring uniform '%s': failed to infer type from %d elements.",
                iShader,
                key.c_str()));
              break;
          }

        if(Property::NONE != uniformValue.GetType())
        {
          shaderDef.mUniforms.Insert(key, uniformValue);
        }
      }

      resources.mShaders.emplace_back(std::move(shaderDef), Shader());
    }
    else
    {
      ExceptionFlinger(ASSERT_LOCATION) << "shader " << iShader << ": Missing vertex / fragment shader definition.";
    }
  }
}

void DliLoader::Impl::ParseMeshes(const TreeNode* meshes, ResourceBundle& resources)
{
  for(auto i0 = meshes->CBegin(), i1 = meshes->CEnd(); i0 != i1; ++i0)
  {
    auto& node = (*i0).second;

    MeshDefinition meshDef;
    if(!ReadString(node.GetChild(URI), meshDef.mUri))
    {
      ExceptionFlinger(ASSERT_LOCATION) << "mesh " << resources.mMeshes.size() << ": Missing required attribute '" << URI << "'.";
    }

    ToUnixFileSeparators(meshDef.mUri);

    std::string primitive;
    if(ReadString(node.GetChild("primitive"), primitive))
    {
      if(primitive == "LINES")
      {
        meshDef.mPrimitiveType = Geometry::LINES;
      }
      else if(primitive == "POINTS")
      {
        meshDef.mPrimitiveType = Geometry::POINTS;
      }
      else if(primitive != "TRIANGLES")
      {
        mOnError(FormatString(
          "mesh %d: Using TRIANGLES instead of unsupported primitive type '%s'.",
          resources.mMeshes.size(),
          primitive.c_str()));
      }
    }

    int attributes;
    if(ReadInt(node.GetChild("attributes"), attributes))
    {
      if(MaskMatch(attributes, MeshDefinition::INDICES) &&
         !ReadAttribAccessor(node.GetChild("indices"), meshDef.mIndices))
      {
        ExceptionFlinger(ASSERT_LOCATION) << FormatString("mesh %d: Failed to read %s.",
                                                          resources.mMeshes.size(),
                                                          "indices");
      }

      if(MaskMatch(attributes, MeshDefinition::POSITIONS) &&
         !ReadAttribAccessor(node.GetChild("positions"), meshDef.mPositions))
      {
        ExceptionFlinger(ASSERT_LOCATION) << FormatString("mesh %d: Failed to read %s.",
                                                          resources.mMeshes.size(),
                                                          "positions");
      }

      if(MaskMatch(attributes, MeshDefinition::NORMALS) &&
         !ReadAttribAccessor(node.GetChild("normals"), meshDef.mNormals))
      {
        mOnError(FormatString("mesh %d: Failed to read %s.", resources.mMeshes.size(), "normals"));
      }

      if(MaskMatch(attributes, MeshDefinition::TEX_COORDS) &&
         !ReadAttribAccessor(node.GetChild("textures"), meshDef.mTexCoords))
      {
        mOnError(FormatString("mesh %d: Failed to read %s.", resources.mMeshes.size(), "textures"));
      }

      if(MaskMatch(attributes, MeshDefinition::TANGENTS) &&
         !ReadAttribAccessor(node.GetChild("tangents"), meshDef.mTangents))
      {
        mOnError(FormatString("mesh %d: Failed to read %s.", resources.mMeshes.size(), "tangents"));
      }

      // NOTE: we're no longer reading bitangents as these are calculated in the shaders.
      if(ReadIndex(node.GetChild("skeleton"), meshDef.mSkeletonIdx))
      {
        if(!MaskMatch(attributes, MeshDefinition::JOINTS_0) &&
           !MaskMatch(attributes, MeshDefinition::WEIGHTS_0))
        {
          mOnError(FormatString("mesh %d: Expected joints0 / weights0 attribute(s) missing.",
                                resources.mMeshes.size()));
        }
        else if(!ReadAttribAccessor(node.GetChild("joints0"), meshDef.mJoints0) ||
                !ReadAttribAccessor(node.GetChild("weights0"), meshDef.mWeights0))
        {
          mOnError(FormatString("mesh %d: Failed to read skinning information.",
                                resources.mMeshes.size()));
        }
      }

      if(auto blendshapeHeader = node.GetChild(BLEND_SHAPE_HEADER))
      {
        std::string blendShapeVersion;
        ReadString(blendshapeHeader->GetChild(VERSION), blendShapeVersion);

        if(0u == blendShapeVersion.compare(BLEND_SHAPE_VERSION_1_0))
        {
          meshDef.mBlendShapeVersion = BlendShapes::Version::VERSION_1_0;
        }
        else if(0u == blendShapeVersion.compare(BLEND_SHAPE_VERSION_2_0))
        {
          meshDef.mBlendShapeVersion = BlendShapes::Version::VERSION_2_0;
        }

        switch(meshDef.mBlendShapeVersion)
        {
          case BlendShapes::Version::VERSION_1_0:
          case BlendShapes::Version::VERSION_2_0: // FALL THROUGH
          {
            ReadAttribBlob(blendshapeHeader, meshDef.mBlendShapeHeader);
            break;
          }
          default:
          {
            // nothing to do
            break;
          }
        }
      }

      if(auto blendShapes = node.GetChild(BLEND_SHAPES))
      {
        meshDef.mBlendShapes.resize(blendShapes->Size());

        auto index = 0u;
        for(auto it = blendShapes->CBegin(), endIt = blendShapes->CEnd(); it != endIt; ++it, ++index)
        {
          // Each blend shape is stored as the difference with the original mesh.

          auto& blendShapeNode = (*it).second;

          auto& blendShape = meshDef.mBlendShapes[index];
          ReadString(blendShapeNode.GetChild("name"), blendShape.name);
          if(auto position = blendShapeNode.GetChild("positions"))
          {
            ReadAttribAccessor(position, blendShape.deltas);
          }
          if(auto normals = blendShapeNode.GetChild("normals"))
          {
            ReadAttribAccessor(normals, blendShape.normals);
          }
          if(auto tangents = blendShapeNode.GetChild("tangents"))
          {
            ReadAttribAccessor(tangents, blendShape.tangents);
          }
          ReadFloat(blendShapeNode.GetChild("weight"), blendShape.weight);
        }
      }

      bool flipV;
      if(ReadBool(node.GetChild("flipV"), flipV))
      {
        meshDef.mFlags |= flipV * MeshDefinition::FLIP_UVS_VERTICAL;
      }

      resources.mMeshes.emplace_back(std::move(meshDef), MeshGeometry());
    }
  }
}

void DliLoader::Impl::ParseMaterials(const TreeNode* materials, ConvertColorCode convertColorCode, ResourceBundle& resources)
{
  for(auto i0 = materials->CBegin(), i1 = materials->CEnd(); i0 != i1; ++i0)
  {
    auto& node = (*i0).second;

    MaterialDefinition materialDef;
    if(auto eEnvironment = node.GetChild("environment"))
    {
      ReadIndex(eEnvironment, materialDef.mEnvironmentIdx);
      if(static_cast<unsigned int>(materialDef.mEnvironmentIdx) >= resources.mEnvironmentMaps.size())
      {
        ExceptionFlinger(ASSERT_LOCATION) << "material " << resources.mMaterials.size() << ": Environment index " << materialDef.mEnvironmentIdx << " out of bounds (" << resources.mEnvironmentMaps.size() << ").";
      }
    }

    //TODO : need to consider AGIF
    std::vector<std::string> texturePaths;
    std::string              texturePath;
    if(ReadString(node.GetChild("albedoMap"), texturePath))
    {
      ToUnixFileSeparators(texturePath);
      const auto semantic = MaterialDefinition::ALBEDO;
      materialDef.mTextureStages.push_back({semantic, TextureDefinition{std::move(texturePath)}});
      materialDef.mFlags |= semantic | MaterialDefinition::TRANSPARENCY; // NOTE: only in dli does single / separate ALBEDO texture mean TRANSPARENCY.
    }
    if(ReadString(node.GetChild("albedoMetallicMap"), texturePath))
    {
      ToUnixFileSeparators(texturePath);

      if(MaskMatch(materialDef.mFlags, MaterialDefinition::ALBEDO))
      {
        mOnError(FormatString("material %d: conflicting semantics; already set %s.", resources.mMaterials.size(), "albedo"));
      }

      const auto semantic = MaterialDefinition::ALBEDO | MaterialDefinition::METALLIC;
      materialDef.mTextureStages.push_back({semantic, TextureDefinition{std::move(texturePath)}});
      materialDef.mFlags |= semantic;
    }

    if(ReadString(node.GetChild("metallicRoughnessMap"), texturePath))
    {
      ToUnixFileSeparators(texturePath);

      if(MaskMatch(materialDef.mFlags, MaterialDefinition::METALLIC))
      {
        mOnError(FormatString("material %d: conflicting semantics; already set %s.", resources.mMaterials.size(), "metallic"));
      }

      const auto semantic = MaterialDefinition::METALLIC | MaterialDefinition::ROUGHNESS;
      materialDef.mTextureStages.push_back({semantic, TextureDefinition{std::move(texturePath)}});
      materialDef.mFlags |= semantic |
                            // We have a metallic-roughhness map and the first texture did not have albedo semantics - we're in the transparency workflow.
                            (MaskMatch(materialDef.mFlags, MaterialDefinition::ALBEDO) * MaterialDefinition::TRANSPARENCY);
    }

    if(ReadString(node.GetChild("normalMap"), texturePath))
    {
      ToUnixFileSeparators(texturePath);

      const auto semantic = MaterialDefinition::NORMAL;
      materialDef.mTextureStages.push_back({semantic, TextureDefinition{std::move(texturePath)}});
      materialDef.mFlags |= semantic |
                            // We have a standalone normal map and the first texture did not have albedo semantics - we're in the transparency workflow.
                            (MaskMatch(materialDef.mFlags, MaterialDefinition::ALBEDO) * MaterialDefinition::TRANSPARENCY);
    }

    if(ReadString(node.GetChild("normalRoughnessMap"), texturePath))
    {
      ToUnixFileSeparators(texturePath);

      if(MaskMatch(materialDef.mFlags, MaterialDefinition::NORMAL))
      {
        mOnError(FormatString("material %d: conflicting semantics; already set %s.", resources.mMaterials.size(), "normal"));
      }

      if(MaskMatch(materialDef.mFlags, MaterialDefinition::ROUGHNESS))
      {
        mOnError(FormatString("material %d: conflicting semantics; already set %s.", resources.mMaterials.size(), "roughness"));
      }

      if(MaskMatch(materialDef.mFlags, MaterialDefinition::TRANSPARENCY))
      {
        mOnError(FormatString("material %d: conflicting semantics; already set %s.", resources.mMaterials.size(), "transparency"));
      }

      const auto semantic = MaterialDefinition::NORMAL | MaterialDefinition::ROUGHNESS;
      materialDef.mTextureStages.push_back({semantic, TextureDefinition{std::move(texturePath)}});
      materialDef.mFlags |= semantic;
    }

    if(ReadString(node.GetChild("subsurfaceMap"), texturePath))
    {
      ToUnixFileSeparators(texturePath);

      const auto semantic = MaterialDefinition::SUBSURFACE;
      materialDef.mTextureStages.push_back({semantic, TextureDefinition{std::move(texturePath)}});
      materialDef.mFlags |= semantic;
    }

    if(ReadString(node.GetChild("occlusionMap"), texturePath))
    {
      ToUnixFileSeparators(texturePath);
      const auto semantic = MaterialDefinition::OCCLUSION;
      materialDef.mTextureStages.push_back({semantic, TextureDefinition{std::move(texturePath)}});
      materialDef.mFlags |= semantic;
    }

    if(ReadColorCodeOrColor(&node, materialDef.mColor, convertColorCode) &&
       materialDef.mColor.a < 1.0f)
    {
      materialDef.mFlags |= MaterialDefinition::TRANSPARENCY;
    }

    ReadFloat(node.GetChild("metallic"), materialDef.mMetallic);
    ReadFloat(node.GetChild("roughness"), materialDef.mRoughness);

    bool mipmaps;
    if(ReadBool(node.GetChild("mipmap"), mipmaps) && mipmaps)
    {
      for(auto& ts : materialDef.mTextureStages)
      {
        ts.mTexture.mSamplerFlags |= SamplerFlags::FILTER_MIPMAP_LINEAR;
      }
    }

    resources.mMaterials.emplace_back(std::move(materialDef), TextureSet());
  }
}

void DliLoader::Impl::ParseNodes(const TreeNode* const nodes, Index index, LoadParams& params)
{
  std::vector<Index> parents;
  parents.reserve(8);

  struct IndexMapper : IIndexMapper
  {
    IndexMapper(size_t numNodes)
    {
      mIndices.reserve(numNodes);
    }

    virtual bool Map(Index iDli, Index iScene) override
    {
      Entry idx{iDli, iScene};
      auto  iInsert = std::lower_bound(mIndices.begin(), mIndices.end(), idx);
      if(iInsert == mIndices.end() || iInsert->iDli != iDli)
      {
        mIndices.insert(iInsert, idx);
      }
      else if(iInsert->iScene != iScene)
      {
        return false;
      }
      return true;
    }

    virtual unsigned int Resolve(Index iDli) override
    {
      auto iFind = std::lower_bound(mIndices.begin(), mIndices.end(), iDli, [](const Entry& idx, Index iDli) {
        return idx.iDli < iDli;
      });
      DALI_ASSERT_ALWAYS(iFind != mIndices.end());
      return iFind->iScene;
    }

  private:
    struct Entry
    {
      unsigned int iDli;
      unsigned int iScene;

      bool operator<(const Entry& other) const
      {
        return iDli < other.iDli;
      }
    };
    std::vector<Entry> mIndices;
  } mapper(nodes->Size());
  ParseNodesInternal(nodes, index, parents, params, mapper);

  auto& scene = params.output.mScene;
  for(size_t i0 = 0, i1 = scene.GetNodeCount(); i0 < i1; ++i0)
  {
    for(auto& c : scene.GetNode(i0)->mConstraints)
    {
      c.mSourceIdx = mapper.Resolve(c.mSourceIdx);
    }
  }
}

void DliLoader::Impl::ParseNodesInternal(const TreeNode* const nodes, Index index, std::vector<Index>& inOutParentStack, LoadParams& params, IIndexMapper& mapper)
{
  // Properties that may be resolved from a JSON value with ReadInt() -- or default to 0.
  struct IndexProperty
  {
    ResourceType::Value type;
    const TreeNode*     source;
    Index&              target;
  };
  std::vector<IndexProperty> resourceIds;
  resourceIds.reserve(4);

  if(auto node = GetNthChild(nodes, index))
  {
    NodeDefinition nodeDef;
    nodeDef.mParentIdx = inOutParentStack.empty() ? INVALID_INDEX : inOutParentStack.back();

    // name
    ReadString(node->GetChild(NAME), nodeDef.mName);

    // transform
    ReadModelTransform(node, nodeDef.mOrientation, nodeDef.mPosition, nodeDef.mScale);

    // Reads the size of the node.
    //
    // * It can be given as 'size' or 'bounds'.
    // * The sdk saves the 'size' as a vector2 in some cases.
    // * To avoid size related issues the following code attemps
    //   to read the 'size/bounds' as a vector3 first, if it's
    //   not successful then reads it as a vector2.
    ReadVector(node->GetChild("size"), nodeDef.mSize.AsFloat(), 3) ||
      ReadVector(node->GetChild("size"), nodeDef.mSize.AsFloat(), 2) ||
      ReadVector(node->GetChild("bounds"), nodeDef.mSize.AsFloat(), 3) ||
      ReadVector(node->GetChild("bounds"), nodeDef.mSize.AsFloat(), 2);

    // visibility
    ReadBool(node->GetChild("visible"), nodeDef.mIsVisible);

    // type classification
    if(auto eCustomization = node->GetChild("customization")) // customization
    {
      std::string tag;
      if(ReadString(eCustomization->GetChild("tag"), tag))
      {
        nodeDef.mCustomization.reset(new NodeDefinition::CustomizationDefinition{tag});
      }
    }
    else // something renderable maybe
    {
      std::unique_ptr<NodeDefinition::Renderable> renderable;
      ModelNode*                                  modelNode = nullptr; // no ownership, aliasing renderable for the right type.

      const TreeNode* eRenderable = nullptr;
      if((eRenderable = node->GetChild("model")))
      {
        // check for mesh before allocating - this can't be missing.
        auto eMesh = eRenderable->GetChild("mesh");
        if(!eMesh)
        {
          ExceptionFlinger(ASSERT_LOCATION) << "node " << nodeDef.mName << ": Missing mesh definition.";
        }

        modelNode = new ModelNode();
        renderable.reset(modelNode);

        resourceIds.push_back({ResourceType::Mesh, eMesh, modelNode->mMeshIdx});
      }
      else if((eRenderable = node->GetChild("arc")))
      {
        // check for mesh before allocating - this can't be missing.
        auto eMesh = eRenderable->GetChild("mesh");
        if(!eMesh)
        {
          ExceptionFlinger(ASSERT_LOCATION) << "node " << nodeDef.mName << ": Missing mesh definition.";
        }

        auto arcNode = new ArcNode;
        renderable.reset(arcNode);
        modelNode = arcNode;

        resourceIds.push_back({ResourceType::Mesh, eMesh, arcNode->mMeshIdx});

        ReadArcField(eRenderable, *arcNode);
      }

      if(renderable) // process common properties of all renderables + register payload
      {
        // shader
        renderable->mShaderIdx = 0;
        auto eShader           = eRenderable->GetChild("shader");
        resourceIds.push_back({ResourceType::Shader, eShader, renderable->mShaderIdx});

        // color
        if(modelNode)
        {
          modelNode->mMaterialIdx = 0; // must offer default of 0
          auto eMaterial          = eRenderable->GetChild("material");
          resourceIds.push_back({ResourceType::Material, eMaterial, modelNode->mMaterialIdx});

          if(!ReadColorCodeOrColor(eRenderable, modelNode->mColor, params.input.mConvertColorCode))
          {
            ReadColorCodeOrColor(node, modelNode->mColor, params.input.mConvertColorCode);
          }
        }

        nodeDef.mRenderable = std::move(renderable);
      }
    }

    // Resolve ints - default to 0 if undefined
    auto& output = params.output;
    for(auto& idRes : resourceIds)
    {
      Index iCheck = 0;
      switch(idRes.type)
      {
        case ResourceType::Shader:
          iCheck = output.mResources.mShaders.size();
          break;

        case ResourceType::Mesh:
          iCheck = output.mResources.mMeshes.size();
          break;

        case ResourceType::Material:
          iCheck = output.mResources.mMaterials.size();
          break;

        default:
          ExceptionFlinger(ASSERT_LOCATION) << "node " << index << ": Invalid resource type: " << idRes.type << " (Programmer error)";
      }

      if(!idRes.source)
      {
        idRes.target = 0;
      }
      else if(idRes.source->GetType() != TreeNode::INTEGER)
      {
        ExceptionFlinger(ASSERT_LOCATION) << "node " << index << ": Invalid " << GetResourceTypeName(idRes.type) << " index type.";
      }
      else
      {
        idRes.target = idRes.source->GetInteger();
      }

      if(idRes.target >= iCheck)
      {
        ExceptionFlinger(ASSERT_LOCATION) << "node " << index << ": " << GetResourceTypeName(idRes.type) << " index " << idRes.target << " out of bounds (" << iCheck << ").";
      }
    }
    resourceIds.clear();

    // Extra properties
    if(auto eExtras = node->GetChild("extras"))
    {
      auto& extras = nodeDef.mExtras;
      extras.reserve(eExtras->Size());

      NodeDefinition::Extra e;
      for(auto i0 = eExtras->CBegin(), i1 = eExtras->CEnd(); i0 != i1; ++i0)
      {
        auto eExtra = *i0;
        e.mKey      = eExtra.first;
        if(e.mKey.empty())
        {
          mOnError(FormatString("node %d: empty string is invalid for name of extra %d; ignored.",
                                index,
                                extras.size()));
          continue;
        }

        e.mValue = ReadPropertyValue(eExtra.second);
        if(e.mValue.GetType() == Property::Type::NONE)
        {
          mOnError(FormatString("node %d: failed to interpret value of extra '%s' : %s; ignored.",
                                index,
                                e.mKey.c_str(),
                                eExtra.second.GetString()));
        }
        else
        {
          auto iInsert = std::lower_bound(extras.begin(), extras.end(), e);
          if(iInsert != extras.end() && iInsert->mKey == e.mKey)
          {
            mOnError(FormatString("node %d: extra '%s' already defined; overriding with %s.",
                                  index,
                                  e.mKey.c_str(),
                                  eExtra.second.GetString()));
            *iInsert = std::move(e);
          }
          else
          {
            extras.insert(iInsert, e);
          }
        }
      }
    }

    // Constraints
    if(auto eConstraints = node->GetChild("constraints"))
    {
      auto& constraints = nodeDef.mConstraints;
      constraints.reserve(eConstraints->Size());

      ConstraintDefinition cDef;
      for(auto i0 = eConstraints->CBegin(), i1 = eConstraints->CEnd(); i0 != i1; ++i0)
      {
        auto eConstraint = *i0;
        if(!ReadIndex(&eConstraint.second, cDef.mSourceIdx))
        {
          mOnError(FormatString("node %d: node ID %s for constraint %d is invalid; ignored.",
                                index,
                                eConstraint.second.GetString(),
                                constraints.size()));
        }
        else
        {
          cDef.mProperty = eConstraint.first;

          auto iInsert = std::lower_bound(constraints.begin(), constraints.end(), cDef);
          if(iInsert != constraints.end() && *iInsert == cDef)
          {
            mOnError(FormatString("node %d: constraint %s@%d already defined; ignoring.",
                                  index,
                                  cDef.mProperty.c_str(),
                                  cDef.mSourceIdx));
          }
          else
          {
            constraints.insert(iInsert, cDef);
          }
        }
      }
    }

    // Determine index for mapping
    const unsigned int myIndex = output.mScene.GetNodeCount();
    if(!mapper.Map(index, myIndex))
    {
      mOnError(FormatString("node %d: error mapping dli index %d: node has multiple parents. Ignoring subtree."));
      return;
    }

    // if the node is a bone in a skeletal animation, it will have the inverse bind pose matrix.
    Matrix invBindMatrix{false};
    if(ReadVector(node->GetChild("inverseBindPoseMatrix"), invBindMatrix.AsFloat(), 16u)) // TODO: more robust error checking?
    {
      mInverseBindMatrices[myIndex] = invBindMatrix;
    }

    // Register nodeDef
    auto rawDef = output.mScene.AddNode(std::make_unique<NodeDefinition>(std::move(nodeDef)));
    if(rawDef) // NOTE: no ownership. Guaranteed to stay in scope.
    {
      // ...And only then parse children.
      if(auto children = node->GetChild("children"))
      {
        inOutParentStack.push_back(myIndex);

        rawDef->mChildren.reserve(children->Size());

        uint32_t iChild = 0;
        for(auto j0 = children->CBegin(), j1 = children->CEnd(); j0 != j1; ++j0, ++iChild)
        {
          auto& child = (*j0).second;
          if(child.GetType() == TreeNode::INTEGER)
          {
            ParseNodesInternal(nodes, child.GetInteger(), inOutParentStack, params, mapper); // child object is created in scene definition.
          }
          else
          {
            ExceptionFlinger(ASSERT_LOCATION) << "node " << index << ", child " << iChild << ": invalid index type.";
          }
        }

        inOutParentStack.pop_back();
      }
      else if(rawDef->mCustomization)
      {
        mOnError(FormatString("node %d: not an actual customization without children.", index));
      }

      if(auto proc = params.input.mNodePropertyProcessor) // optional processing
      {
        // WARNING: constraint IDs are not resolved at this point.
        Property::Map nodeData;
        ParseProperties(*node, nodeData);
        proc(*rawDef, std::move(nodeData), mOnError);
      }
    }
    else
    {
      ExceptionFlinger(ASSERT_LOCATION) << "Node " << index << ": name already used.";
    }
  }
}

void DliLoader::Impl::ParseAnimations(const TreeNode* tnAnimations, LoadParams& params)
{
  auto& definitions = params.output.mAnimationDefinitions;
  definitions.reserve(definitions.size() + tnAnimations->Size());

  for(TreeNode::ConstIterator iAnim = tnAnimations->CBegin(), iAnimEnd = tnAnimations->CEnd();
      iAnim != iAnimEnd;
      ++iAnim)
  {
    const TreeNode&     tnAnim = (*iAnim).second;
    AnimationDefinition animDef;
    ReadString(tnAnim.GetChild(NAME), animDef.mName);

    auto       iFind     = std::lower_bound(definitions.begin(), definitions.end(), animDef, [](const AnimationDefinition& ad0, const AnimationDefinition& ad1) {
      return ad0.mName < ad1.mName;
    });
    const bool overwrite = iFind != definitions.end() && iFind->mName == animDef.mName;
    if(overwrite)
    {
      mOnError(FormatString("Pre-existing animation with name '%s' is being overwritten.", animDef.mName.c_str()));
    }

    // Duration -- We need something that animated properties' delay / duration can
    // be expressed as a multiple of; 0 won't work. This is small enough (i.e. shorter
    // than our frame delay) to not be restrictive WRT replaying. If anything needs
    // to occur more frequently, then Animations are likely not your solution anyway.
    animDef.mDuration = AnimationDefinition::MIN_DURATION_SECONDS;
    if(!ReadFloat(tnAnim.GetChild("duration"), animDef.mDuration))
    {
      mOnError(FormatString("Animation '%s' fails to define '%s', defaulting to %f.",
                            animDef.mName.c_str(),
                            "duration",
                            animDef.mDuration));
    }

    // Get loop count - # of playbacks. Default is once. 0 means repeat indefinitely.
    animDef.mLoopCount = 1;
    if(ReadInt(tnAnim.GetChild("loopCount"), animDef.mLoopCount) &&
       animDef.mLoopCount < 0)
    {
      animDef.mLoopCount = 0;
    }

    std::string endAction;
    if(ReadString(tnAnim.GetChild("endAction"), endAction))
    {
      if("BAKE" == endAction)
      {
        animDef.mEndAction = Animation::BAKE;
      }
      else if("DISCARD" == endAction)
      {
        animDef.mEndAction = Animation::DISCARD;
      }
      else if("BAKE_FINAL" == endAction)
      {
        animDef.mEndAction = Animation::BAKE_FINAL;
      }
    }

    if(ReadString(tnAnim.GetChild("disconnectAction"), endAction))
    {
      if("BAKE" == endAction)
      {
        animDef.mDisconnectAction = Animation::BAKE;
      }
      else if("DISCARD" == endAction)
      {
        animDef.mDisconnectAction = Animation::DISCARD;
      }
      else if("BAKE_FINAL" == endAction)
      {
        animDef.mDisconnectAction = Animation::BAKE_FINAL;
      }
    }

    if(const TreeNode* tnProperties = tnAnim.GetChild("properties"))
    {
      animDef.mProperties.reserve(tnProperties->Size());
      for(TreeNode::ConstIterator iProperty = tnProperties->CBegin(), iPropertyEnd = tnProperties->CEnd();
          iProperty != iPropertyEnd;
          ++iProperty)
      {
        const TreeNode& tnProperty = (*iProperty).second;

        AnimatedProperty animProp;
        if(!ReadString(tnProperty.GetChild("node"), animProp.mNodeName))
        {
          mOnError(FormatString("Animation '%s': Failed to read the 'node' tag.", animDef.mName.c_str()));
          continue;
        }

        if(!ReadString(tnProperty.GetChild("property"), animProp.mPropertyName))
        {
          mOnError(FormatString("Animation '%s': Failed to read the 'property' tag", animDef.mName.c_str()));
          continue;
        }

        // these are the defaults
        animProp.mTimePeriod.delaySeconds    = 0.f;
        animProp.mTimePeriod.durationSeconds = animDef.mDuration;
        if(!ReadTimePeriod(tnProperty.GetChild("timePeriod"), animProp.mTimePeriod))
        {
          mOnError(FormatString("Animation '%s': timePeriod missing in Property #%d: defaulting to %f.",
                                animDef.mName.c_str(),
                                animDef.mProperties.size(),
                                animProp.mTimePeriod.durationSeconds));
        }

        std::string alphaFunctionValue;
        if(ReadString(tnProperty.GetChild("alphaFunction"), alphaFunctionValue))
        {
          animProp.mAlphaFunction = GetAlphaFunction(alphaFunctionValue);
        }

        if(const TreeNode* tnKeyFramesBin = tnProperty.GetChild("keyFramesBin"))
        {
          DALI_ASSERT_ALWAYS(!animProp.mPropertyName.empty() && "Animation must specify a property name");

          std::ifstream binAniFile;
          std::string   animationFilename;
          if(ReadString(tnKeyFramesBin->GetChild(URL), animationFilename))
          {
            std::string animationFullPath = params.input.mAnimationsPath + animationFilename;
            binAniFile.open(animationFullPath, std::ios::binary);
            if(binAniFile.fail())
            {
              ExceptionFlinger(ASSERT_LOCATION) << "Failed to open animation data '" << animationFullPath << "'";
            }
          }

          int byteOffset = 0;
          ReadInt(tnKeyFramesBin->GetChild("byteOffset"), byteOffset);
          DALI_ASSERT_ALWAYS(byteOffset >= 0);

          binAniFile.seekg(byteOffset, std::ios::beg);

          int numKeys = 0;
          ReadInt(tnKeyFramesBin->GetChild("numKeys"), numKeys);
          DALI_ASSERT_ALWAYS(numKeys >= 0);

          animProp.mKeyFrames = KeyFrames::New();

          //In binary animation file only is saved the position, rotation, scale and blend shape weight keys.
          //so, if it is vector3 we assume is position or scale keys, if it is vector4 we assume is rotation,
          // otherwise are blend shape weight keys.
          // TODO support for binary header with size information
          Property::Type propType = Property::FLOAT; // assume blend shape weights
          if(animProp.mPropertyName == "orientation")
          {
            propType = Property::VECTOR4;
          }
          else if((animProp.mPropertyName == "position") || (animProp.mPropertyName == "scale"))
          {
            propType = Property::VECTOR3;
          }

          //alphafunction is reserved for future implementation
          // NOTE: right now we're just using AlphaFunction::LINEAR.
          unsigned char dummyAlphaFunction;

          float           progress;
          Property::Value propValue;
          for(int key = 0; key < numKeys; key++)
          {
            binAniFile.read(reinterpret_cast<char*>(&progress), sizeof(float));
            if(propType == Property::VECTOR3)
            {
              Vector3 value;
              binAniFile.read(reinterpret_cast<char*>(value.AsFloat()), sizeof(float) * 3);
              propValue = Property::Value(value);
            }
            else if(propType == Property::VECTOR4)
            {
              Vector4 value;
              binAniFile.read(reinterpret_cast<char*>(value.AsFloat()), sizeof(float) * 4);
              propValue = Property::Value(Quaternion(value));
            }
            else
            {
              float value;
              binAniFile.read(reinterpret_cast<char*>(&value), sizeof(float));
              propValue = Property::Value(value);
            }

            binAniFile.read(reinterpret_cast<char*>(&dummyAlphaFunction), sizeof(unsigned char));

            animProp.mKeyFrames.Add(progress, propValue, AlphaFunction::LINEAR);
          }
        }
        else if(const TreeNode* tnKeyFrames = tnProperty.GetChild("keyFrames"))
        {
          DALI_ASSERT_ALWAYS(!animProp.mPropertyName.empty() && "Animation must specify a property name");
          animProp.mKeyFrames = KeyFrames::New();

          float progress = 0.0f;
          for(auto i0 = tnKeyFrames->CBegin(), i1 = tnKeyFrames->CEnd(); i1 != i0; ++i0)
          {
            const TreeNode::KeyNodePair& kfKeyChild = *i0;
            bool                         readResult = ReadFloat(kfKeyChild.second.GetChild("progress"), progress);
            DALI_ASSERT_ALWAYS(readResult && "Key frame entry must have 'progress'");

            const TreeNode* tnValue = kfKeyChild.second.GetChild("value");
            DALI_ASSERT_ALWAYS(tnValue && "Key frame entry must have 'value'");

            // For the "orientation" property, convert from Vector4 -> Rotation value
            // This work-around is preferable to a null-pointer exception in the DALi update thread
            Property::Value propValue(ReadPropertyValue(*tnValue));
            if(propValue.GetType() == Property::VECTOR4 &&
               animProp.mPropertyName == "orientation")
            {
              Vector4 v;
              propValue.Get(v);
              propValue = Property::Value(Quaternion(v.w, v.x, v.y, v.z));
            }

            AlphaFunction kfAlphaFunction(AlphaFunction::DEFAULT);
            std::string   alphaFuncStr;
            if(ReadString(kfKeyChild.second.GetChild("alphaFunction"), alphaFuncStr))
            {
              kfAlphaFunction = GetAlphaFunction(alphaFuncStr);
            }

            animProp.mKeyFrames.Add(progress, propValue, kfAlphaFunction);
          }
        }
        else
        {
          const TreeNode* tnValue = tnProperty.GetChild("value");
          if(tnValue)
          {
            animProp.mValue.reset(new AnimatedProperty::Value{ReadPropertyValue(*tnValue)});
            ReadBool(tnProperty.GetChild("relative"), animProp.mValue->mIsRelative);
          }
          else
          {
            mOnError(FormatString("Property '%s' fails to define target value.",
                                  animProp.mPropertyName.c_str()));
          }
        }

        animDef.mProperties.push_back(std::move(animProp));
      }
    }

    if(overwrite)
    {
      *iFind = std::move(animDef);
    }
    else
    {
      iFind = definitions.insert(iFind, std::move(animDef));
    }

    if(auto proc = params.input.mAnimationPropertyProcessor) // optional processing
    {
      Property::Map map;
      ParseProperties(tnAnim, map);
      proc(animDef, std::move(map), mOnError);
    }
  }
}

void DliLoader::Impl::ParseAnimationGroups(const Toolkit::TreeNode* tnAnimationGroups, LoadParams& params)
{
  auto& animGroups = params.output.mAnimationGroupDefinitions;

  int numGroups = 0;
  for(auto iGroups = tnAnimationGroups->CBegin(), iGroupsEnd = tnAnimationGroups->CEnd();
      iGroups != iGroupsEnd;
      ++iGroups, ++numGroups)
  {
    const auto& tnGroup = *iGroups;
    auto        tnName  = tnGroup.second.GetChild(NAME);
    std::string groupName;
    if(!tnName || !ReadString(tnName, groupName))
    {
      mOnError(FormatString("Failed to get the name for the Animation group %d; ignoring.", numGroups));
      continue;
    }

    auto iFind = std::lower_bound(animGroups.begin(), animGroups.end(), groupName, [](const AnimationGroupDefinition& group, const std::string& name) {
      return group.mName < name;
    });
    if(iFind != animGroups.end() && iFind->mName == groupName)
    {
      mOnError(FormatString("Animation group with name '%s' already exists; new entries will be merged.", groupName.c_str()));
    }
    else
    {
      iFind = animGroups.insert(iFind, AnimationGroupDefinition{});
    }

    iFind->mName = groupName;

    auto tnAnims = tnGroup.second.GetChild("animations");
    if(tnAnims && tnAnims->Size() > 0)
    {
      auto& anims = iFind->mAnimations;
      anims.reserve(anims.size() + tnAnims->Size());
      for(auto iAnims = tnAnims->CBegin(), iAnimsEnd = tnAnims->CEnd(); iAnims != iAnimsEnd; ++iAnims)
      {
        anims.push_back((*iAnims).second.GetString());
      }
    }
  }
}

void DliLoader::Impl::GetCameraParameters(std::vector<CameraParameters>& cameras) const
{
  if(const TreeNode* jsonCameras = mParser.GetRoot()->GetChild("cameras"))
  {
    cameras.resize(jsonCameras->Size());
    auto iCamera = cameras.begin();
    for(auto i0 = jsonCameras->CBegin(), i1 = jsonCameras->CEnd(); i0 != i1; ++i0)
    {
      auto& jsonCamera = (*i0).second;

      ReadFloat(jsonCamera.GetChild("fov"), iCamera->yFov);
      ReadFloat(jsonCamera.GetChild("near"), iCamera->zNear);
      ReadFloat(jsonCamera.GetChild("far"), iCamera->zFar);
      if(ReadVector(jsonCamera.GetChild("orthographic"), iCamera->orthographicSize.AsFloat(), 4u))
      {
        iCamera->isPerspective = false;
      }

      if(auto jsonMatrix = jsonCamera.GetChild("matrix"))
      {
        ReadVector(jsonMatrix, iCamera->matrix.AsFloat(), 16u);
      }

      ++iCamera;
    }
  }
}

void DliLoader::Impl::GetLightParameters(std::vector<LightParameters>& lights) const
{
  if(const TreeNode* jsonLights = mParser.GetRoot()->GetChild("lights"))
  {
    lights.resize(jsonLights->Size());
    auto iLight = lights.begin();
    for(auto i0 = jsonLights->CBegin(), i1 = jsonLights->CEnd(); i0 != i1; ++i0)
    {
      auto& jsonLight = (*i0).second;
      if(!ReadVector(jsonLight.GetChild("matrix"), iLight->transform.AsFloat(), 16))
      {
        mOnError(
          FormatString("Failed to parse light %d - \"matrix\" child with 16 floats expected.\n",
                       std::distance(jsonLights->CBegin(), i0)));
        continue;
      }

      int shadowMapSize = 0;
      if(ReadInt(jsonLight.GetChild(SHADOW_MAP_SIZE), shadowMapSize) && shadowMapSize < 0)
      {
        mOnError(
          FormatString("Failed to parse light %d - %s has an invalid value.",
                       std::distance(jsonLights->CBegin(), i0),
                       SHADOW_MAP_SIZE));
        continue;
      }
      iLight->shadowMapSize = shadowMapSize;

      float orthoSize = 0.f;
      if(ReadFloat(jsonLight.GetChild(ORTHOGRAPHIC_SIZE), orthoSize) &&
         (orthoSize < .0f || std::isnan(orthoSize) || std::isinf(orthoSize)))
      {
        mOnError(
          FormatString("Failed to parse light %d - %s has an invalid value.",
                       std::distance(jsonLights->CBegin(), i0),
                       ORTHOGRAPHIC_SIZE));
        continue;
      }
      iLight->orthographicSize = orthoSize;

      if((iLight->shadowMapSize > 0) != (iLight->orthographicSize > .0f))
      {
        mOnError(FormatString(
          "Light %d: Both shadow map size and orthographic size must be set for shadows to work.",
          std::distance(jsonLights->CBegin(), i0)));
      }

      if(!ReadVector(jsonLight.GetChild("color"), iLight->color.AsFloat(), 3)) // color is optional
      {
        iLight->color = Vector3::ONE; // default to white
      }

      if(!ReadFloat(jsonLight.GetChild("intensity"), iLight->intensity)) // intensity is optional
      {
        iLight->intensity = 1.0f; // default to 1.0
      }

      if(!ReadFloat(jsonLight.GetChild("shadowIntensity"), iLight->shadowIntensity)) // intensity is optional
      {
        iLight->shadowIntensity = 1.0f; // default to 1.0
      }

      ++iLight;
    }
  }
}

} // namespace SceneLoader
} // namespace Dali
