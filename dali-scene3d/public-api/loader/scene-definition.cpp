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

// CLASS HEADER
#include <dali-scene3d/public-api/loader/scene-definition.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/common/map-wrapper.h>
#include <dali/public-api/animation/constraints.h>

// INTERNAL
#include <dali-scene3d/internal/graphics/builtin-shader-extern-gen.h>
#include <dali-scene3d/public-api/loader/blend-shape-details.h>
#include <dali-scene3d/public-api/loader/skinning-details.h>
#include <dali-scene3d/public-api/loader/utils.h>

//#define DEBUG_SCENE_DEFINITION
//#define DEBUG_JOINTS

#if defined(DEBUG_SCENE_DEFINITION) || defined(DEBUG_JOINTS)
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

#define LOGD(x) DEBUG_ONLY(printf x; printf("\n"); fflush(stdout))

namespace Dali::Scene3D::Loader
{
namespace
{
const char* JOINT_MATRIX{"jointMatrix"};

const std::map<Property::Type, Constraint (*)(Actor&, Property::Index)>& GetConstraintFactory()
{
  static const std::map<Property::Type, Constraint (*)(Actor&, Property::Index)> sConstraintFactory = {
    {Property::Type::BOOLEAN,
     [](Actor& a, Property::Index i) {
       return Constraint::New<bool>(a, i, [](bool& current, const PropertyInputContainer& inputs) {
         current = inputs[0]->GetBoolean();
       });
     }},
    {Property::Type::INTEGER,
     [](Actor& a, Property::Index i) {
       return Constraint::New<int>(a, i, [](int& current, const PropertyInputContainer& inputs) {
         current = inputs[0]->GetInteger();
       });
     }},
    {Property::Type::FLOAT,
     [](Actor& a, Property::Index i) {
       return Constraint::New<float>(a, i, EqualToConstraint());
     }},
    {Property::Type::VECTOR2,
     [](Actor& a, Property::Index i) {
       return Constraint::New<Vector2>(a, i, EqualToConstraint());
     }},
    {Property::Type::VECTOR3,
     [](Actor& a, Property::Index i) {
       return Constraint::New<Vector3>(a, i, EqualToConstraint());
     }},
    {Property::Type::VECTOR4,
     [](Actor& a, Property::Index i) {
       return Constraint::New<Vector4>(a, i, EqualToConstraint());
     }},
    {Property::Type::MATRIX,
     [](Actor& a, Property::Index i) {
       return Constraint::New<Matrix>(a, i, EqualToConstraint());
     }},
    {Property::Type::MATRIX3,
     [](Actor& a, Property::Index i) {
       return Constraint::New<Matrix3>(a, i, EqualToConstraint());
     }},
    {Property::Type::ROTATION,
     [](Actor& a, Property::Index i) {
       return Constraint::New<Quaternion>(a, i, EqualToConstraint());
     }},
  };
  return sConstraintFactory;
}

struct ResourceReflector : IResourceReflector
{
  Index* iMesh   = nullptr;
  Index* iShader = nullptr;

  void Reflect(ResourceType::Value type, Index& id)
  {
    switch(type)
    {
      case ResourceType::Shader:
        DALI_ASSERT_ALWAYS(!iShader && "Shader index already assigned!");
        iShader = &id;
        break;

      case ResourceType::Mesh:
        DALI_ASSERT_ALWAYS(!iMesh && "Mesh index already assigned!");
        iMesh = &id;
        break;

      default: // Other resource types are not relevant to the problem at hand.
        break;
    }
  }
};

#ifdef DEBUG_JOINTS

Shader sJointDebugShader;
int    sNumScenes = 0;

void EnsureJointDebugShaderCreated()
{
  if(0 == sNumScenes)
  {
    sJointDebugShader = Shader::New(SHADER_SCENE3D_JOINT_DEBUG_VERT, SHADER_SCENE3D_JOINT_DEBUG_FRAG);
  }
  ++sNumScenes;
}

void AddJointDebugVisual(Actor aJoint)
{
  Property::Map attribs;
  attribs["aPosition"] = Property::Type::VECTOR3;
  attribs["aColor"]    = Property::Type::FLOAT;

  PropertyBuffer vbo = PropertyBuffer::New(attribs);

  struct Vertex
  {
    Vector3 pos;
    float   color;
  } vertices[] = {
    {Vector3::ZERO, .999f + .999f * 256.f + .999f * 256.f * 256.f},
    {Vector3::XAXIS, .999f},
    {Vector3::YAXIS, .999f * 256.f},
    {Vector3::ZAXIS, .999f * 256.f * 256.f},
  };

  vbo.SetData(&vertices, std::extent<decltype(vertices)>::value);

  uint16_t indices[] = {0, 1, 0, 2, 0, 3};

  Geometry geo = Geometry::New();
  geo.AddVertexBuffer(vbo);
  geo.SetIndexBuffer(indices, std::extent<decltype(indices)>::value);
  geo.SetType(Geometry::LINES);

  Renderer r = Renderer::New(geo, sJointDebugShader);
  aJoint.AddRenderer(r);

  aJoint.SetVisible(true);
}
#endif //DEBUG_JOINTS

class ActorCreatorVisitor : public NodeDefinition::IVisitor
{
public:
  ActorCreatorVisitor(NodeDefinition::CreateParams& params)
  : mCreationContext(params)
  {
  }

