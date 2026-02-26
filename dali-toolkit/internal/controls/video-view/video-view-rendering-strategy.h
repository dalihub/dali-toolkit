#ifndef DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_RENDERING_STRATEGY_H
#define DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_RENDERING_STRATEGY_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/public-api/controls/video-view/video-view.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/internal/controls/video-view/video-view-impl.h>

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/signals/connection-tracker.h>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

class VideoView;

/**
 * @brief Base strategy interface for VideoView rendering.
 */
class VideoRenderingStrategy : public Dali::ConnectionTracker
{
public:
  /**
   * @brief Constructor
   * @param[in] videoViewHandle Handle to the VideoView
   */
  VideoRenderingStrategy(Toolkit::VideoView videoViewHandle)
  : mVideoViewWeakHandle(videoViewHandle)
  {}

  /**
   * @brief Virtual destructor
   */
  virtual ~VideoRenderingStrategy() = default;

  /**
   * @brief Initialize the rendering target and setup visuals/properties
   * @return true if initialization was successful, false otherwise.
   */
  virtual bool Initialize() = 0;

  /**
   * @brief Update the display area when the actor's transform changes
   */
  virtual void UpdateDisplayArea(Dali::PropertyNotification& source) = 0;

  /**
   * @brief Hook for when Play is called, allowing strategy to trigger animations/continuous rendering
   */
  virtual void Play() = 0;

  /**
   * @brief Hook for when Pause is called
   */
  virtual void Pause() = 0;

  /**
   * @brief Hook for when Stop is called
   */
  virtual void Stop() = 0;

  /**
   * @brief Hook for when visual properties are updated, specifically for shader or texture recreation
   */
  virtual void UpdateProperties(const Property::Map& properties) {}

  /**
   * @brief Hook to enable or disable offscreen frame rendering
   */
  virtual void EnableOffscreenFrameRendering(bool useOffScreenFrame) {}

protected:
  /**
   * @brief Gets the VideoView handle if it's still valid
   * @return Valid VideoView handle or empty handle
   */
  Toolkit::VideoView GetVideoView() const
  {
    return mVideoViewWeakHandle.GetHandle();
  }

  /**
   * @brief Gets the VideoView implementation pointer if it's still valid
   * @return Valid VideoView pointer or nullptr
   */
  VideoView* GetVideoViewImpl() const
  {
    Toolkit::VideoView handle = GetVideoView();
    return &GetImpl(handle);
  }

protected:
  Dali::WeakHandle<Toolkit::VideoView> mVideoViewWeakHandle;
};

/**
 * @brief Strategy for Underlay (Window Surface) rendering.
 *
 * It punches a hole through the UI toolkit and renders the video on a sub-surface.
 * This strategy is used when underlay mode is enabled, providing direct hardware
 * acceleration for video rendering.
 */
class WindowSurfaceStrategy : public VideoRenderingStrategy
{
public:
  /**
   * @brief Constructor for WindowSurfaceStrategy.
   *
   * @param[in] videoViewHandle Handle to the VideoView this strategy manages.
   */
  WindowSurfaceStrategy(Toolkit::VideoView videoViewHandle);

  /**
   * @brief Destructor for WindowSurfaceStrategy.
   *
   * Cleans up all resources including overlay texture visuals and property notifications.
   */
  ~WindowSurfaceStrategy() override;

  /**
   * @brief Initializes the window surface rendering strategy.
   *
   * Sets up the transparent hole in the UI layer, creates property notifications
   * for tracking actor position/scale changes, and initializes overlay texture
   * visuals if offscreen frame rendering is enabled.
   * @return true if initialization was successful, false otherwise.
   */
  bool Initialize() override;

  /**
   * @brief Updates the display area when actor transform changes.
   *
   * Called when the VideoView actor's position, size, or scale changes.
   * Updates the video player's display area to match the new actor bounds.
   *
   * @param[in] source The property notification that triggered this update.
   */
  void UpdateDisplayArea(Dali::PropertyNotification& source) override;

  /**
   * @brief Hook for when Play is called.
   *
   * Starts continuous rendering if needed for frame interpolation effects.
   */
  void Play() override;

