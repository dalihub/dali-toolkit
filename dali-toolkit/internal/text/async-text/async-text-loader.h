#ifndef DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_H
#define DALI_TOOLKIT_TEXT_ASYNC_TEXT_LOADER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
    RENDER_FIXED_HEIGHT,
    RENDER_CONSTRAINT,
    COMPUTE_NATURAL_SIZE,
    COMPUTE_HEIGHT_FOR_WIDTH,
  };
  const char* const RequestTypeName[] =
  {
    "RENDER_FIXED_SIZE",
    "RENDER_FIXED_WIDTH",
    "RENDER_FIXED_HEIGHT",
    "RENDER_CONSTRAINT",
    "COMPUTE_NATURAL_SIZE",
    "COMPUTE_HEIGHT_FOR_WIDTH"
  };
} // namespace Async

struct AsyncTextParameters
{
  AsyncTextParameters()
  : text{},
    fontFamily{},
    textColor{Color::BLACK},
    underlineColor{Color::BLACK},
    strikethroughColor{Color::BLACK},
    shadowColor{Color::BLACK},
    outlineColor{Color::WHITE},
    backgroundColorWithCutout{Color::TRANSPARENT},
    shadowOffset{},
    outlineOffset{},
    padding{0u, 0u, 0u, 0u},
    variationsMap{},
    textFitArray{},
    embossDirection{},
    embossStrength{0.f},
    embossLightColor{Color::TRANSPARENT},
    embossShadowColor{Color::TRANSPARENT},
    fontSize{0.f},
    minLineSize{0.f},
    lineSpacing{0.f},
    relativeLineSize{1.f},
    characterSpacing{0.f},
    fontSizeScale{1.f},
    textWidth{0.f},
    textHeight{0.f},
    underlineHeight{0.f},
    dashedUnderlineWidth{2.f},
    dashedUnderlineGap{1.f},
    strikethroughHeight{0.f},
    shadowBlurRadius{0.f},
    outlineBlurRadius{0.f},
    textFitMinSize{10.f},
    textFitMaxSize{100.f},
    textFitStepSize{1.f},
    autoScrollLoopDelay{0.0f},
    renderScale{1.0f},
    renderScaleWidth{0.f},
    renderScaleHeight{0.f},
    maxTextureSize{0},
    autoScrollSpeed{1},
    autoScrollLoopCount{1},
    autoScrollGap{0},
    outlineWidth{0u},
    requestType{Async::RENDER_FIXED_SIZE},
    horizontalAlignment{Text::HorizontalAlignment::BEGIN},
    verticalAlignment{Text::VerticalAlignment::TOP},
    lineWrapMode{Text::LineWrap::WORD},
    underlineType{Text::Underline::SOLID},
    layoutDirection{Dali::LayoutDirection::LEFT_TO_RIGHT},
    verticalLineAlignment{DevelText::VerticalLineAlignment::TOP},
    layoutDirectionPolicy{DevelText::MatchLayoutDirection::INHERIT},
    ellipsisPosition{DevelText::EllipsisPosition::END},
    ellipsisMode{DevelText::Ellipsize::TRUNCATE},
    autoScrollStopMode{TextLabel::AutoScrollStopMode::FINISH_LOOP},
    fontWeight{FontWeight::NONE},
    fontWidth{FontWidth::NONE},
    fontSlant{FontSlant::NONE},
    manualRender{false},
    isMultiLine{false},
    ellipsis{true},
    enableMarkup{false},
    removeFrontInset{true},
    removeBackInset{true},
    isUnderlineEnabled{false},
    isStrikethroughEnabled{false},
    isTextFitEnabled{false},
    isTextFitArrayEnabled{false},
    isAutoScrollEnabled{false},
    isAutoScrollMaxTextureExceeded{false},
    cutout{false},
    backgroundWithCutoutEnabled{false},
    embossEnabled{false}
  {
  }

  ~AsyncTextParameters()
  {
  }

  std::string text;       ///< The text to be rendered encoded in utf8.
  std::string fontFamily; ///< The font's family.

