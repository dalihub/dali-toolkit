/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

#include "drawable-view-native-renderer.h"
#include <dali/public-api/signals/render-callback.h>
#include <atomic>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

// GLES3+ is required for this to work!
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <unistd.h>
#include <any>

#define GL(x)                                                   \
  {                                                             \
    glGetError();                                               \
    {                                                           \
      x;                                                        \
    };                                                          \
    auto err = glGetError();                                    \
    if(err)                                                     \
    {                                                           \
      printf("%p:%d: ERROR: 0x%X\n", this, __LINE__, int(err)); \
    }                                                           \
  }

namespace
{
/**
 * Vertices of quad to display when using offscreen rendering
 */
// clang-format off
constexpr float QUAD_VERTS[] = {
  1.0f,  1.0f,
  1.0f, -1.0f,
  -1.0f, -1.0f,
  -1.0f,  1.0f,
};

/**
 * Indices of quad for offscreen rendering
 */
constexpr unsigned short QUAD_INDICES[] = {
  0, 1, 2,
  3, 0, 2
};

/**
 * UV coords of quad for offscreen rendering
 */
constexpr float QUAD_UV[] = {
  1.0f, 1.0f,   // top right
  1.0f, 0.0f,   // bottom right
  0.0f, 0.0f,   // bottom left
  0.0f, 1.0f    // top left
};
// clang-format on
} // namespace

namespace Dali::Internal
{
struct DrawableViewNativeRenderer::Impl
{
  /**
   * This structure associates framebuffer with texture and fence object
   */
  struct FrameBufferTexture
  {
    uint32_t textureId{0u};
    uint32_t framebufferId{0u};
    GLsync   fence{nullptr};
  };

  // Queues management
  bool DequeueTextureDrawBuffer(uint32_t& outIndex)
  {
    std::scoped_lock<std::recursive_mutex> lock(mTextureQueueMutex);
    if(mTextureDrawQueue.empty())
    {
      // TODO: probably add textures if necessary
      return false;
    }

    auto retval = mTextureDrawQueue.front();
    mTextureDrawQueue.pop_front();
    outIndex = retval;
    return true;
  }

