#ifndef __DALI_TOOLKIT_PAGE_FACTORY_H__
#define __DALI_TOOLKIT_PAGE_FACTORY_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Toolkit
{

/**
 * PageFactory is an abstract interface for providing image actors to PageTurnView
 * Each image actor is identified by a unique ID, and has a linear order from 0 to GetNumberOfPages()-1
 */
class DALI_IMPORT_API PageFactory
{
public:
  // Property Names
  static const std::string ACTOR_HITTABLE; ///< name "actor-hittable", type bool

public:

  /**
   * Constructor
   * By default, the off screen rendering is disabled
   * Is off screen rendering is required to create the page image,
   * call EnableOffscreenRendering() before pass it as parameter to the PageTurnView
   */
  PageFactory();

  /**
   * Virtual destructor
   */
  virtual ~PageFactory();

  /**
   * Enable the off screen rendering to create the page image from actor tree
   */
  void EnableOffscreenRendering( );

  /**
   * Query whether offscreen rendering is needed to create the page image
   * @return
   */
  bool IsOffscreenRenderingNeeded();

  /**
   * Query the number of pages available from the factory.
   * The maximum available page has an ID of GetNumberOfPages()-1.
   */
  virtual unsigned int GetNumberOfPages() = 0;

  /**
   * Create an actor to represent the page content.
   * @param[in] pageId The ID of the page to create.
   * @return An actor, or an uninitialized pointer if the ID is out of range.
   */
  virtual Actor NewPage( unsigned int pageId ) = 0;

public: //Signal
  /**
   * Signal type for notification
   */
  typedef Signal< void ( int ) > RefreshSignal;

  /**
   * Signal emitted when the Actor tree is ready for rendering into the page image.
   * The signal is connected to the page refresh function inside PageTurnView.
   */
  RefreshSignal& PageRefreshSignal();

  /**
   * Emit the page ready singal. The PageTurn view will be notified to refresh the given page accordingly.
   * @param[in] pageId the index of the page which is ready for refreshing.
   */
  void EmitPageRefreshSignal( int pageId );

protected:

  /**
   * Sets whether an actor should be hittable for the PageTurnView::GetHitActor().
   * It is useful when a sub-tree should be hit instead of the 'leaf' actor in the actor tree.
   * By default, actors are not hittable for PageTurnView::GetHitActor()
   * @param[in] actor The actor to be set with the hittablity
   * @param[in] hittable True to be hittable, false otherwise.
   */
  void SetActorHittability( Actor actor, bool hittable );

  /**
   * Query whether an actor is hittable for the PageTurnView::GetHitActor().
   */
  bool GetActorHittability( Actor actor );

private:

  bool mNeedOffscreenRendering;

  RefreshSignal mPageRefreshSignal;
};

} // namespace Toolkit

} // namespace Dali
#endif /* __DALI_TOOLKIT_PAGE_FACTORY_H__ */
