#ifndef DALI_SCENE3D_LOADER_BUFFER_DEFINITION_H
#define DALI_SCENE3D_LOADER_BUFFER_DEFINITION_H
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
 *
 */

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <fstream>
#include <memory>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Defines a buffer that is loaded from input uri.
 *
 * The buffer can contain 3D resource data such as mesh, animation, and texture.
 * @SINCE_2_2.12
 */
struct DALI_SCENE3D_API BufferDefinition
{
  using Vector = std::vector<BufferDefinition>;

  BufferDefinition();
  BufferDefinition(std::vector<uint8_t>& buffer);

  ~BufferDefinition();

  BufferDefinition(const BufferDefinition& other) = default;
  BufferDefinition& operator=(const BufferDefinition& other) = default;

  BufferDefinition(BufferDefinition&& other);
  BufferDefinition& operator=(BufferDefinition&&) = default;

  /**
   * @brief Retrieves data stream of this buffer.
   * @SINCE_2_2.12
   * @return iostream of this buffer
   */
  std::iostream& GetBufferStream();

  /**
   * @brief Retrieves uri of this buffer
   * @SINCE_2_2.12
   * @return uri of the buffer
   */
  std::string GetUri();

  /**
   * @brief Checks whether the buffer is available or not.
   *
   * It is available, if the buffer is successfully loaded from file or base64 stream.
   * @SINCE_2_2.12
   * @return True if it is available.
   */
  bool IsAvailable();

private:
  /// @cond internal
  /**
   * @brief Loads buffer from file or encoded stream.
   */
  void LoadBuffer();
  /// @endcond

public: // DATA
  std::string mResourcePath;
  std::string mUri;
  uint32_t    mByteLength{0};
  std::string mName;

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;

  bool mIsEmbedded{false};
};

} // namespace Dali::Scene3D::Loader

#endif // DALI_SCENE3D_LOADER_BUFFER_DEFINITION_H