  /**
   * Enqueues framebuffer for the Read queue to be used by
   * the CONSUMER.
   */
  void EnqueueTextureReadBuffer(uint32_t fbId)
  {
    // push ready texture to front of 'read' queue
    std::scoped_lock<std::recursive_mutex> lock(mTextureQueueMutex);

    auto& fb = mFramebufferTexture[fbId];

    // Check state of fence whether the texture can be passed to the CONSUMER
    if(fb.fence)
    {
      auto checkFenceState = glClientWaitSync(fb.fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
      if(checkFenceState == GL_ALREADY_SIGNALED || checkFenceState == GL_CONDITION_SATISFIED)
      {
        // Ready so push directly to Read queue
        mTextureReadQueue.push_back(fbId);
      }
      else
      {
        // Still busy so push to Stage queue
        mTextureStageQueue.push_back(fbId);
      }
    }
  }

  void EnqueueTextureDrawBuffer(uint32_t fbId)
  {
    // push ready texture to front of 'read' queue
    std::scoped_lock<std::recursive_mutex> lock(mTextureQueueMutex);
    mTextureDrawQueue.push_back(fbId);
  }

  int32_t DequeueTextureReadBuffer(FrameBufferTexture& framebufferTexture)
  {
    // executed by DALi RenderThread!
    std::deque<uint32_t>                   backTextures;
    std::scoped_lock<std::recursive_mutex> lock(mTextureQueueMutex);

    if(mTextureReadQueue.empty())
    {
      EnqueueStagedTexture();
    }
    else
    {
      while(!mTextureStageQueue.empty())
      {
        // we have something to render, so discard
        auto stagedId = mTextureStageQueue.back();
        EnqueueTextureDrawBuffer(stagedId);
        mTextureStageQueue.pop_back();
      }
    }

    if(mTextureReadQueue.empty())
    {
      return -1;
    }

    auto retval = mTextureReadQueue.back();
    mTextureReadQueue.pop_back();

    // drain all back images and return them to the 'draw' queue
    // and remove old images
    while(!mTextureReadQueue.empty())
    {
      auto texId = mTextureReadQueue.back();
      if(framebufferTexture.fence)
      {
        glDeleteSync(framebufferTexture.fence);
        framebufferTexture.fence = nullptr;
      }
      mTextureDrawQueue.push_front(texId);
      mTextureReadQueue.pop_back();
    }

    return int32_t(retval);
  }

  /**
   * Enqueues previously staged texture
   */
  uint32_t EnqueueStagedTexture()
  {
    // test stage queue
    std::deque<uint32_t> stagedQueue;
    bool                 found  = false;
    uint32_t             retval = 0;
    while(!mTextureStageQueue.empty())
    {
      auto  stagedId = mTextureStageQueue.front();
      auto& fb       = mFramebufferTexture[stagedId];
      if(!found)
      {
        auto syncResult = glClientWaitSync(fb.fence, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
        if(syncResult == GL_CONDITION_SATISFIED || syncResult == GL_ALREADY_SIGNALED)
        {
          // push texture into the queue
          mTextureReadQueue.push_back(stagedId);
          retval = stagedId;
          found  = true;
        }
        else
        {
          stagedQueue.push_back(stagedId);
        }
      }
      else
      {
        stagedQueue.push_back(stagedId);
      }
      mTextureStageQueue.pop_front();
    }
    mTextureStageQueue = std::move(stagedQueue);
    return retval;
  }

  uint32_t CreateFramebuffer(uint32_t index, uint32_t width, uint32_t height)
  {
    auto& fb = mFramebufferTexture[index];
    if(!fb.framebufferId)
    {
      GLuint offscreenFramebuffer, renderBuffer;
      GL(glGenFramebuffers(1, &offscreenFramebuffer))
      GL(glBindFramebuffer(GL_FRAMEBUFFER, offscreenFramebuffer));
      GL(glGenRenderbuffers(1, &renderBuffer));
      GL(glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer));
      GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb.textureId, 0));
      GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height))
      GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer));
      fb.framebufferId = offscreenFramebuffer;

      [[maybe_unused]] auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      DALI_ASSERT_ALWAYS(result == GL_FRAMEBUFFER_COMPLETE && "Framebuffer incomplete!");
      return offscreenFramebuffer;
    }
    return 0u;
  }

  /**
   * Constructor
   */
  explicit Impl(const NativeRendererCreateInfo& renderCreateInfo)
  : mCreateInfo(renderCreateInfo)
  {
  }

  /**
   * Destructor
   */
  ~Impl()
  {
    Terminate();
    if(mThread)
    {
      mThread->join();
    }
  }

  /**
   * Initializes renderer thread for offscreen rendering
   */
  void InitializeThread()
  {
    // Make mRunning true first
    // Terminate() may be called before StartThread()
    mRunning = true;

    mThread = std::make_unique<std::thread>(&Impl::StartThread, this);
  }

  void PushRenderCallbackInputData(const Dali::RenderCallbackInput& renderCallbackInput)
  {
    std::scoped_lock<std::mutex> lock(mRenderCallbackInputDataMutex);
    mRenderCallbackInputData = renderCallbackInput;
  }

  void PopRenderCallbackInputData(Dali::RenderCallbackInput& renderCallbackInput)
  {
    std::scoped_lock<std::mutex> lock(mRenderCallbackInputDataMutex);
    renderCallbackInput = mRenderCallbackInputData;
  }

  void Terminate()
  {
    mRunning = false;
  }

  /**
   * Function initializes thread for parallel rendering.
   *
   * The internal loop runs until the private EGL context has been
   * initialized.
   */
  void StartThread()
  {
    // We need to acquire shared context, while this is not done
    // it's necessary to wait for context to be bound.
    while(mRunning && !mEglContextBound)
    {
      // Wait for context to be given
      if(!mEglContext)
      {
        continue;
      }
      if(!eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, mEglContext))
      {
        [[maybe_unused]] auto err = eglGetError();
        printf("%d\n", int(err));
      }
      mEglContextBound = true;
    }

    InitializeOffscreenFramebuffers();

    PrepareBlitShader();

    ThreadRunRender();
  }

  /**
   * Reinitializes offscreen framebuffers and textures in case
   * the resize has been requested.
   */
  void ReinitializeFramebuffers()
  {
    int index = 0;
    for(auto& fb : mFramebufferTexture)
    {
      if(fb.fence)
      {
        GL(glDeleteSync(fb.fence));
      }
      if(fb.framebufferId)
      {
        GL(glDeleteFramebuffers(1, &fb.framebufferId))
        fb.framebufferId = 0u;
      }
      if(fb.textureId)
      {
        GL(glDeleteTextures(1, &fb.textureId))
        fb.textureId = 0u;
      }
      fb.textureId     = CreateOffscreenTexture(mWidth, mHeight);
      fb.framebufferId = CreateFramebuffer(index, mWidth, mHeight);
      index++;
    }
  }

  void ThreadRunRender()
  {
    if(mOnInitCallback)
    {
      CallbackBase::Execute(*mOnInitCallback);
    }

    while(mRunning)
    {
      // If there is a resize request waiting, then recreate all framebuffers
      if(mResizeRequest)
      {
        ReinitializeFramebuffers();
        mResizeRequest = false;
      }

      Dali::RenderCallbackInput input;

      PopRenderCallbackInputData(input);

      uint32_t index{0u};
      auto     result = DequeueTextureDrawBuffer(index);
      if(!result)
      {
        continue;
      }

      auto& fb = mFramebufferTexture[index];
      GL(glBindFramebuffer(GL_FRAMEBUFFER, fb.framebufferId))
      GL(glClear(0));

      // Invoke callback
      if(mOnRenderCallback)
      {
        CallbackBase::ExecuteReturn<int>(*mOnRenderCallback);
      }

      // If the framebuffer is guarded with fence object then
      // delete it as at this point it is no longer valid.
      if(fb.fence)
      {
        // Make sure GPU finished
        glDeleteSync(fb.fence);
        fb.fence = nullptr;
      }

      // Inject sync object into the GL commands stream
      fb.fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
      EnqueueTextureReadBuffer(index);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    if(mOnTerminateCallback)
    {
      CallbackBase::Execute(*mOnTerminateCallback);
    }
  }

  void Resize(uint32_t width, uint32_t height)
  {
    mWidth         = width;
    mHeight        = height;
    mResizeRequest = true;
  }

  uint32_t         mWidth{0u};
  uint32_t         mHeight{0u};
  std::atomic_bool mResizeRequest{false};

  /**
   * Clones current EGL context, this function must be called from the render callback
   * and be executed on the DALi RenderThread
   */
  void CloneEglContext()
  {
    // extract shared context (void*)
    auto context = eglGetCurrentContext();

    // Obtain configs
    EGLint configId{0u};
    EGLint size{0u};
    eglGetConfigs(mEglDisplay, nullptr, 0, &size);
    std::vector<EGLConfig> configs;
    configs.resize(size);
    eglGetConfigs(mEglDisplay, configs.data(), EGLint(configs.size()), &size);

    // Find out which config is used by current context
    eglQueryContext(mEglDisplay, context, EGL_CONFIG_ID, &configId);

    // Setup EGL version
    auto                version = int(30); // TODO: get context version and select the same one
    std::vector<EGLint> attribs;
    attribs.push_back(EGL_CONTEXT_MAJOR_VERSION_KHR);
    attribs.push_back(version / 10);
    attribs.push_back(EGL_CONTEXT_MINOR_VERSION_KHR);
    attribs.push_back(version % 10);
    attribs.push_back(EGL_NONE);

    // Create cloned context with shared context
    mEglContext = eglCreateContext(mEglDisplay, configs[configId], mEglSharedContext, attribs.data());
  }

  // Pre-, Post- functions are being called from the callbacks
  void GlViewPreInit(const Dali::RenderCallbackInput& input)
  {
    // This runs on DALi RenderThread!!!

    // Bind the shared context in case of threaded rendering
    if(mThread && !mEglContextBound)
    {
      // Store the shared context just once
      if(!mEglSharedContext)
      {
        // Store the shared context returned by the drawable callback
        mEglSharedContext = std::any_cast<EGLContext>(input.eglContext);
      }
      // Setup the EGL context
      mEglDisplay = eglGetCurrentDisplay();

      // switch to shared context in order to create shared GL resources
      auto currentContext = eglGetCurrentContext();

      // Retrieve current surfaces (read and draw)
      mDrawSurface = eglGetCurrentSurface(EGL_DRAW);
      mReadSurface = eglGetCurrentSurface(EGL_READ);

      eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, mEglSharedContext);

      [[maybe_unused]] auto eglError = eglGetError();

      // Now clone it to create compatible context for our threaded rendering
      CloneEglContext();

      // Bring back current context
      eglMakeCurrent(mEglDisplay, mDrawSurface, mReadSurface, currentContext);
    }
  }

  GLuint CreateProgram(const char* vertexSource, const char* fragmentSource)
  {
    GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, vertexSource);
    if(!vertexShader)
    {
      return 0;
    }
    GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if(!fragmentShader)
    {
      return 0;
    }
    GLuint program = glCreateProgram();
    if(program)
    {
      GL(glAttachShader(program, vertexShader));
      GL(glAttachShader(program, fragmentShader));
      GL(glLinkProgram(program));
      GLint linkStatus = GL_FALSE;
      GL(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));
      if(linkStatus != GL_TRUE)
      {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        if(bufLength)
        {
          char* buf = (char*)malloc(bufLength);
          if(buf)
          {
            glGetProgramInfoLog(program, bufLength, NULL, buf);
            free(buf);
          }
        }
        glDeleteProgram(program);
        program = 0;
      }
    }
    return program;
  }

  GLuint LoadShader(GLenum shaderType, const char* shaderSource)
  {
    GLuint shader = glCreateShader(shaderType);
    if(shader != 0)
    {
      GL(glShaderSource(shader, 1, &shaderSource, NULL));
      GL(glCompileShader(shader));
      GLint compiled = 0;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
      if(compiled != GL_TRUE)
      {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if(infoLen > 0)
        {
          char* logBuffer = (char*)malloc(infoLen);

          if(logBuffer != NULL)
          {
            glGetShaderInfoLog(shader, infoLen, NULL, logBuffer);

            DALI_ASSERT_ALWAYS(true && logBuffer);

            free(logBuffer);
            logBuffer = NULL;
          }

          glDeleteShader(shader);
          shader = 0;
        }
      }
    }
    return shader;
  }

  void PrepareBlitShader()
  {
    static const char glVertexShader[] =
      "attribute vec2 vertexPosition;\n"
      "attribute vec2 texCoords;\n"
      "varying vec2 vTexCoords ;\n"
      "void main()\n"
      "{\n"
      "    gl_Position = vec4(vertexPosition, 0.0, 1.0);\n"
      "    vTexCoords = texCoords;\n"
      "}\n";

    static const char glFragmentShader[] =
      "precision mediump float;\n"
      "varying vec2 vTexCoords;\n"
      "uniform sampler2D tex;\n"
      "void main()\n"
      "{\n"
      "    gl_FragColor = texture2D(tex, vTexCoords);\n"
      "}\n";

    mBlitProgram        = CreateProgram(glVertexShader, glFragmentShader);
    mBlitVertexLocation = glGetAttribLocation(mBlitProgram, "vertexPosition");
    mBlitTexCoord       = glGetAttribLocation(mBlitProgram, "texCoords");
  }

  GLuint mBlitProgram{0u};
  GLuint mBlitVertexLocation{0u};
  GLuint mBlitTexCoord{0u};

  /**
   * Initializes FBO textures
   */
  void InitializeOffscreenFramebuffers()
  {
    std::scoped_lock<std::recursive_mutex> lock(mTextureQueueMutex);
    for(auto i = 0u; i < mCreateInfo.maxOffscreenBuffers; ++i)
    {
      mFramebufferTexture.emplace_back();
      mFramebufferTexture.back().textureId = CreateOffscreenTexture(mWidth, mHeight);

      // Populate Draw queue entries
      mTextureDrawQueue.push_back(i);

      // Create framebuffers
      CreateFramebuffer(i, mWidth, mHeight);
    }
  }

  /**
   * Creates an offscreen texture for threaded renderer
   */
  uint32_t CreateOffscreenTexture(uint32_t width, uint32_t height)
  {
    GLuint offscreenTexture{0u};
    GL(glGenTextures(1, &offscreenTexture));
    GL(glBindTexture(GL_TEXTURE_2D, offscreenTexture));
    GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    return offscreenTexture;
  }

  // Call from DALi render thread
  void GlViewInit(const Dali::RenderCallbackInput& input)
  {
    // Create worker thread solution's context
    GlViewPreInit(input);
    if(!mCreateInfo.threadEnabled)
    {
      if(mOnInitCallback)
      {
        CallbackBase::Execute(*mOnInitCallback, input);
      }
    }
  }

  // Call from DALi render thread
  int GlViewRender(const Dali::RenderCallbackInput& input)
  {
    // Non-threaded solution invokes callback directly
    int retval = 0;
    if(!mCreateInfo.threadEnabled)
    {
      if(mOnRenderCallback)
      {
        retval = CallbackBase::ExecuteReturn<int>(*mOnRenderCallback, input);
      }
    }
    else
    {
      BlitTexture();
    }
    return retval;
  }

  // Call from DALi render thread
  void GlViewTerminate(const Dali::RenderCallbackInput& input)
  {
    // Non-threaded solution invokes callback directly
    if(!mCreateInfo.threadEnabled)
    {
      if(mOnTerminateCallback)
      {
        CallbackBase::Execute(*mOnTerminateCallback, input);
      }
    }
    else
    {
      Terminate();
    }
  }

  void BlitTexture()
  {
    // If no threaded mode, return
    if(!mCreateInfo.threadEnabled)
    {
      return;
    }

    // Read input
    auto x = 0;
    auto y = 0;
    auto w = mWidth;
    auto h = mHeight;

    // Deqeueue texture, there should be always something waiting to be drawn, if not, ignore
    FrameBufferTexture fb;
    auto               textureBufferIndex = DequeueTextureReadBuffer(fb);

    // Do nothing if frame not ready
    if(textureBufferIndex < 0)
    {
      if(mLastTextureBufferIndex >= 0)
      {
        textureBufferIndex = mLastTextureBufferIndex;
      }
      else
      {
        return;
      }
    }
    else
    {
      // return last texture to the pull
      if(mLastTextureBufferIndex >= 0)
      {
        // return it to the queue
        EnqueueTextureDrawBuffer(mLastTextureBufferIndex);
      }
    }

    GL(glViewport(x, y, w, h));
    if(!mBlitStateDone)
    {
      mBlitStateDone = true;
      GL(glUseProgram(mBlitProgram));
      GL(glVertexAttribPointer(mBlitVertexLocation, 2, GL_FLOAT, GL_FALSE, 0, QUAD_VERTS));
      GL(glEnableVertexAttribArray(mBlitVertexLocation));
      GL(glVertexAttribPointer(mBlitTexCoord, 2, GL_FLOAT, GL_FALSE, 0, QUAD_UV));
      GL(glEnableVertexAttribArray(mBlitTexCoord));
      GL(glActiveTexture(GL_TEXTURE0));
    }
    GL(glBindTexture(GL_TEXTURE_2D, mFramebufferTexture[textureBufferIndex].textureId));

    GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, QUAD_INDICES))

    mLastTextureBufferIndex = textureBufferIndex;
  }

  // List of offscreen framebuffers
  std::vector<FrameBufferTexture> mFramebufferTexture{};

  /**
   * Rendering pipeline uses 3 queues:
   * Draw - the producer queue (NativeRenderer thread writes to it)
   * Read - the consumer queue (RenderThread reads from it)
   * Stage - Already written but not ready to be read (not signaled)
   *
   * // Rendering offscreen
   * 1. PRODUCER pops framebuffer from Draw queue
   * 2. PRODUCER renders into the framebuffer
   * 3. PRODUCER pushes framebuffer into Stage queue
   *
   * // Rendering onscreen
   * 1. CONSUMER checks Stage queue for signaled (ready) framebuffers
   * 2. If framebuffers are ready, pushes them into the Read queue
   * 3. If MAILBOX mode, CONSUMER discards all 'outdated' framebuffers and displays the most recent
   * 4. If FIFO mode, CONSUMER displays all the images in order of being produced.
   */
  std::deque<uint32_t> mTextureDrawQueue;
  std::deque<uint32_t> mTextureStageQueue;
  std::deque<uint32_t> mTextureReadQueue;

  // Mutex guarding the queues reads/writes
  std::recursive_mutex         mTextureQueueMutex;
  std::unique_ptr<std::thread> mThread;              ///< Thread for parallel mode
  bool                         mRunning{false};      ///< Thread running flag
  EGLContext                   mEglContext{nullptr}; ///< EGL context associated with the thread
  EGLContext                   mEglSharedContext{nullptr};

  EGLDisplay       mEglDisplay{nullptr};         ///< Current EGL display
  std::atomic_bool mEglContextBound{false};      ///< Flag indicating whether EGL context is bound
  EGLSurface       mDrawSurface{EGL_NO_SURFACE}; ///< Current EGL draw surface
  EGLSurface       mReadSurface{EGL_NO_SURFACE}; ///< Current EGL read surface

  // Callbacks associated with GlView interface
  std::unique_ptr<CallbackBase> mOnInitCallback{nullptr};
  std::unique_ptr<CallbackBase> mOnRenderCallback{nullptr};
  std::unique_ptr<CallbackBase> mOnTerminateCallback{nullptr};

  int32_t mLastTextureBufferIndex{-1};
  bool    mBlitStateDone{false};

  std::mutex                mRenderCallbackInputDataMutex{};
  Dali::RenderCallbackInput mRenderCallbackInputData{};

  NativeRendererCreateInfo mCreateInfo{};
};

