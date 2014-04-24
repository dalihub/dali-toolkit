#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_HANDLES_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_HANDLES_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{
/**
 * Class to create handles and alter their visualisation.
 * Not responsible for positioning.
 */

class TextInputHandles : public ConnectionTracker
{

public:

  typedef SignalV2< bool () > HandlesReleasedSignal;

  /**
   * Signal emitted when a handle is released
   */
  HandlesReleasedSignal& ReleasedSignal(){ return mReleasedSignal; };

  /**
   * Default constructor
   */
  TextInputHandles() {}

  /**
   * Destructor
   */
  ~TextInputHandles() {};

  /**
   * Create the selection handles
   */
  void CreateSelectionHandles(){};

  /**
   * Un-parents the Selection Handles and resets their Image Actors
   */
  void DestorySelectionHandles(){};

  /**
   * Set the Actor visibility on Selection Handle One
   * @param[in] visibility visibility flag
   */
  void SetSelectionHandleOneVisibility( bool visibility ){};

  /**
   * Set the Actor visibility on Selection Handle Two
   * @param[in] visibility visibility flag
   */
  void SetSelectionHandleTwoVisibility( bool visibility ){};

  /**
   * Attach the two selection handles to the pan gesture detector
   * @param[in] panGestureDetector the PanGestureDetector to attach to
   */
  void AttachSelectionHandlesToGivenPanGesture(PanGestureDetector& panGestureDetector ){};

  /**
   * Attach the two selection handles to the tap gesture detector
   * @param[in] tapGestureDetector the TapGestureDetector to attach to
   */
  void AttachSelectionHandlesToGivenTapDetector(TapGestureDetector& tapGestureDetector ){};

  /**
   * Attach the grab handle to the pan gesture detector
   * @param[in] panGestureDetector the PanGestureDetector to attach to
   */
  void AttachGrabHandleToGivenPanGesture( PanGestureDetector& panGestureDetector ){};

  /**
   * Get Selection handle one
   * @return selection handle actor
   */
  Actor GetSelectionHandleOne() { return Actor(); };

  /**
   * Get Selection handle two
   * @return selection handle actor
   */
  Actor GetSelectionHandleTwo() { return Actor(); };

  /**
   * Get the grab handle
   * @return grab handle Actor
   */
  Actor GetGrabHandle()  { return Actor(); };

  /**
   * Create the grab handle that positions the cursor
   * @param[in] image the image to be used.
   */
  void CreateGrabHandle(){};

  /**
   * Removes and Resets GrabHandle
   */
  void DestoryGrabHandle(){};

  /**
   * Set the image to be used as the cursor grab hander
   * @pre The text input actor has been initialised.
   * @param[in] image The image to be used.
   */
  void SetGrabHandleImage( Dali::Image image ){};

  /**
   * Set the Actor visibility on the GrabHandle
   * @param[in] visibility visibility flag
   */
  void SetGrabHandleVisibility( bool visibility ){};

  /* Touch Event Callbacks */

  /**
   * Callback on selection handle touched.
   * Sets the image depending if handle in pressed or normal state
   * @param[in] actor touched
   * @param[in] touch touch event, used to determine if down or up event
   */
  bool OnSelectionHandleTouched(Dali::Actor actor, const TouchEvent& touch){ return true; };

private:

  /**
   * @brief Copy Constructor
   * @param[in] handles
   * Undefined/Hidden.
   */
  TextInputHandles(const TextInputHandles& handles);

  /**
   * @Assignment Constructor
   * @param[in] rhs
   * Undefined/Hidden.
   */
  TextInputHandles& operator=(const TextInputHandles& rhs);

private:

  ImageActor mSelectionHandleOne;                 // First selection handle used for selecting text to cut&paste
  ImageActor mSelectionHandleTwo;                 // Second selection handle used for selecting text to cut&paste
  Actor mHandleOneGrabArea;                       // Invisible actor that receives pans events for the selection handle.
  Actor mHandleTwoGrabArea;                       // Invisible actor that receives pans events for the selection handle.

  Image mSelectionHandleOneImage;                 // image used for selection handle one
  Image mSelectionHandleOneImagePressed;          // image used for selection handle one pressed state
  Image mSelectionHandleTwoImage;                 // image used for selection handle two
  Image mSelectionHandleTwoImagePressed;          // image used for selection handle two pressed state

  Vector3 mSelectionHandleOneOffset;              // Handle One's Offset
  Vector3 mSelectionHandleTwoOffset;              // Handle Two's Offset
  Vector3 mSelectionHandleOneCoordinatePosition;  // Actual x y z position of handle
  Vector3 mSelectionHandleTwoCoordinatePosition;  // Actual x y z position of handle
  std::size_t mSelectionHandleOneStringPosition;  // Position of handle along the string of text
  std::size_t mSelectionHandleTwoStringPosition;  // Position of handle along the string of text

  Image mGrabHandleImage;                         // Image to be used for grab handle
  ImageActor mGrabHandle;                         // Handle used to move cursor for editing
  Actor mGrabHandleGrabArea;                      // invisible actor that receives pans events for the grab handle.

  HandlesReleasedSignal mReleasedSignal;          // Signal emitted when a handle is released

  bool mIsSelectionHandleOneFlipped:1;            // Flag to know whether the handle one is flipped or not.
  bool mIsSelectionHandleTwoFlipped:1;            // Flag to know whether the handle two is flipped or not.
};


} // namespace Internal


} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_INPUT_HANDLES_H__
