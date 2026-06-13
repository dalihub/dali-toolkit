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
namespace
{
constexpr unsigned int WHITE_SPACE_THRESHOLD = 0x21;   ///< All characters below 0x21 are considered white spaces.
constexpr unsigned int CHAR_SPACE            = 0x20;   ///< Space.
constexpr unsigned int CHAR_LF               = 0x000A; ///< NL Line feed, new line.
constexpr unsigned int CHAR_VT               = 0x000B; ///< Vertical tab.
constexpr unsigned int CHAR_FF               = 0x000C; ///< NP Form feed, new page.
constexpr unsigned int CHAR_CR               = 0x000D; ///< Carriage return, new line.
constexpr unsigned int CHAR_NEL              = 0x0085; ///< Next line.
constexpr unsigned int CHAR_LS               = 0x2028; ///< Line separator.
constexpr unsigned int CHAR_PS               = 0x2029; ///< Paragraph separator

constexpr unsigned int CHAR_ZWNJ = 0x200C; ///< Zero width non joiner.
constexpr unsigned int CHAR_ZWJ  = 0x200D; ///< Zero width joiner.
constexpr unsigned int CHAR_LTRM = 0x200E; ///< Left to Right Mark.
constexpr unsigned int CHAR_RTLM = 0x200F; ///< Right to Left Mark.
constexpr unsigned int CHAR_TS   = 0x2009; ///< Thin Space.
constexpr unsigned int CHAR_BOM  = 0xFEFF; ///< Byte Order Mark.
} // namespace

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
        BidirectionalSupport* impl = static_cast<Internal::BidirectionalSupport*>(handle.GetObjectPtr());
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
        FontClient* impl = static_cast<Dali::TextAbstraction::Internal::FontClient*>(handle.GetObjectPtr());
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

  void ClearCache()
  {
  }
  void ClearCacheOnLocaleChanged()
  {
  }
  void SetDpi(unsigned int horizontalDpi, unsigned int verticalDpi)
  {
  }
  void SetDpiFromWindowSystem()
  {
  }
  void GetDpi(unsigned int& horizontalDpi, unsigned int& verticalDpi)
  {
    horizontalDpi = verticalDpi = 96;
  }
  int GetDefaultFontSize()
  {
    return 10;
  }
  void ResetSystemDefaults()
  {
  }
  void GetDefaultFonts(FontList& defaultFonts)
  {
  }
  void InitDefaultFontDescription()
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
  bool IsCharacterSupportedByFont(FontId fontId, Character character)
  {
    return true;
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
  FontId GetFontId(const FontDescription& fontDescription, PointSize26Dot6 pointSize, FaceIndex faceIndex, Property::Map* variationsMapPtr)
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
  bool HasItalicStyle(FontId fontId) const
  {
    return false;
  }
  void GetFontMetrics(FontId fontId, FontMetrics& metrics)
  {
  }
  GlyphIndex GetGlyphIndex(FontId fontId, Character charcode)
  {
    return 0;
  }
  GlyphIndex GetGlyphIndex(FontId fontId, Character charcode, Character variantSelector)
  {
    return 0;
  }
  bool GetGlyphMetrics(GlyphInfo* array, uint32_t size, bool horizontal)
  {
    return true;
  }
  void CreateBitmap(FontId fontId, GlyphIndex glyphIndex, bool softwareItailc, bool softwareBold, Dali::TextAbstraction::GlyphBufferData& data, int outlineWidth)
  {
    // Dummy data for testing
    data.width           = 2;
    data.height          = 4;
    data.format          = Pixel::L8;
    data.compressionType = TextAbstraction::GlyphBufferData::CompressionType::NO_COMPRESSION;

    uint8_t* newBuffer = (uint8_t*)malloc(data.width * data.height * Pixel::GetBytesPerPixel(data.format));
    std::fill(newBuffer, newBuffer + (data.width * data.height), 255); // Fill with white pixels

    data.buffer        = newBuffer;
    data.isBufferOwned = true;
  }
  PixelData CreateBitmap(FontId fontId, GlyphIndex glyphIndex, int outlineWidth)
  {
    TextAbstraction::GlyphBufferData data;

    CreateBitmap(fontId, glyphIndex, false, false, data, outlineWidth);

    // If data is compressed or not owned buffer, copy this.
    if(!data.isBufferOwned || data.compressionType != TextAbstraction::GlyphBufferData::CompressionType::NO_COMPRESSION)
    {
      uint8_t* newBuffer = (uint8_t*)malloc(data.width * data.height * Pixel::GetBytesPerPixel(data.format));

      TextAbstraction::GlyphBufferData::Decompress(data, newBuffer);
      if(data.isBufferOwned)
      {
        free(data.buffer);
      }

      data.buffer          = newBuffer;
      data.isBufferOwned   = true;
      data.compressionType = TextAbstraction::GlyphBufferData::CompressionType::NO_COMPRESSION;
    }

    return PixelData::New(data.buffer,
                          data.width * data.height * Pixel::GetBytesPerPixel(data.format),
                          data.width,
                          data.height,
                          data.format,
                          PixelData::FREE);
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
  bool IsColorFont(FontId fontId)
  {
    return false;
  }
  bool IsRenderableColrV1Font(FontId fontId)
  {
    return false;
  }
  bool IsRenderableColrV1Glyph(FontId fontId, GlyphIndex glyphIndex)
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
  : mText(nullptr),
    mNumChars(0u),
    mFontId(0u)
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
        Shaping* impl = static_cast<Dali::TextAbstraction::Internal::Shaping*>(handle.GetObjectPtr());
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
    if(glyphStore && mappingTable)
    {
      for(unsigned int i = 0; i < mNumChars; ++i)
      {
        GlyphInfo& glyph = *(glyphStore + i);

        // Dummy info for testing
        glyph.fontId   = mFontId;
        glyph.index    = mText[i];
        glyph.advance  = 1.0f;
        glyph.xBearing = 0.0f;
        glyph.yBearing = 0.0f;

        mappingTable[i] = i;
      }
    }
    for(unsigned int i = 0; i < mNumChars; ++i)
    {
      mappingTable[i] = i;
    }
  }

  Length Shape(TextAbstraction::FontClient& fontClient, unsigned int const* text, unsigned int numChars, unsigned int fontId, Script script)
  {
    mFontId = fontId;

    if(mText)
    {
      delete[] mText;
    }
    mText     = new unsigned int[numChars];
    mNumChars = numChars;

    memcpy(mText, text, numChars * sizeof(unsigned int));

    return numChars;
  }

private:
  unsigned int* mText; ///< UTF-32
  unsigned int  mNumChars;
  FontId        mFontId;
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

inline static const TextAbstraction::Internal::FontClient& GetImplementation(const TextAbstraction::FontClient& fontClient)
{
  DALI_ASSERT_ALWAYS(fontClient && "fontClient handle is empty");
  const BaseObject& handle = fontClient.GetBaseObject();
  return static_cast<const TextAbstraction::Internal::FontClient&>(handle);
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

void FontClient::ClearCache()
{
  GetImplementation(*this).ClearCache();
}

void FontClient::ClearCacheOnLocaleChanged()
{
  GetImplementation(*this).ClearCacheOnLocaleChanged();
}

void FontClient::SetDpi(unsigned int horizontalDpi, unsigned int verticalDpi)
{
  GetImplementation(*this).SetDpi(horizontalDpi, verticalDpi);
}

void FontClient::SetDpiFromWindowSystem()
{
  GetImplementation(*this).SetDpiFromWindowSystem();
}

void FontClient::GetDpi(unsigned int& horizontalDpi, unsigned int& verticalDpi)
{
  GetImplementation(*this).GetDpi(horizontalDpi, verticalDpi);
}

int FontClient::GetDefaultFontSize()
{
  return GetImplementation(*this).GetDefaultFontSize();
}

void FontClient::ResetSystemDefaults()
{
  GetImplementation(*this).ResetSystemDefaults();
}

void FontClient::GetDefaultFonts(FontList& defaultFonts)
{
  GetImplementation(*this).GetDefaultFonts(defaultFonts);
}

void FontClient::InitDefaultFontDescription()
{
  GetImplementation(*this).InitDefaultFontDescription();
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

bool FontClient::IsCharacterSupportedByFont(FontId fontId, Character character)
{
  return GetImplementation(*this).IsCharacterSupportedByFont(fontId, character);
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
                             FaceIndex              faceIndex,
                             Property::Map*         variationsMapPtr)
{
  return GetImplementation(*this).GetFontId(fontDescription,
                                            pointSize,
                                            faceIndex,
                                            variationsMapPtr);
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

bool FontClient::HasItalicStyle(FontId fontId) const
{
  return GetImplementation(*this).HasItalicStyle(fontId);
}

void FontClient::GetFontMetrics(FontId fontId, FontMetrics& metrics)
{
  GetImplementation(*this).GetFontMetrics(fontId, metrics);
}

GlyphIndex FontClient::GetGlyphIndex(FontId fontId, Character charcode)
{
  return GetImplementation(*this).GetGlyphIndex(fontId, charcode);
}

GlyphIndex FontClient::GetGlyphIndex(FontId fontId, Character charcode, Character variantSelector)
{
  return GetImplementation(*this).GetGlyphIndex(fontId, charcode, variantSelector);
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

bool FontClient::IsColorFont(FontId fontId)
{
  return GetImplementation(*this).IsColorFont(fontId);
}

bool FontClient::IsRenderableColrV1Font(FontId fontId)
{
  return GetImplementation(*this).IsRenderableColrV1Font(fontId);
}

bool FontClient::IsRenderableColrV1Glyph(FontId fontId, GlyphIndex glyphIndex)
{
  return GetImplementation(*this).IsRenderableColrV1Glyph(fontId, glyphIndex);
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

// Script.h

bool IsRightToLeftScript(Script script)
{
  return ((ARABIC == script) ||
          (HEBREW == script));
}

Script GetCharacterScript(Character character)
{
  return LATIN;
}

bool IsWhiteSpace(Character character)
{
  return character < WHITE_SPACE_THRESHOLD;
}

bool IsSpace(Character character)
{
  return CHAR_SPACE == character;
}

bool IsNewParagraph(Character character)
{
  return ((CHAR_LF == character) ||
          (CHAR_VT == character) ||
          (CHAR_FF == character) ||
          (CHAR_CR == character) ||
          (CHAR_NEL == character) ||
          (CHAR_LS == character) ||
          (CHAR_PS == character));
}

bool IsZeroWidthNonJoiner(Character character)
{
  return CHAR_ZWNJ == character;
}

bool IsZeroWidthJoiner(Character character)
{
  return CHAR_ZWJ == character;
}

bool IsZeroWidthSpace(Character character)
{
  return CHAR_ZWS == character;
}

bool IsLeftToRightMark(Character character)
{
  return CHAR_LTRM == character;
}

bool IsRightToLeftMark(Character character)
{
  return CHAR_RTLM == character;
}

bool IsThinSpace(Character character)
{
  return CHAR_TS == character;
}

bool IsByteOrderMark(Character character)
{
  return CHAR_BOM == character;
}

bool IsCommonScript(Character character)
{
  return (IsWhiteSpace(character) ||
          IsZeroWidthNonJoiner(character) ||
          IsZeroWidthJoiner(character) ||
          IsZeroWidthSpace(character) ||
          IsLeftToRightMark(character) ||
          IsRightToLeftMark(character) ||
          IsThinSpace(character) ||
          IsNewParagraph(character) ||
          IsByteOrderMark(character) ||
          IsCombiningDiacriticalMarks(character));
}

bool HasLigatureMustBreak(Script script)
{
  return ((LATIN == script) ||
          (ARABIC == script));
}

Length GetNumberOfScripts()
{
  return SYMBOLS_NSLCL + 1;
}

bool IsCombiningDiacriticalMarks(Character character)
{
  return character >= 0x0300 && character <= 0x036f;
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
void Segmentation::GetLineBreakPositions(unsigned int const* text, unsigned int length, char* breakInfo)
{
  // For test codes, Mark end of text is break position
  if(breakInfo && text && length > 0u)
  {
    for(unsigned int i = 0; i < length - 1; ++i)
    {
      if(IsWhiteSpace(text[i]))
      {
        breakInfo[i] = TextAbstraction::LINE_ALLOW_BREAK;
      }
    }
    breakInfo[length - 1] = TextAbstraction::LINE_MUST_BREAK;
  }
}
void Segmentation::GetWordBreakPositions(unsigned int const* text, unsigned int length, char* breakInfo)
{
}
void Segmentation::GetLineBreakPositionsUtf8(unsigned char const* text, unsigned int length, char* breakInfo)
{
}
void Segmentation::GetWordBreakPositionsUtf8(unsigned char const* text, unsigned int length, char* breakInfo)
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