  void Start(NodeDefinition& n)
  {
    mCreationContext.mXforms.modelStack.Push(n.GetLocalSpace());

    Actor a = n.CreateActor(mCreationContext);
    if(!mActorStack.empty())
    {
      mActorStack.back().Add(a);
    }
    else
    {
      mRoot = a;
    }
    mActorStack.push_back(a);
  }

  void Finish(NodeDefinition& n)
  {
    mActorStack.pop_back();
    mCreationContext.mXforms.modelStack.Pop();
  }

  Actor GetRoot() const
  {
    return mRoot;
  }

private:
  NodeDefinition::CreateParams& mCreationContext;
  std::vector<Actor>            mActorStack;
  Actor                         mRoot;
};

bool IsAncestor(const SceneDefinition& scene, Index ancestor, Index node, Index rootHint = INVALID_INDEX)
{
  bool isAncestor = false;
  while(node != rootHint && !isAncestor)
  {
    node       = scene.GetNode(node)->mParentIdx;
    isAncestor = ancestor == node;
  }
  return isAncestor;
}

void InsertUniqueSorted(std::vector<Index>& data, Index value)
{
  auto iInsert = std::lower_bound(data.begin(), data.end(), value);
  if(iInsert == data.end() || *iInsert != value)
  {
    data.insert(iInsert, value);
  }
}

void RemoveFromSorted(std::vector<Index>& data, Index value)
{
  auto iRemove = std::lower_bound(data.begin(), data.end(), value);
  if(iRemove != data.end() && *iRemove == value)
  {
    data.erase(iRemove);
  }
}

Property::Index ConfigureJointMatrix(Actor actor, Actor ancestor, Property::Index propJointMatrix)
{
  Actor parent = actor.GetParent();
  if(parent != ancestor)
  {
    propJointMatrix = ConfigureJointMatrix(parent, ancestor, propJointMatrix);
  }

  auto myPropJointMatrix = actor.GetPropertyIndex(JOINT_MATRIX);
  if(myPropJointMatrix == Property::INVALID_INDEX)
  {
    myPropJointMatrix     = actor.RegisterProperty(JOINT_MATRIX, Matrix{false});
    Constraint constraint = Constraint::New<Matrix>(actor, propJointMatrix, [](Matrix& output, const PropertyInputContainer& inputs) {
      Matrix jointMatrix{false};
      jointMatrix.SetTransformComponents(Vector3::ONE, inputs[0]->GetQuaternion(), inputs[1]->GetVector3());

      Matrix::Multiply(output, jointMatrix, inputs[2]->GetMatrix());
    });
    constraint.AddSource(Source{actor, Actor::Property::ORIENTATION});
    constraint.AddSource(Source{actor, Actor::Property::POSITION});
    constraint.AddSource(Source{parent, propJointMatrix});
    constraint.Apply();
  }

  return myPropJointMatrix;
}

void SortAndDeduplicateSkinningRequests(std::vector<SkinningShaderConfigurationRequest>& requests)
{
  // Sort requests by shaders.
  std::sort(requests.begin(), requests.end());

  // Remove duplicates.
  auto   i           = requests.begin();
  auto   iEnd        = requests.end();
  Shader s           = i->mShader;
  Index  skeletonIdx = i->mSkeletonIdx;
  ++i;
  do
  {
    // Multiple identical shader instances are removed.
    while(i != iEnd && i->mShader == s)
    {
      // Cannot have multiple skeletons input to the same shader.
      // NOTE: DliModel now makes sure this doesn't happen.
      DALI_ASSERT_ALWAYS(i->mSkeletonIdx == skeletonIdx &&
                         "Skinning shader must not be shared between different skeletons.");

      i->mShader = Shader();
      ++i;
    }

    if(i == iEnd)
    {
      break;
    }
    s           = i->mShader;
    skeletonIdx = i->mSkeletonIdx;
    ++i;
  } while(true);

  requests.erase(std::remove_if(requests.begin(), requests.end(), [](const SkinningShaderConfigurationRequest& sscr) {
                   return !sscr.mShader;
                 }),
                 requests.end());
}

void ConfigureBoneMatrix(const Matrix& ibm, Actor joint, Shader& shader, Index& boneIdx)
{
  // Register bone transform on shader.
  char propertyNameBuffer[32];
  snprintf(propertyNameBuffer, sizeof(propertyNameBuffer), "%s[%d]", Skinning::BONE_UNIFORM_NAME, boneIdx);
  DALI_ASSERT_DEBUG(shader.GetPropertyIndex(propertyNameBuffer) == Property::INVALID_INDEX);
  auto propBoneXform = shader.RegisterProperty(propertyNameBuffer, Matrix{false});

  // Constrain bone matrix to joint transform.
  Constraint constraint = Constraint::New<Matrix>(shader, propBoneXform, [ibm](Matrix& output, const PropertyInputContainer& inputs) {
    Matrix::Multiply(output, ibm, inputs[0]->GetMatrix());
  });

  auto propJointMatrix = joint.GetPropertyIndex(JOINT_MATRIX);
  constraint.AddSource(Source{joint, propJointMatrix});
  constraint.Apply();

  ++boneIdx;
}

template<class Visitor, class SceneDefinition>
void VisitInternal(Index iNode, const Customization::Choices& choices, Visitor& v, SceneDefinition& sd)
{
  auto& node = *sd.GetNode(iNode);
  v.Start(node);

  if(node.mCustomization)
  {
    if(!node.mChildren.empty())
    {
      auto  choice = choices.Get(node.mCustomization->mTag);
      Index i      = std::min(choice != Customization::NONE ? choice : 0, static_cast<Index>(node.mChildren.size() - 1));
      sd.Visit(node.mChildren[i], choices, v);
    }
  }
  else
  {
    for(auto i : node.mChildren)
    {
      sd.Visit(i, choices, v);
    }
  }

  v.Finish(node);
}

} // namespace

SceneDefinition::SceneDefinition()
{
  mNodes.reserve(128);

#ifdef DEBUG_JOINTS
  EnsureJointDebugShaderCreated();
#endif
}

SceneDefinition::SceneDefinition(SceneDefinition&& other)
: mNodes(std::move(other.mNodes)),
  mRootNodeIds(std::move(other.mRootNodeIds))
{
#ifdef DEBUG_JOINTS
  EnsureJointDebugShaderCreated();
#endif
}

SceneDefinition::~SceneDefinition()
{
#ifdef DEBUG_JOINTS
  --sNumScenes;
  if(sNumScenes == 0)
  {
    sJointDebugShader = Shader();
  }
#endif
}

uint32_t Dali::Scene3D::Loader::SceneDefinition::AddRootNode(Index iNode)
{
  if(iNode < mNodes.size())
  {
    uint32_t result = mRootNodeIds.size();
    mRootNodeIds.push_back(iNode);
    return result;
  }
  else
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to add new root with node " << iNode << " -- index out of bounds.";
    return -1;
  }
}

