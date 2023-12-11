/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

#include <chipmunk/chipmunk.h>
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-adaptor-impl.h>
#include <dali-physics/internal/chipmunk-impl/chipmunk-physics-debug-renderer.h>

namespace
{
GLuint LoadShader(GLenum shaderType, const char* shaderSource)
{
  GLuint shader = glCreateShader(shaderType);
  if(shader != 0)
  {
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(compiled != GL_TRUE)
    {
      GLint infoLen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

      if(infoLen > 0)
      {
        std::vector<char> logBuffer;
        logBuffer.resize(infoLen + 1);
        glGetShaderInfoLog(shader, infoLen, NULL, &logBuffer[0]);
        fprintf(stderr, "%s\n", &logBuffer[0]);
        fflush(stderr);

        glDeleteShader(shader);
        shader = 0;
      }
    }
  }
  return shader;
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
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if(linkStatus != GL_TRUE)
    {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if(bufLength)
      {
        std::vector<char> logBuffer;
        logBuffer.resize(bufLength + 1);
        glGetProgramInfoLog(program, bufLength, NULL, &logBuffer[0]);
        fprintf(stderr, "%s\n", &logBuffer[0]);
        fflush(stderr);
      }
      glDeleteProgram(program);
      program = 0;
    }
  }
  return program;
}
} // namespace

namespace Dali::Toolkit::Physics::Internal
{
static void DebugDrawCircleImpl(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawCircle(pos, angle, radius, outlineColor, fillColor);
}

static void DebugDrawSegmentImpl(cpVect a, cpVect b, cpSpaceDebugColor color, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawSegment(a, b, color);
}

void DebugDrawFatSegmentImpl(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawFatSegment(a, b, radius, outlineColor, fillColor);
}

void DebugDrawPolygonImpl(int count, const cpVect* verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawPolygon(count, verts, radius, outlineColor, fillColor);
}

void DebugDrawDotImpl(cpFloat size, cpVect pos, cpSpaceDebugColor color, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  debugRenderer->DrawDot(size, pos, color);
}

cpSpaceDebugColor DebugDrawColorForShapeImpl(cpShape* shape, cpDataPointer data)
{
  auto debugRenderer = static_cast<PhysicsDebugRenderer*>(data);
  return debugRenderer->DrawColorForShape(shape);
}

std::unique_ptr<PhysicsDebugRenderer> PhysicsDebugRenderer::New(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor)
{
  auto renderer = std::make_unique<PhysicsDebugRenderer>(width, height, camera, adaptor);

  renderer->mRenderCallback = Dali::RenderCallback::New<PhysicsDebugRenderer>(renderer.get(), &PhysicsDebugRenderer::OnRender);
  return renderer;
}

PhysicsDebugRenderer::PhysicsDebugRenderer(uint32_t width, uint32_t height, Dali::CameraActor camera, PhysicsAdaptor* adaptor)
: mCamera(camera),
  mWidth(width),
  mHeight(height),
  mAdaptor(*adaptor),
  mPositionLocation(-1),
  mUvsLocation(-1),
  mRadiusLocation(-1),
  mFillColourLocation(-1),
  mOutlineColourLocation(-1),
  mProjectionLocation(-1),
  mModelViewLocation(-1),
  mIndexBufferId(0u),
  mVertexBufferId(0u),
  mProgramId(0u)
{
  mDebugDrawOptions.drawCircle     = DebugDrawCircleImpl;
  mDebugDrawOptions.drawSegment    = DebugDrawSegmentImpl;
  mDebugDrawOptions.drawFatSegment = DebugDrawFatSegmentImpl;
  mDebugDrawOptions.drawPolygon    = DebugDrawPolygonImpl;
  mDebugDrawOptions.drawDot        = DebugDrawDotImpl;

  mDebugDrawOptions.flags               = static_cast<cpSpaceDebugDrawFlags>(CP_SPACE_DEBUG_DRAW_SHAPES |
                                                               CP_SPACE_DEBUG_DRAW_COLLISION_POINTS |
                                                               CP_SPACE_DEBUG_DRAW_CONSTRAINTS);
  mDebugDrawOptions.colorForShape       = DebugDrawColorForShapeImpl;
  mDebugDrawOptions.shapeOutlineColor   = cpSpaceDebugColor{0.0f, 1.0f, 1.0f, 0.9f};
  mDebugDrawOptions.constraintColor     = cpSpaceDebugColor{0.5f, 0.5f, 0.5f, 0.9f};
  mDebugDrawOptions.collisionPointColor = cpSpaceDebugColor{1.0f, 0.0f, 0.0f, 1.0f};
  mDebugDrawOptions.data                = this;
}

bool PhysicsDebugRenderer::OnRender(const Dali::RenderCallbackInput& input)
{
  if(mState == State::INIT)
  {
    Setup();
    mState = State::RENDER;
  }
  glViewport(0, 0, mWidth, mHeight);

  RenderLines(input);

  return false;
}

// Run on first invocation of callback
void PhysicsDebugRenderer::Setup()
{
  PrepareShader();
  mPositionLocation      = glGetAttribLocation(mProgramId, "position");
  mUvsLocation           = glGetAttribLocation(mProgramId, "uvs");
  mRadiusLocation        = glGetAttribLocation(mProgramId, "radius");
  mFillColourLocation    = glGetAttribLocation(mProgramId, "fillColor");
  mOutlineColourLocation = glGetAttribLocation(mProgramId, "outlineColor");

  mProjectionLocation = glGetUniformLocation(mProgramId, "projection");
  mModelViewLocation  = glGetUniformLocation(mProgramId, "modelView");

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, mWidth, mHeight);

