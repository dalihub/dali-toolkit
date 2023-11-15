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

#include "test-application.h"

#include "test-encoded-image-buffer.h"

namespace Dali
{
EncodedImageBuffer ConvertFileToEncodedImageBuffer(const char* url, EncodedImageBuffer::ImageType imageType)
{
  EncodedImageBuffer buffer;
  FILE*              fp;
  fp = fopen(url, "rb");
  if(fp != NULL)
  {
    fseek(fp, 0, SEEK_END);
    size_t                size = ftell(fp);
    Dali::Vector<uint8_t> data;
    data.Resize(size);
    fseek(fp, 0, SEEK_SET);
    size_t realSize = fread(data.Begin(), sizeof(uint8_t), size, fp);
    fclose(fp);
    data.Resize(realSize);
    buffer = EncodedImageBuffer::New(data, imageType);
  }
  return buffer;
}

} // namespace Dali
