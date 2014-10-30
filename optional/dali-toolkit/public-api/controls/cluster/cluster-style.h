#ifndef __DALI_TOOLKIT_CLUSTER_STYLE_H__
#define __DALI_TOOLKIT_CLUSTER_STYLE_H__

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

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/enums.h>

namespace Dali
{

class Actor;

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class ClusterStyle;
class ClusterStyleRandom;
}

class ClusterStyle;

typedef IntrusivePtr<ClusterStyle> ClusterStylePtr; ///< Pointer to a Dali::Toolkit::ClusterStyle object

/**
 * @brief A ClusterStyle describes the constraints which are imposed on the child actors in the cluster.
 */
class DALI_IMPORT_API ClusterStyle : public Dali::BaseHandle
{
public:

  static const unsigned int UNLIMITED_CHILDREN; ///< Constant that represents an unlimited number of children.

public:

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ClusterStyle();

  /**
   * @brief Query the maximum number of children this Style can handle.
   *
   * If return value is UNLIMITED_CHILDREN, then this style has no
   * limit.
   * @return The maximum number of children.
   */
  unsigned int GetMaximumNumberOfChildren() const;

  /**
   * @brief Applies style (position) to child actor over a specified time duration.
   *
   * @param[in] child The child actor to apply
   * @param[in] index The style position index for the actor to transform to.
   * @param[in] alpha The alpha function to use.
   * @param[in] durationSeconds The time period to apply this style.
   */
  void ApplyStyle(Actor child, unsigned int index, AlphaFunction alpha, const TimePeriod& durationSeconds);

  /**
   * @brief Applies style to background actor over a specified time duration.
   *
   * @param[in] background The background actor to apply
   * @param[in] alpha The alpha function to use.
   * @param[in] durationSeconds The time period to apply this style.
   */
  void ApplyStyleToBackground(Actor background, AlphaFunction alpha, const TimePeriod& durationSeconds);

  /**
   * @brief Applies style to title actor over a specified time duration.
   *
   * @param[in] title The title actor to apply
   * @param[in] alpha The alpha function to use.
   * @param[in] durationSeconds The time period to apply this style.
   */
  void ApplyStyleToTitle(Actor title, AlphaFunction alpha, const TimePeriod& durationSeconds);

protected:

  /**
   * @brief Create a new ClusterStyle; Only derived versions are instantiatable.
   */
  ClusterStyle();

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ClusterStyle(Internal::ClusterStyle* internal);
};

/**
 * @brief A ClusterStyle describes the constraints, which are imposed on the child actors in the cluster.
 */
class DALI_IMPORT_API ClusterStyleStandard : public ClusterStyle
{
public:

  /**
   * @brief Cluster Style type.
   */
  enum StyleType
  {
    ClusterStyle1,     ///< Style number 1
    ClusterStyle2,     ///< Style number 2
    ClusterStyle3,     ///< Style number 3
    ClusterStyle4,     ///< Style number 4
    TotalClusterStyles ///< The number of styles
  };

public:

  /**
   * @brief Create an initialized style.
   */
  static ClusterStyleStandard New(StyleType style);

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ClusterStyleStandard(Internal::ClusterStyle* internal);
};

/**
 * @brief A ClusterStyle describes the constraints, which are imposed on the child actors in the cluster.
 */
class DALI_IMPORT_API ClusterStyleRandom : public ClusterStyle
{
public:

  /**
   * @brief Create an initialized style.
   */
  static ClusterStyleRandom New();

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] internal A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ClusterStyleRandom(Internal::ClusterStyle* internal);
};

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_CLUSTER_STYLE_H__
