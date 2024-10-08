#ifndef DALI_SCENE3D_LOADER_NODE_DEFINITION_H_
#define DALI_SCENE3D_LOADER_NODE_DEFINITION_H_
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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector4.h>
#include <functional>
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/customization.h>
#include <dali-scene3d/public-api/loader/matrix-stack.h>
#include <dali-scene3d/public-api/loader/resource-bundle.h>
#include <dali-scene3d/public-api/loader/shader-manager.h>
#include <dali-scene3d/public-api/model-components/model-node.h>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
class ViewProjection;

/**
 * @brief Interface to report (const) resource ids to.
 * @SINCE_2_0.7
 */
class DALI_SCENE3D_API IResourceReceiver
{
public:
  virtual ~IResourceReceiver() = default;

  virtual void Register(ResourceType::Value type, Index id) = 0;
};

/**
 * @brief Interface to report modifiable resource ids to.
 * @SINCE_2_0.7
 * @note These are supposed to be transient. Obviously, the references collected
 *  this way must not outlive the objects that they came from.
 */
class DALI_SCENE3D_API IResourceReflector
{
public:
  virtual ~IResourceReflector() = default;

  virtual void Reflect(ResourceType::Value type, Index& id) = 0;
};

/**
 * @brief Intermediate representation for a constraint that shall be
 *  set up after the Actors were created. The target of the constraint
 *  is the node definition that carries it.
 * @SINCE_2_0.7
 */
struct DALI_SCENE3D_API ConstraintDefinition
{
  std::string mProperty;  ///< name of the property to constrain.
  Index       mSourceIdx; ///< index of the node to serve as the source of the constraint.

  bool operator<(const ConstraintDefinition& other) const
  {
    return mProperty < other.mProperty;
  }

  bool operator==(const ConstraintDefinition& other) const
  {
    return mSourceIdx == other.mSourceIdx && mProperty == other.mProperty;
  }

  bool operator!=(const ConstraintDefinition& other) const
  {
    return !operator==(other);
  }
};

struct DALI_SCENE3D_API Transforms
{
  MatrixStack           modelStack;
  const ViewProjection& viewProjection;
};

/**
 * @brief Information about a skeleton and the shader that needs to be configured with it.
 * @SINCE_2_0.7
 * @note Multiple skeletons shalt not share the same shader.
 */
struct DALI_SCENE3D_API SkinningShaderConfigurationRequest
{
  Index          mSkeletonIdx;
  Shader         mShader;
  ModelPrimitive mPrimitive;

  bool operator<(const SkinningShaderConfigurationRequest& other) const
  {
    return mShader < other.mShader || (mShader == other.mShader && mPrimitive < other.mPrimitive);
  }
};

/**
 * @brief Needed to configure blend shape properties.
 * @SINCE_2_0.7
 */
struct DALI_SCENE3D_API BlendshapeShaderConfigurationRequest
{
  std::string    mNodeName;
  Index          mMeshIdx;
  Shader         mShader;
  ModelPrimitive mPrimitive;

  bool operator<(const BlendshapeShaderConfigurationRequest& other) const
  {
    return mShader < other.mShader || (mShader == other.mShader && mPrimitive < other.mPrimitive);
  }
};

/**
 * @brief Request for creating a constraint, output from NodeDefinition::OnCreate.
 * @SINCE_2_0.7
 */
struct DALI_SCENE3D_API ConstraintRequest
{
  const ConstraintDefinition* const mConstraint; ///< Definition of the constraint to create.
  Actor                             mTarget;     ///< Target of the constraint.
};

/**
 * @brief Defines a node, consisting of a name, a transform, a size, a list of child nodes,
 *  and slots for customization and rendering logic, which are mutually exclusive in the
 *  current implementation.
 * @SINCE_2_0.7
 */
struct DALI_SCENE3D_API NodeDefinition
{
public: // TYPES
  using Vector = std::vector<NodeDefinition>;

  struct CreateParams
  {
  public: // input
    ResourceBundle&                         mResources;
    Transforms&                             mXforms;
    Dali::Scene3D::Loader::ShaderManagerPtr mShaderManager;

  public: // output
    std::vector<ConstraintRequest>                    mConstrainables;
    std::vector<SkinningShaderConfigurationRequest>   mSkinnables;
    std::vector<BlendshapeShaderConfigurationRequest> mBlendshapeRequests;
  };

  class DALI_SCENE3D_API Renderable
  {
  public: // DATA
    Index mShaderIdx = INVALID_INDEX;

  public: // METHODS
    virtual ~Renderable() = default;

    virtual bool GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const;
    virtual void RegisterResources(IResourceReceiver& receiver) const;
    virtual void ReflectResources(IResourceReflector& reflector);
    virtual void OnCreate(const NodeDefinition& nodeDefinition, CreateParams& params, ModelNode& node) const;

  protected:
    void CreateRenderer(const NodeDefinition& nodeDefinition, CreateParams& params, Geometry& geometry, ModelNode& node) const;
  };

