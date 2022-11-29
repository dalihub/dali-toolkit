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
 *
 */

// FILE HEADER
#include <dali-toolkit/devel-api/text/text-utils-devel.h>

// EXTERNAL INCLUDES
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/text-renderer-layout-helper.h>
#include <dali/devel-api/text-abstraction/text-renderer.h>
#include <dali/integration-api/debug.h>
#include <cstring>
#include <limits>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/bidirectional-support.h>
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/color-segmentation.h>
#include <dali-toolkit/internal/text/layouts/layout-engine.h>
#include <dali-toolkit/internal/text/layouts/layout-parameters.h>
#include <dali-toolkit/internal/text/markup-processor.h>
#include <dali-toolkit/internal/text/multi-language-support.h>
#include <dali-toolkit/internal/text/segmentation.h>
#include <dali-toolkit/internal/text/shaper.h>
#include <dali-toolkit/internal/text/text-enumerations-impl.h>
#include <dali-toolkit/internal/text/text-font-style.h>
#include <dali-toolkit/internal/text/text-model.h>

namespace Dali
{
using namespace TextAbstraction;

namespace Toolkit
{
using namespace Text;

namespace DevelText
{
namespace Layout
{
/**
 * @brief The text's layout.
 */
enum Type
{
  SINGLELINE, ///< The text is laid out on a single line.
  MULTILINE,  ///< The text is laid out in multiple lines.
  CIRCULAR,   ///< The text is laid out on a single line following a circular path.
};

} // namespace Layout

namespace CircularAlignment
{
/**
 * @brief The enumerations for the circular alignment.
 */
enum Type
{
  BEGIN,  ///< The text is aligned to the @p begin angle of the arc (or to the @p begin+increment if it's a RTL text).
  CENTER, ///< The text is centered within the arc.
  END,    ///< The text is aligned to the @p begin+increment angle of the arc (or to the @p begin if it's a RTL text).
};

} // namespace CircularAlignment

const float TO_POINT_26_DOT_6 = 64.f;
const float TO_FLOAT          = 1.f / 255.f;
const float TO_UCHAR          = 255.f;
const bool  RTL               = true;
const float TWO_PI            = 2.f * Dali::Math::PI;    ///< 360 degrees in radians
const float RAD_135           = Math::PI_2 + Math::PI_4; ///< 135 degrees in radians;
const float RAD_225           = RAD_135 + Math::PI_2;    ///< 225 degrees in radians;
const float RAD_270           = 3.f * Math::PI_2;        ///< 270 degrees in radians;
const float RAD_315           = RAD_225 + Math::PI_2;    ///< 315 degrees in radians;
const float MAX_INT           = std::numeric_limits<int>::max();

DALI_ENUM_TO_STRING_TABLE_BEGIN(LAYOUT_TYPE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelText::Layout, SINGLELINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelText::Layout, MULTILINE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelText::Layout, CIRCULAR)
DALI_ENUM_TO_STRING_TABLE_END(LAYOUT_TYPE)

DALI_ENUM_TO_STRING_TABLE_BEGIN(CIRCULAR_ALIGNMENT_TYPE)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelText::CircularAlignment, BEGIN)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelText::CircularAlignment, CENTER)
  DALI_ENUM_TO_STRING_WITH_SCOPE(DevelText::CircularAlignment, END)
DALI_ENUM_TO_STRING_TABLE_END(CIRCULAR_ALIGNMENT_TYPE)

struct InternalDataModel
{
  InternalDataModel(FontClient&    fontClient,
                    MetricsPtr     metrics,
                    Text::ModelPtr textModel)
  : fontClient(fontClient),
    metrics(metrics),
    textModel(textModel),
    numberOfCharacters{0u},
    isTextMirrored{false},
    numberOfGlyphs{0u}
  {
    layoutEngine.SetMetrics(metrics);
  }

  FontClient&               fontClient;
  MetricsPtr                metrics;
  Text::Layout::Engine      layoutEngine; ///< The layout engine.
  Text::ModelPtr            textModel;    ///< Pointer to the text's model.
  Vector<ColorBlendingMode> blendingMode; ///< How embedded items and bitmap font glyphs are blended with color text.
  Vector<bool>              isEmoji;      ///< Whether the glyph is an emoji.

  Vector<Character> mirroredUtf32Characters; // The utf32Characters Characters but mirrored if there are RTL text.

  Length numberOfCharacters; // The number of characters (not glyphs!).
  bool   isTextMirrored;     // Whether the text has been mirrored.

