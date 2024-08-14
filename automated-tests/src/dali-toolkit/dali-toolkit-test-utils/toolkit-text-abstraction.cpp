/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/common/singleton-service.h>
#include <dali/devel-api/text-abstraction/bidirectional-support.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/font-metrics.h>
#include <dali/devel-api/text-abstraction/glyph-info.h>
#include <dali/devel-api/text-abstraction/script.h>
#include <dali/devel-api/text-abstraction/segmentation.h>
#include <dali/devel-api/text-abstraction/shaping.h>
#include <dali/public-api/object/base-object.h>
#include <cstring>

using namespace Dali;

namespace Dali
{
class Adaptor;

namespace TextAbstraction
{
namespace Internal
{
class BidirectionalSupport : public BaseObject
{
public:
  BidirectionalSupport()
  {
  }

  ~BidirectionalSupport()
  {
  }

  static TextAbstraction::BidirectionalSupport Get()
  {
    TextAbstraction::BidirectionalSupport bidirectionalSupportHandle;

    Dali::SingletonService service(Dali::SingletonService::Get());
    if(service)
    {
      // Check whether the singleton is already created
      BaseHandle handle = service.GetSingleton(typeid(TextAbstraction::BidirectionalSupport));
      if(handle)
      {
        // If so, downcast the handle
        BidirectionalSupport* impl = dynamic_cast<Internal::BidirectionalSupport*>(handle.GetObjectPtr());
        bidirectionalSupportHandle = TextAbstraction::BidirectionalSupport(impl);
      }
      else // create and register the object
      {
        bidirectionalSupportHandle = TextAbstraction::BidirectionalSupport(new BidirectionalSupport);
        service.Register(typeid(bidirectionalSupportHandle), bidirectionalSupportHandle);
      }
    }
    return bidirectionalSupportHandle;
  }
  BidiInfoIndex CreateInfo(const Character* const paragraph, Length numberOfCharacters, bool matchSystemLanguageDirection, LayoutDirection::Type layoutDirection)
  {
    return 0;
  }

  void DestroyInfo(BidiInfoIndex bidiInfoIndex)
  {
  }

  void Reorder(BidiInfoIndex bidiInfoIndex, CharacterIndex firstCharacterIndex, Length numberOfCharacters, CharacterIndex* visualToLogicalMap)
  {
  }

  bool GetMirroredText(Character* text, CharacterDirection* directions, Length numberOfCharacters)
  {
    return true;
  }

  bool GetParagraphDirection(BidiInfoIndex bidiInfoIndex) const
  {
    return true;
  }

  void GetCharactersDirection(BidiInfoIndex bidiInfoIndex, CharacterDirection* directions, Length numberOfCharacters)
  {
  }
}; // class BidirectionalSupport

class FontClient : public BaseObject
{
public:
  FontClient()
  : mGlyphInfo()
  {
  }

  ~FontClient()
  {
  }

  static Dali::TextAbstraction::FontClient Get()
  {
    Dali::TextAbstraction::FontClient fontClientHandle;

    Dali::SingletonService service(SingletonService::Get());
    if(service)
    {
      // Check whether the singleton is already created
      Dali::BaseHandle handle = service.GetSingleton(typeid(Dali::TextAbstraction::FontClient));
      if(handle)
      {
        // If so, downcast the handle
        FontClient* impl = dynamic_cast<Dali::TextAbstraction::Internal::FontClient*>(handle.GetObjectPtr());
        fontClientHandle = Dali::TextAbstraction::FontClient(impl);
      }
      else // create and register the object
      {
        fontClientHandle = Dali::TextAbstraction::FontClient(new FontClient);
        service.Register(typeid(fontClientHandle), fontClientHandle);
      }
    }

    return fontClientHandle;
  }

  void SetDpi(unsigned int horizontalDpi, unsigned int verticalDpi)
  {
  }
  void GetDpi(unsigned int& horizontalDpi, unsigned int& verticalDpi)
  {
    horizontalDpi = verticalDpi = 96;
  }

