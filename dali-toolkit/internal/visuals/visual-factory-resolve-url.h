#ifndef DALI_TOOLKIT_VISUAL_FACTORY_URL_RESOLVE_H
#define DALI_TOOLKIT_VISUAL_FACTORY_URL_RESOLVE_H

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

#include <ctype.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace UrlType
{
  /**
   * The type of the URL based on the string contents
   */
  enum Type
  {
    REGULAR_IMAGE,
    N_PATCH,
    SVG,
    GIF
  };
}

/**
 * Helper to resolve URL type from the string
 * @param[in] url to check
 * @return UrlType
 */
inline UrlType::Type ResolveUrlType( const std::string& url )
{
  // if only one char in string, can only be regular image
  const std::size_t count = url.size();
  if( count > 0 )
  {
    // parsing from the end for better chance of early outs
    enum { SUFFIX, HASH, HASH_DOT } state = SUFFIX;
    char SVG[ 4 ] = { 'g', 'v', 's', '.' };
    char GIF[ 4 ] = { 'f', 'i', 'g', '.' };
    unsigned int svgScore = 0;
    unsigned int gifScore = 0;
    int index = count;
    while( --index >= 0 )
    {
      const char currentChar = url[ index ];
      const std::size_t offsetFromEnd = count - index - 1u;
      if( ( offsetFromEnd < sizeof(SVG) )&&( tolower( currentChar ) == SVG[ offsetFromEnd ] ) )
      {
        // early out if SVG as can't be used in N patch for now
        if( ++svgScore == sizeof(SVG) )
        {
          return UrlType::SVG;
        }
      }
      if( ( offsetFromEnd < sizeof(GIF) )&&( tolower( currentChar ) == GIF[ offsetFromEnd ] ) )
      {
        // early out if GIF
        if( ++gifScore == sizeof(GIF) )
        {
          return UrlType::GIF;
        }
      }
      switch( state )
      {
        case SUFFIX:
        {
          if( '.' == currentChar )
          {
            state = HASH;
          }
          break;
        }
        case HASH:
        {
          if( ( '#' == currentChar ) || ( '9' == currentChar ) )
          {
            state = HASH_DOT;
          }
          else
          {
            // early out, not a valid N/9-patch URL
            return UrlType::REGULAR_IMAGE;
          }
          break;
        }
        case HASH_DOT:
        {
          if( '.' == currentChar )
          {
            return UrlType::N_PATCH;
          }
          else
          {
            // early out, not a valid N/9-patch URL
            return UrlType::REGULAR_IMAGE;
          }
          break;
        }
      }
    }
  }
  // if we got here it is a regular image
  return UrlType::REGULAR_IMAGE;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif /* DALI_TOOLKIT_VISUAL_FACTORY_URL_RESOLVE_H */
