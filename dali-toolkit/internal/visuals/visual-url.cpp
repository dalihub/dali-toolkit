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
VisualUrl::ProtocolType ResolveLocation(const std::string& url)
{
  const char*    urlCStr = url.c_str();
  const uint32_t length  = url.size();
  if((length > 7) && urlCStr[5] == ':' && urlCStr[6] == '/' && urlCStr[7] == '/')
  {
    // https:// or enbuf://
    const char hOre = tolower(urlCStr[0]);
    const char tOrn = tolower(urlCStr[1]);
    const char tOrb = tolower(urlCStr[2]);
    const char pOru = tolower(urlCStr[3]);
    const char sOrf = tolower(urlCStr[4]);
    if(('h' == hOre) &&
       ('t' == tOrn) &&
       ('t' == tOrb) &&
       ('p' == pOru) &&
       ('s' == sOrf))
    {
      return VisualUrl::REMOTE;
    }
    if(('e' == hOre) &&
       ('n' == tOrn) &&
       ('b' == tOrb) &&
       ('u' == pOru) &&
       ('f' == sOrf))
    {
      return VisualUrl::BUFFER;
    }
  }
  else if((length > 6) && urlCStr[4] == ':' && urlCStr[5] == '/' && urlCStr[6] == '/')
  {
    // http:// or dali://
    const char hOrd = tolower(urlCStr[0]);
    const char tOra = tolower(urlCStr[1]);
    const char tOrl = tolower(urlCStr[2]);
    const char pOri = tolower(urlCStr[3]);
    if(('h' == hOrd) &&
       ('t' == tOra) &&
       ('t' == tOrl) &&
       ('p' == pOri))
    {
      return VisualUrl::REMOTE;
    }
    if(('d' == hOrd) &&
       ('a' == tOra) &&
       ('l' == tOrl) &&
       ('i' == pOri))
    {
      return VisualUrl::TEXTURE;
    }
  }
  else if((length > 5) && urlCStr[3] == ':' && urlCStr[4] == '/' && urlCStr[5] == '/')
  {
    // ftp:// or ssh://
    const char fOrs = tolower(urlCStr[0]);
    const char tOrs = tolower(urlCStr[1]);
    const char pOrh = tolower(urlCStr[2]);
    if(('f' == fOrs) &&
       ('t' == tOrs) &&
       ('p' == pOrh))
    {
      return VisualUrl::REMOTE;
    }
    if(('s' == fOrs) &&
       ('s' == tOrs) &&
       ('h' == pOrh))
    {
      return VisualUrl::REMOTE;
    }
  }
  return VisualUrl::LOCAL;
}

VisualUrl::Type ResolveType(const std::string& url)
{
  // if only one char in string, can only be regular image
  const std::size_t count = url.size();
  if(count > 0)
  {
    // parsing from the end for better chance of early outs
    enum
    {
      SUFFIX,
      HASH,
      HASH_DOT
    } state                = SUFFIX;
    char         SVG[4]    = {'g', 'v', 's', '.'};
    char         GIF[4]    = {'f', 'i', 'g', '.'};
    char         WEBP[5]   = {'p', 'b', 'e', 'w', '.'};
    char         JSON[5]   = {'n', 'o', 's', 'j', '.'};
    unsigned int svgScore  = 0;
    unsigned int gifScore  = 0;
    unsigned int webpScore = 0;
    unsigned int jsonScore = 0;
    int          index     = count;
    while(--index >= 0)
    {
      const char        currentChar   = tolower(url[index]);
      const std::size_t offsetFromEnd = count - index - 1u;
      if((offsetFromEnd < sizeof(SVG)) && (currentChar == SVG[offsetFromEnd]))
      {
        // early out if SVG as can't be used in N patch for now
        if(++svgScore == sizeof(SVG))
        {
          return VisualUrl::SVG;
        }
      }
      if((offsetFromEnd < sizeof(GIF)) && (currentChar == GIF[offsetFromEnd]))
      {
        // early out if GIF as can't be used in N patch for now
        if(++gifScore == sizeof(GIF))
        {
          return VisualUrl::GIF;
        }
      }
      if((offsetFromEnd < sizeof(WEBP)) && (currentChar == WEBP[offsetFromEnd]))
      {
        // early out if WEBP as can't be used in N patch for now
        if(++webpScore == sizeof(WEBP))
        {
          return VisualUrl::WEBP;
        }
      }
      if((offsetFromEnd < sizeof(JSON)) && (currentChar == JSON[offsetFromEnd]))
      {
        // early out if JSON as can't be used in N patch for now
        if(++jsonScore == sizeof(JSON))
        {
          return VisualUrl::JSON;
        }
      }
      switch(state)
      {
        case SUFFIX:
        {
          if('.' == currentChar)
          {
            state = HASH;
          }
          break;
        }
        case HASH:
        {
          if(('#' == currentChar) || ('9' == currentChar))
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
          if('.' == currentChar)
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

} // namespace

VisualUrl::VisualUrl()
: mUrl(),
  mType(VisualUrl::REGULAR_IMAGE),
  mLocation(VisualUrl::LOCAL)
{
}

VisualUrl::VisualUrl(const std::string& url)
: mUrl(url),
  mType(VisualUrl::REGULAR_IMAGE),
  mLocation(VisualUrl::LOCAL)
{
  if(!url.empty())
  {
    mLocation = ResolveLocation(url);
    if(VisualUrl::TEXTURE != mLocation && VisualUrl::BUFFER != mLocation)
    {
      // TEXTURE and BUFFER location url doesn't need type resolving, REGULAR_IMAGE is fine
      mType = ResolveType(url);
    }
  }
}

VisualUrl::VisualUrl(const VisualUrl& url)
: mUrl(url.mUrl),
  mType(url.mType),
  mLocation(url.mLocation)
{
}

VisualUrl::~VisualUrl()
{
}

VisualUrl& VisualUrl::operator=(const VisualUrl& url)
{
  if(&url != this)
  {
    mUrl      = url.mUrl;
    mType     = url.mType;
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

VisualUrl::ProtocolType VisualUrl::GetProtocolType() const
{
  return mLocation;
}

bool VisualUrl::IsValid() const
{
  return mUrl.size() > 0u;
}

bool VisualUrl::IsLocalResource() const
{
  return mLocation == VisualUrl::LOCAL;
}

bool VisualUrl::IsBufferResource() const
{
  return mLocation == VisualUrl::BUFFER;
}

std::string VisualUrl::GetLocation() const
{
  return GetLocation(mUrl);
}

std::string VisualUrl::CreateTextureUrl(const std::string& location)
{
  return "dali://" + location;
}

std::string VisualUrl::CreateBufferUrl(const std::string& location)
{
  return "enbuf://" + location;
}

VisualUrl::ProtocolType VisualUrl::GetProtocolType(const std::string& url)
{
  return ResolveLocation(url);
}

std::string VisualUrl::GetLocation(const std::string& url)
{
  const auto location = url.find("://");
  if(std::string::npos != location)
  {
    return url.substr(location + 3u); // 3 characters forwards from the start of ://
  }
  return url;
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