  glGenBuffers(1, &mIndexBufferId);
  glGenBuffers(1, &mVertexBufferId);
}

void PhysicsDebugRenderer::UpdateWindowSize(Dali::Vector2 size)
{
  mWidth  = size.width;
  mHeight = size.height;
}

void PhysicsDebugRenderer::PrepareShader()
{
  static const char glVertexShader[] =
    "#version 300 es\n"
    "in vec2 position;\n"
    "in vec2 uvs;\n"
    "in float radius;\n"
    "in vec4 fillColor;\n"
    "in vec4 outlineColor;\n"
    "out vec2 v_uvs;\n"
    "out vec4 v_fill;\n"
    "out vec4 v_outline;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 modelView;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * modelView * vec4(position.xy+radius*uvs, 0.0, 1.0);\n"
    "    v_uvs=uvs;\n"
    "    v_fill = fillColor;\n"
    "    v_fill.rgb *= v_fill.a;\n"
    "    v_outline = outlineColor;\n"
    "    v_outline.a *= v_outline.a;\n"
    "}\n";

  static const char glFragmentShader[] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 v_uvs;\n"
    "in vec4 v_fill;\n"
    "in vec4 v_outline;\n"
    "out vec4 fragColor;\n"
    "void main()\n"
    "{\n"
    "    float len=length(v_uvs);\n"
    "    float fw = length(vec2(dFdx(len), dFdy(len)));\n"
    "    float mask=smoothstep(-1.0, fw-1.0, -len);\n"
    "    float outline=1.0-fw;\n"
    "    float outline_mask=smoothstep(outline-fw, outline, len);\n"
    "    vec4 color = v_fill + (v_outline - v_fill*v_outline.a)*outline_mask;\n"
    "    fragColor = color*mask;\n"
    "}\n";

  mProgramId = CreateProgram(glVertexShader, glFragmentShader);
}

void PhysicsDebugRenderer::RenderLines(const Dali::RenderCallbackInput& input)
{
  mModelViewMatrix.SetIdentity();
  mProjectionMatrix = input.projection;

  Matrix::Multiply(mModelViewMatrix, mModelViewMatrix, input.view);
  glUseProgram(mProgramId);

  // In theory, input.clippingBox should tell us the actor position in clip-space.
  // But, it appears to be bugged.
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(mIndices.size() * sizeof(uint16_t)), &mIndices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(mVertices.size() * sizeof(Vertex)), &mVertices[0], GL_STATIC_DRAW);

  GLint stride = 52; // 4*(2 + 2 + 1 + 4 + 4) = 4*13=52
  glVertexAttribPointer(mPositionLocation, 2, GL_FLOAT, GL_FALSE, stride, 0);
  glEnableVertexAttribArray(mPositionLocation);

  glVertexAttribPointer(mUvsLocation, 2, GL_FLOAT, GL_FALSE, stride, (const void*)8);
  glEnableVertexAttribArray(mUvsLocation);

  glVertexAttribPointer(mRadiusLocation, 1, GL_FLOAT, GL_FALSE, stride, (const void*)16);
  glEnableVertexAttribArray(mRadiusLocation);

  glVertexAttribPointer(mFillColourLocation, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(20));
  glEnableVertexAttribArray(mFillColourLocation);
  glVertexAttribPointer(mOutlineColourLocation, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const void*>(36));
  glEnableVertexAttribArray(mOutlineColourLocation);

  glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, mProjectionMatrix.AsFloat());
  glUniformMatrix4fv(mModelViewLocation, 1, GL_FALSE, mModelViewMatrix.AsFloat());

  glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, 0);
  mIndices.clear();
  mVertices.clear();
}

