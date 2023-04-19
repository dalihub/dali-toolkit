#ifndef DALI_SCENE3D_LOADER_SCENE_DEFINITION_H_
#define DALI_SCENE3D_LOADER_SCENE_DEFINITION_H_
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector4.h>
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/customization.h>
#include <dali-scene3d/public-api/loader/node-definition.h>
#include <dali-scene3d/public-api/loader/string-callback.h>
#include <dali-scene3d/public-api/loader/utils.h>

namespace Dali::Scene3D::Loader
{
class MatrixStack;

/*
 * @brief Intermediate representation of a scene with functionality required to
 *  create DALi objects (Actors, Renderers) from it.
 */
class DALI_SCENE3D_API SceneDefinition
{
public: // TYPES
  using NodePredicate     = std::function<bool(const NodeDefinition&)>;
  using NodeConsumer      = std::function<void(NodeDefinition&)>;
  using ConstNodeConsumer = std::function<void(const NodeDefinition&)>;

public: // METHODS
  SceneDefinition();
  SceneDefinition(SceneDefinition&& other);
  ~SceneDefinition();

  /*
   * @brief Registers a scene root node.
   * @return The index of the scene root node *registration*.
   */
  Index AddRootNode(Index iNode);

  /*
   * @return the list of scene root node IDs in the order of their loading.
   */
  const std::vector<Index>& GetRoots() const;

  /*
   * @brief Removes scene root registration at the given index @a iRoot.
   * @note @a iRoot is the index of the registration (i.e. into the vector returned by GetRoots()),
   *  not of the node.
   */
  void RemoveRootNode(Index iRoot);

  /*
   * @return The number of node( definition)s in the scene.
   */
  uint32_t GetNodeCount() const;

  /*
   * @return Const pointer to the node (definition) at the given index.
   */
  const NodeDefinition* GetNode(Index iNode) const;

  /*
   * @return Pointer to the node (definition) at the given index.
   */
  NodeDefinition* GetNode(Index iNode);

  /*
   * @brief Traverses the scene starting from the node at the given index into
   *  nodes, using the given customization @a choices and the visitor @a v.
   */
  void Visit(Index iNode, const Customization::Choices& choices, NodeDefinition::IVisitor& v);

  /*
   * @brief Traverses the scene starting from the node at the given index into
   *  nodes, using the given customization @a choices and the visitor @a v.
   */
  void Visit(Index iNode, const Customization::Choices& choices, NodeDefinition::IConstVisitor& v) const;

  /*
   * @brief Counts the references to meshes, shaders, materials that nodes in
   *  the scene are holding, writing the results into @a refCounts.
   * @note @a refCounts' entries must have the correct size. Use ResourceBundle::GetRefCounter().
   */
  void CountResourceRefs(Index iNode, const Customization::Choices& choices, ResourceRefCounts& refCounts) const;

  /*
   * @brief Given a bundle of @a resources that are loaded, and customization
   *  @a choices, this method traverses the scene, creating the ModelNodes and renderers
   *  from node definitions.
   * @return Handle to the root node.
   */
  ModelNode CreateNodes(Index iNode, const Customization::Choices& choices, NodeDefinition::CreateParams& params);

  /*
   * @brief Creates / update a registry of mappings from customization tags to
   *  a lists of names of customizable nodes under each tag, and the number of
   *  options. If @a outMissingChoices was specified, each tag that it encounters
   *  in the scene but not in @a choices, will be registered on it with the default
   *  choice of 0.
   */
  void GetCustomizationOptions(const Customization::Choices& choices,
                               Customization::Map&           outCustomizationOptions,
                               Customization::Choices*       outMissingChoices) const;

  /*
   * @brief Attempts to add @a nodeDef to the end of nodes, and its index to the end of
   *  its parent's list of children (if iParent != NodeDefinition::INVALID_PARENT).
   * @return If the operation was successful - which requires nodeDef->name to be unique -
   *  a pointer to the stored node definition; nullptr otherwise.
   */
  NodeDefinition* AddNode(std::unique_ptr<NodeDefinition>&& nodeDef);

  /*
   * @brief Moves the node to some other parent and / or to a different index.
   * @return Whether the operation was successful.
   * @note This is currently breaking an assumption of never having a child of a node at a lower
   *  index as that of the node itself, due to the fact that we're only changing parent ids (and
   *  entries into the vector of children of node definitions), to save the complication of having
   *  to move about, and offset indices to, everything past the reparented node. This should be
   *  sufficient AT LEAST AS LONG AS we recreate the SceneDefinition when loading the scene; if
   *  we ever needed to serialize it, we should ensure correct ordering of nodes.
   */
  bool ReparentNode(const std::string& name, const std::string& newParentName, Index siblingOrder);

  /*
   * @brief Removes a node with the given name, including all of its children, and updating
   *  the indices on all remaining node definitions.
   * @return Whether the operation was successful.
   */
  bool RemoveNode(const std::string& name);

