#ifndef __DALI_JAVASCRIPT_APPLICATION_OPTIONS_H__
#define __DALI_JAVASCRIPT_APPLICATION_OPTIONS_H__

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

#include <public-api/dali-core.h>
#include <public-api/adaptor-framework/window.h>
#include <v8.h>

namespace DaliNodeAddon
{

struct WindowOptions
{
  WindowOptions()
  : positionSize(0,0,0,0)
  {
  }

  std::string name;
  Dali::PositionSize positionSize;
  bool transparent:1;
};

struct StereoScopicOptions
{
  StereoScopicOptions()
  : viewMode(Dali::MONO),
    stereoBase(65.f)
  {
  }

  Dali::ViewMode viewMode;
  float stereoBase;
};

struct ApplicationOptions
{
  WindowOptions window;
  StereoScopicOptions stereo;
  std::string stylesheet;
};

/**
 * @brief parse the dali JavaScript application options
 * The options are passed to dali when require is called.
 * E.g. var dali.require('dali.js)(  {options object}  )
 *
 * @param[in] args JavaScript arguments
 * @param[out] options assigned Dali options
 * @return true on success, false on failure
 *
 */
bool GetApplicationOptions( const v8::FunctionCallbackInfo<v8::Value>& args, ApplicationOptions& options );


} // namespace DaliNodeAddon



#endif // __DALI_JAVASCRIPT_APPLICATION_OPTIONS_H__
