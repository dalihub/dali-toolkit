#ifndef TEST_ENCODED_IMAGE_BUFFER_H
#define TEST_ENCODED_IMAGE_BUFFER_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>

namespace DALI_NAMESPACE
{
// util function to convert local file to EncodedImageBuffer
EncodedImageBuffer ConvertFileToEncodedImageBuffer(const char* url, EncodedImageBuffer::ImageType imageType = EncodedImageBuffer::ImageType::DEFAULT);

} //namespace DALI_NAMESPACE

#endif // TEST_ENCODED_IMAGE_BUFFER_H