PhysicsDebugRenderer::Vertex* PhysicsDebugRenderer::PushVertices(uint32_t vertexCount, uint32_t indexCount, const uint16_t* indices)
{
  auto base = mVertices.size();
  mVertices.resize(mVertices.size() + vertexCount);
  mIndices.reserve(mIndices.size() + indexCount);
  for(uint32_t i = 0; i < indexCount; ++i)
  {
    mIndices.push_back(base + indices[i]);
  }

  return &mVertices[base];
}

PhysicsDebugRenderer::Vertex PhysicsDebugRenderer::MakeVertex(cpVect pos, float u, float v, float r, Vector4 fill, Vector4 outline)
{
  auto daliPos = mAdaptor.TranslateFromPhysicsSpace(Vector3((float)pos.x, (float)pos.y, 0.0f));
  return Vertex{Vector2(daliPos.x, daliPos.y), Vector2(u, v), r, fill, outline};
}

void PhysicsDebugRenderer::DrawCircle(cpVect pos, cpFloat angle, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
  float                 r = (float)radius + mPointLineScale;
  Vector4               fill(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
  Vector4               outline(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);
  static const uint16_t indices[] = {0, 1, 2, 0, 2, 3};

  Vertex* vertices = PushVertices(4, 6, indices);

  vertices[0] = MakeVertex(pos, -1, -1, r, fill, outline);
  vertices[1] = MakeVertex(pos, -1, 1, r, fill, outline);
  vertices[2] = MakeVertex(pos, 1, 1, r, fill, outline);
  vertices[3] = MakeVertex(pos, 1, -1, r, fill, outline);

  DrawSegment(pos, cpvadd(pos, cpvmult(cpvforangle(angle), 0.75f * radius)), outlineColor);
}

void PhysicsDebugRenderer::DrawSegment(cpVect a, cpVect b, cpSpaceDebugColor color)
{
  DrawFatSegment(a, b, 0.0f, color, color);
}

void PhysicsDebugRenderer::DrawFatSegment(cpVect a, cpVect b, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
  static const uint16_t indices[] = {0, 1, 2, 1, 2, 3, 2, 3, 4, 3, 4, 5, 4, 5, 6, 5, 6, 7};
  Vertex*               vertices  = PushVertices(8, 18, indices);

  cpVect t = cpvnormalize(cpvsub(b, a));

  float   r = (float)radius * mPointLineScale;
  Vector4 fill(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
  Vector4 outline(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);

  vertices[0] = MakeVertex(a, (-t.x + t.y), (-t.x - t.y), r, fill, outline);
  vertices[1] = MakeVertex(a, (-t.x - t.y), (+t.x - t.y), r, fill, outline);
  vertices[2] = MakeVertex(a, (-0.0 + t.y), (-t.x + 0.0), r, fill, outline);
  vertices[3] = MakeVertex(a, (-0.0 - t.y), (+t.x + 0.0), r, fill, outline);
  vertices[4] = MakeVertex(a, (+0.0 + t.y), (-t.x - 0.0), r, fill, outline);
  vertices[5] = MakeVertex(a, (+0.0 - t.y), (+t.x - 0.0), r, fill, outline);
  vertices[6] = MakeVertex(a, (+t.x + t.y), (-t.x + t.y), r, fill, outline);
  vertices[7] = MakeVertex(a, (+t.x - t.y), (+t.x + t.y), r, fill, outline);
}

void PhysicsDebugRenderer::DrawPolygon(int count, const cpVect* verts, cpFloat radius, cpSpaceDebugColor outlineColor, cpSpaceDebugColor fillColor)
{
  Vector4 fill(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
  Vector4 outline(outlineColor.r, outlineColor.g, outlineColor.b, outlineColor.a);

  std::vector<uint16_t> indices;
  for(int i = 0; i < count - 2; i++)
  {
    indices.push_back(0);
    indices.push_back(4 * (i + 1));
    indices.push_back(4 * (i + 2));
  }

  // Polygon outline triangles.
  for(int i0 = 0; i0 < count; i0++)
  {
    int i1 = (i0 + 1) % count;
    indices.push_back(4 * i0 + 0);
    indices.push_back(4 * i0 + 1);
    indices.push_back(4 * i0 + 2);
    indices.push_back(4 * i0 + 0);
    indices.push_back(4 * i0 + 2);
    indices.push_back(4 * i0 + 3);
    indices.push_back(4 * i0 + 0);
    indices.push_back(4 * i0 + 3);
    indices.push_back(4 * i1 + 0);
    indices.push_back(4 * i0 + 3);
    indices.push_back(4 * i1 + 0);
    indices.push_back(4 * i1 + 1);
  }

  float inset  = (float)-cpfmax(0, 2 * mPointLineScale - radius);
  float outset = (float)radius + mPointLineScale;
  float r      = outset - inset;

  Vertex* vertices = PushVertices(4 * count, 3 * (5 * count - 2), &indices[0]);
  for(int i = 0; i < count; i++)
  {
    cpVect v0     = verts[i];
    cpVect v_prev = verts[(i + (count - 1)) % count];
    cpVect v_next = verts[(i + (count + 1)) % count];

    cpVect n1 = cpvnormalize(cpvrperp(cpvsub(v0, v_prev)));
    cpVect n2 = cpvnormalize(cpvrperp(cpvsub(v_next, v0)));
    cpVect of = cpvmult(cpvadd(n1, n2), 1.0 / (cpvdot(n1, n2) + 1.0f));
    cpVect v  = cpvadd(v0, cpvmult(of, inset));

    vertices[4 * i + 0] = MakeVertex(v, 0.0f, 0.0f, 0.0f, fill, outline);
    vertices[4 * i + 1] = MakeVertex(v, (float)n1.x, (float)n1.y, r, fill, outline);
    vertices[4 * i + 2] = MakeVertex(v, (float)of.x, (float)of.y, r, fill, outline);
    vertices[4 * i + 3] = MakeVertex(v, (float)n2.x, (float)n2.y, r, fill, outline);
  }
}

void PhysicsDebugRenderer::DrawDot(cpFloat size, cpVect pos, cpSpaceDebugColor color)
{
  float                 r = (float)(size * 0.5f * mPointLineScale);
  Vector4               fill(color.r, color.g, color.b, color.a);
  static const uint16_t indices[] = {0, 1, 2, 0, 2, 3};
  Vertex*               vertex    = PushVertices(4, 6, indices);
  vertex[0]                       = MakeVertex(pos, -1, -1, r, fill, fill);
  vertex[1]                       = MakeVertex(pos, -1, 1, r, fill, fill);
  vertex[2]                       = MakeVertex(pos, 1, 1, r, fill, fill);
  vertex[3]                       = MakeVertex(pos, 1, -1, r, fill, fill);
}

cpSpaceDebugColor PhysicsDebugRenderer::DrawColorForShape(cpShape* shape)
{
  static cpSpaceDebugColor Colors[] = {
    {0xb5 / 255.0f, 0x89 / 255.0f, 0x00 / 255.0f, 1.0f},
    {0xcb / 255.0f, 0x4b / 255.0f, 0x16 / 255.0f, 1.0f},
    {0xdc / 255.0f, 0x32 / 255.0f, 0x2f / 255.0f, 1.0f},
    {0xd3 / 255.0f, 0x36 / 255.0f, 0x82 / 255.0f, 1.0f},
    {0x6c / 255.0f, 0x71 / 255.0f, 0xc4 / 255.0f, 1.0f},
    {0x26 / 255.0f, 0x8b / 255.0f, 0xd2 / 255.0f, 1.0f},
    {0x2a / 255.0f, 0xa1 / 255.0f, 0x98 / 255.0f, 1.0f},
    {0x85 / 255.0f, 0x99 / 255.0f, 0x00 / 255.0f, 1.0f},
  };

  if(cpShapeGetSensor(shape))
  {
    return cpSpaceDebugColor{1.0f, 1.0f, 1.0f, 0.1f};
  }
  else
  {
    cpBody* body = cpShapeGetBody(shape);

    if(cpBodyIsSleeping(body))
    {
      return cpSpaceDebugColor{0x58 / 255.0f, 0x6e / 255.0f, 0x75 / 255.0f, 1.0f};
    }
    else if(cpBodyIsSleepThresholdExceeded(body, shape))
    {
      return cpSpaceDebugColor{0x93 / 255.0f, 0xa1 / 255.0f, 0xa1 / 255.0f, 1.0f};
    }
    else
    {
      uint32_t val = (uint32_t)cpShapeGetHashId(shape);

      // scramble the bits up using Robert Jenkins' 32 bit integer hash function
      val = (val + 0x7ed55d16) + (val << 12);
      val = (val ^ 0xc761c23c) ^ (val >> 19);
      val = (val + 0x165667b1) + (val << 5);
      val = (val + 0xd3a2646c) ^ (val << 9);
      val = (val + 0xfd7046c5) + (val << 3);
      val = (val ^ 0xb55a4f09) ^ (val >> 16);

      return Colors[val & 0x7];
    }
  }
}

} // namespace Dali::Toolkit::Physics::Internal
