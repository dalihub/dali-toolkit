#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/async-text/async-text-module.h>
#include <dali-toolkit/internal/text/text-model-interface.h>
#include <dali-toolkit/public-api/text/text-enumerations.h>

// EXTERNAL INCLUDES
#include <dali/public-api/actors/actor-enumerations.h>
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/rendering/visual-renderer.h>

// DEVEL INCLUDES
#include <dali-toolkit/devel-api/controls/text-controls/text-label-devel.h>

namespace Dali
{
namespace Toolkit
{
namespace Text
{
namespace Internal DALI_INTERNAL
{
class AsyncTextLoader;

} // namespace DALI_INTERNAL

namespace Async
{
  enum RequestType
  {
    RENDER_FIXED_SIZE,
    RENDER_FIXED_WIDTH,
    RENDER_CONSTRAINT,
    COMPUTE_NATURAL_SIZE,
    COMPUTE_HEIGHT_FOR_WIDTH,
  };
  const char* const RequestTypeName[] =
  {
    "RENDER_FIXED_SIZE",
    "RENDER_FIXED_WIDTH",
    "RENDER_CONSTRAINT",
    "COMPUTE_NATURAL_SIZE",
    "COMPUTE_HEIGHT_FOR_WIDTH"
  };
} // namespace Async

struct AsyncTextParameters
{
  AsyncTextParameters()
  : requestType{Async::RENDER_FIXED_SIZE},
    manualRender{false},
    maxTextureSize{0},
    text{},
    fontSize{0.f},
    textColor{Color::BLACK},
    fontFamily{},
    fontWeight{FontWeight::NONE},
    fontWidth{FontWidth::NONE},
    fontSlant{FontSlant::NONE},
    isMultiLine{false},
    ellipsis{true},
    enableMarkup{false},
    removeFrontInset{true},
    removeBackInset{true},
    minLineSize{0.f},
    lineSpacing{0.f},
    relativeLineSize{1.f},
    characterSpacing{0.f},
    fontSizeScale{1.f},
    textWidth{0.f},
    textHeight{0.f},
    padding{0u, 0u, 0u, 0u},
    horizontalAlignment{Text::HorizontalAlignment::BEGIN},
    verticalAlignment{Text::VerticalAlignment::TOP},
    verticalLineAlignment{DevelText::VerticalLineAlignment::TOP},
    lineWrapMode{Text::LineWrap::WORD},
    layoutDirection{Dali::LayoutDirection::LEFT_TO_RIGHT},
    layoutDirectionPolicy{DevelText::MatchLayoutDirection::INHERIT},
    ellipsisPosition{DevelText::EllipsisPosition::END},
    isUnderlineEnabled{false},
    underlineType{Text::Underline::SOLID},
    underlineColor{Color::BLACK},
    underlineHeight{0.f},
    dashedUnderlineWidth{2.f},
    dashedUnderlineGap{1.f},
    isStrikethroughEnabled{false},
    strikethroughColor{Color::BLACK},
    strikethroughHeight{0.f},
    shadowBlurRadius{0.f},
    shadowColor{Color::BLACK},
    shadowOffset{},
    outlineWidth{0u},
    outlineColor{Color::WHITE},
    outlineBlurRadius{0.f},
    outlineOffset{},
    isTextFitEnabled{false},
    textFitMinSize{10.f},
    textFitMaxSize{100.f},
    textFitStepSize{1.f},
    isTextFitArrayEnabled{false},
    textFitArray{},
    isAutoScrollEnabled{false},
    autoScrollStopMode{TextLabel::AutoScrollStopMode::FINISH_LOOP},
    autoScrollSpeed{1},
    autoScrollLoopCount{1},
    autoScrollLoopDelay{0.0f},
    autoScrollGap{0},
    isAutoScrollMaxTextureExceeded{false},
    cutout{false},
    backgroundWithCutoutEnabled{false},
    backgroundColorWithCutout{Color::TRANSPARENT}
  {
  }

  ~AsyncTextParameters()
  {
  }

