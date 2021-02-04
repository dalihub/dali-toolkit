#ifndef DALI_TOOLKIT_INTERNAL_BUILDER_FILESYSTEM_H
#define DALI_TOOLKIT_INTERNAL_BUILDER_FILESYSTEM_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <fstream>
#include <sstream>
#include <string>

#include <stdio.h>

#include <dali/devel-api/adaptor-framework/file-loader.h>

inline std::string GetFileContents(const std::string& fn)
{
  std::streampos     bufferSize = 0;
  Dali::Vector<char> fileBuffer;
  if(!Dali::FileLoader::ReadFile(fn, bufferSize, fileBuffer, Dali::FileLoader::FileType::BINARY))
  {
    return std::string();
  }

  return std::string(&fileBuffer[0], bufferSize);
}

#endif // DALI_TOOLKIT_INTERNAL_BUILDER_FILESYSTEM_H
