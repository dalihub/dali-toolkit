#ifndef __DALI_TOOLKIT_INTERNAL_STYLE_CHANGE_PROCESSOR_H_
#define __DALI_TOOLKIT_INTERNAL_STYLE_CHANGE_PROCESSOR_H_

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
#include <dali/dali.h>

namespace Dali
{

namespace Toolkit
{

class ControlImpl;

namespace Internal
{

/**
 * This observes and processes when any style changes occur.  When they do occur, it traverses through
 * all registered controls and calls the StyleChanged method.
 *
 * This is created when a control first registers with it.  Subsequent registrations increase the
 * reference count.  When the last control unregisters, i.e. the reference count is 0, the instance
 * is also destroyed.
 */
class StyleChangeProcessor : public ConnectionTracker
{
public:

  /**
   * Non virtual destructor.
   * We should not derive from StyleChangeProcessor.
   * Destructor is called when the last control unregisters.
   */
  ~StyleChangeProcessor();

  /**
   * Registers a control with the StyleChangeProcessor.
   * @param[in] control The raw Control pointer.
   */
  static void Register( ControlImpl* control );

  /**
   * Unregisters a control from the StyleChangeProcessor.
   * @param[in] control The raw Control pointer.
   */
  static void Unregister( ControlImpl* control );

public:

  /**
   * Increment the processor's reference count.
   */
  void Reference();

  /**
   * Decrement the processor's reference count.
   */
  void Unreference();

  /**
   * Retrieve the processor's reference count.
   * @return The reference count
   */
  unsigned int ReferenceCount() const;

private:

  /**
   * Constructor.
   * We should only create an instance upon first registration.
   */
  StyleChangeProcessor();

  // Undefined
  StyleChangeProcessor(const StyleChangeProcessor&);
  StyleChangeProcessor& operator=(const StyleChangeProcessor&);

private:

  /**
   * Callback for the StyleMonitor when the style changes on the platform.
   * @param[in]  styleMonitor  The Style Monitor.
   * @param[in]  styleChange   The style change information.
   */
  void StyleChanged(Dali::StyleMonitor styleMonitor, Dali::StyleChange styleChange);

  /**
   * Propagates the style change to all Controls in the actor hierarchy.
   * This is done with a bottom-up approach, i.e. the leaf Control's StyleChange method gets
   * called first followed by its parent and so on.
   * @param[in]  actor   The actor whose children to process and send style change notification to.
   * @param[in]  change  The style change.
   */
  static void PropagateStyleChange(Actor actor, Dali::StyleChange change);

private:

  unsigned int              mCount;        ///< The reference count
  std::vector<ControlImpl*> mControls;     ///< Stores all registered controls.
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_STYLE_CHANGE_PROCESSOR_H_