DrawableViewNativeRenderer::DrawableViewNativeRenderer(const NativeRendererCreateInfo& createInfo)
: mImpl(new Impl(createInfo))
{
  if(createInfo.threadEnabled)
  {
    mImpl->InitializeThread();
  }
}

DrawableViewNativeRenderer::~DrawableViewNativeRenderer() = default;

void DrawableViewNativeRenderer::RegisterGlCallbacks(Dali::CallbackBase* onInitCallback, Dali::CallbackBase* onRenderCallback, Dali::CallbackBase* onTerminateCallback)
{
  mImpl->mOnInitCallback.reset(onInitCallback);
  mImpl->mOnRenderCallback.reset(onRenderCallback);
  mImpl->mOnTerminateCallback.reset(onTerminateCallback);
}

void DrawableViewNativeRenderer::InvokeGlInitCallback(const RenderCallbackInput& renderCallbackInput)
{
  mImpl->GlViewInit(renderCallbackInput);
}

void DrawableViewNativeRenderer::InvokeGlRenderCallback(const RenderCallbackInput& renderCallbackInput)
{
  mImpl->GlViewRender(renderCallbackInput);
}

void DrawableViewNativeRenderer::InvokeGlTerminateCallback(const RenderCallbackInput& renderCallbackInput)
{
  mImpl->GlViewTerminate(renderCallbackInput);
}

void DrawableViewNativeRenderer::Resize(uint32_t width, uint32_t height)
{
  mImpl->Resize(width, height);
}

void DrawableViewNativeRenderer::PushRenderCallbackInputData(const Dali::RenderCallbackInput& renderCallbackInput)
{
  mImpl->PushRenderCallbackInputData(renderCallbackInput);
}

void DrawableViewNativeRenderer::Terminate()
{
  mImpl->Terminate();
}

} // namespace Dali::Internal
