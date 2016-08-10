#ifndef __DALI_TOOLKIT_VIDEO_VIEW_H__
#define __DALI_TOOLKIT_VIDEO_VIEW_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
  class VideoView;
} // namespace Internal

/**
 * @addtogroup dali_toolkit_controls_video_view
 * @{
 */

/**
 * @brief VideoView is a control for video playback and display.
 *
 * For working VideoView, a video plugin for a platform should be provided.
 *
 * Signals
 * | %Signal Name  | Method                  |
 * |---------------|-------------------------|
 * | finished      | @ref FinishedSignal()   |
 * @SINCE_1_1.38
 *
 * Actions
 * | %Action Name    | Attributes                                         | Description                                             |
 * |-----------------|----------------------------------------------------|---------------------------------------------------------|
 * | videoPlay       | Doesn't have attributes                            | Plays video. See @ref DoAction()                        |
 * | videoPause      | Doesn't have attributes                            | Pauses video. See @ref DoAction()                       |
 * | videoStop       | Doesn't have attributes                            | Stops video. See @ref DoAction()                        |
 * | videoForward    | The position ( millisecond ) for forward playback  | Sets forward position for playback. See @ref DoAction() |
 * | videoBackward   | The position ( millisecond ) for backward playback | Sets backward position for playback. See @ref DoAction()|
 * @SINCE_1_1.38
 *
 */
class DALI_IMPORT_API VideoView: public Control
{
public:

  // Signal
  typedef Signal< void (VideoView&) > VideoViewSignalType; ///< Video playback finished signal type @ SINCE_1_1.38

  enum PropertyRange
  {
    PROPERTY_START_INDEX = Control::CONTROL_PROPERTY_END_INDEX + 1,  ///< @SINCE_1_0.0
  };

  struct Property
  {
    enum
    {
      /**
       * @brief name "video", video file url as string type or Property::Map
       * @SINCE_1_1.38
       */
      VIDEO = PROPERTY_START_INDEX,

     /**
       * @brief name "looping", looping status, true or false
       * @SINCE_1_1.38
       */
      LOOPING,

     /**
       * @brief name "muted", mute status, true or false
       * @SINCE_1_1.38
       */
      MUTED,

     /**
       * @brief name "volume", left and right volume scalar as float type, Property::Map with two values ( "left" and "right" )
       * @SINCE_1_1.38
       */
      VOLUME
    };
  };

public:

  /**
   * @brief Creates an initialized VideoView.
   * @SINCE_1_1.38
   * @return A handle to a newly allocated Dali ImageView
   *
   * @note VideoView will not display anything
   */
  static VideoView New();

  /**
   * @brief Creates an initialized VideoView.
   * If the string is empty, VideoView will not display anything
   *
   * @SINCE_1_1.38
   * @param[in] url The url of the video resource to display
   * @return A handle to a newly allocated Dali VideoView
   */
  static VideoView New( const std::string& url );

  /**
   * @brief Create an uninitialized VideoView
   * @SINCE_1_1.38
   */
  VideoView();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handel types must not contain data or virtual methods
   * @SINCE_1_1.38
   */
  ~VideoView();

  /*
   * @brief Copy constructor
   *
   * @SINCE_1_1.38
   * @param[in] videoView VideoView to copy. The copied VideoView will point at the same implementation
   */
  VideoView( const VideoView& videoView );

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_1.38
   * @param[in] videoView The VideoView to assign from.
   * @return The updated VideoView.
   */
  VideoView& operator=( const VideoView& videoView );

  /**
   * @brief Downcast a handle to VideoView handle.
   *
   * If handle points to a VideoView the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @SINCE_1_1.38
   * @param[in] handle Handle to an object
   * @return handle to a VideoView or an uninitialized handle
   */
  static VideoView DownCast( BaseHandle handle );

  /**
   * @brief Starts the video playback.
   * @SINCE_1_1.38
   */
  void Play();

  /**
   * @brief Pauses the video playback.
   * @SINCE_1_1.38
   */
  void Pause();

  /**
   * @brief Stops the video playback.
   * @SINCE_1_1.38
   */
  void Stop();

  /**
   * @brief Seeks forward by the specified number of milliseconds.
   *
   * @SINCE_1_1.38
   * @param[in] millisecond The position for forward playback
   */
  void Forward( int millisecond );

  /**
   * @brief Seeks backward by the specified number of milliseconds.
   *
   * @SINCE_1_1.38
   * @param[in] millisecond The position for backward playback
   */
  void Backward( int millisecond );

  /**
   * @brief Connect to this signal to be notified when a video playback have finished.
   *
   * @SINCE_1_1.38
   * @return A signal object to connect with.
   */
  VideoViewSignalType& FinishedSignal();

public: // Not intended for application developers

  /**
   * @internal
   * @brief Creates a handle using the Toolkit::Internal implementation.
   *
   * @SINCE_1_1.38
   * @param[in] implementation The VideoView implementation.
   */
  DALI_INTERNAL VideoView( Internal::VideoView& implementation );

  /**
   * @internal
   * @brief Allows the creation of this VideoView from an Internal::CustomActor pointer.
   *
   * @SINCE_1_1.38
   * @param[in] internal A pointer to the internal CustomActor.
   */
  DALI_INTERNAL VideoView( Dali::Internal::CustomActor* internal );

};

/**
 * @}
 */

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_VIDEO_VIEW_H__