  /*
   * @brief Builds the model matrix stack for the node at the given @a index.
   * @note It only pushes new matrices; does not require the stack to be empty (or cares if it was not).
   */
  void GetNodeModelStack(Index index, MatrixStack& model) const;

  /*
   * @brief Attempts to find the definition of a node with the given @a name. Only upon
   *  success, and if @a outIndex is non-null, the index of the node is written to it.
   * @return Pointer to the node definition; nullptr if not found.
   * @note No ownership transfer.
   */
  NodeDefinition* FindNode(const std::string& name, Index* outIndex = nullptr);

  /*
   * @brief Attempts to find the definition of a node with the given @a name. Only upon
   *  success, and if @a outIndex is non-null, the index of the node is written to it.
   * @return Pointer to the node definition; nullptr if not found.
   * @note No ownership transfer.
   */
  const NodeDefinition* FindNode(const std::string& name, Index* outIndex = nullptr) const;

  /*
   * @return The index of the given NodeDefinition@ a node, or -1 if the node definition
   *  was not found.
   */
  Index FindNodeIndex(const NodeDefinition& node) const;

  /*
   * @brief Calls @a consumer with up to @a limit NodeDefinitions that evaluate to true
   *  with @a predicate.
   * @note A @a limit value of 0 means no limit.
   */
  void FindNodes(NodePredicate predicate, NodeConsumer consumer, unsigned int limit = 0);

  /*
   * @brief Calls @a consumer with up to @a limit NodeDefinitions that evaluate to true
   *  with @a predicate.
   * @note A @a limit value of 0 means no limit.
   */
  void FindNodes(NodePredicate predicate, ConstNodeConsumer consumer, unsigned int limit = 0) const;

  /*
   * @brief Applies constraints from the given requests.
   */
  void ApplyConstraints(Actor&                           root,
                        std::vector<ConstraintRequest>&& constrainables,
                        StringCallback                   onError = DefaultErrorCallback) const;

  /*
   * @brief Ensures that there is no overlap between shaders used by nodes that have
   *  meshes skinned to different skeletons.
   */
  void EnsureUniqueSkinningShaderInstances(ResourceBundle& resources) const;

  /*
   * @brief Performs the configuration of the given skinning shaders with the given skeleton
   *   This means that the absolute transforms of the joints are calculated and set as one of
   *   the uniforms in the mat4 @b uBone array (in depth first traversal). Further, the following
   *   are created:<br />
   *   - a @b jointMatrix property on each joint Actor;<br />
   *   - constraint from the Actor's local position and rotation (and if it has a @e joint
   *     parent, the jointMatrix of the parent) to its @b jointMatrix property;<br />
   *   - a constraint from the the Actor's @b jointMatrix property to the related entry in
   *     the shader's @b uBone property;<br />
   *   This ensures the automatic update of the skeletal animation, should any of the joints'
   *   transform changes, by whatever means.
   * @return The success of the operations. Error messages will be posted to the optional
   *   @a onError callback.
   * @note A maximum of SkinningDetails::MAX_JOINTS joints per skeleton are supported at the moment.
   * @note Even if multiple skinned meshes use the same skinning shader, the correct number
   *   of separate instances need to be declared in the .dli to avoid clashing uniform
   *   definitions and constraints.
   */
  void ConfigureSkinningShaders(const ResourceBundle&                             resources,
                                Actor                                             root,
                                std::vector<SkinningShaderConfigurationRequest>&& requests) const;

  /*
   * @brief Ensures there is no two meshes with blend shapes sharing the same shader.
   */
  void EnsureUniqueBlendShapeShaderInstances(ResourceBundle& resources) const;

  /**
   * @brief Performs the configuration of the given blend shapes.
   *
   * For each node with blend shapes it registers into the actor the weights properties for each morph target
   * and some needed uniforms into the shader.
   * 
   * @param[in] root The root actor.
   * @param[in] requests The requests to configure blend shapes.
   * @param[in] resources The resources bundle. Meshes need to be accessed to configure the blend shapes.
   * @param[in] onError The error callback.
   */
  bool ConfigureBlendshapeShaders(const ResourceBundle&                               resources,
                                  Actor                                               root,
                                  std::vector<BlendshapeShaderConfigurationRequest>&& requests,
                                  StringCallback                                      onError = DefaultErrorCallback) const;

  SceneDefinition& operator=(SceneDefinition&& other);

private: // METHODS
  bool FindNode(const std::string& name, std::unique_ptr<NodeDefinition>** result);

private:                                               // DATA
  std::vector<std::unique_ptr<NodeDefinition>> mNodes; // size unknown up front (may discard nodes).
  std::vector<Index>                           mRootNodeIds;
};

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADER_SCENE_DEFINITION_H_
