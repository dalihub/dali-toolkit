#ifndef DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_CONSTRAINTS_H
#define DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_CONSTRAINTS_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/constraint.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class ScrollView;

/// Sets up and owns the Constraints used by the ScrollView class
class ScrollViewConstraints
{
public:

  ScrollViewConstraints() = default;  ///< Default Constructor
  ~ScrollViewConstraints() = default; ///< Default Non-Virtual Destructor

  // Not copyable or moveable
  ScrollViewConstraints(const ScrollViewConstraints&) = delete;
  ScrollViewConstraints(ScrollViewConstraints&&) = delete;
  ScrollViewConstraints& operator=(const ScrollViewConstraints&) = delete;
  ScrollViewConstraints& operator=(ScrollViewConstraints&&) = delete;

  /**
   * Updates the main internal scroll constraints with new ruler and domain values
   *
   * @param[in] scrollView A reference to the scroll view object
   */
  void UpdateMainInternalConstraint(Internal::ScrollView& scrollView);

  /**
   * Enables/disables the overshoot constraints
   *
   * @param[in] scrollView A reference to the scroll view object
   * @param[in] enabled whether to enable or disable the overshoot constraints
   */
  void SetOvershootConstraintsEnabled(ScrollView& scrollView, bool enabled);

  /**
   * Sets internal constraints for this ScrollView.
   * Many of these internal constraints are based on properties within ScrollView.
   *
   * @param[in] scrollView A reference to the scroll view object
   */
  void SetInternalConstraints(ScrollView& scrollView);

public:

  Constraint mScrollMainInternalPrePositionConstraint;
  Constraint mScrollMainInternalPositionConstraint;
  Constraint mScrollMainInternalOvershootXConstraint;
  Constraint mScrollMainInternalOvershootYConstraint;
  Constraint mScrollMainInternalDeltaConstraint;
  Constraint mScrollMainInternalFinalConstraint;
  Constraint mScrollMainInternalRelativeConstraint;
  Constraint mScrollMainInternalDomainConstraint;
  Constraint mScrollMainInternalPrePositionMaxConstraint;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_SCROLL_VIEW_CONSTRAINTS_H