  /**
   * @brief Hook for when Pause is called.
   *
   * Pauses continuous rendering.
   */
  void Pause() override;

  /**
   * @brief Hook for when Stop is called.
   *
   * Stops continuous rendering and resets overlay state.
   */
  void Stop() override;

  /**
   * @brief Enables or disables offscreen frame rendering.
   *
   * When enabled, creates overlay texture visuals for smooth frame interpolation.
   * When disabled, cleans up overlay visuals.
   *
   * @param[in] useOffScreenFrame True to enable offscreen frame rendering, false to disable.
   */
  void EnableOffscreenFrameRendering(bool useOffScreenFrame) override;

private:
  /**
   * @brief Creates the overlay texture visual for frame interpolation.
   *
   * Creates and configures an ImageVisual that will display interpolated frames
   * for smooth transitions between video frames. This visual overlays on top
   * of the transparent hole when offscreen frame rendering is enabled.
   */
  void CreateOverlayTextureVisual();

  /**
   * @brief Resets and removes the overlay texture visual.
   *
   * Cleans up the overlay visual resources and removes it from the VideoView actor.
   */
  void ResetOverlayTextureVisual();

private:
  Dali::PropertyNotification mPositionUpdateNotification;
  Dali::PropertyNotification mSizeUpdateNotification;
  Dali::PropertyNotification mScaleUpdateNotification;

  Dali::Texture mPreviousFrameTexture;
  Dali::Texture mCurrentFrameTexture;
  Dali::Toolkit::Visual::Base mOverlayTextureVisual;
  Dali::Property::Index mOverlayTextureVisualIndex{Dali::Property::INVALID_INDEX};
};

/**
 * @brief Strategy for Native Image rendering.
 *
 * It pulls video frames into a native texture and renders them directly via ImageVisual.
 * This strategy is used when underlay mode is disabled, providing software-accelerated
 * or hardware-accelerated rendering within the UI layer.
 */
class NativeImageStrategy : public VideoRenderingStrategy
{
public:
  /**
   * @brief Constructor for NativeImageStrategy.
   *
   * @param[in] videoViewHandle Handle to the VideoView this strategy manages.
   */
  NativeImageStrategy(Toolkit::VideoView videoViewHandle);

  /**
   * @brief Destructor for NativeImageStrategy.
   *
   * Cleans up all resources including native image source and visuals.
   */
  ~NativeImageStrategy() override;

  /**
   * @brief Initializes the native image rendering strategy.
   *
   * Creates the native image source, sets up the ImageVisual for rendering,
   * and configures initial shader properties.
   * @return true if initialization was successful, false otherwise.
   */
  bool Initialize() override;

  /**
   * @brief Updates the display area when actor transform changes.
   *
   * Called when the VideoView actor's position, size, or scale changes.
   * Updates the native image source to match the new actor bounds.
   *
   * @param[in] source The property notification that triggered this update.
   */
  void UpdateDisplayArea(Dali::PropertyNotification& source) override;

  /**
   * @brief Hook for when Play is called.
   *
   * Starts the video playback and rendering loop.
   */
  void Play() override;

  /**
   * @brief Hook for when Pause is called.
   *
   * Pauses the video playback.
   */
  void Pause() override;

  /**
   * @brief Hook for when Stop is called.
   *
   * Stops the video playback and resets the rendering state.
   */
  void Stop() override;

  /**
   * @brief Updates visual properties, specifically for shader or texture recreation.
   *
   * Called when the effect property map is updated. Recreates the visual
   * with new shader properties if needed.
   *
   * @param[in] properties The new property map containing shader configurations.
   */
  void UpdateProperties(const Property::Map& properties) override;

private:
  /**
   * @brief Creates a shader property map for video rendering.
   *
   * Configures vertex and fragment shaders for video texture rendering.
   * Supports custom shaders through the effect property map.
   *
   * @return Property::Map containing shader configuration.
   */
  Property::Map CreateShader();
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_VIDEO_VIEW_RENDERING_STRATEGY_H