  Length numberOfGlyphs;
  Size   textLayoutArea;
};

bool GetLayoutEnumeration(const Property::Value& propertyValue, DevelText::Layout::Type& layout)
{
  return Scripting::GetEnumerationProperty(propertyValue, LAYOUT_TYPE_TABLE, LAYOUT_TYPE_TABLE_COUNT, layout);
}

bool GetCircularAlignmentEnumeration(const Property::Value& propertyValue, DevelText::CircularAlignment::Type& circularAlignment)
{
  return Scripting::GetEnumerationProperty(propertyValue, CIRCULAR_ALIGNMENT_TYPE_TABLE, CIRCULAR_ALIGNMENT_TYPE_TABLE_COUNT, circularAlignment);
}

void ShapeTextPreprocess(const RendererParameters& textParameters, TextAbstraction::TextRenderer::Parameters& rendererParameters, InternalDataModel& internalDataModel)
{
  MultilanguageSupport multilanguageSupport = MultilanguageSupport::Get();
  const uint8_t*       utf8                 = NULL; // pointer to the first character of the text (encoded in utf8)
  Length               textSize             = 0u;   // The length of the utf8 string.

  Length&            numberOfCharacters      = internalDataModel.numberOfCharacters;
  Vector<Character>& mirroredUtf32Characters = internalDataModel.mirroredUtf32Characters;
  Text::ModelPtr&    textModel               = internalDataModel.textModel;

  Vector<Character>&                     utf32Characters     = textModel->mLogicalModel->mText;                       // Characters encoded in utf32.
  Vector<LineBreakInfo>&                 lineBreakInfo       = textModel->mLogicalModel->mLineBreakInfo;              // The line break info.
  Vector<ScriptRun>&                     scripts             = textModel->mLogicalModel->mScriptRuns;                 // Charactes's script.
  Vector<FontDescriptionRun>&            fontDescriptionRuns = textModel->mLogicalModel->mFontDescriptionRuns;        // Desired font descriptions.
  Vector<FontRun>&                       validFonts          = textModel->mLogicalModel->mFontRuns;                   // Validated fonts.
  Vector<BidirectionalParagraphInfoRun>& bidirectionalInfo   = textModel->mLogicalModel->mBidirectionalParagraphInfo; // The bidirectional info per paragraph.
  Vector<CharacterDirection>&            directions          = textModel->mLogicalModel->mCharacterDirections;        // Character's directions.
  Vector<ColorRun>&                      colorRuns           = textModel->mLogicalModel->mColorRuns;                  // colors of the text.

  // the default font's description.
  FontDescription defaultFontDescription;
  PointSize26Dot6 defaultPointSize = FontClient::DEFAULT_POINT_SIZE;

  ////////////////////////////////////////////////////////////////////////////////
  // Process the markup string if the mark-up processor is enabled.
  ////////////////////////////////////////////////////////////////////////////////

  MarkupProcessData markupProcessData(colorRuns,
                                      fontDescriptionRuns,
                                      textModel->mLogicalModel->mEmbeddedItems,
                                      textModel->mLogicalModel->mAnchors,
                                      textModel->mLogicalModel->mUnderlinedCharacterRuns,
                                      textModel->mLogicalModel->mBackgroundColorRuns);

  if(textParameters.markupEnabled)
  {
    ProcessMarkupString(textParameters.text, markupProcessData);
    textSize = markupProcessData.markupProcessedText.size();

    // This is a bit horrible but std::string returns a (signed) char*
    utf8 = reinterpret_cast<const uint8_t*>(markupProcessData.markupProcessedText.c_str());
  }
  else
  {
    textSize = textParameters.text.size();

    // This is a bit horrible but std::string returns a (signed) char*
    utf8 = reinterpret_cast<const uint8_t*>(textParameters.text.c_str());
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Convert from utf8 to utf32
  ////////////////////////////////////////////////////////////////////////////////

  utf32Characters.Resize(textSize);

  // Transform a text array encoded in utf8 into an array encoded in utf32.
  // It returns the actual number of characters.
  numberOfCharacters = Utf8ToUtf32(utf8, textSize, utf32Characters.Begin());
  utf32Characters.Resize(numberOfCharacters);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the Line and Word Break Info.
  ////////////////////////////////////////////////////////////////////////////////

  lineBreakInfo.Resize(numberOfCharacters, LINE_NO_BREAK);

  SetLineBreakInfo(utf32Characters,
                   0u,
                   numberOfCharacters,
                   lineBreakInfo);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the script runs.
  ////////////////////////////////////////////////////////////////////////////////

  multilanguageSupport.SetScripts(utf32Characters,
                                  0u,
                                  numberOfCharacters,
                                  scripts);

  // Check if there are emojis.
  // If there are an RGBA8888 pixel format is needed.
  for(const auto& run : scripts)
  {
    if(run.script == TextAbstraction::Script::EMOJI)
    {
      rendererParameters.pixelFormat = TextAbstraction::TextRenderer::Parameters::RGBA8888;
      break;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the font runs.
  ////////////////////////////////////////////////////////////////////////////////

  // Set the description font run with the given text parameters.
  FontDescriptionRun fontDescriptionRun;
  fontDescriptionRun.characterRun.characterIndex     = 0u;
  fontDescriptionRun.characterRun.numberOfCharacters = numberOfCharacters;

  fontDescriptionRun.familyLength  = 0u;
  fontDescriptionRun.familyName    = nullptr;
  fontDescriptionRun.familyDefined = !textParameters.fontFamily.empty();
  if(fontDescriptionRun.familyDefined)
  {
    // The allocated memory will be freed when the logical model is destroyed.
    fontDescriptionRun.familyLength = textParameters.fontFamily.size();
    fontDescriptionRun.familyName   = new char[fontDescriptionRun.familyLength];
    memcpy(fontDescriptionRun.familyName, textParameters.fontFamily.c_str(), fontDescriptionRun.familyLength);
  }

  fontDescriptionRun.weightDefined = !textParameters.fontWeight.empty();
  if(fontDescriptionRun.weightDefined)
  {
    fontDescriptionRun.weight = StringToWeight(textParameters.fontWeight.c_str());
  }

  fontDescriptionRun.widthDefined = !textParameters.fontWidth.empty();
  if(fontDescriptionRun.widthDefined)
  {
    fontDescriptionRun.width = StringToWidth(textParameters.fontWidth.c_str());
  }

  fontDescriptionRun.slantDefined = !textParameters.fontSlant.empty();
  if(fontDescriptionRun.slantDefined)
  {
    fontDescriptionRun.slant = StringToSlant(textParameters.fontSlant.c_str());
  }

  fontDescriptionRun.sizeDefined = !EqualsZero(textParameters.fontSize);
  if(fontDescriptionRun.sizeDefined)
  {
    fontDescriptionRun.size = static_cast<unsigned int>(textParameters.fontSize * TO_POINT_26_DOT_6);
  }

  fontDescriptionRuns.PushBack(fontDescriptionRun);

  // Validates the fonts. If there is a character with no assigned font it sets a default one.
  // After this call, fonts are validated.
  multilanguageSupport.ValidateFonts(utf32Characters,
                                     scripts,
                                     fontDescriptionRuns,
                                     defaultFontDescription,
                                     defaultPointSize,
                                     0u,
                                     numberOfCharacters,
                                     validFonts);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the Bidirectional info.
  ////////////////////////////////////////////////////////////////////////////////

  bidirectionalInfo.Reserve(1u);

  SetBidirectionalInfo(utf32Characters,
                       scripts,
                       lineBreakInfo,
                       0u,
                       numberOfCharacters,
                       bidirectionalInfo);

  const bool hasBidirectionalText = 0u != bidirectionalInfo.Count();
  if(hasBidirectionalText)
  {
    // Only set the character directions if there is right to left characters.
    GetCharactersDirection(bidirectionalInfo,
                           numberOfCharacters,
                           0u,
                           numberOfCharacters,
                           directions);

    // This paragraph has right to left text. Some characters may need to be mirrored.
    // TODO: consider if the mirrored string can be stored as well.

    internalDataModel.isTextMirrored = GetMirroredText(utf32Characters,
                                                       directions,
                                                       bidirectionalInfo,
                                                       0u,
                                                       numberOfCharacters,
                                                       mirroredUtf32Characters);
  }
}

void ShapeText(TextAbstraction::TextRenderer::Parameters& rendererParameters, Vector<EmbeddedItemInfo>& embeddedItemLayout, InternalDataModel& internalDataModel)
{
  Vector<GlyphIndex>           newParagraphGlyphs; // Glyphs for the new paragraph characters.
  const Length                 numberOfCharacters      = internalDataModel.numberOfCharacters;
  const bool                   isTextMirrored          = internalDataModel.isTextMirrored;
  Text::ModelPtr&              textModel               = internalDataModel.textModel;
  const Vector<Character>&     mirroredUtf32Characters = internalDataModel.mirroredUtf32Characters;
  FontClient&                  fontClient              = internalDataModel.fontClient;
  const Vector<Character>&     utf32Characters         = textModel->mLogicalModel->mText;          // Characters encoded in utf32.
  const Vector<LineBreakInfo>& lineBreakInfo           = textModel->mLogicalModel->mLineBreakInfo; // The line break info.
  const Vector<ScriptRun>&     scripts                 = textModel->mLogicalModel->mScriptRuns;    // Charactes's script.
  const Vector<FontRun>&       validFonts              = textModel->mLogicalModel->mFontRuns;      // Validated fonts.

  Vector<CharacterIndex>& glyphsToCharacters = textModel->mVisualModel->mGlyphsToCharacters; // Glyphs to character map.
  Vector<Length>&         charactersPerGlyph = textModel->mVisualModel->mCharactersPerGlyph; // Number of characters per glyph.

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyphs. Text shaping
  ////////////////////////////////////////////////////////////////////////////////

  const Vector<Character>& textToShape = isTextMirrored ? mirroredUtf32Characters : utf32Characters;

  newParagraphGlyphs.Reserve(1u);

  // Shapes the text.
  ShapeText(textToShape,
            lineBreakInfo,
            scripts,
            validFonts,
            0u,
            0u,
            numberOfCharacters,
            rendererParameters.glyphs,
            glyphsToCharacters,
            charactersPerGlyph,
            newParagraphGlyphs);

  // Create the 'number of glyphs' per character and the glyph to character conversion tables.
  textModel->mVisualModel->CreateGlyphsPerCharacterTable(0u, 0u, numberOfCharacters);
  textModel->mVisualModel->CreateCharacterToGlyphTable(0u, 0u, numberOfCharacters);

  internalDataModel.numberOfGlyphs = rendererParameters.glyphs.Count();

  // Once the text has been shaped and the glyphs created it's possible to replace the font id of those glyphs
  // that represent an image or an item and create the embedded item layout info.
  // Note: the position of the embedded item can't be set until the text is laid-out.
  embeddedItemLayout.Reserve(textModel->mLogicalModel->mEmbeddedItems.Count());
  for(const auto& item : textModel->mLogicalModel->mEmbeddedItems)
  {
    // Get the glyph that matches with the character index.
    const GlyphIndex glyphIndex = textModel->mVisualModel->mCharactersToGlyph[item.characterIndex];
    GlyphInfo&       glyph      = rendererParameters.glyphs[glyphIndex];

    glyph.fontId                                                     = 0u;
    Pixel::Format                                        pixelFormat = Pixel::A8;
    TextAbstraction::FontClient::EmbeddedItemDescription description = {std::string(item.url, item.urlLength), item.width, item.height, item.colorBlendingMode};
    glyph.index                                                      = fontClient.CreateEmbeddedItem(description, pixelFormat); // Set here an index to an item.

    if((Pixel::RGBA8888 == pixelFormat) || (Pixel::BGRA8888 == pixelFormat))
    {
      rendererParameters.pixelFormat = TextAbstraction::TextRenderer::Parameters::RGBA8888;
    }

    // If the url is empty the item is going to be added after the text is rendered. It's needed to store the layout here.
    if(description.url.empty())
    {
      EmbeddedItemInfo embeddedInfo =
        {
          item.characterIndex,
          glyphIndex,
          Vector2::ZERO,
          Size(static_cast<float>(item.width), static_cast<float>(item.height)),
          Size(static_cast<float>(item.width), static_cast<float>(item.height)),
          Degree(0.f),
          item.colorBlendingMode};

      embeddedItemLayout.PushBack(embeddedInfo);
    }
  }
}

void SetColorSegmentation(const RendererParameters& textParameters, InternalDataModel& internalDataModel)
{
  Text::ModelPtr&            textModel    = internalDataModel.textModel;
  Vector<ColorBlendingMode>& blendingMode = internalDataModel.blendingMode;

  Vector<ColorRun>& colorRuns = textModel->mLogicalModel->mColorRuns; // colors of the text.

  Vector<GlyphIndex>& charactersToGlyph  = textModel->mVisualModel->mCharactersToGlyph;  // Characters to glyphs map.
  Vector<Length>&     glyphsPerCharacter = textModel->mVisualModel->mGlyphsPerCharacter; // The number of glyphs that are shaped.

  ////////////////////////////////////////////////////////////////////////////////
  // Set the color runs in glyphs.
  ////////////////////////////////////////////////////////////////////////////////

  SetColorSegmentationInfo(colorRuns,
                           charactersToGlyph,
                           glyphsPerCharacter,
                           0u,
                           0u,
                           internalDataModel.numberOfCharacters,
                           textModel->mVisualModel->mColors,
                           textModel->mVisualModel->mColorIndices);

  // Insert the default color at the beginning of the vector.
  textModel->mVisualModel->mColors.Insert(textModel->mVisualModel->mColors.Begin(), textParameters.textColor);

  // Set how the embedded items are blended with text color.
  blendingMode.Resize(internalDataModel.numberOfGlyphs, textParameters.isTextColorSet ? ColorBlendingMode::MULTIPLY : ColorBlendingMode::NONE);

  if(!textParameters.isTextColorSet)
  {
    // Traverse the color runs.
    for(const auto& run : colorRuns)
    {
      const GlyphIndex     firstGlyph       = textModel->mVisualModel->mCharactersToGlyph[run.characterRun.characterIndex];
      const CharacterIndex lastCharacter    = run.characterRun.characterIndex + run.characterRun.numberOfCharacters - 1u;
      const GlyphIndex     lastGlyphPlusOne = textModel->mVisualModel->mCharactersToGlyph[lastCharacter] + textModel->mVisualModel->mGlyphsPerCharacter[lastCharacter];

      for(GlyphIndex index = firstGlyph; index < lastGlyphPlusOne; ++index)
      {
        blendingMode[index] = ColorBlendingMode::MULTIPLY;
      }
    }
  }

  // Traverse the embedded items and update the blending mode vector.
  for(const auto& item : textModel->mLogicalModel->mEmbeddedItems)
  {
    const GlyphIndex glyphIndex = textModel->mVisualModel->mCharactersToGlyph[item.characterIndex];
    blendingMode[glyphIndex]    = item.colorBlendingMode;
  }
}

void SetEmojiVector(InternalDataModel& internalDataModel)
{
  Vector<bool>&   isEmoji        = internalDataModel.isEmoji;
  Text::ModelPtr& textModel      = internalDataModel.textModel;
  const Length    numberOfGlyphs = internalDataModel.numberOfGlyphs;

  const Vector<ScriptRun>& scripts = textModel->mLogicalModel->mScriptRuns; // Charactes's script.
  ////////////////////////////////////////////////////////////////////////////////
  // Set the isEmoji Vector
  ////////////////////////////////////////////////////////////////////////////////

  isEmoji.Resize(numberOfGlyphs, false);

  for(const auto& run : scripts)
  {
    if(run.script == TextAbstraction::Script::EMOJI)
    {
      const GlyphIndex     firstGlyph       = textModel->mVisualModel->mCharactersToGlyph[run.characterRun.characterIndex];
      const CharacterIndex lastCharacter    = run.characterRun.characterIndex + run.characterRun.numberOfCharacters - 1u;
      const GlyphIndex     lastGlyphPlusOne = textModel->mVisualModel->mCharactersToGlyph[lastCharacter] + textModel->mVisualModel->mGlyphsPerCharacter[lastCharacter];

      for(GlyphIndex index = firstGlyph; index < lastGlyphPlusOne; ++index)
      {
        isEmoji[index] = true;
      }
    }
  }
}

void Align(const RendererParameters& textParameters, TextAbstraction::TextRenderer::Parameters& rendererParameters, Vector<EmbeddedItemInfo>& embeddedItemLayout, InternalDataModel& internalDataModel, const Size& newLayoutSize)
{
  Text::Layout::Engine& layoutEngine       = internalDataModel.layoutEngine;
  Text::ModelPtr&       textModel          = internalDataModel.textModel;
  const Length          numberOfCharacters = internalDataModel.numberOfCharacters;
  Size&                 textLayoutArea     = internalDataModel.textLayoutArea;

  Vector<LineRun>& lines = textModel->mVisualModel->mLines; // The laid out lines.

  ////////////////////////////////////////////////////////////////////////////////
  // Align the text.
  ////////////////////////////////////////////////////////////////////////////////

  HorizontalAlignment::Type horizontalAlignment         = Toolkit::HorizontalAlignment::CENTER;
  HorizontalAlignment::Type horizontalCircularAlignment = Toolkit::HorizontalAlignment::CENTER;
  VerticalAlignment::Type   verticalAlignment           = VerticalAlignment::CENTER;
  CircularAlignment::Type   circularAlignment           = CircularAlignment::BEGIN;

  Layout::Type layout = Layout::SINGLELINE;

  // Sets the alignment
  Property::Value horizontalAlignmentStr(textParameters.horizontalAlignment);
  GetHorizontalAlignmentEnumeration(horizontalAlignmentStr, horizontalAlignment);
  horizontalCircularAlignment = horizontalAlignment;

  Property::Value verticalAlignmentStr(textParameters.verticalAlignment);
  GetVerticalAlignmentEnumeration(verticalAlignmentStr, verticalAlignment);

  Property::Value circularAlignmentStr(textParameters.circularAlignment);
  GetCircularAlignmentEnumeration(circularAlignmentStr, circularAlignment);

  Property::Value layoutStr(textParameters.layout);
  GetLayoutEnumeration(layoutStr, layout);

  // Whether the layout is circular.
  const bool isCircularTextLayout = (Layout::CIRCULAR == layout);
  const bool isClockwise          = isCircularTextLayout && (0.f < textParameters.incrementAngle);

  // Convert CircularAlignment to HorizontalAlignment.
  if(isCircularTextLayout)
  {
    switch(circularAlignment)
    {
      case CircularAlignment::BEGIN:
      {
        horizontalCircularAlignment = Toolkit::HorizontalAlignment::BEGIN;
        break;
      }
      case CircularAlignment::CENTER:
      {
        horizontalCircularAlignment = Toolkit::HorizontalAlignment::CENTER;
        break;
      }
      case CircularAlignment::END:
      {
        horizontalCircularAlignment = Toolkit::HorizontalAlignment::END;
        break;
      }
    }
  }
  textModel->mHorizontalAlignment = isCircularTextLayout ? horizontalCircularAlignment : horizontalAlignment;

  // Retrieve the line of text to know the direction and the width. @todo multi-line
  const LineRun& line = lines[0u];

  if(isCircularTextLayout)
  {
    // Set the circular alignment.
    rendererParameters.circularLayout = isClockwise ? TextRenderer::Parameters::CLOCKWISE : TextRenderer::Parameters::COUNTER_CLOCKWISE;

    // Update the text's height to be used by the ellipsis code.
    textLayoutArea.height = newLayoutSize.height;

    // Set the size of the text laid out on a straight horizontal line.
    rendererParameters.circularWidth  = static_cast<unsigned int>(newLayoutSize.width);
    rendererParameters.circularHeight = static_cast<unsigned int>(newLayoutSize.height);

    // Calculate the center of the circular text according the horizontal and vertical alingments and the radius.
    switch(horizontalAlignment)
    {
      case HorizontalAlignment::BEGIN:
      {
        rendererParameters.centerX = static_cast<int>(textParameters.radius);
        break;
      }
      case HorizontalAlignment::CENTER:
      {
        rendererParameters.centerX = static_cast<int>(textParameters.textWidth / 2u);
        break;
      }
      case HorizontalAlignment::END:
      {
        rendererParameters.centerX = static_cast<int>(textParameters.textWidth) - static_cast<int>(textParameters.radius);
        break;
      }
    }

    switch(verticalAlignment)
    {
      case VerticalAlignment::TOP:
      {
        rendererParameters.centerY = static_cast<int>(textParameters.radius);
        break;
      }
      case VerticalAlignment::CENTER:
      {
        rendererParameters.centerY = static_cast<int>(textParameters.textHeight / 2u);
        break;
      }
      case VerticalAlignment::BOTTOM:
      {
        rendererParameters.centerY = static_cast<int>(textParameters.textHeight) - static_cast<int>(textParameters.radius);
        break;
      }
    }

    // Calculate the beginning angle according with the given horizontal alignment.
    const bool isRTL = RTL == line.direction;

    CircularAlignment::Type alignment = circularAlignment;
    if(isRTL)
    {
      // Swap the alignment type if the line is right to left.
      switch(alignment)
      {
        case CircularAlignment::BEGIN:
        {
          alignment = CircularAlignment::END;
          break;
        }
        case CircularAlignment::CENTER:
        {
          // Nothing to do.
          break;
        }
        case CircularAlignment::END:
        {
          alignment = CircularAlignment::BEGIN;
          break;
        }
      }
    }

    float angleOffset = 0.f;

    switch(alignment)
    {
      case CircularAlignment::BEGIN:
      {
        angleOffset = 0.f;
        break;
      }
      case CircularAlignment::CENTER:
      {
        const bool  isNeg     = textParameters.incrementAngle < 0.f;
        const float textWidth = static_cast<float>(rendererParameters.circularWidth);
        angleOffset           = (isNeg ? -0.5f : 0.5f) * (textLayoutArea.width - textWidth) / static_cast<float>(rendererParameters.radius);
        break;
      }
      case CircularAlignment::END:
      {
        const bool  isNeg     = textParameters.incrementAngle < 0.f;
        const float textWidth = static_cast<float>(rendererParameters.circularWidth);
        angleOffset           = (isNeg ? -1.f : 1.f) * (textLayoutArea.width - textWidth) / static_cast<float>(rendererParameters.radius);
        break;
      }
    }

    // Update the beginning angle with the calculated offset.
    rendererParameters.beginAngle = Radian(Degree(textParameters.beginAngle)) + angleOffset;

    // Set the vertical position of the glyphs.
    for(auto& position : rendererParameters.positions)
    {
      position.y = 0.f;
    }
  }
  else
  {
    // Calculate the vertical offset according with the given alignment.
    float penY = 0.f;

    switch(verticalAlignment)
    {
      case VerticalAlignment::TOP:
      {
        penY = line.ascender;
        break;
      }
      case VerticalAlignment::CENTER:
      {
        penY = line.ascender + 0.5f * (textLayoutArea.height - (line.ascender - line.descender));
        break;
      }
      case VerticalAlignment::BOTTOM:
      {
        penY = textLayoutArea.height;
        break;
      }
    }

    // Calculate the horizontal offset according with the given alignment.
    float alignmentOffset = 0.f;
    layoutEngine.Align(textLayoutArea,
                       0u,
                       numberOfCharacters,
                       horizontalAlignment,
                       lines,
                       alignmentOffset,
                       Dali::LayoutDirection::LEFT_TO_RIGHT,
                       false);

    // Update the position of the glyphs with the calculated offsets.
    for(auto& position : rendererParameters.positions)
    {
      position.x += line.alignmentOffset;
      position.y = penY;
    }
  }

  // Cairo adds the bearing to the position of the glyph
  // that has already been added by the DALi's layout engine,
  // so it's needed to be removed here.
  for(unsigned int index = 0u; index < rendererParameters.glyphs.Count(); ++index)
  {
    const GlyphInfo& glyph    = rendererParameters.glyphs[index];
    Vector2&         position = rendererParameters.positions[index];

    position.x -= glyph.xBearing;
  }

  // Set the position of the embedded items (if there is any).
  EmbeddedItemInfo* embeddedItemLayoutBuffer = embeddedItemLayout.Begin();

  for(Length index = 0u, endIndex = embeddedItemLayout.Count(); index < endIndex; ++index)
  {
    EmbeddedItemInfo& embeddedItem = *(embeddedItemLayoutBuffer + index);

    embeddedItem.position = rendererParameters.positions[embeddedItem.glyphIndex];

    if(isCircularTextLayout)
    {
      // Calculate the new position of the embedded item in the circular path.

      // Center of the bitmap.
      const float halfWidth  = 0.5f * embeddedItem.size.width;
      const float halfHeight = 0.5f * embeddedItem.size.height;
      double      centerX    = static_cast<double>(embeddedItem.position.x + halfWidth);
      double      centerY    = static_cast<double>(embeddedItem.position.y - halfHeight);

      Dali::TextAbstraction::CircularTextParameters circularTextParameters;

      circularTextParameters.radius     = static_cast<double>(rendererParameters.radius);
      circularTextParameters.invRadius  = 1.0 / circularTextParameters.radius;
      circularTextParameters.beginAngle = static_cast<double>(-rendererParameters.beginAngle + Dali::Math::PI_2);
      circularTextParameters.centerX    = 0.5f * static_cast<double>(textParameters.textWidth);
      circularTextParameters.centerY    = 0.5f * static_cast<double>(textParameters.textHeight);

      // Calculate the rotation angle.
      float radians = rendererParameters.beginAngle;
      if(isClockwise)
      {
        radians += static_cast<float>(circularTextParameters.invRadius * centerX);
        radians = -radians;
      }
      else
      {
        radians -= static_cast<float>(circularTextParameters.invRadius * centerX);
        radians = -radians + Dali::Math::PI;
      }
      embeddedItem.angle = Degree(Radian(radians));

      Dali::TextAbstraction::TransformToArc(circularTextParameters, centerX, centerY);

      // Recalculate the size of the embedded item after the rotation to position it correctly.
      float width  = embeddedItem.size.width;
      float height = embeddedItem.size.height;

      // Transform the input angle into the range [0..2PI]
      radians = fmod(radians, TWO_PI);
      radians += (radians < 0.f) ? TWO_PI : 0.f;

      // Does the same operations than rotate by shear.
      if((radians > Math::PI_4) && (radians <= RAD_135))
      {
        std::swap(width, height);
        radians -= Math::PI_2;
      }
      else if((radians > RAD_135) && (radians <= RAD_225))
      {
        radians -= Math::PI;
      }
      else if((radians > RAD_225) && (radians <= RAD_315))
      {
        std::swap(width, height);
        radians -= RAD_270;
      }

      if(fabs(radians) > Dali::Math::MACHINE_EPSILON_10)
      {
        const float angleSinus   = fabs(sin(radians));
        const float angleCosinus = cos(radians);

        // Calculate the rotated image dimensions.
        embeddedItem.rotatedSize.height = width * angleSinus + height * angleCosinus;
        embeddedItem.rotatedSize.width  = height * angleSinus + width * angleCosinus + 1.f;
      }

      embeddedItem.position.x = floor(static_cast<float>(centerX) - 0.5f * embeddedItem.rotatedSize.width);
      embeddedItem.position.y = floor(static_cast<float>(centerY) - 0.5f * embeddedItem.rotatedSize.height);
    }
    else
    {
      embeddedItem.position.y -= embeddedItem.size.height;
    }
  }
}

void Ellipsis(const RendererParameters& textParameters, TextAbstraction::TextRenderer::Parameters& rendererParameters, Vector<EmbeddedItemInfo>& embeddedItemLayout, InternalDataModel& internalDataModel)
{
  Text::ModelPtr& textModel  = internalDataModel.textModel;
  FontClient&     fontClient = internalDataModel.fontClient;

  Vector<LineRun>& lines          = textModel->mVisualModel->mLines; // The laid out lines.
  Vector<bool>&    isEmoji        = internalDataModel.isEmoji;
  const Size       textLayoutArea = internalDataModel.textLayoutArea;
  ////////////////////////////////////////////////////////////////////////////////
  // Ellipsis the text.
  ////////////////////////////////////////////////////////////////////////////////

  if(textParameters.ellipsisEnabled)
  {
    const LineRun& line = lines[0u]; // TODO: multi-line

    if(line.ellipsis)
    {
      Length finalNumberOfGlyphs = 0u;

      if((GetLineHeight(line)) > textLayoutArea.height)
      {
        // The height of the line is bigger than the height of the text area.
        // Show the ellipsis glyph even if it doesn't fit in the text area.
        // If no text is rendered then issues are rised and it may be a while
        // until is find out that the text area is too small.

        // Get the first glyph which is going to be replaced and the ellipsis glyph.
        GlyphInfo&       glyphInfo     = rendererParameters.glyphs[0u];
        const GlyphInfo& ellipsisGlyph = fontClient.GetEllipsisGlyph(fontClient.GetPointSize(glyphInfo.fontId));

        // Change the 'x' and 'y' position of the ellipsis glyph.
        Vector2& position = rendererParameters.positions[0u];
        position.x        = ellipsisGlyph.xBearing;
        position.y        = textLayoutArea.height - ellipsisGlyph.yBearing;

        // Replace the glyph by the ellipsis glyph.
        glyphInfo = ellipsisGlyph;

        // Set the final number of glyphs
        finalNumberOfGlyphs = 1u;
      }
      else
      {
        // firstPenX, penY and firstPenSet are used to position the ellipsis glyph if needed.
        float firstPenX   = 0.f; // Used if rtl text is elided.
        bool  firstPenSet = false;

        // Add the ellipsis glyph.
        bool   inserted              = false;
        float  removedGlypsWidth     = 0.f;
        Length numberOfRemovedGlyphs = 0u;
        if(line.glyphRun.numberOfGlyphs > 0u)
        {
          GlyphIndex index = line.glyphRun.numberOfGlyphs - 1u;

          GlyphInfo* glyphs         = rendererParameters.glyphs.Begin();
          Vector2*   glyphPositions = rendererParameters.positions.Begin();

          float penY = 0.f;

          // The ellipsis glyph has to fit in the place where the last glyph(s) is(are) removed.
          while(!inserted)
          {
            const GlyphInfo& glyphToRemove = *(glyphs + index);

            if(0u != glyphToRemove.fontId)
            {
              // i.e. The font id of the glyph shaped from the '\n' character is zero.

              // Need to reshape the glyph as the font may be different in size.
              const GlyphInfo& ellipsisGlyph = fontClient.GetEllipsisGlyph(fontClient.GetPointSize(glyphToRemove.fontId));

              if(!firstPenSet)
              {
                const Vector2& position = *(glyphPositions + index);

                // Calculates the penY of the current line. It will be used to position the ellipsis glyph.
                penY = position.y;

                // Calculates the first penX which will be used if rtl text is elided.
                firstPenX = position.x - glyphToRemove.xBearing;
                if(firstPenX < -ellipsisGlyph.xBearing)
                {
                  // Avoids to exceed the bounding box when rtl text is elided.
                  firstPenX = -ellipsisGlyph.xBearing;
                }

                removedGlypsWidth = -ellipsisGlyph.xBearing;

                firstPenSet = true;
              }

              removedGlypsWidth += std::min(glyphToRemove.advance, (glyphToRemove.xBearing + glyphToRemove.width));

              // Calculate the width of the ellipsis glyph and check if it fits.
              const float ellipsisGlyphWidth = ellipsisGlyph.width + ellipsisGlyph.xBearing;
              if(ellipsisGlyphWidth < removedGlypsWidth)
              {
                GlyphInfo& glyphInfo = *(glyphs + index);
                Vector2&   position  = *(glyphPositions + index);
                position.x -= (0.f > glyphInfo.xBearing) ? glyphInfo.xBearing : 0.f;

                // Replace the glyph by the ellipsis glyph.
                glyphInfo = ellipsisGlyph;

                // Update the isEmoji vector
                isEmoji[index] = false;

                // Change the 'x' and 'y' position of the ellipsis glyph.

                if(position.x > firstPenX)
                {
                  position.x = firstPenX + removedGlypsWidth - ellipsisGlyphWidth;
                }

                position.x += ellipsisGlyph.xBearing;
                position.y = penY;

                inserted = true;
              }
            }

            if(!inserted)
            {
              if(index > 0u)
              {
                --index;
              }
              else
              {
                // No space for the ellipsis.
                inserted = true;
              }
              ++numberOfRemovedGlyphs;
            }

            // Set the final number of glyphs
            finalNumberOfGlyphs = line.glyphRun.numberOfGlyphs - numberOfRemovedGlyphs;
          }
        }

        // Resize the number of glyphs/positions
        rendererParameters.glyphs.Resize(finalNumberOfGlyphs);
        rendererParameters.positions.Resize(finalNumberOfGlyphs);

        // Remove from the embedded items those exceding the last laid out glyph.
        embeddedItemLayout.Erase(std::remove_if(embeddedItemLayout.Begin(),
                                                embeddedItemLayout.End(),
                                                [finalNumberOfGlyphs](const EmbeddedItemInfo& item) {
                                                  return item.glyphIndex >= finalNumberOfGlyphs;
                                                }),
                                 embeddedItemLayout.End());
      }
    }
  }
}

Size LayoutText(const RendererParameters& textParameters, TextAbstraction::TextRenderer::Parameters& rendererParameters, Vector<EmbeddedItemInfo>& embeddedItemLayout, InternalDataModel& internalDataModel)
{
  ////////////////////////////////////////////////////////////////////////////////
  // Layout the text.
  ////////////////////////////////////////////////////////////////////////////////
  Text::ModelPtr&          textModel               = internalDataModel.textModel;
  Text::Layout::Engine&    layoutEngine            = internalDataModel.layoutEngine;
  FontClient&              fontClient              = internalDataModel.fontClient;
  const Length             numberOfGlyphs          = internalDataModel.numberOfGlyphs;
  const bool               isTextMirrored          = internalDataModel.isTextMirrored;
  const Vector<Character>& mirroredUtf32Characters = internalDataModel.mirroredUtf32Characters;
  const Length             numberOfCharacters      = internalDataModel.numberOfCharacters;
  const auto               ellipsisPosition        = textModel->mEllipsisPosition;

  Layout::Type layout = Layout::SINGLELINE;

  Property::Value layoutStr(textParameters.layout);
  GetLayoutEnumeration(layoutStr, layout);

  // Whether the layout is multi-line.
  const Text::Layout::Engine::Type horizontalLayout = (Layout::MULTILINE == layout) ? Text::Layout::Engine::MULTI_LINE_BOX : Text::Layout::Engine::SINGLE_LINE_BOX;
  layoutEngine.SetLayout(horizontalLayout); // TODO: multi-line.

  // Set minimun line size
  layoutEngine.SetDefaultLineSize(textParameters.minLineSize);

  // Whether the layout is circular.
  const bool isCircularTextLayout = (Layout::CIRCULAR == layout);
  const bool isClockwise          = isCircularTextLayout && (0.f < textParameters.incrementAngle);

  // Calculates the max ascender or the max descender.
  // Is used to calculate the radius of the base line of the text.
  float maxAscenderDescender = 0.f;
  if(isCircularTextLayout)
  {
    FontId currentFontId = 0u;
    for(const auto& glyph : rendererParameters.glyphs)
    {
      if(currentFontId != glyph.fontId)
      {
        currentFontId = glyph.fontId;
        FontMetrics metrics;
        fontClient.GetFontMetrics(currentFontId, metrics);
        maxAscenderDescender = std::max(maxAscenderDescender, isClockwise ? metrics.ascender : metrics.descender);
      }
    }
  }
  const unsigned int radius = textParameters.radius - static_cast<unsigned int>(maxAscenderDescender);

  // Set the layout parameters.
  Size textLayoutArea = Size(static_cast<float>(textParameters.textWidth),
                             static_cast<float>(textParameters.textHeight));

  // padding
  Extents padding                  = textParameters.padding;
  internalDataModel.textLayoutArea = Size(textLayoutArea.x - (padding.start + padding.end), textLayoutArea.y - (padding.top + padding.bottom));

  if(isCircularTextLayout)
  {
    // In a circular layout, the length of the text area depends on the radius.
    rendererParameters.radius              = radius;
    internalDataModel.textLayoutArea.width = fabs(Radian(Degree(textParameters.incrementAngle)) * static_cast<float>(rendererParameters.radius));
  }
  // Resize the vector of positions to have the same size than the vector of glyphs.
  rendererParameters.positions.Resize(numberOfGlyphs);

  textModel->mLineWrapMode          = Text::LineWrap::WORD;
  textModel->mIgnoreSpacesAfterText = false;
  Text::Layout::Parameters layoutParameters(internalDataModel.textLayoutArea,
                                            textModel);

  // Whether the last character is a new paragraph character.
  const Vector<Character>& textToShape = isTextMirrored ? mirroredUtf32Characters : textModel->mLogicalModel->mText;
  layoutParameters.isLastNewParagraph  = TextAbstraction::IsNewParagraph(textToShape[numberOfCharacters - 1u]);

  // The initial glyph and the number of glyphs to layout.
  layoutParameters.startGlyphIndex        = 0u;
  layoutParameters.numberOfGlyphs         = numberOfGlyphs;
  layoutParameters.startLineIndex         = 0u;
  layoutParameters.estimatedNumberOfLines = 1u;
  layoutParameters.interGlyphExtraAdvance = 0.f;

  // Update the visual model.
  Size newLayoutSize;
  bool isAutoScrollEnabled = false;
  bool isAutoScrollMaxTextureExceeded = false;
  bool isHiddenInputEnabled           = false;

  layoutEngine.LayoutText(layoutParameters,
                          newLayoutSize,
                          textParameters.ellipsisEnabled,
                          isAutoScrollEnabled,
                          isAutoScrollMaxTextureExceeded,
                          isHiddenInputEnabled,
                          ellipsisPosition);

  return newLayoutSize;
}

Devel::PixelBuffer RenderText(const RendererParameters& textParameters, TextAbstraction::TextRenderer::Parameters& rendererParameters)
{
  ////////////////////////////////////////////////////////////////////////////////
  // Render the text.
  ////////////////////////////////////////////////////////////////////////////////

  rendererParameters.width  = textParameters.textWidth;
  rendererParameters.height = textParameters.textHeight;

  TextAbstraction::TextRenderer renderer = TextAbstraction::TextRenderer::Get();
  return renderer.Render(rendererParameters);
}

Devel::PixelBuffer Render(const RendererParameters& textParameters, Vector<EmbeddedItemInfo>& embeddedItemLayout)
{
  if(textParameters.text.empty())
  {
    Dali::Devel::PixelBuffer pixelBuffer = Dali::Devel::PixelBuffer::New(textParameters.textWidth,
                                                                         textParameters.textHeight,
                                                                         Dali::Pixel::RGBA8888);

    const unsigned int bufferSize = textParameters.textWidth * textParameters.textHeight * Dali::Pixel::GetBytesPerPixel(Dali::Pixel::RGBA8888);
    unsigned char*     buffer     = pixelBuffer.GetBuffer();
    memset(buffer, 0, bufferSize);

    return pixelBuffer;
  }

  FontClient fontClient = FontClient::Get();
  MetricsPtr metrics;
  // Use this to access FontClient i.e. to get down-scaled Emoji metrics.
  metrics = Metrics::New(fontClient);

  Text::ModelPtr    textModel = Text::Model::New();
  InternalDataModel internalData(fontClient, metrics, textModel);

  TextAbstraction::TextRenderer::Parameters rendererParameters(internalData.textModel->mVisualModel->mGlyphs,
                                                               internalData.textModel->mVisualModel->mGlyphPositions,
                                                               internalData.textModel->mVisualModel->mColors,
                                                               internalData.textModel->mVisualModel->mColorIndices,
                                                               internalData.blendingMode,
                                                               internalData.isEmoji);

  rendererParameters.width       = textParameters.textWidth;
  rendererParameters.height      = textParameters.textHeight;
  rendererParameters.pixelFormat = TextAbstraction::TextRenderer::Parameters::RGBA8888; // @note: At the moment all textures are generated RGBA8888

  ////////////////////////////////////////////////////////////////////////////////
  // Process the markup string if the mark-up processor is enabled.
  ////////////////////////////////////////////////////////////////////////////////
  ShapeTextPreprocess(textParameters, rendererParameters, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyphs. Text shaping
  ////////////////////////////////////////////////////////////////////////////////
  ShapeText(rendererParameters, embeddedItemLayout, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyph's metrics.
  ////////////////////////////////////////////////////////////////////////////////

  metrics->GetGlyphMetrics(rendererParameters.glyphs.Begin(), internalData.numberOfGlyphs);

  ////////////////////////////////////////////////////////////////////////////////
  // Set the color runs in glyphs.
  ////////////////////////////////////////////////////////////////////////////////
  SetColorSegmentation(textParameters, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Set the isEmoji Vector
  ////////////////////////////////////////////////////////////////////////////////
  SetEmojiVector(internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Layout the text
  ////////////////////////////////////////////////////////////////////////////////
  Size newLayoutSize = LayoutText(textParameters, rendererParameters, embeddedItemLayout, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Align the text.
  ////////////////////////////////////////////////////////////////////////////////
  Align(textParameters, rendererParameters, embeddedItemLayout, internalData, newLayoutSize);

  ////////////////////////////////////////////////////////////////////////////////
  // Ellipsis the text.
  ////////////////////////////////////////////////////////////////////////////////
  Ellipsis(textParameters, rendererParameters, embeddedItemLayout, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Render the text.
  ////////////////////////////////////////////////////////////////////////////////
  return RenderText(textParameters, rendererParameters);
}

Devel::PixelBuffer CreateShadow(const ShadowParameters& shadowParameters)
{
  // The size of the pixel data.
  const int width  = static_cast<int>(shadowParameters.input.GetWidth());
  const int height = static_cast<int>(shadowParameters.input.GetHeight());

  // The shadow's offset.
  const int xOffset = static_cast<int>(shadowParameters.offset.x);
  const int yOffset = static_cast<int>(shadowParameters.offset.y);

  // The size in bytes of the pixel of the input's buffer.
  const Pixel::Format inputFormat    = shadowParameters.input.GetPixelFormat();
  const unsigned int  inputPixelSize = Pixel::GetBytesPerPixel(inputFormat);
  const bool          isA8           = Pixel::A8 == inputFormat;

  // Creates the output pixel buffer.
  Devel::PixelBuffer outputPixelBuffer = Devel::PixelBuffer::New(width, height, Pixel::RGBA8888);

  // Clear the output buffer
  unsigned char* outputPixelBufferPtr = outputPixelBuffer.GetBuffer();
  memset(outputPixelBufferPtr, 0, width * height * Pixel::GetBytesPerPixel(Pixel::RGBA8888));

  // Gets the buffer of the input pixel buffer.
  const unsigned char* const inputPixelBuffer = shadowParameters.input.GetBuffer();

  float textColor[4u];
  if(isA8)
  {
    memcpy(textColor, shadowParameters.textColor.AsFloat(), 4u * sizeof(float));
  }
  const float* const shadowColor = shadowParameters.color.AsFloat();

  // Traverse the input pixel buffer and write the text on the foreground and the shadow on the background.
  for(int rowIndex = 0; rowIndex < height; ++rowIndex)
  {
    // Calculates the rowIndex to the input pixel buffer for the shadow and whether it's within the boundaries.
    const int  yOffsetIndex    = rowIndex - yOffset;
    const bool isValidRowIndex = ((yOffsetIndex >= 0) && (yOffsetIndex < height));

    const int rows       = rowIndex * width;
    const int offsetRows = yOffsetIndex * width;
    for(int columnIndex = 0; columnIndex < width; ++columnIndex)
    {
      // Index to the input buffer to retrieve the alpha value of the foreground text.
      const unsigned int index = inputPixelSize * static_cast<unsigned int>(rows + columnIndex);

      // Build the index to the input buffer to retrieve the alpha value of the background shadow.
      unsigned int shadowIndex        = 0u;
      bool         isValidShadowIndex = false;
      if(isValidRowIndex)
      {
        const int xOffsetIndex = columnIndex - xOffset;
        isValidShadowIndex     = ((xOffsetIndex >= 0) && (xOffsetIndex < width));

        if(isValidShadowIndex)
        {
          shadowIndex = inputPixelSize * static_cast<unsigned int>(offsetRows + xOffsetIndex);
        }
      }

      // If the input buffer is an alpha mask, retrieve the values for the foreground text and the background shadow.
      // If not retrieve the color.
      float inputShadowOffsetAlphaValue = 1.f;
      float inputAlphaValue             = 1.f;
      if(isA8)
      {
        // Retrieve the alpha value for the shadow.
        inputShadowOffsetAlphaValue = isValidShadowIndex ? (static_cast<float>(*(inputPixelBuffer + shadowIndex)) / 255.f) : 0.f;

        // Retrieve the alpha value for the text.
        inputAlphaValue = static_cast<float>(*(inputPixelBuffer + index)) / 255.f;
      }
      else
      {
        // The input buffer is not an alpha mask. Retrieve the color.
        textColor[0u]               = TO_FLOAT * static_cast<float>(*(inputPixelBuffer + index + 0u));
        textColor[1u]               = TO_FLOAT * static_cast<float>(*(inputPixelBuffer + index + 1u));
        textColor[2u]               = TO_FLOAT * static_cast<float>(*(inputPixelBuffer + index + 2u));
        textColor[3u]               = TO_FLOAT * static_cast<float>(*(inputPixelBuffer + index + 3u));
        inputAlphaValue             = textColor[3u];
        inputShadowOffsetAlphaValue = isValidShadowIndex ? TO_FLOAT * static_cast<float>(*(inputPixelBuffer + shadowIndex + 3u)) : 0.f;
      }

      // Build the output color.
      float outputColor[4u];

      if(shadowParameters.blendShadow)
      {
        // Blend the shadow's color with the text's color on top
        const float textAlpha   = textColor[3u] * inputAlphaValue;
        const float shadowAlpha = shadowColor[3u] * inputShadowOffsetAlphaValue;

        // Blends the alpha.
        outputColor[3u]              = 1.f - ((1.f - textAlpha) * (1.f - shadowAlpha));
        const bool isOutputAlphaZero = outputColor[3u] < Dali::Math::MACHINE_EPSILON_1000;
        if(isOutputAlphaZero)
        {
          std::fill(outputColor, outputColor + 4u, 0.f);
        }
        else
        {
          // Blends the RGB components.
          float shadowComponent = 0.f;
          float textComponent   = 0.f;

          shadowComponent = shadowColor[0u] * inputShadowOffsetAlphaValue;
          textComponent   = textColor[0u] * inputAlphaValue;
          outputColor[0u] = (textComponent * textAlpha / outputColor[3u]) + (shadowComponent * shadowAlpha * (1.f - textAlpha) / outputColor[3u]);

          shadowComponent = shadowColor[1u] * inputShadowOffsetAlphaValue;
          textComponent   = textColor[1u] * inputAlphaValue;
          outputColor[1u] = (textComponent * textAlpha / outputColor[3u]) + (shadowComponent * shadowAlpha * (1.f - textAlpha) / outputColor[3u]);

          shadowComponent = shadowColor[2u] * inputShadowOffsetAlphaValue;
          textComponent   = textColor[2u] * inputAlphaValue;
          outputColor[2u] = (textComponent * textAlpha / outputColor[3u]) + (shadowComponent * shadowAlpha * (1.f - textAlpha) / outputColor[3u]);
        }
      }
      else
      {
        // No blending!!!
        std::fill(outputColor, outputColor + 4u, 0.f);

        const float textAlpha   = textColor[3u];
        const float shadowAlpha = shadowColor[3u] * inputShadowOffsetAlphaValue;

        // Write shadow first.
        if(shadowAlpha > Dali::Math::MACHINE_EPSILON_1000)
        {
          outputColor[0u] = shadowColor[0u] * inputShadowOffsetAlphaValue;
          outputColor[1u] = shadowColor[1u] * inputShadowOffsetAlphaValue;
          outputColor[2u] = shadowColor[2u] * inputShadowOffsetAlphaValue;
          outputColor[3u] = shadowAlpha;
        }

        // Write character on top.
        if(textAlpha > Dali::Math::MACHINE_EPSILON_1000)
        {
          outputColor[0u] = textColor[0u];
          outputColor[1u] = textColor[1u];
          outputColor[2u] = textColor[2u];
          outputColor[3u] = textAlpha;
        }
      }

      // Write the color into the output pixel buffer.
      const unsigned int outputIndex             = 4u * (rows + columnIndex);
      *(outputPixelBufferPtr + outputIndex + 0u) = static_cast<unsigned char>(TO_UCHAR * outputColor[0u]);
      *(outputPixelBufferPtr + outputIndex + 1u) = static_cast<unsigned char>(TO_UCHAR * outputColor[1u]);
      *(outputPixelBufferPtr + outputIndex + 2u) = static_cast<unsigned char>(TO_UCHAR * outputColor[2u]);
      *(outputPixelBufferPtr + outputIndex + 3u) = static_cast<unsigned char>(TO_UCHAR * outputColor[3u]);
    }
  }

  // Returns the pixel buffer.
  return outputPixelBuffer;
}

Devel::PixelBuffer ConvertToRgba8888(Devel::PixelBuffer pixelBuffer, const Vector4& color, bool multiplyByAlpha)
{
  if(Dali::Pixel::A8 != pixelBuffer.GetPixelFormat())
  {
    // Does nothing.
    return pixelBuffer;
  }

  const unsigned int width          = pixelBuffer.GetWidth();
  const unsigned int height         = pixelBuffer.GetHeight();
  Devel::PixelBuffer newPixelBuffer = Devel::PixelBuffer::New(width, height, Dali::Pixel::RGBA8888);

  unsigned char*             dstBuffer = newPixelBuffer.GetBuffer();
  const unsigned char* const srcBuffer = pixelBuffer.GetBuffer();

  const unsigned char r = static_cast<unsigned char>(TO_UCHAR * color.r);
  const unsigned char g = static_cast<unsigned char>(TO_UCHAR * color.g);
  const unsigned char b = static_cast<unsigned char>(TO_UCHAR * color.b);

  unsigned char dstColor[4];
  for(unsigned int j = 0u; j < height; ++j)
  {
    const unsigned int lineIndex = j * width;
    for(unsigned int i = 0u; i < width; ++i)
    {
      const unsigned int srcIndex = lineIndex + i;

      const float srcAlpha = static_cast<float>(*(srcBuffer + srcIndex));

      if(multiplyByAlpha)
      {
        dstColor[0u] = static_cast<unsigned char>(srcAlpha * color.r);
        dstColor[1u] = static_cast<unsigned char>(srcAlpha * color.g);
        dstColor[2u] = static_cast<unsigned char>(srcAlpha * color.b);
        dstColor[3u] = static_cast<unsigned char>(srcAlpha * color.a);
      }
      else
      {
        dstColor[0u] = r;
        dstColor[1u] = g;
        dstColor[2u] = b;
        dstColor[3u] = static_cast<unsigned char>(srcAlpha);
      }

      const unsigned int dstIndex = srcIndex * 4u;
      memcpy(dstBuffer + dstIndex, dstColor, 4u);
    }
  }

  return newPixelBuffer;
}

void UpdateBuffer(Devel::PixelBuffer src, Devel::PixelBuffer dst, unsigned int x, unsigned int y, bool blend)
{
  const Dali::Pixel::Format pixelFormat = dst.GetPixelFormat();
  if(src.GetPixelFormat() != pixelFormat)
  {
    DALI_LOG_ERROR("PixelBuffer::SetBuffer. The pixel format of the new data must be the same of the current pixel buffer.");
    return;
  }

  const unsigned int srcWidth  = src.GetWidth();
  const unsigned int srcHeight = src.GetHeight();
  const unsigned int dstWidth  = dst.GetWidth();
  const unsigned int dstHeight = dst.GetHeight();

  if((x > dstWidth) ||
     (y > dstHeight) ||
     (x + srcWidth > dstWidth) ||
     (y + srcHeight > dstHeight))
  {
    DALI_LOG_ERROR("PixelBuffer::SetBuffer. The source pixel buffer is out of the boundaries of the destination pixel buffer.");
    return;
  }

  const unsigned int bytesPerPixel = Dali::Pixel::GetBytesPerPixel(pixelFormat);
  // Ignore when pixelFormat is invalid or contain float
  if(bytesPerPixel == 0u || bytesPerPixel == 6u || bytesPerPixel == 12u)
  {
    return;
  }
  const unsigned int alphaIndex = bytesPerPixel - 1u;

  const unsigned char* const srcBuffer = src.GetBuffer();
  unsigned char*             dstBuffer = dst.GetBuffer();

  if(!blend)
  {
    const unsigned int currentLineSize = dstWidth * bytesPerPixel;
    const unsigned int newLineSize     = srcWidth * bytesPerPixel;
    unsigned char*     currentBuffer   = dstBuffer + (y * dstWidth + x) * bytesPerPixel;
    for(unsigned int j = 0u; j < srcHeight; ++j)
    {
      memcpy(currentBuffer + j * currentLineSize, srcBuffer + j * newLineSize, newLineSize);
    }
  }
  else
  {
    float outputColor[4u];

    // Blend the src pixel buffer with the dst pixel buffer as background.
    //
    //  fgColor, fgAlpha, bgColor, bgAlpha
    //
    //  alpha = 1 - ( 1 - fgAlpha ) * ( 1 - bgAlpha )
    //  color = ( fgColor * fgAlpha / alpha ) + ( bgColor * bgAlpha * ( 1 - fgAlpha ) / alpha )

    // Jump till the 'x,y' position
    const unsigned int dstWidthBytes = dstWidth * bytesPerPixel;
    dstBuffer += (y * dstWidthBytes + x * bytesPerPixel);

    for(unsigned int j = 0u; j < srcHeight; ++j)
    {
      const unsigned int srcLineIndex = j * srcWidth;
      for(unsigned int i = 0u; i < srcWidth; ++i)
      {
        const float srcAlpha = TO_FLOAT * static_cast<float>(*(srcBuffer + bytesPerPixel * (srcLineIndex + i) + alphaIndex));
        const float dstAlpha = TO_FLOAT * static_cast<float>(*(dstBuffer + i * bytesPerPixel + alphaIndex));

        // Blends the alpha channel.
        const float oneMinusSrcAlpha = 1.f - srcAlpha;
        outputColor[alphaIndex]      = 1.f - (oneMinusSrcAlpha * (1.f - dstAlpha));

        // Blends the RGB channels.
        const bool isOutputAlphaZero = outputColor[alphaIndex] < Dali::Math::MACHINE_EPSILON_1000;
        if(isOutputAlphaZero)
        {
          std::fill(outputColor, outputColor + bytesPerPixel, 0.f);
        }
        else
        {
          const float srcAlphaOverOutputAlpha                 = srcAlpha / outputColor[alphaIndex];                    // fgAlpha / alpha
          const float dstAlphaOneMinusSrcAlphaOverOutputAlpha = dstAlpha * oneMinusSrcAlpha / outputColor[alphaIndex]; // bgAlpha * ( 1 - fgAlpha ) / alpha
          for(unsigned int index = 0u; index < alphaIndex; ++index)
          {
            const float dstComponent = TO_FLOAT * static_cast<float>(*(dstBuffer + i * bytesPerPixel + index)) * dstAlpha;
            const float srcComponent = TO_FLOAT * static_cast<float>(*(srcBuffer + bytesPerPixel * (srcLineIndex + i) + index)) * srcAlpha;
            outputColor[index]       = (srcComponent * srcAlphaOverOutputAlpha) + (dstComponent * dstAlphaOneMinusSrcAlphaOverOutputAlpha);
          }
        }

        for(unsigned int index = 0u; index < bytesPerPixel; ++index)
        {
          *(dstBuffer + i * bytesPerPixel + index) = static_cast<unsigned char>(TO_UCHAR * outputColor[index]);
        }
      }

      dstBuffer += dstWidthBytes;
    }
  }
}

Dali::Property::Array RenderForLastIndex(RendererParameters& textParameters)
{
  Property::Array offsetValues;
  if(textParameters.text.empty())
  {
    return offsetValues;
  }
  FontClient fontClient = FontClient::Get();
  MetricsPtr metrics;
  metrics = Metrics::New(fontClient);

  Text::ModelPtr    textModel = Text::Model::New();
  InternalDataModel internalData(fontClient, metrics, textModel);

  TextAbstraction::TextRenderer::Parameters rendererParameters(textModel->mVisualModel->mGlyphs,
                                                               textModel->mVisualModel->mGlyphPositions,
                                                               textModel->mVisualModel->mColors,
                                                               textModel->mVisualModel->mColorIndices,
                                                               internalData.blendingMode,
                                                               internalData.isEmoji);

  rendererParameters.width  = textParameters.textWidth;
  rendererParameters.height = textParameters.textHeight;

  ////////////////////////////////////////////////////////////////////////////////
  // Process the markup string if the mark-up processor is enabled.
  ////////////////////////////////////////////////////////////////////////////////
  ShapeTextPreprocess(textParameters, rendererParameters, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyphs. Text shaping
  ////////////////////////////////////////////////////////////////////////////////
  Dali::Vector<Dali::Toolkit::DevelText::EmbeddedItemInfo> embeddedItemLayout;
  ShapeText(rendererParameters, embeddedItemLayout, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Retrieve the glyph's metrics.
  ////////////////////////////////////////////////////////////////////////////////
  metrics->GetGlyphMetrics(rendererParameters.glyphs.Begin(), internalData.numberOfGlyphs);

  ////////////////////////////////////////////////////////////////////////////////
  // Layout the text
  ////////////////////////////////////////////////////////////////////////////////
  int boundingBox           = textParameters.textHeight - (textParameters.padding.top + textParameters.padding.bottom);
  textParameters.textHeight = MAX_INT; // layout for the entire area.
  LayoutText(textParameters, rendererParameters, embeddedItemLayout, internalData);

  ////////////////////////////////////////////////////////////////////////////////
  // Calculation last character index
  ////////////////////////////////////////////////////////////////////////////////
  Vector<LineRun>& lines              = internalData.textModel->mVisualModel->mLines;
  unsigned int     numberOfLines      = lines.Count();
  int              numberOfCharacters = 0;
  float            penY               = 0.f;
  float            lineSize           = internalData.layoutEngine.GetDefaultLineSize();
  float            lineOffset         = 0.f;
  for(unsigned int index = 0u; index < numberOfLines; ++index)
  {
    const LineRun& line = *(lines.Begin() + index);
    numberOfCharacters += line.characterRun.numberOfCharacters;

    lineOffset = lineSize > 0.f ? lineSize : GetLineHeight(line);
    penY += lineOffset;
    if((penY + lineOffset) > boundingBox)
    {
      offsetValues.PushBack(numberOfCharacters);
      penY = 0.f;
    }
  }
  if(penY > 0.f)
  {
    // add remain character index
    offsetValues.PushBack(numberOfCharacters);
  }

  return offsetValues;
}

Dali::Property::Array GetLastCharacterIndex(RendererParameters& textParameters)
{
  Dali::Property::Array offsetValues = Toolkit::DevelText::RenderForLastIndex(textParameters);
  return offsetValues;
}

} // namespace DevelText

} // namespace Toolkit

} // namespace Dali
