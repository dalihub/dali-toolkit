#ifndef DALI_TOOLKIT_VECTOR_ANIMATION_TASK_H
#define DALI_TOOLKIT_VECTOR_ANIMATION_TASK_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
   * @brief Sets the target image size.
   *
   * @param[in] width The target image width
   * @param[in] height The target image height
   */
  void SetSize( uint32_t width, uint32_t height );

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
   * @brief Render one frame. The current frame number will be increased.
   */
  void RenderFrame();

  /**
   * @brief This callback is called after the animation is finished.
   * @param[in] callback The animation finished callback
   */
  void SetAnimationFinishedCallback( EventThreadCallback* callback );

  /**
   * @brief Enable looping for 'count' repeats. -1 means to repeat forever.
   * @param[in] count The number of times to loop
   */
  void SetLoopCount( int32_t count );

  /**
   * @brief Set the playing range in frame number.
   * @param[in] startFrame The frame number to specify minimum progress.
   * @param[in] endFrame The frame number to specify maximum progress.
   * The animation will play between those values.
   */
  void SetPlayRange( uint32_t startFrame, uint32_t endFrame );

  /**
   * @brief Gets the playing range in frame number.
   * @param[out] startFrame The frame number to specify minimum progress.
   * @param[out] endFrame The frame number to specify maximum progress.
   */
  void GetPlayRange( uint32_t& startFrame, uint32_t& endFrame );

  /**
   * @brief Get the play state
   * @return The play state
   */
  DevelImageVisual::PlayState::Type GetPlayState() const;

  /**
   * @brief Sets the current frame number of the animation.
   * @param[in] frameNumber The new frame number between [0, the maximum frame number] or between the play range if specified.
   */
  void SetCurrentFrameNumber( uint32_t frameNumber );

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
   * @brief Gets the frame number when the animation is stopped according to the stop behavior.
   */
  uint32_t GetStoppedFrame( uint32_t startFrame, uint32_t endFrame, uint32_t currentFrame );

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
  VectorAnimationThread&                 mVectorAnimationThread;
  ConditionalWait                        mConditionalWait;
  std::unique_ptr< EventThreadCallback > mAnimationFinishedTrigger;
  Vector2                                mPlayRange;
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
  int32_t                                mLoopCount;
  int32_t                                mCurrentLoop;
  bool                                   mResourceReady;
  bool                                   mCurrentFrameUpdated;
  bool                                   mCurrentLoopUpdated;
  bool                                   mForward;
  bool                                   mUpdateFrameNumber;
  bool                                   mNeedAnimationFinishedTrigger;

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VECTOR_ANIMATION_TASK_H
