#ifndef __DALI_TOOLKIT_CLUSTER_H__
#define __DALI_TOOLKIT_CLUSTER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/control.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Cluster;
}

class ClusterStyle;

/**
 * Cluster is a container of grouped actors positioned in different cluster styles.
 */
class DALI_IMPORT_API Cluster : public Control
{
public:

  // Custom properties

  static const std::string CLUSTER_ACTOR_DEPTH;                           ///< Property, name "cluster-actor-depth",      type FLOAT

  //Action Names
  static const char* const ACTION_EXPAND;
  static const char* const ACTION_COLLAPSE;
  static const char* const ACTION_TRANSFORM;

public:

  /**
   * Create a Cluster handle; this can be initialised with Cluster::New()
   * Calling member functions with an uninitialised handle is not allowed.
   */
  Cluster();

  /**
   * Copy Constructor.
   */
  Cluster( const Cluster& cluster );

  /**
   * Assignment Operator.
   */
  Cluster& operator=( const Cluster& cluster );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Cluster();

  /**
   * Create the Cluster control with the given style.
   * @param[in] style The style of the cluster
   * @return A handle to the Cluster control.
   */
  static Cluster New( ClusterStyle& style );

  /**
   * Downcast an Object handle to Cluster. If handle points to a Cluster the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle Handle to an object
   * @return handle to a Cluster or an uninitialized handle
   */
  static Cluster DownCast( BaseHandle handle );

  /**
   * Adds a child to the Cluster
   * Will automatically choose a position for the child.
   * @pre The child actor has been initialized.
   * @param[in] child The child to add
   */
  void AddChild( Actor child );

  /**
   * Adds a child to the Cluster
   * User specifies the position for the child.
   * @pre The child actor has been initialized.
   * @param[in] child The child to add
   * @param[in] positionIndex The position for this child
   */
  void AddChild( Actor child, unsigned int positionIndex );

  /**
   * Adds a child to the Cluster to be inserted at a specified
   * depth index.
   * Will automatically choose a position for the child.
   * @pre The child actor has been initialized.
   * @param[in] child The child to add
   * @param[in] index The depth position for this child
   */
  void AddChildAt( Actor child, unsigned int index );

  /**
   * Adds a child to the Cluster to be inserted at a specified
   * depth index.
   * User specifies the position for the child.
   * @pre The child actor has been initialized.
   * @param[in] child The child to add
   * @param[in] positionIndex The position for this child
   * @param[in] index The depth position for this child
   */
  void AddChildAt( Actor child, unsigned int positionIndex, unsigned int index );

  /**
   * Returns a child from the given layout position
   * Note! if there is no child in this layout position this method returns an uninitialized
   * Actor handle
   * @param[in] index The child index in the cluster
   * @return The child that was in the layout position or an uninitialized handle
   */
  Actor GetChildAt( unsigned int index );

  /**
   * Removes a child from the given layout position
   * Note! if there is no child in this layout position this method does nothing
   * @param[in] index The index of the child to remove
   * @return The child that was removed or an uninitialized handle
   */
  Actor RemoveChildAt( unsigned int index );

  /**
   * Expands a child
   * A child will move away from the cluster.
   * @param[in] index The child position index to expand
   */
  void ExpandChild( unsigned int index );

  /**
   * Expands all children
   * All children that have been collapsed will
   * move away from the cluster
   */
  void ExpandAllChildren();

  /**
   * Collapses a child
   * A child that has been expanded will move
   * back to its original positions.
   * @param[in] index The child index to collapse
   * @param[in] front Whether to move child to the front or
   * back of cluster (depth).
   */
  void CollapseChild( unsigned int index, bool front = false );

  /**
   * Collapses all children.
   * All children that have been expanded will move
   * back to their original positions.
   * @param[in] front Whether to move child to the front or
   * back of cluster (depth).
   */
  void CollapseAllChildren( bool front = false );

  /**
   * Transforms Actor from default transform to new transform
   * @param[in] index The child index to move
   * @param[in] position The position to move to
   * @param[in] scale The scale to change to
   * @param[in] rotation The rotation to change to
   * @param[in] alpha The alpha function to use to tween to this transform
   * @param[in] period The duration for this transformation to take
   */
  void TransformChild( unsigned int index, const Vector3& position, const Vector3& scale, const Quaternion& rotation, AlphaFunction alpha, const TimePeriod& period );

  /**
   * Restores Actor to the default transform (based on current style)
   * @param[in] index The child index to move back
   * @param[in] alpha The alpha function to use to tween to this transform
   * @param[in] period The duration for this transformation to take
   * @param[in] front Whether to move child to the front or
   * back of cluster (depth).
   */
  void RestoreChild( unsigned int index, AlphaFunction alpha, const TimePeriod& period, bool front = false );

  /**
   * Sets the background image.
   * @param[in] image The background image.
   */
  void SetBackgroundImage( Actor image );

  /**
   * Sets the title.
   *
   * @param[in] text Title text.
   */
  void SetTitle( Actor text );

  /**
   * Sets the style of the cluster
   * @param[in] style The style of the cluster
   */
  void SetStyle(ClusterStyle style);

  /**
   * Gets the style of the cluster
   * @return style of the cluster
   */
  ClusterStyle GetStyle() const;

  /**
   * Gets the number of children that have been expanded in this cluster.
   * @return the number of children expanded.
   */
  unsigned int GetExpandedCount() const;

  /**
   * Gets the number of children that have been added to this cluster.
   * @return the total number of children.
   */
  unsigned int GetTotalCount() const;

public: // Not intended for application developers

  /**
   * Creates a handle using the Toolkit::Internal implementation.
   * @param[in]  implementation  The Control implementation.
   */
  DALI_INTERNAL Cluster( Internal::Cluster& implementation );

  /**
   * Allows the creation of this Control from an Internal::CustomActor pointer.
   * @param[in]  internal  A pointer to the internal CustomActor.
   */
  explicit DALI_INTERNAL Cluster( Dali::Internal::CustomActor* internal );
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CLUSTER_H__
