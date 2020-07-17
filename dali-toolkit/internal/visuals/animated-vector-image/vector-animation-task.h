#ifndef DALI_TOOLKIT_VECTOR_ANIMATION_TASK_H
#define DALI_TOOLKIT_VECTOR_ANIMATION_TASK_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/object/property-array.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali/devel-api/threading/conditional-wait.h>
#include <memory>
#include <chrono>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

class VisualFactoryCache;
class VectorAnimationThread;
class VectorAnimationTask;
typedef IntrusivePtr< VectorAnimationTask > VectorAnimationTaskPtr;

/**
 * The task of the vector animation.
 */
class VectorAnimationTask : public RefObject
{
public:

  using UploadCompletedSignalType = Dali::VectorAnimationRenderer::UploadCompletedSignalType;

  /**
   * Flags for re-sending data to the vector animation thread
   */
  enum ResendFlags
  {
    RESEND_PLAY_RANGE    = 1 << 0,
    RESEND_LOOP_COUNT    = 1 << 1,
    RESEND_STOP_BEHAVIOR = 1 << 2,
    RESEND_LOOPING_MODE  = 1 << 3,
    RESEND_CURRENT_FRAME = 1 << 4,
    RESEND_SIZE          = 1 << 5,
    RESEND_PLAY_STATE    = 1 << 6
  };

  /**
   * @brief Structure used to pass parameters to the vector animation task
   */
  struct AnimationData
  {
    AnimationData()
    : resendFlag( 0 ),
      playRange(),
      playState(),
      stopBehavior( DevelImageVisual::StopBehavior::CURRENT_FRAME ),
      loopingMode( DevelImageVisual::LoopingMode::RESTART ),
      currentFrame( 0 ),
      width( 0 ),
      height( 0 ),
      loopCount( -1 )
    {
    }

    AnimationData& operator= ( const AnimationData& rhs )
    {
      resendFlag |= rhs.resendFlag; // OR resend flag
      playRange = rhs.playRange;
      playState = rhs.playState;
      stopBehavior = rhs.stopBehavior;
      loopingMode = rhs.loopingMode;
      currentFrame = rhs.currentFrame;
      width = rhs.width;
      height = rhs.height;
      loopCount = rhs.loopCount;
      return *this;
    }

    uint32_t                             resendFlag;
    Property::Array                      playRange;
    DevelImageVisual::PlayState::Type    playState;
    DevelImageVisual::StopBehavior::Type stopBehavior;
    DevelImageVisual::LoopingMode::Type  loopingMode;
    uint32_t                             currentFrame;
    uint32_t                             width;
    uint32_t                             height;
    int32_t                              loopCount;
  };

  /**
   * @brief Constructor.
   *
   * @param[in] factoryCache A pointer pointing to the VisualFactoryCache object
   * @param[in] url The url of the vector animation file
   */
  VectorAnimationTask( VisualFactoryCache& factoryCache, const std::string& url );

  /**
   * @brief Destructor.
   */
  virtual ~VectorAnimationTask();

  /**
   * @brief Finalizes the task.
   */
  void Finalize();

  /**
   * @brief Sets the renderer used to display the result image.
   *
   * @param[in] renderer The renderer used to display the result image
   */
  void SetRenderer( Renderer renderer );

  /**
   * @brief Sets data to specify animation playback.
   * @param[in] data The animation data
   */
  void SetAnimationData( const AnimationData& data );

  /**
   * @brief This callback is called after the animation is finished.
   * @param[in] callback The animation finished callback
   */
  void SetAnimationFinishedCallback( EventThreadCallback* callback );

  /**
   * @brief Gets the playing range in frame number.
   * @param[out] startFrame The frame number to specify minimum progress.
   * @param[out] endFrame The frame number to specify maximum progress.
   */
  void GetPlayRange( uint32_t& startFrame, uint32_t& endFrame );

  /**
   * @brief Retrieves the current frame number of the animation.
   * @return The current frame number
   */
  uint32_t GetCurrentFrameNumber() const;

  /**
   * @brief Retrieves the total frame number of the animation.
   * @return The total frame number
   */
  uint32_t GetTotalFrameNumber() const;

  /**
   * @brief Gets the default size of the file,.
   * @return The default size of the file
   */
  void GetDefaultSize( uint32_t& width, uint32_t& height ) const;

  /**
   * @brief Gets the layer information of all the child layers.
   * @param[out] map The layer information
   */
  void GetLayerInfo( Property::Map& map ) const;