const std::vector<Index>& SceneDefinition::GetRoots() const
{
  return mRootNodeIds;
}

void SceneDefinition::RemoveRootNode(Index iRoot)
{
  if(iRoot < mRootNodeIds.size())
  {
    mRootNodeIds.erase(mRootNodeIds.begin() + iRoot);
  }
  else
  {
    ExceptionFlinger(ASSERT_LOCATION) << "Failed to remove root " << iRoot << " -- index out of bounds.";
  }
}

uint32_t SceneDefinition::GetNodeCount() const
{
  return mNodes.size();
}

const NodeDefinition* SceneDefinition::GetNode(Index iNode) const
{
  return mNodes[iNode].get();
}

NodeDefinition* SceneDefinition::GetNode(Index iNode)
{
  if(iNode != Scene3D::Loader::INVALID_INDEX && iNode < mNodes.size())
  {
    return mNodes[iNode].get();
  }
  return nullptr;
}

void SceneDefinition::Visit(Index iNode, const Customization::Choices& choices, NodeDefinition::IVisitor& v)
{
  VisitInternal(iNode, choices, v, *this);
}

void SceneDefinition::Visit(Index iNode, const Customization::Choices& choices, NodeDefinition::IConstVisitor& v) const
{
  VisitInternal(iNode, choices, v, *this);
}

void SceneDefinition::CountResourceRefs(Index iNode, const Customization::Choices& choices, ResourceRefCounts& refCounts) const
{
  struct RefCounter : IResourceReceiver
  {
    ResourceRefCounts* refCounts;

    void Register(ResourceType::Value type, Index id)
    {
      ++(*refCounts)[type][id];
    }
  };

  struct : NodeDefinition::IConstVisitor
  {
    RefCounter counter;

    void Start(const NodeDefinition& n)
    {
      for(auto& renderable : n.mRenderables)
      {
        renderable->RegisterResources(counter);
      }
    }

    void Finish(const NodeDefinition& n)
    {
    }

  } refCounterVisitor;
  refCounterVisitor.counter.refCounts = &refCounts;

  Visit(iNode, choices, refCounterVisitor);
}

