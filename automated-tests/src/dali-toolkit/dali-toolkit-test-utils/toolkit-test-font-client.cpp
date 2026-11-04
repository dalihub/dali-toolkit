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

// Test implementation of the font boundary. No font files, fontconfig, window
// system DPI, or host font selection are used. Keep script, bidi, segmentation,
// layout, and rendering in the production libraries under test.
#include <dali/devel-api/common/singleton-service.h>
#include <dali/devel-api/text-abstraction/bitmap-font.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/font-metrics.h>
#include <dali/devel-api/text-abstraction/glyph-info.h>
#include <dali/devel-api/text-abstraction/script.h>
#include <dali/devel-api/text-abstraction/shaping.h>
#include <dali/devel-api/text-abstraction/hyphenation.h>
#include <dali/public-api/adaptor-framework/image-loading.h>
#include <dali/public-api/object/base-object.h>
#include <dali/integration-api/string-utils.h>
#include <toolkit-environment-variable.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>

namespace DALI_NAMESPACE::TextAbstraction
{
namespace
{
struct TestFont
{
  FontDescription description;
  PointSize26Dot6  size{12u * 64u};
  BitmapFont      bitmap;
  bool            color{false};
};

class TestFontClient : public BaseObject
{
public:
  TestFontClient()
  {
    fonts.emplace_back(); // FontId zero is invalid (or an embedded item).
  }

  FontId Add(FontDescription description, PointSize26Dot6 size)
  {
    if(description.family.empty())
    {
      description.family = "Test Sans";
    }
    if(description.family == "NoSuchFont" || description.path.find("/invalid/") == 0u)
    {
      return 0u;
    }
    if(description.family == "TizenSans" || description.family == "Test No Style")
    {
      description.weight = FontWeight::NORMAL;
      description.slant  = FontSlant::NORMAL;
    }
    size = size ? size : 12u * 64u;
    for(FontId id = 1u; id < fonts.size(); ++id)
    {
      const auto& font = fonts[id];
      if(!font.bitmap.name.empty() && font.description.family == description.family)
      {
        return id; // Bitmap fonts have fixed metrics regardless of point size.
      }
      if(font.size == size && font.description.family == description.family &&
         font.description.weight == description.weight && font.description.width == description.width &&
         font.description.slant == description.slant)
      {
        return id;
      }
    }
    TestFont font;
    font.description = description;
    font.size        = size;
    font.color       = description.family.find("Emoji") != std::string::npos || description.family.find("Test Color") == 0u || description.family == "Test BGRA";
    fonts.push_back(font);
    return fonts.size() - 1u;
  }

  const TestFont& Get(FontId id) const
  {
    return fonts[id < fonts.size() ? id : 0u];
  }

  float Pixels(FontId id) const
  {
    const float requested = static_cast<float>(Get(id).size) / 64.f * static_cast<float>(verticalDpi) / 72.f;
    return atlasEnabled ? std::min(requested, std::min(atlasBlock.height - 5.f, 2.f * (atlasBlock.width - 5.f))) : requested;
  }

