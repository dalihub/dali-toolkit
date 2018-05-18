#ifndef DALI_TOOLKIT_INTERNAL_LAYOUT_CONTROLLER_H
#define DALI_TOOLKIT_INTERNAL_LAYOUT_CONTROLLER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/core.h>
#include <dali-toolkit/public-api/controls/control.h>
#include <dali-toolkit/devel-api/layouting/layout-controller.h>
#include <dali-toolkit/devel-api/layouting/layout-item-impl.h>
#include <dali-toolkit/devel-api/layouting/layout-group-impl.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

/**
 * @brief Layout controller handles measurement and layout of all
 * controls that utilize Layouts.
 */
class LayoutController : public Dali::BaseObject, public Integration::Processor
{
public:
  /**
   * Constructor
   */
  LayoutController();

  /**
   * Destructor
   */
  ~LayoutController();

  /**
   * Second stage initialization
   */
  void Initialize();

  /**
   * This marks the given layout and all its parents as dirty.
   */
  void RequestLayout( LayoutItem& layout );

  /**
   * Measures next level of layouts in the actor hierarchy.
   */
  void MeasureHierarchy( Actor root, MeasureSpec widthSpec, MeasureSpec heightSpec );

  /**
   * Perform layout of the hierarchy
   */
  void PerformLayout( Actor root, int left, int top, int right, int bottom );

protected: // Implementation of Processor

  /**
   * @copydoc Dali::Integration::Processor::Process()
   */
  virtual void Process();

private:
  bool mLayoutRequested;
};

} // namespace Internal

inline Internal::LayoutController& GetImpl( Dali::Toolkit::LayoutController& handle )
{
  DALI_ASSERT_ALWAYS(handle);
  Dali::BaseObject& object = handle.GetBaseObject();
  return static_cast<Internal::LayoutController&>(object);
}

inline const Internal::LayoutController& GetImpl( const Dali::Toolkit::LayoutController& handle )
{
  DALI_ASSERT_ALWAYS(handle);
  const Dali::BaseObject& object = handle.GetBaseObject();
  return static_cast<const Internal::LayoutController&>(object);
}

} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_LAYOUT_CONTROLLER_H