Actor SceneDefinition::CreateNodes(Index iNode, const Customization::Choices& choices, NodeDefinition::CreateParams& params)
{
  ActorCreatorVisitor actorCreatorVisitor(params);

  Visit(iNode, choices, actorCreatorVisitor);

  return actorCreatorVisitor.GetRoot();
}

void SceneDefinition::GetCustomizationOptions(const Customization::Choices& choices,
                                              Customization::Map&           outCustomizationOptions,
                                              Customization::Choices*       outMissingChoices) const
{
  struct : NodeDefinition::IConstVisitor
  {
    const Customization::Choices* choices;        // choices that we know about.
    Customization::Map*           options;        // tags are registered here. NO OWNERSHIP.
    Customization::Choices*       missingChoices; // tags will be registered with the default 0. NO OWNERSHIP.

    void Start(const NodeDefinition& n)
    {
      if(n.mCustomization)
      {
        const std::string& tag = n.mCustomization->mTag;
        if(missingChoices != nullptr && choices->Get(tag) == Customization::NONE)
        {
          missingChoices->Set(tag, 0);
        }

        auto customization = options->Get(tag);
        if(!customization)
        {
          customization = options->Set(tag, {});
        }
        customization->nodes.push_back(n.mName);
        customization->numOptions = std::max(customization->numOptions,
                                             static_cast<uint32_t>(n.mChildren.size()));
      }
    }

    void Finish(const NodeDefinition& n)
    {
    }

  } customizationRegistrationVisitor;
  customizationRegistrationVisitor.choices        = &choices;
  customizationRegistrationVisitor.options        = &outCustomizationOptions;
  customizationRegistrationVisitor.missingChoices = outMissingChoices;

  for(auto i : mRootNodeIds)
  {
    Visit(i, choices, customizationRegistrationVisitor);
  }
}

NodeDefinition* SceneDefinition::AddNode(std::unique_ptr<NodeDefinition>&& nodeDef)
{
  // add next index (to which we're about to push) as a child to the designated parent, if any.
  if(nodeDef->mParentIdx != INVALID_INDEX)
  {
    mNodes[nodeDef->mParentIdx]->mChildren.push_back(mNodes.size());
  }

  mNodes.push_back(std::move(nodeDef));

  return mNodes.back().get();
}

bool SceneDefinition::ReparentNode(const std::string& name, const std::string& newParentName, Index siblingOrder)
{
  LOGD(("reparenting %s to %s @ %d", name.c_str(), newParentName.c_str(), siblingOrder));

  std::unique_ptr<NodeDefinition>* nodePtr      = nullptr;
  std::unique_ptr<NodeDefinition>* newParentPtr = nullptr;
  if(!FindNode(name, &nodePtr) || !FindNode(newParentName, &newParentPtr))
  {
    return false;
  }

  auto& node  = *nodePtr;
  auto  iNode = std::distance(mNodes.data(), nodePtr);

  DEBUG_ONLY(auto dumpNode = [](NodeDefinition const& n) {
    std::ostringstream stream;
    stream << n.mName << " (" << n.mParentIdx << "):";
    for(auto i : n.mChildren)
    {
      stream << i << ", ";
    }
    LOGD(("%s", stream.str().c_str()));
  };)

  // Remove node from children of previous parent (if any).
  if(node->mParentIdx != INVALID_INDEX)
  {
    LOGD(("old parent:"));
    DEBUG_ONLY(dumpNode(*mNodes[node->mParentIdx]);)

    auto& children = mNodes[node->mParentIdx]->mChildren;
    children.erase(std::remove(children.begin(), children.end(), iNode), children.end());

    DEBUG_ONLY(dumpNode(*mNodes[node->mParentIdx]);)
  }

  // Register node to new parent.
  LOGD(("new parent:"));
  DEBUG_ONLY(dumpNode(**newParentPtr);)
  auto& children = (*newParentPtr)->mChildren;
  if(siblingOrder > children.size())
  {
    siblingOrder = children.size();
  }
  children.insert(children.begin() + siblingOrder, 1, iNode);
  DEBUG_ONLY(dumpNode(**newParentPtr);)

  // Update parent index.
  LOGD(("node:"));
  DEBUG_ONLY(dumpNode(*node);)
  auto iParent     = std::distance(mNodes.data(), newParentPtr);
  node->mParentIdx = iParent;
  DEBUG_ONLY(dumpNode(*node);)
  return true;
}

