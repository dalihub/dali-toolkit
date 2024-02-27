#ifndef DALI_PROJECT_DRAWABLE_VIEW_NATIVE_RENDERER_H
#define DALI_PROJECT_DRAWABLE_VIEW_NATIVE_RENDERER_H

/*
* Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/signals/callback.h>
#include <dali/public-api/signals/render-callback.h>
#include <memory>


namespace Dali::Internal
{
/**
 * The structure containing the initialization data for the
 * DrawableViewNativeRenderer instance.
 */
struct NativeRendererCreateInfo
{
  /**
   * Presentation mode is used only for threaded renderer
   */
  enum class PresentationMode
  {
    FIFO, // First-in first-out
    MAILBOX // Only most recent out
  };

  bool threadEnabled { false };
  bool directExecution { false };

  uint32_t maxOffscreenBuffers { 3u };
  PresentationMode presentationMode {PresentationMode::FIFO};
};

/**
 * The DrawableViewNativeRenderer is responsible for delegating rendering
 * either to the own thread (in parallel mode) or calling the GlView render callbacks
 * directly.
 *
 * The parallel mode creates the thread that invokes GLView callbacks directly.
 * The actual render callback will only pass the input data and blit the result.
 *
 * Parallel mode renders always to the offscreen framebuffer.
 */
class DrawableViewNativeRenderer
{
public:
  explicit DrawableViewNativeRenderer( const NativeRendererCreateInfo& createInfo );
  ~DrawableViewNativeRenderer();

  /**
   * Registers GlView callbacks
   */
  void RegisterGlCallbacks( Dali::CallbackBase* onInitCallback,Dali::CallbackBase* onRenderCallback, Dali::CallbackBase* onTerminateCallback );

  /**
   * Dispatches the GlView init callback
   * @param renderCallbackInput
   */
  void InvokeGlInitCallback( const RenderCallbackInput& renderCallbackInput );

  /**
   * Dispatches the GlView render callback
   * @param renderCallbackInput
   */
  void InvokeGlRenderCallback(  const RenderCallbackInput& renderCallbackInput  );

  /**
   * Dispatches the GlView terminate callback
   * @param[in] renderCallbackInput
   */
  void InvokeGlTerminateCallback(  const RenderCallbackInput& renderCallbackInput );

  /**
   * @brief Resizes the render surface
   *
   * @param[in] width Width of surface
   * @param[in] height Height of surface
   */
  void Resize( uint32_t width, uint32_t height );

  /**
   * @brief Pushes render callback input data into the native renderer thread
   *
   * @param[in] renderCallbackInput Valid RenderCallbackInput object
   */
  void PushRenderCallbackInputData( const Dali::RenderCallbackInput& renderCallbackInput );

  /**
   * @brief Terminates thread in parallel mode
   */
  void Terminate();

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};
}

#endif // DALI_PROJECT_DRAWABLE_VIEW_NATIVE_RENDERER_H
