/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <string>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>

namespace Dali
{

namespace FileLoader
{

int ReadFile(
  const std::string& filename,
  Dali::Vector<char> & memblock,
  FileLoader::FileType fileType)
{
  return 0;
}

int ReadFile(
  const std::string& filename,
  std::streampos& fileSize,
  Dali::Vector<char> & memblock,
  FileLoader::FileType fileType)
{
  return 0;
}

std::streampos GetFileSize(const std::string& filename)
{
  return 0;
}

}
}