bool SceneDefinition::RemoveNode(const std::string& name)
{
  std::unique_ptr<NodeDefinition>* node = nullptr;
  if(!FindNode(name, &node))
  {
    return false;
  }

  // Reset node def pointers recursively.
  auto&                                                 thisNodes = mNodes;
  unsigned int                                          numReset  = 0;
  std::function<void(std::unique_ptr<NodeDefinition>&)> resetFn =
    [&thisNodes, &resetFn, &numReset](std::unique_ptr<NodeDefinition>& nd) {
      LOGD(("resetting %d", &nd - thisNodes.data()));
      for(auto i : nd->mChildren)
      {
        resetFn(thisNodes[i]);
      }
      nd.reset();
      ++numReset;
    };

  resetFn(*node);

  // Gather indices of dead nodes into a vector which we sort on insertion.
  std::vector<Index> offsets;
  offsets.reserve(numReset);
  for(auto& n : mNodes)
  {
    if(!n)
    {
      offsets.push_back(std::distance(mNodes.data(), &n));
    }
  }

  // Erase dead nodes as they don't have to be processed anymore.
  mNodes.erase(std::remove(mNodes.begin(), mNodes.end(), decltype(mNodes)::value_type()), mNodes.end());

  // Offset all indices (parent and child) by the index they'd sort into in offsets.
  enum
  {
    INDEX_FOR_REMOVAL = INVALID_INDEX
  };
  auto offsetter = [&offsets](Index& i) {
    auto iFind = std::lower_bound(offsets.begin(), offsets.end(), i);
    if(iFind != offsets.end() && *iFind == i)
    {
      LOGD(("marking %d for removal.", i));
      i = INDEX_FOR_REMOVAL;
      return false;
    }
    else
    {
      auto distance = std::distance(offsets.begin(), iFind);
      if(distance > 0)
      {
        LOGD(("offsetting %d by %d.", i, distance));
        i -= distance;
      }
      return true;
    }
  };

  for(auto& nd : mNodes)
  {
    bool parentOffsetResult = offsetter(nd->mParentIdx);
    DALI_ASSERT_ALWAYS(parentOffsetResult); // since nodes were recursively removed, we should not be finding invalid parents at this point.

    auto& children = nd->mChildren;
    for(auto i0 = children.begin(), i1 = children.end(); i0 != i1; ++i0)
    {
      offsetter(*i0);
    }

    children.erase(std::remove(children.begin(), children.end(), INDEX_FOR_REMOVAL), children.end());
  }

  return true;
}

void SceneDefinition::GetNodeModelStack(Index index, MatrixStack& model) const
{
  auto&                    thisNodes  = mNodes;
  std::function<void(int)> buildStack = [&model, &thisNodes, &buildStack](int i) {
    auto node = thisNodes[i].get();
    if(node->mParentIdx != INVALID_INDEX)
    {
      buildStack(node->mParentIdx);
    }
    model.Push(node->GetLocalSpace());
  };
  buildStack(index);
}

NodeDefinition* SceneDefinition::FindNode(const std::string& name, Index* outIndex)
{
  auto iBegin = mNodes.begin();
  auto iEnd   = mNodes.end();
  auto iFind  = std::find_if(iBegin, iEnd, [&name](const std::unique_ptr<NodeDefinition>& nd) {
    return nd->mName == name;
  });

  auto result = iFind != iEnd ? iFind->get() : nullptr;
  if(result && outIndex)
  {
    *outIndex = std::distance(iBegin, iFind);
  }
  return result;
}

const NodeDefinition* SceneDefinition::FindNode(const std::string& name, Index* outIndex) const
{
  auto iBegin = mNodes.begin();
  auto iEnd   = mNodes.end();
  auto iFind  = std::find_if(iBegin, iEnd, [&name](const std::unique_ptr<NodeDefinition>& nd) {
    return nd->mName == name;
  });

  auto result = iFind != iEnd ? iFind->get() : nullptr;
  if(result && outIndex)
  {
    *outIndex = std::distance(iBegin, iFind);
  }
  return result;
}

Index SceneDefinition::FindNodeIndex(const NodeDefinition& node) const
{
  auto iBegin = mNodes.begin();
  auto iEnd   = mNodes.end();
  auto iFind  = std::find_if(iBegin, iEnd, [&node](const std::unique_ptr<NodeDefinition>& n) {
    return n.get() == &node;
  });
  return iFind != iEnd ? std::distance(iBegin, iFind) : INVALID_INDEX;
}

void SceneDefinition::FindNodes(NodePredicate predicate, NodeConsumer consumer, unsigned int limit)
{
  unsigned int n = 0;
  for(auto& defp : mNodes)
  {
    if(predicate(*defp))
    {
      consumer(*defp);
      ++n;
      if(n == limit)
      {
        break;
      }
    }
  }
}

void SceneDefinition::FindNodes(NodePredicate predicate, ConstNodeConsumer consumer, unsigned int limit) const
{
  unsigned int n = 0;
  for(auto& defp : mNodes)
  {
    if(predicate(*defp))
    {
      consumer(*defp);
      ++n;
      if(n == limit)
      {
        break;
      }
    }
  }
}

