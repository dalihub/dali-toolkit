#ifndef __DALI_TOOLKIT_INTERNAL_BUILDER_FILESYSTEM_H__
#define __DALI_TOOLKIT_INTERNAL_BUILDER_FILESYSTEM_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <string>
#include <fstream>
#include <sstream>

#include <wordexp.h>
#include <stdio.h>
#include <unistd.h>

inline std::string ExpandPath(const std::string &name)
{
  wordexp_t p;
  char** w;
  wordexp( name.c_str(), &p, 0 );
  w = p.we_wordv;
  std::stringstream s;
  for (size_t i=0; i<p.we_wordc;i++ )
  {
    s << w[i];
  }
  wordfree( &p );
  return s.str();
}


inline std::string ExePath(void)
{
  char buf[256];
  readlink("/proc/self/exe", buf, sizeof(buf));
  return std::string(buf);
}

inline std::string GetFileContents(const std::string &fn)
{
  std::ifstream t(fn.c_str());
  return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

#endif // __DALI_TOOLKIT_INTERNAL_BUILDER_FILESYSTEM_H__
