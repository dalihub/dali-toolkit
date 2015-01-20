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
 *
 */

// CLASS HEADER
#include "frame-buffer-image-api.h"

// EXTERNAL INCLUDES
#include <dali/public-api/images/native-image.h>

// INTERNAL INCLUDES
#include <v8-utils.h>
#include <image/image-wrapper.h>


namespace Dali
{

namespace V8Plugin
{

namespace NativeImageApi
{

/**
 * ## NativeImage **Not Supported**
 * NativeImage represents an image resource that can be added to ImageActors.
 * Its data is provided by native resources, such as shared bitmap memory or pixmap from X11 or ECORE-X11, etc.
 * @class NativeImage
 * @extends Image
 */

/**
 * **Currently not supported. This is a Place holder**
 * @constructor
 * @method NativeImage
 * @for NativeImage
 * @return {Object} Image
 */
Image New( const v8::FunctionCallbackInfo< v8::Value >& args )
{
  // return empty image handle
  return NativeImage();
}

} // NativeImageApi

} // namespace V8Plugin

} // namespace Dali