void SceneDefinition::ApplyConstraints(Actor&                           root,
                                       std::vector<ConstraintRequest>&& constrainables,
                                       StringCallback                   onError) const
{
  for(auto& cr : constrainables)
  {
    auto&           nodeDef    = mNodes[cr.mConstraint->mSourceIdx];
    auto            sourceName = nodeDef->mName.c_str();
    Property::Index iTarget    = cr.mTarget.GetPropertyIndex(cr.mConstraint->mProperty);
    if(iTarget != Property::INVALID_INDEX)
    {
      auto propertyType = cr.mTarget.GetPropertyType(iTarget);
      auto iFind        = GetConstraintFactory().find(propertyType);
      if(iFind == GetConstraintFactory().end())
      {
        onError(FormatString("node '%s': Property '%s' has unsupported type '%s'; ignored.",
                             sourceName,
                             cr.mConstraint->mProperty.c_str(),
                             PropertyTypes::GetName(propertyType)));
        continue;
      }

      Constraint constraint = iFind->second(cr.mTarget, iTarget);

      Actor source = root.FindChildByName(nodeDef->mName);
      if(!source)
      {
        auto targetName = cr.mTarget.GetProperty(Actor::Property::NAME).Get<std::string>();
        onError(FormatString("node '%s': Failed to locate constraint source %s@%s; ignored.",
                             sourceName,
                             cr.mConstraint->mProperty.c_str(),
                             targetName.c_str()));
        continue;
      }
      else if(source == cr.mTarget)
      {
        onError(FormatString("node '%s': Cyclic constraint definition for property '%s'; ignored.",
                             sourceName,
                             cr.mConstraint->mProperty.c_str()));
        continue;
      }

      Property::Index iSource = source.GetPropertyIndex(cr.mConstraint->mProperty);
      constraint.AddSource(Source{source, iSource});
      constraint.Apply();
    }
    else
    {
      auto targetName = cr.mTarget.GetProperty(Actor::Property::NAME).Get<std::string>();
      onError(FormatString("node '%s': Failed to create constraint for property %s@%s; ignored.",
                           sourceName,
                           cr.mConstraint->mProperty.c_str(),
                           targetName.c_str()));
    }
  }
}

