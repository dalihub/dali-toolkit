#ifndef DALI_TOOLKIT_BITMAP_LOADER_H
#define DALI_TOOLKIT_BITMAP_LOADER_H
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#define DALI_BITMAP_LOADER_H

#include <string>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/images/image-operations.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Internal
{
class BitmapLoader;
}

class DALI_IMPORT_API BitmapLoader : public BaseHandle
{
public:

  static BitmapLoader New( const std::string& url,
                           ImageDimensions size = ImageDimensions( 0, 0 ),
                           FittingMode::Type fittingMode = FittingMode::DEFAULT,
                           SamplingMode::Type samplingMode = SamplingMode::BOX_THEN_LINEAR,
                           bool orientationCorrection = true);
  BitmapLoader();
  ~BitmapLoader();
  BitmapLoader(const BitmapLoader& handle);
  BitmapLoader& operator=(const BitmapLoader& rhs);

public:
  void Load();
  bool IsLoaded();
  std::string GetUrl() const;
  PixelData GetPixelData() const;

  void WaitForLoading();
  static BitmapLoader GetLatestCreated(); // return the latest created BitmapLoader
  static void ResetLatestCreated();

public: // Not intended for application developers

  explicit DALI_INTERNAL BitmapLoader(Internal::BitmapLoader*);
};

} // Dali

#endif // DALI_TOOLKIT_BITMAP_LOADER_H