  Async::RequestType requestType;
  bool               manualRender : 1;

  int         maxTextureSize; ///< The maximum size of texture.
  std::string text;           ///< The text to be rendered encoded in utf8.
  float       fontSize;       ///< The font's size (in points).
  Vector4     textColor;      ///< The default text's color. Default is white.

  std::string fontFamily;     ///< The font's family.
  FontWeight  fontWeight;     ///< The font's weight.
  FontWidth   fontWidth;      ///< The font's width.
  FontSlant   fontSlant;      ///< The font's slant.

  bool isMultiLine      : 1; ///< Whether the multi-line layout is enabled.
  bool ellipsis         : 1; ///< Whether the ellipsis layout option is enabled.
  bool enableMarkup     : 1; ///< Whether the mark-up processor is enabled.
  bool removeFrontInset : 1; ///< Whether to ignore xBearing of the first glyph. Default is true.
  bool removeBackInset  : 1; ///< Whether to ignore advance of the last glyph. Default is true.

  float minLineSize;      ///< The line's minimum size (in pixels).
  float lineSpacing;      ///< The default extra space between lines in points. (in pixels).
  float relativeLineSize; ///< The relative height of the line (a factor that will be multiplied by text height).
  float characterSpacing; ///< The space between characters.
  float fontSizeScale;    ///< The font's size scale.

  float textWidth;        ///< The width in pixels of the boundaries where the text is going to be laid-out.
  float textHeight;       ///< The height in pixels of the boundaries where the text is going to be laid-out.
  Extents  padding;       ///< The padding of the boundaries where the text is going to be laid-out.

  Text::HorizontalAlignment::Type        horizontalAlignment;   ///< The horizontal alignment: one of {BEGIN, CENTER, END}.
  Text::VerticalAlignment::Type          verticalAlignment;     ///< The vertical alignment: one of {TOP, CENTER, BOTTOM}.
  DevelText::VerticalLineAlignment::Type verticalLineAlignment; ///< The vertical line alignment: one of {TOP, MIDDLE, BOTTOM}.
  Text::LineWrap::Mode                   lineWrapMode;          ///< The line wrap mode: one of {WORD, CHARACTER, HYPHENATION, MIXED}.
  Dali::LayoutDirection::Type            layoutDirection;       ///< The layout direction: one of {LEFT_TO_RIGHT, RIGHT_TO_LEFT}.
  DevelText::MatchLayoutDirection        layoutDirectionPolicy; ///< The policy used to set the text layout direction : one of {INHERIT, LOCALE, CONTENTS}.
  DevelText::EllipsisPosition::Type      ellipsisPosition;      ///< The position of the ellipsis glyph: one of {END, START, MIDDLE}.

  bool                  isUnderlineEnabled : 1;     ///< Underline properties
  Text::Underline::Type underlineType;
  Vector4               underlineColor;
  float                 underlineHeight;
  float                 dashedUnderlineWidth;
  float                 dashedUnderlineGap;

  bool                  isStrikethroughEnabled : 1; ///< Strikethrough properties
  Vector4               strikethroughColor;
  float                 strikethroughHeight;

  float                 shadowBlurRadius;           ///< Shadow properties
  Vector4               shadowColor;
  Vector2               shadowOffset;

  uint16_t              outlineWidth;               ///< Outline properties
  Vector4               outlineColor;
  float                 outlineBlurRadius;
  Vector2               outlineOffset;

  bool                  isTextFitEnabled : 1;       ///< TextFit
  float                 textFitMinSize;
  float                 textFitMaxSize;
  float                 textFitStepSize;

  bool                                   isTextFitArrayEnabled : 1; ///< TextFitArray
  std::vector<DevelTextLabel::FitOption> textFitArray;

  bool                                   isAutoScrollEnabled : 1;   ///< Auto scroll
  TextLabel::AutoScrollStopMode::Type    autoScrollStopMode;
  int                                    autoScrollSpeed;
  int                                    autoScrollLoopCount;
  float                                  autoScrollLoopDelay;
  int                                    autoScrollGap;
  bool                                   isAutoScrollMaxTextureExceeded : 1;