  Vector4 textColor;                 ///< The default text's color. Default is white.
  Vector4 underlineColor;
  Vector4 strikethroughColor;
  Vector4 shadowColor;
  Vector4 outlineColor;
  Vector4 backgroundColorWithCutout; ///< Background color with cutout.

  Vector2 shadowOffset;
  Vector2 outlineOffset;

  Extents padding;       ///< The padding of the boundaries where the text is going to be laid-out.

  Property::Map variationsMap; ///< The map for variable fonts. it might be replaced by variable map run.
  std::vector<DevelTextLabel::FitOption> textFitArray;

  Vector2 embossDirection;
  float embossStrength;
  Vector4 embossLightColor;
  Vector4 embossShadowColor;

  float fontSize;             ///< The font's size (in points).
  float minLineSize;          ///< The line's minimum size (in pixels).
  float lineSpacing;          ///< The default extra space between lines in points. (in pixels).
  float relativeLineSize;     ///< The relative height of the line (a factor that will be multiplied by text height).
  float characterSpacing;     ///< The space between characters.
  float fontSizeScale;        ///< The font's size scale.
  float textWidth;            ///< The width in pixels of the boundaries where the text is going to be laid-out.
  float textHeight;           ///< The height in pixels of the boundaries where the text is going to be laid-out.
  float underlineHeight;
  float dashedUnderlineWidth;
  float dashedUnderlineGap;
  float strikethroughHeight;
  float shadowBlurRadius;
  float outlineBlurRadius;
  float textFitMinSize;
  float textFitMaxSize;
  float textFitStepSize;
  float autoScrollLoopDelay;
  float renderScale;          ///< The render scale.
  float renderScaleWidth;     ///< The requested original textWidth when using render scale.
  float renderScaleHeight;    ///< The requested original textHeight when using render scale.

  int maxTextureSize;      ///< The maximum size of texture.
  int autoScrollSpeed;     ///< auto scroll properties.
  int autoScrollLoopCount;
  int autoScrollGap;

  uint16_t outlineWidth; ///< The width of the outline, if it is greater than 1, it is enabled.

  Async::RequestType requestType;
  Text::HorizontalAlignment::Type        horizontalAlignment;   ///< The horizontal alignment: one of {BEGIN, CENTER, END}.
  Text::VerticalAlignment::Type          verticalAlignment;     ///< The vertical alignment: one of {TOP, CENTER, BOTTOM}.
  Text::LineWrap::Mode                   lineWrapMode;          ///< The line wrap mode: one of {WORD, CHARACTER, HYPHENATION, MIXED}.
  Text::Underline::Type                  underlineType;         ///< The type of underline: one of {SOLID, DASHED, DOUBLE}.
  Dali::LayoutDirection::Type            layoutDirection;       ///< The layout direction: one of {LEFT_TO_RIGHT, RIGHT_TO_LEFT}.
  DevelText::VerticalLineAlignment::Type verticalLineAlignment; ///< The vertical line alignment: one of {TOP, MIDDLE, BOTTOM}.
  DevelText::MatchLayoutDirection        layoutDirectionPolicy; ///< The policy used to set the text layout direction : one of {INHERIT, LOCALE, CONTENTS}.
  DevelText::EllipsisPosition::Type      ellipsisPosition;      ///< The position of the ellipsis glyph: one of {END, START, MIDDLE}.
  DevelText::Ellipsize::Mode             ellipsisMode;          ///< The mode of the ellipsis: one of {TRUNCATE, AUTO_SCROLL}.
  TextLabel::AutoScrollStopMode::Type    autoScrollStopMode;    ///< The auto scroll stop mode: one of {FINISH_LOOP, IMMEDIATE}.
  FontWeight                             fontWeight;            ///< The font's weight.
  FontWidth                              fontWidth;             ///< The font's width.
  FontSlant                              fontSlant;             ///< The font's slant.