  void ResetSystemDefaults()
  {
  }
  void GetDefaultFonts(FontList& defaultFonts)
  {
  }
  void GetDefaultPlatformFontDescription(FontDescription& fontDescription)
  {
  }
  void GetSystemFonts(FontList& systemFonts)
  {
  }
  void GetDescription(FontId id, FontDescription& fontDescription)
  {
  }
  PointSize26Dot6 GetPointSize(FontId id)
  {
    return 9;
  }
  FontId FindDefaultFont(Character charcode, PointSize26Dot6 pointSize, bool preferColor)
  {
    return 0;
  }
  FontId FindFallbackFont(Character charcode, const FontDescription& fontDescription, PointSize26Dot6 pointSize, bool preferColor)
  {
    return 0;
  }
  FontId GetFontId(const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex)
  {
    return 0;
  }
  FontId GetFontId(const FontDescription& fontDescription, PointSize26Dot6 pointSize, FaceIndex faceIndex)
  {
    return 0;
  }
  bool IsScalable(const FontPath& path)
  {
    return true;
  }
  bool IsScalable(const FontDescription& fontDescription)
  {
    return true;
  }
  void GetFixedSizes(const FontPath& path, Dali::Vector<PointSize26Dot6>& sizes)
  {
  }
  void GetFixedSizes(const FontDescription& fontDescription, Dali::Vector<PointSize26Dot6>& sizes)
  {
  }
  void GetFontMetrics(FontId fontId, FontMetrics& metrics)
  {
  }
  GlyphIndex GetGlyphIndex(FontId fontId, Character charcode)
  {
    return 0;
  }
  bool GetGlyphMetrics(GlyphInfo* array, uint32_t size, bool horizontal)
  {
    return true;
  }
  void CreateBitmap(FontId fontId, GlyphIndex glyphIndex, bool softwareItailc, bool softwareBold, Dali::TextAbstraction::GlyphBufferData& data, int outlineWidth)
  {
  }
  PixelData CreateBitmap(FontId fontId, GlyphIndex glyphIndex, int outlineWidth)
  {
    return PixelData();
  }
  void CreateVectorBlob(FontId fontId, GlyphIndex glyphIndex, VectorBlob*& blob, unsigned int& blobLength, unsigned int& nominalWidth, unsigned int& nominalHeight)
  {
    blobLength = 0;
  }
  const GlyphInfo& GetEllipsisGlyph(PointSize26Dot6 pointSize)
  {
    return mGlyphInfo;
  }
  bool IsColorGlyph(FontId fontId, GlyphIndex glyphIndex)
  {
    return false;
  }

private:
  GlyphInfo mGlyphInfo;
}; // class FontClient

class Shaping : public BaseObject
{
public:
  Shaping()
  : mText(NULL),
    mNumChars(0)
  {
  }

  ~Shaping()
  {
    delete[] mText;
  }

  static Dali::TextAbstraction::Shaping Get()
  {
    Dali::TextAbstraction::Shaping shapingHandle;

    Dali::SingletonService service(SingletonService::Get());
    if(service)
    {
      // Check whether the singleton is already created
      Dali::BaseHandle handle = service.GetSingleton(typeid(Dali::TextAbstraction::Shaping));
      if(handle)
      {
        // If so, downcast the handle
        Shaping* impl = dynamic_cast<Dali::TextAbstraction::Internal::Shaping*>(handle.GetObjectPtr());
        shapingHandle = Dali::TextAbstraction::Shaping(impl);
      }
      else // create and register the object
      {
        shapingHandle = Dali::TextAbstraction::Shaping(new Shaping);
        service.Register(typeid(shapingHandle), shapingHandle);
      }
    }
    return shapingHandle;
  }

  void GetGlyphs(GlyphInfo* glyphStore, unsigned int* mappingTable)
  {
    // Return store & mapping table (0, 1, 2, 3... N-1))
    if(glyphStore)
    {
      memcpy(glyphStore, mText, mNumChars);
    }
    for(unsigned int i = 0; i < mNumChars; ++i)
    {
      mappingTable[i] = i;
    }
  }

