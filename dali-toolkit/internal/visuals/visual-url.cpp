/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/visuals/visual-url.h>

// EXTERNAL HEADERS
#include <cstring> // for toupper()

namespace Dali
{
namespace Toolkit
{
namespace Internal
{

namespace
{

VisualUrl::Location ResolveLocation( const std::string& url)
{
  const char FTP[] = { 'f', 't', 'p', ':', '/', '/' };
  const char SSH[] = { 's', 's', 'h', ':', '/', '/' };
  const char HTTP[] = { 'h', 't', 't', 'p', ':', '/', '/' };
  const char HTTPS[] = { 'h', 't', 't', 'p', 's', ':', '/', '/' };

  const int MATCH_FTP = 0x01;
  const int MATCH_SSH = 0x02;
  const int MATCH_HTTP = 0x04;
  const int MATCH_HTTPS = 0x08;

  const char* urlCStr = url.c_str();
  if( url.size() > 6 )
  {
    if( urlCStr[3] == ':' || urlCStr[4] == ':' || urlCStr[5] == ':' )
    {
      int flags = 0x0F;
      for( unsigned int i=0; i < sizeof(HTTPS); ++i )
      {
        char c = tolower( urlCStr[i] );
        if( i < sizeof(FTP) && (flags & MATCH_FTP) && c != FTP[i] )
        {
          flags &= ~MATCH_FTP;
        }
        if( i < sizeof(SSH) && (flags & MATCH_SSH) && c != SSH[i] )
        {
          flags &= ~MATCH_SSH;
        }
        if( i < sizeof(HTTP) && (flags & MATCH_HTTP) && c != HTTP[i] )
        {
          flags &= ~MATCH_HTTP;
        }
        if( i < sizeof(HTTPS) && (flags & MATCH_HTTPS) && c != HTTPS[i] )
        {
          flags &= ~MATCH_HTTPS;
        }

        if( (flags & (MATCH_FTP | MATCH_SSH | MATCH_HTTP | MATCH_HTTPS )) == 0 )
        {
          break;
        }
      }

      if( flags )
      {
        return VisualUrl::REMOTE;
      }
    }
  }
  return VisualUrl::LOCAL;
}


VisualUrl::Type ResolveType( const std::string& url )
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
          return VisualUrl::SVG;
        }
      }
      if( ( offsetFromEnd < sizeof(GIF) )&&( tolower( currentChar ) == GIF[ offsetFromEnd ] ) )
      {
        // early out if GIF
        if( ++gifScore == sizeof(GIF) )
        {
          return VisualUrl::GIF;
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
            return VisualUrl::REGULAR_IMAGE;
          }
          break;
        }
        case HASH_DOT:
        {
          if( '.' == currentChar )
          {
            return VisualUrl::N_PATCH;
          }
          else
          {
            // early out, not a valid N/9-patch URL
            return VisualUrl::REGULAR_IMAGE;
          }
          break;
        }
      }
    }
  }
  // if we got here it is a regular image
  return VisualUrl::REGULAR_IMAGE;
}

}


VisualUrl::VisualUrl()
: mUrl(),
  mType( VisualUrl::REGULAR_IMAGE ),
  mLocation( VisualUrl::LOCAL )
{
}

VisualUrl::VisualUrl( const std::string& url )
: mUrl( url ),
  mType( VisualUrl::REGULAR_IMAGE ),
  mLocation( VisualUrl::LOCAL )
{
  if( ! url.empty() )
  {
    mLocation = ResolveLocation( url );
    mType = ResolveType( url );
  }
}

VisualUrl::VisualUrl( const VisualUrl& url )
: mUrl( url.mUrl ),
  mType( url.mType ),
  mLocation( url.mLocation )
{
}

VisualUrl& VisualUrl::operator=( const VisualUrl& url )
{
  if( &url != this )
  {
    mUrl = url.mUrl;
    mType = url.mType;
    mLocation = url.mLocation;
  }
  return *this;
}

const std::string& VisualUrl::GetUrl() const
{
  return mUrl;
}

VisualUrl::Type VisualUrl::GetType() const
{
  return mType;
}

VisualUrl::Location VisualUrl::GetLocation() const
{
  return mLocation;
}

bool VisualUrl::IsValid() const
{
  return mUrl.size() > 0u;
}

bool VisualUrl::IsLocal() const
{
  return mLocation == VisualUrl::LOCAL;
}



} // Internal
} // Toolkit
} // Dali