  struct CustomizationDefinition
  {
    std::string mTag;

    Index GetChildId(const Customization::Choices& choices, const NodeDefinition& node)
    {
      auto choice = choices.Get(mTag);
      return std::min(choice != Customization::NONE ? choice : 0,
                      static_cast<Index>(node.mChildren.size() - 1));
    }
  };

  class IVisitor
  {
  public:
    virtual void Start(NodeDefinition& n)  = 0;
    virtual void Finish(NodeDefinition& n) = 0;

  protected:
    ~IVisitor() = default; // deliberately non-virtual these are transient objects and we don't want to pay for the vtable.
  };

  class IConstVisitor
  {
  public:
    virtual void Start(const NodeDefinition& n)  = 0;
    virtual void Finish(const NodeDefinition& n) = 0;

  protected:
    ~IConstVisitor() = default; // deliberately non-virtual these are transient objects and we don't want to pay for the vtable.
  };

  struct Extra
  {
    std::string     mKey;
    Property::Value mValue;

    bool operator<(const Extra& other) const
    {
      return mKey < other.mKey;
    }
  };

public: // METHODS
  /**
   * @brief Creates a ModelNode from this definition only.
   * @SINCE_2_0.7
   * @note Not recursive.
   */
  ModelNode CreateModelNode(CreateParams& params);

  /**
   * @brief Gets local space matrix of this node
   * @SINCE_2_1.32
   * @return Matrix of local space.
   */
  Matrix GetLocalSpace() const;

  /**
   * @brief Retrieves minimum and maximum position of this node in local space.
   * @SINCE_2_1.32
   * @param[in] resources ResourceBundle that contains mesh information of this node.
   * @param[out] min Minimum position of the mesh of this node.
   * @param[out] max Maximum position of the mesh of this node.
   * @return true If the node has mesh.
   */
  bool GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const;

  /**
   * @brief Retrieves Scale Factor uniform name.
   *
   * This uniform name can be used to change scale factor for ibl.
   * @SINCE_2_1.32
   * @return std::string_view of the scale factor uniform name.
   */
  static std::string_view GetIblScaleFactorUniformName();

  /**
   * @brief Retrieves ibl Ydirection uniform name.
   *
   * This uniform name can be used to flip y direction of ibl in shader.
   * @SINCE_2_1.32
   * @return std::string_view of the YDirection uniform name.
   */
  static std::string_view GetIblYDirectionUniformName();

  /**
   * @brief Retrieves ibl MaxLod uniform name.
   *
   * This uniform name can be used to set max lod of ibl in shader.
   * @SINCE_2_2.19
   * @return std::string_view of the Max Lod uniform name.
   */
  static std::string_view GetIblMaxLodUniformName();

public: // DATA
  static const char* ORIGINAL_MATRIX_PROPERTY_NAME;

  std::string mName;
  uint32_t    mNodeId = INVALID_INDEX;

  Vector3    mPosition    = Vector3::ZERO;
  Quaternion mOrientation = Quaternion::IDENTITY;
  Vector3    mScale       = Vector3::ONE;
  Vector3    mSize        = Vector3::ONE;

  bool mIsVisible = true;

  std::vector<std::unique_ptr<Renderable>> mRenderables;
  std::unique_ptr<CustomizationDefinition> mCustomization;
  std::vector<Extra>                       mExtras;
  std::vector<ConstraintDefinition>        mConstraints;

  std::vector<Index> mChildren;
  Index              mParentIdx = INVALID_INDEX;
};

class DALI_SCENE3D_API ModelRenderable : public NodeDefinition::Renderable
{
public: // DATA
  Vector4 mColor       = Color::WHITE;
  Index   mMeshIdx     = INVALID_INDEX;
  Index   mMaterialIdx = INVALID_INDEX;

public: // METHODS
  bool GetExtents(const ResourceBundle& resources, Vector3& min, Vector3& max) const override;
  void RegisterResources(IResourceReceiver& receiver) const override;
  void ReflectResources(IResourceReflector& reflector) override;
  void OnCreate(const NodeDefinition& nodeDefinition, NodeDefinition::CreateParams& params, ModelNode& node) const override;
};

/**
 * @brief Parameters for an Arc node.
 * @SINCE_2_0.7
 */
class DALI_SCENE3D_API ArcRenderable : public ModelRenderable
{
public: // DATA
  bool  mAntiAliasing      = true;
  int   mArcCaps           = 0;
  float mStartAngleDegrees = .0f;
  float mEndAngleDegrees   = .0f;
  float mRadius            = .0f;

public: // METHODS
  static void GetEndVectorWithDiffAngle(float startAngle, float endAngle, Vector2& endVector);

  void OnCreate(const NodeDefinition& nodeDefinition, NodeDefinition::CreateParams& params, ModelNode& node) const override;
};

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_NODE_DEFINITION_H_