  /**
   * @brief Connect to this signal to be notified when the texture upload is completed.
   * @return The signal to connect to.
   */
  UploadCompletedSignalType& UploadCompletedSignal();

  /**
   * @brief Rasterizes the current frame.
   * @return true if the animation is running, false otherwise.
   */
  bool Rasterize();

  /**
   * @brief Calculates the time for the next frame rasterization.
   * @return The time for the next frame rasterization.
   */
  std::chrono::time_point< std::chrono::system_clock > CalculateNextFrameTime( bool renderNow );

  /**
   * @brief Gets the time for the next frame rasterization.
   * @return The time for the next frame rasterization.
   */
  std::chrono::time_point< std::chrono::system_clock > GetNextFrameTime();

private:

  /**
   * @brief Initializes the vector renderer.
   */
  void Initialize();

  /**
   * @brief Play the vector animation.
   */
  void PlayAnimation();

  /**
   * @brief Stop the vector animation.
   */
  void StopAnimation();

  /**
   * @brief Pause the vector animation.
   */
  void PauseAnimation();

  /**
   * @brief Sets the target image size.
   *
   * @param[in] width The target image width
   * @param[in] height The target image height
   */
  void SetSize( uint32_t width, uint32_t height );

  /**
   * @brief Enable looping for 'count' repeats. -1 means to repeat forever.
   * @param[in] count The number of times to loop
   */
  void SetLoopCount( int32_t count );

  /**
   * @brief Set the playing range in frame number.
   * @param[in] playRange The array to specify minimum and maximum progress.
   * The animation will play between those values.
   */
  void SetPlayRange( const Property::Array& playRange );

  /**
   * @brief Sets the current frame number of the animation.
   * @param[in] frameNumber The new frame number between [0, the maximum frame number] or between the play range if specified.
   */
  void SetCurrentFrameNumber( uint32_t frameNumber );

  /**
   * @brief Sets the stop behavior of the animation. This is performed when the animation is stopped.
   * @param[in] stopBehavior The stop behavior
   */
  void SetStopBehavior( DevelImageVisual::StopBehavior::Type stopBehavior );

  /**
   * @brief Sets the looping mode.
   * Animation plays forwards and then restarts from the beginning or runs backwards again.
   * @param[in] loopingMode The looping mode
   */
  void SetLoopingMode( DevelImageVisual::LoopingMode::Type loopingMode );

  /**
   * @brief Gets the frame number when the animation is stopped according to the stop behavior.
   */
  uint32_t GetStoppedFrame( uint32_t startFrame, uint32_t endFrame, uint32_t currentFrame );

  /**
   * @brief Applies the animation data set by the main thread.
   */
  void ApplyAnimationData();

  // Undefined
  VectorAnimationTask( const VectorAnimationTask& task ) = delete;

  // Undefined
  VectorAnimationTask& operator=( const VectorAnimationTask& task ) = delete;

private:

  enum class PlayState
  {
    STOPPING,  ///< The animation is stopping
    STOPPED,   ///< The animation has stopped
    PLAYING,   ///< The animation is playing
    PAUSED     ///< The animation is paused
  };

  std::string                            mUrl;
  VectorAnimationRenderer                mVectorRenderer;
  AnimationData                          mAnimationData[2];
  VectorAnimationThread&                 mVectorAnimationThread;
  ConditionalWait                        mConditionalWait;
  std::unique_ptr< EventThreadCallback > mAnimationFinishedTrigger;
  PlayState                              mPlayState;
  DevelImageVisual::StopBehavior::Type   mStopBehavior;
  DevelImageVisual::LoopingMode::Type    mLoopingMode;
  std::chrono::time_point< std::chrono::system_clock > mNextFrameStartTime;
  int64_t                                mFrameDurationNanoSeconds;
  float                                  mFrameRate;
  uint32_t                               mCurrentFrame;
  uint32_t                               mTotalFrame;
  uint32_t                               mStartFrame;
  uint32_t                               mEndFrame;
  uint32_t                               mWidth;
  uint32_t                               mHeight;
  uint32_t                               mAnimationDataIndex;
  int32_t                                mLoopCount;
  int32_t                                mCurrentLoop;
  bool                                   mForward;
  bool                                   mUpdateFrameNumber;
  bool                                   mNeedAnimationFinishedTrigger;
  bool                                   mAnimationDataUpdated;
  bool                                   mDestroyTask;
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VECTOR_ANIMATION_TASK_H