  bool cutout                      : 1; ///< Cutout enabled flag
  bool backgroundWithCutoutEnabled : 1; ///< Background with cutout enabled flag.
  Vector4 backgroundColorWithCutout;    ///< Background color with cutout.
};

struct AsyncTextRenderInfo
{
  AsyncTextRenderInfo()
  : requestType(Async::RENDER_FIXED_SIZE),
    textPixelData(),
    stylePixelData(),
    overlayStylePixelData(),
    maskPixelData(),
    autoScrollPixelData(),
    width(0u),
    height(0u),
    controlSize(),
    renderedSize(),
    lineCount(0),
    autoScrollWrapGap(0.f),
    hasMultipleTextColors(false),
    containsColorGlyph(false),
    styleEnabled(false),
    isOverlayStyle(false),
    isTextDirectionRTL(false),
    isCutout(false),
    manualRendered(false)
  {
  }

  ~AsyncTextRenderInfo()
  {
  }
  Async::RequestType requestType;
  PixelData          textPixelData;
  PixelData          stylePixelData;
  PixelData          overlayStylePixelData;
  PixelData          maskPixelData;
  PixelData          autoScrollPixelData;
  uint32_t           width;
  uint32_t           height;
  Size               controlSize;
  Size               renderedSize;
  int                lineCount;
  float              autoScrollWrapGap;
  bool               hasMultipleTextColors : 1;
  bool               containsColorGlyph    : 1;
  bool               styleEnabled          : 1;
  bool               isOverlayStyle        : 1;
  bool               isTextDirectionRTL    : 1;
  bool               isCutout              : 1;
  bool               manualRendered        : 1;
};

/**
 * AsyncTextLoader
 *
 */
class AsyncTextLoader : public BaseHandle
{
public:
  /**
   * @brief Create an uninitialized AsyncTextLoader handle.
   *
   */
  AsyncTextLoader();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AsyncTextLoader();

  /**
   * @brief This constructor is used by AsyncTextLoader::Get().
   *
   * @param[in] implementation a pointer to the internal async text loader object.
   */
  explicit DALI_INTERNAL AsyncTextLoader(Internal::AsyncTextLoader* implementation);

  /**
   * @brief Create a handle to the new AsyncTextLoader instance.
   *
   * @return A handle to the AsyncTextLoader.
   */
  static AsyncTextLoader New();

  /**
   * @brief Clear the cache of the async text module.
   */
  void ClearModule();

  /**
   * @brief Sets custom fonts directories.
   *
   * @param[in] customFontDirectories List of the custom font paths.
   */
  void SetCustomFontDirectories(const TextAbstraction::FontPathList& customFontDirectories);

  /**
   * @brief Sets a flag indicating that module's cache clearing is needed.
   *
   * When the async text loader is available, clear is processed on the main thread.
   *
   * @param[in] clear Whether to clear the cache or not.
   */
  void SetModuleClearNeeded(bool clear);

  /**
   * @brief Whether module's cache clearing is needed.
   *
   * @return A flag that indicates whether the cache should be cleared or not.
   */
  bool IsModuleClearNeeded();

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderText(AsyncTextParameters& parameters);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderTextFit(AsyncTextParameters& parameters);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderAutoScroll(AsyncTextParameters& parameters);

  /**
   * @brief Gets the natural size of text.
   *
   * @param[in] parameters All options required to compute text.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo GetNaturalSize(AsyncTextParameters& parameters);

  /**
   * @brief Gets the height for width of text.
   *
   * @param[in] parameters All options required to compute text.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo GetHeightForWidth(AsyncTextParameters& parameters);

public:
  // Default copy and move operator
  AsyncTextLoader(const AsyncTextLoader& rhs) = default;
  AsyncTextLoader(AsyncTextLoader&& rhs)      = default;
  AsyncTextLoader& operator=(const AsyncTextLoader& rhs) = default;
  AsyncTextLoader& operator=(AsyncTextLoader&& rhs) = default;
};

} // namespace Text

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_H
