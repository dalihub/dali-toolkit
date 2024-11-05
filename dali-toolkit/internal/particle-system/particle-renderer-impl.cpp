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

#include <dali-toolkit/internal/particle-system/particle-emitter-impl.h>
#include <dali-toolkit/internal/particle-system/particle-list-impl.h>
#include <dali-toolkit/internal/particle-system/particle-renderer-impl.h>
#include <dali/devel-api/rendering/renderer-devel.h>

#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/common/capabilities.h>
#include <dali/graphics-api/graphics-buffer.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-program.h>
#include <dali/graphics-api/graphics-shader.h>

namespace Dali::Toolkit::ParticleSystem::Internal
{
ParticleRenderer::ParticleRenderer()
{
  mStreamBufferUpdateCallback = Dali::VertexBufferUpdateCallback::New(this, &ParticleRenderer::OnStreamBufferUpdate);
}

void ParticleRenderer::SetEmitter(ParticleSystem::Internal::ParticleEmitter* emitter)
{
  mEmitter = emitter;
}

void ParticleRenderer::SetTexture(const Dali::Texture& texture)
{
  mTexture = texture;
}

void ParticleRenderer::SetBlendingMode(BlendingMode blendingMode)
{
  mBlendingMode = blendingMode;
}

BlendingMode ParticleRenderer::GetBlendingMode() const
{
  return mBlendingMode;
}

void ParticleRenderer::CreateShader()
{
  // Create shader dynamically
  auto& list        = GetImplementation(mEmitter->GetParticleList());
  auto  streamCount = list.GetStreamCount();

  static const char* ATTR_GLSL_TYPES[] =
    {
      "float", "vec2", "vec3", "vec4", "int", "ivec2", "ivec3", "ivec4"};

  static const Property::Type ATTR_TYPES[] =
    {
      Property::Type::FLOAT,
      Property::Type::VECTOR2,
      Property::Type::VECTOR3,
      Property::Type::VECTOR4,
      Property::Type::INTEGER,
      Property::Type::VECTOR2, // This represents floats but by binary write it shouldn't matter (?)
      Property::Type::VECTOR3,
      Property::Type::VECTOR4,
    };

  struct Vertex2D
  {
    Vertex2D(const Vector2& _co, const Vector2& _uv)
    : co(_co),
      uv(_uv)
    {
    }
    Dali::Vector2 co{};
    Dali::Vector2 uv{};
  };

  uint32_t      streamElementSize = 0u;
  Property::Map streamAtttributes;

  std::stringstream ss;
  for(auto i = 0u; i < streamCount; ++i)
  {
    // Don't add local streams to the shader
    if(!list.IsStreamLocal(i))
    {
      uint32_t    dataTypeSize  = list.GetStreamDataTypeSize(i);
      auto        dataTypeIndex = uint32_t(list.GetStreamDataType(i));
      const auto& streamName    = list.GetStreamName(i);
      streamElementSize += dataTypeSize;
      char key[256];
      if(streamName.empty())
      {
        snprintf(key, sizeof(key), "aStreamAttr_%d", i);
      }
      else
      {
        snprintf(key, sizeof(key), "%s", streamName.c_str());
      }
      streamAtttributes.Add(key, ATTR_TYPES[dataTypeIndex]);

      // Add shader attribute line
      ss << "INPUT mediump " << ATTR_GLSL_TYPES[dataTypeIndex] << " " << key << ";\n";
    }
  }

  auto streamAttributesStr = ss.str();

  /**
   * - The MVP comes from the Actor that the particle renderer is attached to
   * - Attributes are added dynamically based on the particle system properties
   * - There are two buffers bound
   *   * Geometry buffer (in this instance, a quad)
   *   * ParticleSystem stream buffer with interleaved data
   * - ParticleSystem buffer is being updated every frame
   */
  std::string vertexShaderCode = streamAttributesStr + std::string(
                                                         "//@version 100\n\
      INPUT mediump vec2 aPosition;\n\
      INPUT mediump vec2 aTexCoords;\n\
      \n\
      UNIFORM_BLOCK VertBlock \n\
      {\n\
      UNIFORM mediump mat4   uMvpMatrix;\n\
      UNIFORM mediump vec3   uSize;\n\
      UNIFORM lowp vec4      uColor;\n\
      };\n\
      OUTPUT mediump vec2 vTexCoord;\n\
      OUTPUT mediump vec4 vColor;\n\
      \n\
      void main()\n\
      {\n\
        vec4 pos = vec4(aPosition, 0.0, 1.0) * vec4(aStreamScale, 1.0);\n\
        vec4 position =  pos + vec4(aStreamPosition, 0.0);\n\
        vTexCoord     = aTexCoords;\n\
        vColor = uColor * aStreamColor;\n\
        gl_Position   = uMvpMatrix * position ;\n\
      }\n");

  std::string fragmentShaderCode =
    {"//@version 100\n\
      INPUT mediump vec2       vTexCoord;\n\
      INPUT mediump vec4       vColor;\n\
      UNIFORM sampler2D sTexture;\n\
      \n\
      void main()\n\
      {\n\
        lowp vec4 col = TEXTURE(sTexture, vTexCoord) * vColor;\n\
        if(col.a < 0.1) { discard; }\
        gl_FragColor = col;\n\
      }\n"};

  mShader   = Shader::New(Dali::Shader::GetVertexShaderPrefix() + vertexShaderCode, Dali::Shader::GetFragmentShaderPrefix() + fragmentShaderCode);
  mGeometry = Geometry::New();

  // Configure geometry attributes
  Property::Map geometryMap;
  geometryMap.Add("aPosition", Dali::Property::VECTOR2);
  geometryMap.Add("aTexCoords", Dali::Property::VECTOR2);

  // One vertex buffer with geometry
  VertexBuffer vertexBuffer0 = VertexBuffer::New(geometryMap);

  // fill the buffer entirely
  // 2D quad
  const static Vector2 C(0.5f, 0.5f);
  struct Quad2D
  {
    Vertex2D a0{Vector2(0.0f, 0.0f) - C, Vector2(0.0f, 0.0f)};
    Vertex2D a1{Vector2(1.0f, 0.0f) - C, Vector2(1.0f, 0.0f)};
    Vertex2D a2{Vector2(1.0f, 1.0f) - C, Vector2(1.0f, 1.0f)};
    Vertex2D a3{Vector2(0.0f, 0.0f) - C, Vector2(0.0f, 0.0f)};
    Vertex2D a4{Vector2(1.0f, 1.0f) - C, Vector2(1.0f, 1.0f)};
    Vertex2D a5{Vector2(0.0f, 1.0f) - C, Vector2(0.0f, 1.0f)};
  } QUAD;

  std::vector<Quad2D> quads;
  quads.resize(mEmitter->GetParticleList().GetCapacity());
  std::fill(quads.begin(), quads.end(), QUAD);
  vertexBuffer0.SetData(quads.data(), 6u * quads.size());

  // Second vertex buffer with stream data
  VertexBuffer vertexBuffer1 = VertexBuffer::New(streamAtttributes);

  /**
   * For more efficient stream management we need to support glVertexAttribDivisor() function.
   * This will allow step 1 attribute per 4 vertices (GLES3+). Problem: DALi doesn't support instancing
   *
   * For older GLES2 we need to duplicate stream data (4x more memory in case of using a quad geometry)
   *
   * Point-sprites may be of use in the future (problem: point sprites use screen space)
   */

  // Based on the particle system, populate buffer
  mGeometry.AddVertexBuffer(vertexBuffer0);
  mGeometry.AddVertexBuffer(vertexBuffer1);

  mGeometry.SetType(Geometry::TRIANGLES);

  mVertexBuffer = vertexBuffer0;
  mStreamBuffer = vertexBuffer1;

  // Set some initial data for streambuffer to force initialization
  std::vector<uint8_t> data;
  // Resize using only-non local streams
  auto elementSize = mEmitter->GetParticleList().GetParticleDataSize(false);
  data.resize(elementSize *
              mEmitter->GetParticleList().GetCapacity() * 6u);
  mStreamBuffer.SetData(data.data(), mEmitter->GetParticleList().GetCapacity() * 6u); // needed to initialize

  // Sets up callback
  mStreamBuffer.SetVertexBufferUpdateCallback(std::move(mStreamBufferUpdateCallback));

  mRenderer = Renderer::New(mGeometry, mShader);

  mRenderer.SetProperty(DevelRenderer::Property::RENDERING_BEHAVIOR, DevelRenderer::Rendering::CONTINUOUSLY);
  // If no texture created, the substitute rect 2x2 texture will be used
  if(!mTexture)
  {
    mTexture         = Texture::New(TextureType::TEXTURE_2D, Pixel::RGBA8888, 2u, 2u);
    auto* pixelArray = new uint32_t[4]{
      0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

    auto pixelData = PixelData::New(reinterpret_cast<uint8_t*>(pixelArray), 16, 2, 2, Pixel::Format::RGBA8888, PixelData::DELETE_ARRAY);
    mTexture.Upload(pixelData);
  }
  mTextureSet = TextureSet::New();
  mTextureSet.SetTexture(0, mTexture);
  mRenderer.SetTextures(mTextureSet);
  mTextureSet.SetSampler(0, Sampler());

  if(mBlendingMode == BlendingMode::SCREEN)
  {
    if(Dali::Capabilities::IsBlendEquationSupported(Dali::DevelBlendEquation::SCREEN))
    {
      mEmitter->GetActor().SetProperty(Dali::DevelActor::Property::BLEND_EQUATION, Dali::DevelBlendEquation::SCREEN);
    }
    else // Fallback to default
    {
      mRenderer.SetProperty(Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD);
    }
  }
  else
  {
    mRenderer.SetProperty(Renderer::Property::BLEND_EQUATION_RGB, BlendEquation::ADD);
  }
}

uint32_t ParticleRenderer::OnStreamBufferUpdate(void* streamData, size_t size)
{
  auto& list = GetImplementation(mEmitter->GetParticleList());

  auto particleCount    = list.GetActiveParticleCount(); // active particle count
  auto particleMaxCount = list.GetParticleCount();
  if(!particleCount)
  {
    return 0;
  }

  auto streamCount = list.GetStreamCount();

  auto elementSize = 0u; // elements size should be cached (it's also stride of buffer) (in bytes)
  for(auto i = 0u; i < streamCount; ++i)
  {
    if(!list.IsStreamLocal(i))
    {
      elementSize += list.GetStreamDataTypeSize(i);
    }
  }

  // Prepare source buffer (MUST BE OPTIMIZED TO AVOID ALLOCATING AND COPYING!!)
  auto totalSize = particleMaxCount * elementSize * 6u;

  // buffer sizes must match
  if(totalSize != size)
  {
    // ASSERT here ?
    return 0;
  }

  auto* dst = reinterpret_cast<uint8_t*>(streamData);

  auto& particles = list.GetParticles();

  // prepare worker threads
  auto workerCount = GetThreadPool().GetWorkerCount();

  // divide particles if over the threshold

  [[maybe_unused]] bool runParallel = true;
  if(!mEmitter->IsParallelProcessingEnabled() || particleCount < workerCount * 10) // don't run parallel if only a few particles to update
  {
    runParallel = false;
  }
  else
  {
    // Partial to handle
    auto partialSize = (particleCount / workerCount);

    struct UpdateTask
    {
      UpdateTask(Internal::ParticleRenderer& renderer, Internal::ParticleList& list, uint32_t particleStartIndex, uint32_t particleCount, void* basePtr)
      : owner(renderer),
        particleList(list),
        startIndex(particleStartIndex),
        count(particleCount),
        ptr(reinterpret_cast<uint8_t*>(basePtr))
      {
      }

      void Update()
      {
        // execute task
        owner.UpdateParticlesTask(particleList, startIndex, count, ptr);
      }

      Internal::ParticleRenderer& owner;
      Internal::ParticleList&     particleList;
      uint32_t                    startIndex;
      uint32_t                    count;
      uint8_t*                    ptr;
    };

    std::vector<UpdateTask> tasks;
    tasks.reserve(workerCount);
    std::vector<Task> taskQueue;
    auto              count = partialSize;

    for(auto i = 0u; i < workerCount; ++i)
    {
      auto index = i * partialSize;
      count      = partialSize;

      // make sure there's no leftover particles!
      if(i == workerCount - 1 && index + count < particleCount)
      {
        count = particleCount - index;
      }

      tasks.emplace_back(*this, list, index, count, streamData);
      taskQueue.emplace_back([&t = tasks.back()](uint32_t threadId) { t.Update(); });
    }

    // Execute worker tasks
    auto future = GetThreadPool().SubmitTasks(taskQueue, 0);
    // wait to finish
    future->Wait();
  }

  // less particles so run on a single thread
  if(!runParallel)
  {
    for(auto& p : particles)
    {
      // without instancing we need to duplicate data 4 times per each quad
      auto* particleDst = dst;
      for(auto s = 0u; s < streamCount; ++s)
      {
        if(!list.IsStreamLocal(s))
        {
          // Pointer to stream value
          auto* valuePtr = &p.GetByIndex<uint8_t*>(s);

          // Size of data
          auto dataSize = list.GetStreamDataTypeSize(s);

          memcpy(dst, valuePtr, dataSize);
          dst += dataSize;
        }
      }
      // Replicate data 5 more times for each vertex (GLES2)
      memcpy(dst, particleDst, elementSize);
      dst += elementSize;
      memcpy(dst, particleDst, elementSize);
      dst += elementSize;
      memcpy(dst, particleDst, elementSize);
      dst += elementSize;
      memcpy(dst, particleDst, elementSize);
      dst += elementSize;
      memcpy(dst, particleDst, elementSize);
      dst += elementSize;
    }
  }
  return particleCount * 6u; // return number of elements to render
}

Renderer ParticleRenderer::GetRenderer() const
{
  return mRenderer;
}

void ParticleRenderer::UpdateParticlesTask(Internal::ParticleList& list,
                                           uint32_t                particleStartIndex,
                                           uint32_t                particleCount,
                                           uint8_t*                basePtr)
{
  auto& particles   = list.GetParticles();
  auto  streamCount = list.GetStreamCount();
  auto  elementSize = list.GetStreamElementSize(false);

  // calculate begin of buffer
  uint8_t* dst = (basePtr + (elementSize * 6u) * particleStartIndex);

  auto it = particles.begin();
  std::advance(it, particleStartIndex);

  for(; particleCount; particleCount--, it++)
  {
    ParticleSystem::Particle& p = *it;
    // without instancing we need to duplicate data 4 times per each quad
    auto* particleDst = dst;
    for(auto s = 0u; s < streamCount; ++s)
    {
      if(!list.IsStreamLocal(s))
      {
        // Pointer to stream value
        auto* valuePtr = &p.GetByIndex<uint8_t*>(s);

        // Size of data
        auto dataSize = list.GetStreamDataTypeSize(s);

        memcpy(dst, valuePtr, dataSize);
        dst += dataSize;
      }
    }
    // Replicate data 5 more times for each vertex (GLES2)
    memcpy(dst, particleDst, elementSize);
    dst += elementSize;
    memcpy(dst, particleDst, elementSize);
    dst += elementSize;
    memcpy(dst, particleDst, elementSize);
    dst += elementSize;
    memcpy(dst, particleDst, elementSize);
    dst += elementSize;
    memcpy(dst, particleDst, elementSize);
    dst += elementSize;
  }
}

bool ParticleRenderer::Initialize()
{
  if(!mInitialized)
  {
    CreateShader();
    mInitialized = true;
    return true;
  }

  return false;
}

void ParticleRenderer::PrepareToDie()
{
  if(mStreamBuffer)
  {
    mStreamBuffer.ClearVertexBufferUpdateCallback();
  }
}

} // namespace Dali::Toolkit::ParticleSystem::Internal