  Length Shape(TextAbstraction::FontClient& fontClient, unsigned int const* text, unsigned int numChars, unsigned int fontId, Script script)
  {
    mText     = new unsigned char[numChars];
    mNumChars = numChars;

    memcpy(mText, text, numChars);

    return numChars;
  }

private:
  unsigned char* mText;
  unsigned int   mNumChars;
};

} // namespace Internal
} // namespace TextAbstraction

inline static TextAbstraction::Internal::BidirectionalSupport& GetImplementation(TextAbstraction::BidirectionalSupport& bidirectionalSupport)
{
  DALI_ASSERT_ALWAYS(bidirectionalSupport && "bidirectional support handle is empty");
  BaseObject& object = bidirectionalSupport.GetBaseObject();
  return static_cast<TextAbstraction::Internal::BidirectionalSupport&>(object);
}

inline static const TextAbstraction::Internal::BidirectionalSupport& GetImplementation(const TextAbstraction::BidirectionalSupport& bidirectionalSupport)
{
  DALI_ASSERT_ALWAYS(bidirectionalSupport && "bidirectional support handle is empty");
  const BaseObject& object = bidirectionalSupport.GetBaseObject();
  return static_cast<const TextAbstraction::Internal::BidirectionalSupport&>(object);
}

inline static TextAbstraction::Internal::FontClient& GetImplementation(TextAbstraction::FontClient& fontClient)
{
  DALI_ASSERT_ALWAYS(fontClient && "fontClient handle is empty");
  BaseObject& handle = fontClient.GetBaseObject();
  return static_cast<TextAbstraction::Internal::FontClient&>(handle);
}

inline static TextAbstraction::Internal::Shaping& GetImplementation(TextAbstraction::Shaping& shaping)
{
  DALI_ASSERT_ALWAYS(shaping && "shaping handle is empty");
  BaseObject& handle = shaping.GetBaseObject();
  return static_cast<TextAbstraction::Internal::Shaping&>(handle);
}

////////////////////////////////////////////////////////////////////////////////
/******************************************************************************/

namespace TextAbstraction
{
const PointSize26Dot6 FontClient::DEFAULT_POINT_SIZE = 768u; // 12*64

BidirectionalSupport::BidirectionalSupport()
{
}

BidirectionalSupport::~BidirectionalSupport()
{
}

BidirectionalSupport::BidirectionalSupport(Internal::BidirectionalSupport* implementation)
: BaseHandle(implementation)
{
}

BidirectionalSupport BidirectionalSupport::Get()
{
  return Internal::BidirectionalSupport::Get();
}

BidiInfoIndex BidirectionalSupport::CreateInfo(const Character* const paragraph,
                                               Length                 numberOfCharacters,
                                               bool                   matchSystemLanguageDirection,
                                               LayoutDirection::Type  layoutDirection)
{
  return GetImplementation(*this).CreateInfo(paragraph, numberOfCharacters, matchSystemLanguageDirection, layoutDirection);
}

void BidirectionalSupport::DestroyInfo(BidiInfoIndex bidiInfoIndex)
{
  GetImplementation(*this).DestroyInfo(bidiInfoIndex);
}

void BidirectionalSupport::Reorder(BidiInfoIndex   bidiInfoIndex,
                                   CharacterIndex  firstCharacterIndex,
                                   Length          numberOfCharacters,
                                   CharacterIndex* visualToLogicalMap)
{
  GetImplementation(*this).Reorder(bidiInfoIndex, firstCharacterIndex, numberOfCharacters, visualToLogicalMap);
}

bool BidirectionalSupport::GetMirroredText(Character*          text,
                                           CharacterDirection* directions,
                                           Length              numberOfCharacters)
{
  return GetImplementation(*this).GetMirroredText(text, directions, numberOfCharacters);
}

bool BidirectionalSupport::GetParagraphDirection(BidiInfoIndex bidiInfoIndex) const
{
  return GetImplementation(*this).GetParagraphDirection(bidiInfoIndex);
}

void BidirectionalSupport::GetCharactersDirection(BidiInfoIndex       bidiInfoIndex,
                                                  CharacterDirection* directions,
                                                  Length              numberOfCharacters)
{
  GetImplementation(*this).GetCharactersDirection(bidiInfoIndex, directions, numberOfCharacters);
}

FontClient FontClient::Get()
{
  return Internal::FontClient::Get();
}

FontClient::FontClient()
{
}

FontClient::~FontClient()
{
}

FontClient::FontClient(const FontClient& handle)
: BaseHandle(handle)
{
}

GlyphBufferData::GlyphBufferData()
{
}

GlyphBufferData::~GlyphBufferData()
{
}

FontClient& FontClient::operator=(const FontClient& handle)
{
  BaseHandle::operator=(handle);
  return *this;
}

void FontClient::SetDpi(unsigned int horizontalDpi, unsigned int verticalDpi)
{
  GetImplementation(*this).SetDpi(horizontalDpi, verticalDpi);
}

void FontClient::GetDpi(unsigned int& horizontalDpi, unsigned int& verticalDpi)
{
  GetImplementation(*this).GetDpi(horizontalDpi, verticalDpi);
}

void FontClient::ResetSystemDefaults()
{
  GetImplementation(*this).ResetSystemDefaults();
}

void FontClient::GetDefaultFonts(FontList& defaultFonts)
{
  GetImplementation(*this).GetDefaultFonts(defaultFonts);
}

void FontClient::GetDefaultPlatformFontDescription(FontDescription& fontDescription)
{
  GetImplementation(*this).GetDefaultPlatformFontDescription(fontDescription);
}

void FontClient::GetSystemFonts(FontList& systemFonts)
{
  GetImplementation(*this).GetSystemFonts(systemFonts);
}

void FontClient::GetDescription(FontId id, FontDescription& fontDescription)
{
  GetImplementation(*this).GetDescription(id, fontDescription);
}

PointSize26Dot6 FontClient::GetPointSize(FontId id)
{
  return GetImplementation(*this).GetPointSize(id);
}

FontId FontClient::FindDefaultFont(Character charcode, PointSize26Dot6 pointSize, bool preferColor)
{
  return GetImplementation(*this).FindDefaultFont(charcode, pointSize, preferColor);
}

FontId FontClient::FindFallbackFont(Character charcode, const FontDescription& fontDescription, PointSize26Dot6 pointSize, bool preferColor)
{
  return GetImplementation(*this).FindFallbackFont(charcode, fontDescription, pointSize, preferColor);
}

FontId FontClient::GetFontId(const FontPath& path, PointSize26Dot6 pointSize, FaceIndex faceIndex)
{
  return GetImplementation(*this).GetFontId(path, pointSize, faceIndex);
}

FontId FontClient::GetFontId(const FontDescription& fontDescription,
                             PointSize26Dot6        pointSize,
                             FaceIndex              faceIndex)
{
  return GetImplementation(*this).GetFontId(fontDescription,
                                            pointSize,
                                            faceIndex);
}

bool FontClient::IsScalable(const FontPath& path)
{
  return GetImplementation(*this).IsScalable(path);
}

bool FontClient::IsScalable(const FontDescription& fontDescription)
{
  return GetImplementation(*this).IsScalable(fontDescription);
}

void FontClient::GetFixedSizes(const FontPath& path, Dali::Vector<PointSize26Dot6>& sizes)
{
  GetImplementation(*this).GetFixedSizes(path, sizes);
}

void FontClient::GetFixedSizes(const FontDescription&         fontDescription,
                               Dali::Vector<PointSize26Dot6>& sizes)
{
  GetImplementation(*this).GetFixedSizes(fontDescription, sizes);
}

void FontClient::GetFontMetrics(FontId fontId, FontMetrics& metrics)
{
  GetImplementation(*this).GetFontMetrics(fontId, metrics);
}

GlyphIndex FontClient::GetGlyphIndex(FontId fontId, Character charcode)
{
  return GetImplementation(*this).GetGlyphIndex(fontId, charcode);
}

bool FontClient::GetGlyphMetrics(GlyphInfo* array, uint32_t size, GlyphType type, bool horizontal)
{
  return GetImplementation(*this).GetGlyphMetrics(array, size, horizontal);
}

void FontClient::CreateBitmap(FontId fontId, GlyphIndex glyphIndex, bool softwareItailc, bool softwareBold, Dali::TextAbstraction::GlyphBufferData& data, int outlineWidth)
{
  GetImplementation(*this).CreateBitmap(fontId, glyphIndex, softwareItailc, softwareBold, data, outlineWidth);
}

PixelData FontClient::CreateBitmap(FontId fontId, GlyphIndex glyphIndex, int outlineWidth)
{
  return GetImplementation(*this).CreateBitmap(fontId, glyphIndex, outlineWidth);
}

void FontClient::CreateVectorBlob(FontId        fontId,
                                  GlyphIndex    glyphIndex,
                                  VectorBlob*&  blob,
                                  unsigned int& blobLength,
                                  unsigned int& nominalWidth,
                                  unsigned int& nominalHeight)
{
  GetImplementation(*this).CreateVectorBlob(fontId, glyphIndex, blob, blobLength, nominalWidth, nominalHeight);
}

const GlyphInfo& FontClient::GetEllipsisGlyph(PointSize26Dot6 pointSize)
{
  return GetImplementation(*this).GetEllipsisGlyph(pointSize);
}

bool FontClient::IsColorGlyph(FontId fontId, GlyphIndex glyphIndex)
{
  return GetImplementation(*this).IsColorGlyph(fontId, glyphIndex);
}

FontClient::FontClient(Internal::FontClient* internal)
: BaseHandle(internal)
{
}

FontMetrics::FontMetrics()
: ascender(0.f),
  descender(0.f),
  height(0.f),
  underlinePosition(0.f),
  underlineThickness(0.f)
{
}

GlyphInfo::GlyphInfo()
{
}

Script GetCharacterScript(unsigned int x)
{
  return LATIN;
}
bool HasLigatureMustBreak(Script x)
{
  return false;
}
bool IsCommonScript(unsigned int character)
{
  return false;
}
bool IsNewParagraph(unsigned int character)
{
  return false;
}
bool IsRightToLeftScript(Script)
{
  return false;
}
bool IsWhiteSpace(unsigned int character)
{
  return character < 0x21;
}

Segmentation Segmentation::Get()
{
  return Segmentation();
}
Segmentation::Segmentation()
{
}
Segmentation::~Segmentation()
{
}
void Segmentation::GetLineBreakPositions(unsigned int const*, unsigned int, char*)
{
}
void Segmentation::GetWordBreakPositions(unsigned int const*, unsigned int, char*)
{
}
void Segmentation::GetLineBreakPositionsUtf8(unsigned char const*, unsigned int, char*)
{
}
void Segmentation::GetWordBreakPositionsUtf8(unsigned char const*, unsigned int, char*)
{
}

Shaping Shaping::Get()
{
  return TextAbstraction::Internal::Shaping::Get();
}

Shaping::Shaping()
{
}

Shaping::Shaping(Internal::Shaping* internal)
: BaseHandle(internal)
{
}

Shaping::~Shaping()
{
}

void Shaping::GetGlyphs(GlyphInfo* glyphStore, unsigned int* mappingTable)
{
  // Return store & mapping table (0, 1, 2, 3... N-1))
  GetImplementation(*this).GetGlyphs(glyphStore, mappingTable);
}

Length Shaping::Shape(TextAbstraction::FontClient& fontClient, unsigned int const* text, unsigned int numChars, unsigned int fontId, Script script)
{
  return GetImplementation(*this).Shape(fontClient, text, numChars, fontId, script);
}

} // namespace TextAbstraction
} // namespace Dali
