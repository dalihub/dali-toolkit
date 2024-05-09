#ifndef DALI_TOOLKIT_VECTOR_IMAGE_RASTERIZE_THREAD_H
#define DALI_TOOLKIT_VECTOR_IMAGE_RASTERIZE_THREAD_H

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
#include <dali/devel-api/threading/mutex.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/integration-api/adaptor-framework/log-factory-interface.h>
#include <string>
#include <memory>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/visuals/image-visual-properties-devel.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

/**
 * The worker thread for vector image rasterization.
 */
class VectorRasterizeThread : public Thread
{
public:

  /**
   * @brief Constructor.
   *
   * @param[in] url The url of the vector animation file
   */
  VectorRasterizeThread( const std::string& url );

  /**
   * @brief Destructor.
   */
  virtual ~VectorRasterizeThread();

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
   * @brief This callback is called after the first frame is ready.
   * @param[in] callback The resource ready callback
   */
  void SetResourceReadyCallback( EventThreadCallback* callback );

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
   * @brief Gets the loop count. -1 means to repeat forever.
   * @return The number of times to loop
   */
  int32_t GetLoopCount() const;

  /**
   * @brief Set the playing range.
   * @param[in] range Two values between [0,1] to specify minimum and maximum progress.
   * The animation will play between those values.
   */
  void SetPlayRange( Vector2 range );

  /**
   * @brief Gets the playing range.
   * @return The play range defined for the animation
   */
  Vector2 GetPlayRange() const;

  /**
   * @brief Get the play state
   * @return The play state
   */
  DevelImageVisual::PlayState GetPlayState() const;

  /**
   * @brief Queries whether the resource is ready.
   * @return true if ready, false otherwise
   */
  bool IsResourceReady() const;

  /**
   * @brief Sets the progress of the animation.
   * @param[in] progress The new progress as a normalized value between [0,1] or between the play range if specified.
   */
  void SetCurrentProgress( float progress );

  /**
   * @brief Retrieves the current progress of the animation.
   * @return The current progress as a normalized value between [0,1]
   */
  float GetCurrentProgress() const;

  /**
   * @brief Gets the default size of the file,.
   * @return The default size of the file
   */
  void GetDefaultSize( uint32_t& width, uint32_t& height ) const;

protected:

  /**
   * @brief The entry function of the worker thread.
   *        It rasterizes the vector image.
   */
  void Run() override;

private:

  /**
   * @brief Initialize the vector renderer.
   */
  void Initialize();

  /**
   * @brief Rasterize the current frame.
   */
  void Rasterize();

  // Undefined
  VectorRasterizeThread( const VectorRasterizeThread& thread ) = delete;

  // Undefined
  VectorRasterizeThread& operator=( const VectorRasterizeThread& thread ) = delete;

private:

  std::string                 mUrl;
  VectorAnimationRenderer     mVectorRenderer;
  ConditionalWait             mConditionalWait;
  std::unique_ptr< EventThreadCallback > mResourceReadyTrigger;
  std::unique_ptr< EventThreadCallback > mAnimationFinishedTrigger;
  Vector2                     mPlayRange;
  DevelImageVisual::PlayState mPlayState;
  int64_t                     mFrameDurationNanoSeconds;
  float                       mProgress;
  float                       mFrameRate;
  uint32_t                    mCurrentFrame;
  uint32_t                    mTotalFrame;
  uint32_t                    mStartFrame;
  uint32_t                    mEndFrame;
  uint32_t                    mWidth;
  uint32_t                    mHeight;
  int32_t                     mLoopCount;
  int32_t                     mCurrentLoop;
  bool                        mNeedRender;
  bool                        mDestroyThread;  ///< Whether the thread be destroyed
  bool                        mResourceReady;
  bool                        mCurrentFrameUpdated;
  const Dali::LogFactoryInterface& mLogFactory; ///< The log factory

};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_VECTOR_IMAGE_RASTERIZE_THREAD_H