  unsigned int             horizontalDpi{96u};
  unsigned int             verticalDpi{96u};
  bool                     atlasEnabled{FontClient::DEFAULT_ATLAS_LIMITATION_ENABLED};
  Size                     atlasBlock{FontClient::MAX_SIZE_FIT_IN_ATLAS};
  std::vector<TestFont>     fonts;
  FontPathList             directories;
  CustomFontAddedSignalType customFontAdded;
  std::vector<FontClient::EmbeddedItemDescription> embedded;
  GlyphInfo                ellipsis;
};

TestFontClient& State(const FontClient& client)
{
  return static_cast<TestFontClient&>(const_cast<BaseObject&>(client.GetBaseObject()));
}

bool IsInvisible(Character character)
{
  return character == 0x200cu || character == 0x200du || character == 0xfe0eu || character == 0xfe0fu ||
         character == 0x200eu || character == 0x200fu || character == 0x200bu || character == 0xfeffu ||
         (character >= 0x300u && character <= 0x36fu);
}

class TestShaping : public BaseObject
{
public:
  std::vector<GlyphInfo> glyphs;
  std::vector<CharacterIndex> characters;
};

TestShaping& ShapeState(Shaping& shaping)
{
  return static_cast<TestShaping&>(shaping.GetBaseObject());
}
} // namespace

FontClient::FontClient() = default;
FontClient::~FontClient() = default;
FontClient::FontClient(const FontClient&) = default;
FontClient::FontClient(FontClient&&) noexcept = default;
FontClient& FontClient::operator=(const FontClient&) = default;
FontClient& FontClient::operator=(FontClient&&) noexcept = default;
FontClient::FontClient(Internal::FontClient* implementation)
: BaseHandle(reinterpret_cast<TestFontClient*>(implementation))
{
}

FontClient FontClient::New()
{
  return FontClient(reinterpret_cast<Internal::FontClient*>(new TestFontClient));
}
FontClient FontClient::New(uint32_t horizontal, uint32_t vertical)
{
  auto client = New();
  client.SetDpi(horizontal, vertical);
  return client;
}
FontClient FontClient::Get()
{
  auto service = SingletonService::Get();
  if(!service)
  {
    return {};
  }
  // A separate key prevents production preinitialization from registering an
  // adaptor object with a different implementation layout under our handle.
  const auto key = DALI_TYPE_ID(TestFontClient);
  auto handle = service.GetSingleton(key);
  if(handle)
  {
    return FontClient(reinterpret_cast<Internal::FontClient*>(handle.GetObjectPtr()));
  }
  auto client = New();
  service.Register(key, client);
  return client;
}

void FontClient::ClearCache() {}
void FontClient::ClearCacheOnLocaleChanged() {}
void FontClient::SetDpi(unsigned int horizontal, unsigned int vertical)
{
  State(*this).horizontalDpi = horizontal ? horizontal : 96u;
  State(*this).verticalDpi   = vertical ? vertical : 96u;
}
void FontClient::SetDpiFromWindowSystem() { SetDpi(96u, 96u); }
void FontClient::GetDpi(unsigned int& horizontal, unsigned int& vertical)
{
  horizontal = State(*this).horizontalDpi;
  vertical   = State(*this).verticalDpi;
}
int FontClient::GetDefaultFontSize() { return -1; }
uint32_t FontClient::GetNumberOfPointsPerOneUnitOfPointSize() const { return 64u; }
void FontClient::ResetSystemDefaults() {}
void FontClient::InitDefaultFontDescription() {}
void FontClient::GetDefaultPlatformFontDescription(FontDescription& description)
{
  description = FontDescription();
  description.family = "Test Sans";
}
void FontClient::GetDefaultFonts(FontList& fonts)
{
  FontDescription description;
  GetDefaultPlatformFontDescription(description);
  fonts.push_back(description);
}
void FontClient::GetSystemFonts(FontList& fonts) { GetDefaultFonts(fonts); }
void FontClient::GetDescription(FontId id, FontDescription& description) { description = State(*this).Get(id).description; }
PointSize26Dot6 FontClient::GetPointSize(FontId id) { return State(*this).Get(id).size; }
bool FontClient::IsCharacterSupportedByFont(FontId id, Character character)
{
  if(!id || character == 0x10ffffu)
  {
    return false;
  }
  const auto& font = State(*this).Get(id);
  if(!font.bitmap.name.empty())
  {
    return std::any_of(font.bitmap.glyphs.begin(), font.bitmap.glyphs.end(), [character](const auto& glyph) { return glyph.utf32 == character; });
  }
  if(font.description.family == "Test Latin")
  {
    return character < 0x100u;
  }
  return !font.color || character >= 0x2300u || IsInvisible(character);
}
FontId FontClient::FindDefaultFont(Character character, PointSize26Dot6 size, bool preferColor)
{
  if(character == 0x10ffffu) return 0u;
  FontDescription description;
  description.family = preferColor ? "Test Color" : "Test Sans";
  return State(*this).Add(description, size);
}
FontId FontClient::FindFallbackFont(Character character, const FontDescription&, PointSize26Dot6 size, bool preferColor)
{
  return FindDefaultFont(character, size, preferColor);
}
FontId FontClient::GetFontId(const FontPath& path, PointSize26Dot6 size, FaceIndex)
{
  FontDescription description;
  description.family = path.substr(path.find_last_of('/') + 1u);
  description.path   = path;
  return State(*this).Add(description, size);
}
FontId FontClient::GetFontId(const FontDescription& description, PointSize26Dot6 size, FaceIndex, Property::Map*)
{
  return State(*this).Add(description, size);
}
FontId FontClient::GetFontId(const BitmapFont& bitmap)
{
  FontDescription description;
  description.family = bitmap.name;
  const auto id = State(*this).Add(description, DEFAULT_POINT_SIZE);
  State(*this).fonts[id].bitmap = bitmap;
  for(const auto& glyph : bitmap.glyphs)
  {
    State(*this).fonts[id].bitmap.ascender = std::max(State(*this).fonts[id].bitmap.ascender, glyph.ascender);
    State(*this).fonts[id].bitmap.descender = std::min(State(*this).fonts[id].bitmap.descender, glyph.descender);
  }
  State(*this).fonts[id].color  = bitmap.isColorFont;
  return id;
}
bool FontClient::IsScalable(const FontPath&) { return true; }
bool FontClient::IsScalable(const FontDescription&) { return true; }
void FontClient::GetFixedSizes(const FontPath&, Dali::Vector<PointSize26Dot6>&) {}
void FontClient::GetFixedSizes(const FontDescription&, Dali::Vector<PointSize26Dot6>&) {}
bool FontClient::HasItalicStyle(FontId) const { return false; }
void FontClient::GetFontMetrics(FontId id, FontMetrics& metrics)
{
  const auto& font = State(*this).Get(id);
  const float pixels = State(*this).Pixels(id);
  metrics.ascender           = pixels * 0.8f;
  metrics.descender          = -pixels * 0.2f;
  metrics.height             = pixels;
  metrics.underlinePosition  = -pixels * 0.1f;
  metrics.underlineThickness = std::max(1.f, pixels * 0.05f);
  if(!font.bitmap.name.empty())
  {
    metrics.ascender = font.bitmap.ascender;
    metrics.descender = font.bitmap.descender;
    metrics.height = metrics.ascender - metrics.descender;
    metrics.underlinePosition = font.bitmap.underlinePosition;
    metrics.underlineThickness = font.bitmap.underlineThickness;
  }
}
GlyphIndex FontClient::GetGlyphIndex(FontId id, Character character) { return IsCharacterSupportedByFont(id, character) ? character : 0u; }
GlyphIndex FontClient::GetGlyphIndex(FontId id, Character character, Character selector)
{
  const auto index = GetGlyphIndex(id, character);
  // A separate synthetic glyph namespace makes variation-sensitive font
  // selection observable without an OpenType cmap on the host.
  return index && (selector == 0xfe0eu || selector == 0xfe0fu) ? index + 0x110000u : index;
}
bool FontClient::GetGlyphMetrics(GlyphInfo* glyphs, uint32_t count, GlyphType, bool)
{
  for(uint32_t i = 0u; i < count; ++i)
  {
    auto& glyph = glyphs[i];
    const auto& font = State(*this).Get(glyph.fontId);
    const float pixels = State(*this).Pixels(glyph.fontId);
    glyph.width    = IsInvisible(glyph.index) ? 0.f : std::max(1.f, pixels * (font.color ? 1.f : 0.5f));
    glyph.height   = pixels;
    glyph.xBearing = 0.f;
    glyph.yBearing = pixels * 0.8f;
    glyph.advance  = glyph.width;
    glyph.scaleFactor = 1.f;
    if(!font.bitmap.name.empty())
    {
      for(const auto& bitmap : font.bitmap.glyphs)
      {
        if(bitmap.utf32 == glyph.index)
        {
          const auto buffer = LoadImageFromFile(Integration::ToDaliStringView(bitmap.url));
          glyph.width = buffer.GetWidth();
          glyph.height = buffer.GetHeight();
          glyph.yBearing = glyph.height + bitmap.descender;
          glyph.advance = glyph.width;
          break;
        }
      }
    }
    if(glyph.fontId == 0u && glyph.index > 0u && glyph.index <= State(*this).embedded.size())
    {
      const auto& item = State(*this).embedded[glyph.index - 1u];
      glyph.width = item.width;
      glyph.height = item.height;
      glyph.advance = glyph.width;
      glyph.yBearing = glyph.height;
    }
  }
  return true;
}
void FontClient::CreateBitmap(FontId id, GlyphIndex index, bool, bool, GlyphBufferData& data, int outline)
{
  GlyphInfo glyph;
  glyph.fontId = id;
  glyph.index  = index;
  GetGlyphMetrics(&glyph, 1u, BITMAP_GLYPH, true);
  data.width  = std::min(1024u, static_cast<uint32_t>(std::ceil(glyph.width)) + 2u * std::max(0, outline));
  data.height = std::min(1024u, static_cast<uint32_t>(std::ceil(glyph.height)) + 2u * std::max(0, outline));
  const auto& font = State(*this).Get(id);
  data.isColorBitmap = font.bitmap.isColorFont || id == 0u;
  data.isColorEmoji = font.color && !data.isColorBitmap;
  data.format = (data.isColorEmoji || data.isColorBitmap) ? Pixel::BGRA8888 : Pixel::L8;
  if(font.description.family == "Test Color RGBA") data.format = Pixel::RGBA8888;
  const auto bytes = data.width * data.height * Pixel::GetBytesPerPixel(data.format);
  std::vector<uint8_t> pixels(bytes);
  for(unsigned int i = 0u; i < bytes; ++i)
  {
    // Transparent, translucent, and opaque samples exercise compositing.
    pixels[i] = i % 3u == 0u ? 0u : (i % 3u == 1u ? 136u : 255u);
  }
  data.compressionType = GlyphBufferData::CompressionType::NO_COMPRESSION;
  const auto policy = EnvironmentVariable::GetEnvironmentVariable("DALI_RENDERED_GLYPH_COMPRESS_POLICY");
  if(data.format == Pixel::L8 && bytes && policy && *policy)
  {
    data.compressionType = *policy == 'm' ? GlyphBufferData::CompressionType::RLE_4 : GlyphBufferData::CompressionType::BPP_4;
    GlyphBufferData::Compress(pixels.data(), data);
  }
  else
  {
    data.buffer = static_cast<uint8_t*>(std::malloc(bytes));
    if(bytes) std::memcpy(data.buffer, pixels.data(), bytes);
    data.isBufferOwned = true;
  }
}
PixelData FontClient::CreateBitmap(FontId id, GlyphIndex index, int outline)
{
  GlyphBufferData data;
  CreateBitmap(id, index, false, false, data, outline);
  const auto bytes = data.width * data.height * Pixel::GetBytesPerPixel(data.format);
  auto buffer = data.buffer;
  if(data.compressionType == GlyphBufferData::CompressionType::NO_COMPRESSION)
  {
    data.isBufferOwned = false;
  }
  else
  {
    buffer = static_cast<uint8_t*>(std::malloc(bytes));
    GlyphBufferData::Decompress(data, buffer);
  }
  return PixelData::New(buffer, bytes, data.width, data.height, data.format, PixelData::FREE);
}
void FontClient::CreateVectorBlob(FontId, GlyphIndex index, VectorBlob*& blob, unsigned int& length, unsigned int& width, unsigned int& height)
{
  // The mock GL backend consumes an atlas payload; no outline rasterizer is
  // involved. Both complete and partial atlas rows are exercised.
  static thread_local std::array<VectorBlob, 65u> payload{};
  blob = payload.data();
  length = index % 2u ? 64u : 65u;
  width = 8u;
  height = 16u;
}
const GlyphInfo& FontClient::GetEllipsisGlyph(PointSize26Dot6 size)
{
  auto& glyph = State(*this).ellipsis;
  glyph.fontId = FindDefaultFont(0x2026u, size, false);
  glyph.index = 0x2026u;
  GetGlyphMetrics(&glyph, 1u, BITMAP_GLYPH, true);
  return glyph;
}
bool FontClient::IsColorGlyph(FontId id, GlyphIndex) { return State(*this).Get(id).color; }
bool FontClient::IsColorFont(FontId id) { return State(*this).Get(id).color; }
bool FontClient::IsRenderableColrV1Font(FontId id) { return State(*this).Get(id).description.family == "Test Color COLRv1"; }
bool FontClient::IsRenderableColrV1Glyph(FontId id, GlyphIndex index) { return IsRenderableColrV1Font(id) && index != 0u; }
bool FontClient::AddCustomFontDirectory(const FontPath& path)
{
  auto& state = State(*this);
  if(std::find(state.directories.begin(), state.directories.end(), path) == state.directories.end())
  {
    state.directories.push_back(path);
    state.customFontAdded.Emit(path);
  }
  return true;
}
const FontPathList& FontClient::GetCustomFontDirectories() { return State(*this).directories; }
CustomFontAddedSignalType& FontClient::CustomFontAddedSignal() { return State(*this).customFontAdded; }
GlyphIndex FontClient::CreateEmbeddedItem(const EmbeddedItemDescription& item, Pixel::Format& format)
{
  auto copy = item;
  if((copy.width == 0u || copy.height == 0u) && !copy.url.empty())
  {
    const auto buffer = LoadImageFromFile(Integration::ToDaliStringView(copy.url));
    copy.width = buffer.GetWidth();
    copy.height = buffer.GetHeight();
  }
  State(*this).embedded.push_back(copy);
  format = Pixel::RGBA8888;
  return State(*this).embedded.size();
}
void FontClient::EnableAtlasLimitation(bool enabled) { State(*this).atlasEnabled = enabled; }
bool FontClient::IsAtlasLimitationEnabled() const { return State(*this).atlasEnabled; }
Size FontClient::GetMaximumTextAtlasSize() const { return MAX_TEXT_ATLAS_SIZE; }
Size FontClient::GetDefaultTextAtlasSize() const { return DEFAULT_TEXT_ATLAS_SIZE; }
Size FontClient::GetCurrentMaximumBlockSizeFitInAtlas() const { return State(*this).atlasBlock; }
bool FontClient::SetCurrentMaximumBlockSizeFitInAtlas(const Size& size)
{
  if(size.width > MAX_SIZE_FIT_IN_ATLAS.width || size.height > MAX_SIZE_FIT_IN_ATLAS.height) return false;
  State(*this).atlasBlock = size;
  return true;
}

Shaping::Shaping() = default;
Shaping::~Shaping() = default;
Shaping::Shaping(Internal::Shaping* implementation)
: BaseHandle(reinterpret_cast<TestShaping*>(implementation))
{
}
Shaping Shaping::New() { return Shaping(reinterpret_cast<Internal::Shaping*>(new TestShaping)); }
Shaping Shaping::Get()
{
  auto service = SingletonService::Get();
  if(!service) return {};
  const auto key = DALI_TYPE_ID(TestShaping);
  auto handle = service.GetSingleton(key);
  if(handle) return Shaping(reinterpret_cast<Internal::Shaping*>(handle.GetObjectPtr()));
  auto shaping = New();
  service.Register(key, shaping);
  return shaping;
}
Length Shaping::Shape(FontClient& client, const Character* const text, Length count, FontId id, Script)
{
  auto& state = ShapeState(*this);
  state.glyphs.clear();
  state.characters.clear();
  for(CharacterIndex i = 0u; i < count;)
  {
    GlyphInfo glyph;
    glyph.fontId = id;
    glyph.index = client.GetGlyphIndex(id, text[i]);
    glyph.isShaped = true;
    state.glyphs.push_back(glyph);
    state.characters.push_back(i);
    const bool emoji = text[i] >= 0x1f000u || (text[i] >= 0x2300u && text[i] <= 0x2fffu);
    const bool flag = text[i] >= 0x1f1e6u && text[i] <= 0x1f1ffu;
    ++i;
    if(flag && i < count && text[i] >= 0x1f1e6u && text[i] <= 0x1f1ffu) ++i;
    while(i < count)
    {
      if((text[i] >= 0x300u && text[i] <= 0x36fu) || text[i] == 0xfe0eu || text[i] == 0xfe0fu || text[i] == 0x20e3u ||
         (emoji && ((text[i] >= 0x1f3fbu && text[i] <= 0x1f3ffu) || (text[i] >= 0xe0020u && text[i] <= 0xe007fu))))
      {
        ++i;
      }
      else if(emoji && text[i] == 0x200du && i + 1u < count)
      {
        i += 2u;
      }
      else break;
    }
  }
  return state.glyphs.size();
}
void Shaping::GetGlyphs(GlyphInfo* glyphs, CharacterIndex* characters)
{
  const auto& state = ShapeState(*this);
  for(unsigned int i = 0u; i < state.glyphs.size(); ++i)
  {
    // ShapeText initializes the software styling flags; the adaptor boundary
    // supplies glyph IDs, advances, and offsets without overwriting the flags.
    glyphs[i].fontId = state.glyphs[i].fontId;
    glyphs[i].index = state.glyphs[i].index;
    glyphs[i].advance = 0.f;
    glyphs[i].xBearing = glyphs[i].yBearing = 0.f;
  }
  std::copy(state.characters.begin(), state.characters.end(), characters);
}

// A dictionary is another host input to text layout. These are deliberately
// synthetic break opportunities, not a model of any language's dictionary.
Hyphenation::Hyphenation() = default;
Hyphenation::~Hyphenation() = default;
Hyphenation::Hyphenation(Internal::Hyphenation* implementation)
: BaseHandle(reinterpret_cast<TestShaping*>(implementation))
{
}
Hyphenation Hyphenation::New()
{
  return Hyphenation(reinterpret_cast<Internal::Hyphenation*>(new TestShaping));
}
Hyphenation Hyphenation::Get() { return New(); }
const char* Hyphenation::GetDictionaryEncoding(const char* language)
{
  return language && std::strcmp(language, "test-utf8") == 0 ? "UTF-8" : "UTF-32";
}
Vector<bool> Hyphenation::GetWordHyphens(const char* text, Length bytes, const char* language)
{
  Vector<bool> breaks;
  auto count = bytes / sizeof(Character);
  if(language && std::strcmp(language, "test-utf8") == 0)
  {
    count = 0u;
    for(unsigned int i = 0u; i < bytes; ++i)
    {
      if((static_cast<unsigned char>(text[i]) & 0xc0u) != 0x80u) ++count;
    }
  }
  breaks.Resize(count, false);
  for(unsigned int i = 3u; i + 2u < count; i += 4u) breaks[i] = true;
  return breaks;
}
} // namespace DALI_NAMESPACE::TextAbstraction