void SceneDefinition::ConfigureSkeletonJoints(uint32_t iRoot, const SkeletonDefinition::Vector& skeletons, Actor root) const
{
  // 1, For each skeleton, for each joint, walk upwards until we reach mNodes[iRoot]. If we do, record +1
  // to the refcount of each node we have visited, in our temporary registry. Those with refcount 1
  // are the leaves, while the most descendant node with the highest refcount is the root of the skeleton.
  std::map<Index, std::vector<Index>> rootsJoints;
  std::vector<Index>                  path;
  path.reserve(16);
  for(auto& s : skeletons)
  {
    std::map<uint32_t, uint32_t> jointRefs;
    for(auto& j : s.mJoints)
    {
      auto nodeIdx = j.mNodeIdx;
      do // Traverse upwards and record each node we have visited until we reach the scene root.
      {
        path.push_back(nodeIdx);
        if(nodeIdx == iRoot)
        {
          break;
        }
        auto node = GetNode(nodeIdx);
        nodeIdx   = node->mParentIdx;
      } while(nodeIdx != INVALID_INDEX);

      if(nodeIdx == iRoot) // If the joint is in the correct scene, increment the reference count for all visited nodes.
      {
        for(auto i : path)
        {
          ++jointRefs[i];
        }
      }

      path.clear();
    }

    // Only record the skeleton if we have encountered the root of the current scene.
    if(jointRefs.empty())
    {
      continue;
    }

    Index    root   = s.mRootNodeIdx;
    uint32_t maxRef = 0;
    auto     iFind  = jointRefs.find(root);
    if(iFind != jointRefs.end())
    {
      maxRef = iFind->second;
    }

    std::vector<Index> joints;
    for(auto& j : jointRefs) // NOTE: jointRefs are sorted, so joints will also be.
    {
      // The most descendant node with the highest ref count is the root of the skeleton.
      if(j.second > maxRef || (j.second == maxRef && IsAncestor(*this, root, j.first, iRoot)))
      {
        maxRef = j.second;

        RemoveFromSorted(joints, root);
        root = j.first;
      }
      else if(j.second == 1) // This one's a leaf.
      {
        InsertUniqueSorted(joints, j.first);
      }
    }

    // Merge skeletons that share the same root.
    auto& finalJoints = rootsJoints[root];
    for(auto j : joints)
    {
      if(std::find_if(finalJoints.begin(), finalJoints.end(), [this, j, root](Index jj) {
           return IsAncestor(*this, j, jj, root);
         }) != finalJoints.end())
      {
        continue; // if the joint is found to be an ancestor of another joint already registered, move on.
      }

      auto i = j;
      while(i != root) // See if the current joint is a better leaf, i.e. descended from another leaf - which we'll then remove.
      {
        auto node = GetNode(i);
        i         = node->mParentIdx;

        RemoveFromSorted(finalJoints, i);
      }

      InsertUniqueSorted(finalJoints, j);
    }
  }

  // 2, Merge records where one root joint is descendant of another. Handle leaf node changes - remove previous
  // leaf nodes that now have descendants, and add new ones.
  auto iRoots    = rootsJoints.begin();
  auto iRootsEnd = rootsJoints.end();
  while(iRoots != iRootsEnd)
  {
    auto i      = iRoots->first;
    bool merged = false;
    while(i != iRoot) // Starting with the root joint of the skeleton, traverse upwards.
    {
      auto node = GetNode(i);
      i         = node->mParentIdx;

      auto iFind = rootsJoints.find(i);
      if(iFind != rootsJoints.end()) // Check if we've reached the root of another skeleton.
      {
        // Now find out which leaf of iFind is an ancestor, if any.
        auto iFindLeaf = std::find_if(iFind->second.begin(), iFind->second.end(), [this, iRoots, iFind](Index j) {
          return IsAncestor(*this, j, iRoots->first, iFind->first);
        });
        if(iFindLeaf != iFind->second.end())
        {
          iFind->second.erase(iFindLeaf); // Will no longer be a leaf -- remove it.
        }

        // Merge iRoots with iFind
        auto& targetJoints = iFind->second;
        if(iRoots->second.empty()) // The root is a leaf.
        {
          InsertUniqueSorted(targetJoints, iRoots->first);
        }
        else
          for(auto j : iRoots->second)
          {
            InsertUniqueSorted(targetJoints, j);
          }

        merged = true;
        break; // Traverse no more
      }
    }

    iRoots = merged ? rootsJoints.erase(iRoots) : std::next(iRoots);
  }

  // 3, For each root, register joint matrices and constraints
  for(const auto& r : rootsJoints)
  {
    auto node      = GetNode(r.first);
    auto rootJoint = root.FindChildByName(node->mName);
    DALI_ASSERT_ALWAYS(!!rootJoint);

    DALI_ASSERT_DEBUG(rootJoint.GetPropertyIndex(JOINT_MATRIX) == Property::INVALID_INDEX);
    auto       propJointMatrix = rootJoint.RegisterProperty(JOINT_MATRIX, Matrix{false});
    Constraint constraint      = Constraint::New<Matrix>(rootJoint, propJointMatrix, [](Matrix& output, const PropertyInputContainer& inputs) {
      output.SetTransformComponents(Vector3::ONE, inputs[0]->GetQuaternion(), inputs[1]->GetVector3());
    });
    constraint.AddSource(Source(rootJoint, Actor::Property::ORIENTATION));
    constraint.AddSource(Source(rootJoint, Actor::Property::POSITION));
    constraint.Apply();

    for(const auto j : r.second)
    {
      node       = GetNode(j);
      auto joint = rootJoint.FindChildByName(node->mName);
      ConfigureJointMatrix(joint, rootJoint, propJointMatrix);
    }
  }
}

void SceneDefinition::EnsureUniqueSkinningShaderInstances(ResourceBundle& resources) const
{
  std::map<Index, std::map<Index, std::vector<Index*>>> skinningShaderUsers;
  for(auto& node : mNodes)
  {
    for(auto& renderable : node->mRenderables)
    {
      ResourceReflector reflector;
      renderable->ReflectResources(reflector);

      if(reflector.iMesh)
      {
        const auto& mesh = resources.mMeshes[*reflector.iMesh].first;
        if(mesh.IsSkinned())
        {
          skinningShaderUsers[*reflector.iShader][mesh.mSkeletonIdx].push_back(reflector.iShader);
        }
      }
    }
  }

  // For each shader, and each skeleton using the same shader as the first skeleton,
  // update the shader references (from nodes with skinned meshes) with a new copy of
  // the shader definition from the node using the first skeleton.
  for(auto& users : skinningShaderUsers)
  {
    auto& skeletons    = users.second;
    auto  iterSkeleton = skeletons.begin();
    // skipping the first skeleton.
    ++iterSkeleton;

    resources.mShaders.reserve(resources.mShaders.size() + std::distance(iterSkeleton, skeletons.end()));
    const ShaderDefinition& shaderDef = resources.mShaders[users.first].first;

    while(iterSkeleton != skeletons.end())
    {
      Index iShader = resources.mShaders.size();
      resources.mShaders.push_back({shaderDef, Shader()});

      for(auto& i : iterSkeleton->second)
      {
        *i = iShader;
      }
      ++iterSkeleton;
    }
  }
}