  bool manualRender                   : 1; ///< Whether the manual rendered or not.
  bool isMultiLine                    : 1; ///< Whether the multi-line layout is enabled.
  bool ellipsis                       : 1; ///< Whether the ellipsis layout option is enabled.
  bool enableMarkup                   : 1; ///< Whether the mark-up processor is enabled.
  bool removeFrontInset               : 1; ///< Whether to ignore xBearing of the first glyph. Default is true.
  bool removeBackInset                : 1; ///< Whether to ignore advance of the last glyph. Default is true.
  bool isUnderlineEnabled             : 1; ///< Underline enabeld flag.
  bool isStrikethroughEnabled         : 1; ///< Strikethrough enabeld flag.
  bool isTextFitEnabled               : 1; ///< TextFit enabeld flag.
  bool isTextFitArrayEnabled          : 1; ///< TextFitArray enabeld flag.
  bool isAutoScrollEnabled            : 1; ///< Auto scroll enabeld flag.
  bool isAutoScrollMaxTextureExceeded : 1; ///< Whether the auto scroll texture size exceeds the maximum texture width.
  bool cutout                         : 1; ///< Cutout enabled flag.
  bool backgroundWithCutoutEnabled    : 1; ///< Background with cutout enabled flag.
  bool embossEnabled                  : 1; ///< Emboss enabled flag.
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
    size(),
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
    manualRendered(false),
    embossEnabled(false)
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
  Size               size;
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
  bool               embossEnabled         : 1;
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
   * @brief Sets the locale.
   *
   * @param[in] locale The locale.
   */
  void SetLocale(const std::string& locale);

  /**
   * @brief Sets a flag indicating that module's locale updating is needed.
   *
   * When the async text loader is available, update is processed on the main thread.
   *
   * @param[in] update Whether to update the locale or not.
   */
  void SetLocaleUpdateNeeded(bool update);

  /**
   * @brief Whether module's locale updating is needed.
   *
   * @return A flag that indicates whether the locale should be updated or not.
   */
  bool IsLocaleUpdateNeeded();

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
   * @brief Setup render scale.
   * Sets the control size to be rendered to fit the given scale.
   * The scaled rendering result cannot be exactly the same as the original.
   * However, we guarantee the ellipsis result.
   * If the original is ellipsised, the scaled result will always be ellipsised.
   * If the original is not ellipsised, the scaled result will not be ellipsised.
   * Occasionally, the scaled result exceeds the size of the control.
   * Since we need to ensure the size of the control, we slightly reduce the glyph's advance to adjust the total width to fit the control size.
   * While this may cause rendering quality issues at smaller point sizes, there is almost no noticeable difference at moderate sizes of 20pt or larger.
   *
   * @param[in] parameters All options required to compute size of text.
   * @param[out] cachedNaturalSize Whether the natural size has been calculated.
   *
   * @return The natural size of text.
   */
  Size SetupRenderScale(AsyncTextParameters& parameters, bool& cachedNaturalSize);

  /**
   * @brief Compute natural size of text.
   *
   * @param[in] parameters All options required to compute size of text.
   *
   * @return The natural size of text.
   */
  Size ComputeNaturalSize(AsyncTextParameters& parameters);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   * @param[in] useCachedNaturalSize Indicates whether to use the provided natural size or calculate it internally.
   * @param[in] naturalSize The natural size of the text to be used if useCachedNaturalSize is true.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderText(AsyncTextParameters& parameters, bool useCachedNaturalSize, const Size& naturalSize);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   * @param[in] useCachedNaturalSize Indicates whether to use the provided natural size or calculate it internally.
   * @param[in] naturalSize The natural size of the text to be used if useCachedNaturalSize is true.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderTextFit(AsyncTextParameters& parameters, bool useCachedNaturalSize, const Size& naturalSize);

  /**
   * @brief Renders text into a pixel buffer.
   *
   * @param[in] parameters All options required to render text.
   * @param[in] useCachedNaturalSize Indicates whether to use the provided natural size or calculate it internally.
   * @param[in] naturalSize The natural size of the text to be used if useCachedNaturalSize is true.
   *
   * @return An AsyncTextRenderInfo.
   */
  AsyncTextRenderInfo RenderAutoScroll(AsyncTextParameters& parameters, bool useCachedNaturalSize, const Size& naturalSize);

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