void SceneDefinition::ConfigureSkinningShaders(const ResourceBundle&                             resources,
                                               Actor                                             rootActor,
                                               std::vector<SkinningShaderConfigurationRequest>&& requests) const
{
  if(requests.empty())
  {
    return;
  }

  SortAndDeduplicateSkinningRequests(requests);

  for(auto& request : requests)
  {
    auto& skeleton = resources.mSkeletons[request.mSkeletonIdx];
    if(skeleton.mJoints.empty())
    {
      LOGD(("Skeleton %d has no joints.", request.mSkeletonIdx));
      continue;
    }

    Index boneIdx = 0;
    for(auto& joint : skeleton.mJoints)
    {
      auto  node  = GetNode(joint.mNodeIdx);
      Actor actor = rootActor.FindChildByName(node->mName);
      ConfigureBoneMatrix(joint.mInverseBindMatrix, actor, request.mShader, boneIdx);
    }
  }
}

bool SceneDefinition::ConfigureBlendshapeShaders(const ResourceBundle&                               resources,
                                                 Actor                                               rootActor,
                                                 std::vector<BlendshapeShaderConfigurationRequest>&& requests,
                                                 StringCallback                                      onError) const
{
  if(requests.empty())
  {
    return true;
  }

  // Sort requests by shaders.
  std::sort(requests.begin(), requests.end());

  // Remove duplicates.
  auto   i    = requests.begin();
  auto   iEnd = requests.end();
  Shader s    = i->mShader;
  ++i;
  do
  {
    // Multiple identical shader instances are removed.
    while(i != iEnd && i->mShader == s)
    {
      i->mShader = Shader();
      ++i;
    }

    if(i == iEnd)
    {
      break;
    }
    s = i->mShader;
    ++i;
  } while(true);

  // Configure the rest.
  bool ok = true;

  for(auto& i : requests)
  {
    Index iNode;
    if(FindNode(i.mNodeName, &iNode))
    {
      const auto& node = GetNode(iNode);

      const auto& mesh = resources.mMeshes[i.mMeshIdx];

      if(mesh.first.HasBlendShapes())
      {
        Actor actor = rootActor.FindChildByName(node->mName);

        // Sets the property to be animated.
        BlendShapes::ConfigureProperties(mesh, i.mShader, actor);
      }
    }
  }

  return ok;
}

void SceneDefinition::EnsureUniqueBlendShapeShaderInstances(ResourceBundle& resources) const
{
  std::map<Index, std::map<std::string, std::vector<Index*>>> blendShapeShaderUsers;
  for(auto& node : mNodes)
  {
    for(auto& renderable : node->mRenderables)
    {
      ResourceReflector reflector;
      renderable->ReflectResources(reflector);

      if(reflector.iMesh)
      {
        const auto& mesh = resources.mMeshes[*reflector.iMesh].first;
        if(mesh.HasBlendShapes())
        {
          blendShapeShaderUsers[*reflector.iShader][node->mName].push_back(reflector.iShader);
        }
      }
    }
  }

  for(auto& users : blendShapeShaderUsers)
  {
    resources.mShaders.reserve(resources.mShaders.size() + users.second.size() - 1u);
    const ShaderDefinition& shaderDef = resources.mShaders[users.first].first;

    auto nodesIt    = users.second.begin();
    auto nodesEndIt = users.second.end();
    // skipping the first node.
    ++nodesIt;
    while(nodesIt != nodesEndIt)
    {
      Index iShader = resources.mShaders.size();
      resources.mShaders.push_back({shaderDef, Shader()});

      auto& nodes = *nodesIt;
      for(auto& shader : nodes.second)
      {
        *shader = iShader;
      }
      ++nodesIt;
    }
  }
}

SceneDefinition& SceneDefinition::operator=(SceneDefinition&& other)
{
  SceneDefinition temp(std::move(other));
  std::swap(mNodes, temp.mNodes);
  std::swap(mRootNodeIds, temp.mRootNodeIds);
  return *this;
}

bool SceneDefinition::FindNode(const std::string& name, std::unique_ptr<NodeDefinition>** result)
{
  // We're searching from the end assuming a higher probability of operations targeting
  // recently added nodes. (conf.: root, which is immovable, cannot be removed, and was
  // the first to be added, is index 0.)
  auto iFind = std::find_if(mNodes.rbegin(), mNodes.rend(), [&name](const std::unique_ptr<NodeDefinition>& nd) {
                 return nd->mName == name;
               }).base();

  const bool success = iFind != mNodes.begin();
  if(success && result)
  {
    --iFind;
    *result = &*iFind;
  }

  return success;
}

} // namespace Dali::Scene3D::Loader
