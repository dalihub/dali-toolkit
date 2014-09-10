#ifndef __DALI_TOOLKIT_INTERNAL_RELAYOUT_UTILITIES_H__
#define __DALI_TOOLKIT_INTERNAL_RELAYOUT_UTILITIES_H__

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

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/text-view/text-view-impl.h>
#include <dali-toolkit/internal/controls/text-view/text-view-processor-types.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

//Forward declarations.
class TextActorCache;

namespace TextViewRelayout
{

extern const float MINIMUM_FADE_BOUNDARY; // When the fade boundary is the same as the text-view boundary, this constant reduces it in order to avoid a zero division.

/**
 * Define the type of line wrap.
 */
enum HorizontalWrapType
{
  WrapByCharacter,    ///< Wrap a line per character (It may split a word in two).
  WrapByWord,         ///< Wrap a line by word.
  WrapByWordAndSplit, ///< Wrap the line by word and split a word if it doesn't fit.
  WrapByLine,         ///< Wrap the line when a \n is found.
  WrapByLineAndSplit  ///< Wrap the line when a \n is found and split if it doesn't fit.
};

/**
 * Different types of visibility tests (text-actor - text-view).
 */
enum VisibilityTestType
{
  FULLY_VISIBLE,           ///< The text-actor is completely inside the text-view.
  FULLY_VISIBLE_WIDTH,     ///< The text-actor is completely between the right and the left boundaries of the text-view.
  FULLY_VISIBLE_HEIGHT,    ///< The text-actor is completely between the top and the bottom boundaries of the text-view.
  PARTIALLY_VISIBLE,       ///< The text-actor is partially inside the text-view.
  PARTIALLY_VISIBLE_WIDTH, ///< The text-actor is partially inside the width of the text-view. It may be completely above and bellow the top and bottom boundaries of the text-view.
  PARTIALLY_VISIBLE_HEIGHT ///< The text-actor is partially inside the height of the text-view. It may be completely on the left and on the right the left and right boundaries of the text-view.
};

/**
 * Temporary parameters used in the relayout process.
 */
struct RelayoutParameters
{
  /**
   * Default constructor.
   *
   * Initializes all members to their defaults.
   */
  RelayoutParameters();

  /**
   * Empty destructor.
   *
   * @note Added to increase coverage.
   */
  ~RelayoutParameters();

  Vector3                            mPositionOffset;           ///< Offset (position.x + size.width, position.y, position.z) of the previous text-actor.
  Size                               mLineSize;                 ///< Current line's size.
  Size                               mWordSize;                 ///< Current word's size.
  Size                               mCharacterSize;            ///< Current character's size.
  TextViewProcessor::TextInfoIndices mIndices;                  ///< Current indices to line, word and character.
  std::size_t                        mCharacterGlobalIndex;     ///< Index to a single character within the whole text.
  bool                               mIsFirstCharacter:1;       ///< Whether is the first character of the whole text.
  bool                               mIsFirstCharacterOfWord:1; ///< Whether is the first character of the word.
  bool                               mIsNewLine:1;              ///< Whether the current character is the first character of a new line.
  bool                               mIsNewLineCharacter:1;     ///< Whether the current character is a new line character.
  bool                               mIsWhiteSpace:1;           ///< Whether the current character is a white space.
  bool                               mIsVisible:1;              ///< Whether the current character is visible.
};

/**
 * Parameters used to calculate the gradient of text-actors when fading is enabled.
 */
struct FadeParameters
{
  /**
   * Default constructor.
   *
   * Initializes all members to their defaults.
   */
  FadeParameters();

  /**
   * Empty destructor.
   *
   * @note Added to increase coverage.
   */
  ~FadeParameters();

  float   mRightFadeBoundary;         ///< Distance from the right edge of the text-view to the right edge of the fade boundary.
  float   mRightFadeThreshold;        ///< Point from where fade out starts (by right edge).
  float   mRightFadeBoundaryOffset;   ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  float   mRightFadeThresholdOffset;  ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  Vector2 mRightAlphaCoeficients;     ///< The fade out rect coeficients for the right side of the text-view.
  float   mLeftFadeBoundary;          ///< Distance from the left edge of the text-view to the left edge of the fade boundary.
  float   mLeftFadeThreshold;         ///< Point from where fade out starts (by left edge).
  float   mLeftFadeBoundaryOffset;    ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  float   mLeftFadeThresholdOffset;   ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  Vector2 mLeftAlphaCoeficients;      ///< The fade out rect coeficients for the left side of the text-view.
  float   mTopFadeBoundary;           ///< Distance from the top edge of the text-view to the top edge of the fade boundary.
  float   mTopFadeThreshold;          ///< Point from where fade out starts (by top edge).
  float   mTopFadeBoundaryOffset;     ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  float   mTopFadeThresholdOffset;    ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  Vector2 mTopAlphaCoeficients;       ///< The fade out rect coeficients for the top side of the text-view.
  float   mBottomFadeBoundary;        ///< Distance from the bottom edge of the text-view to the bottom edge of the fade boundary.
  float   mBottomFadeThreshold;       ///< Point from where fade out starts (by bottom edge).
  float   mBottomFadeBoundaryOffset;  ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  float   mBottomFadeThresholdOffset; ///< Same as above plus an offset if the value is zero. Used to avoid a zero division.
  Vector2 mBottomAlphaCoeficients;    ///< The fade out rect coeficients for the bottom side of the text-view.
  bool    mIsPartiallyVisible:1;      ///< Whether the current character is partially visible.
};

/**
 * Parameters used to calculate the ellipsize.
 */
struct EllipsizeParameters
{
  /**
   * Default constructor.
   *
   * Initializes all members to their defaults.
   */
  EllipsizeParameters();

  /**
   * Empty destructor.
   *
   * @note Added to increase coverage.
   */
  ~EllipsizeParameters();

  Vector3     mPosition;                       ///< Position of the first character of the ellipsize text.
  float       mLineDescender;                  ///< Distance from the base line to the bottom.
  float       mLineWidth;                      ///< Current laid out line's width.
  Size        mEllipsizeBoundary;              ///< Where to start to ellipsize a line.
  std::size_t mFirstIndex;                     ///< Global index within the whole text of the first character of the laid out line.
  std::size_t mLastIndex;                      ///< Global index within the whole text of the last character of the laid out line.
  bool        mEllipsizeLine:1;                ///< Whether current line must be ellipsized.
  bool        mIsLineWidthFullyVisible:1;      ///< Whether current line fits in text-view's width.
  bool        mIsLineHeightFullyVisible:1;     ///< Whether current line fits in text-view's height.
  bool        mIsNextLineFullyVisibleHeight:1; ///< Whether next line fits in text-view's height.
  bool        mCreateEllipsizedTextActors:1;   ///< Whether to create text-actors for the ellipsized text.
  bool        mLineFits:1;                     ///< Whether the current line fits in the boundary of the text-view.
  bool        mWordFits:1;                     ///< Whether the current word fits in the boundary of the text-view.
};

/**
 * Stores underline info for a group of consecutive characters in the same laid out line.
 */
struct UnderlineInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their defaults.
   */
  UnderlineInfo();

  /**
   * Empty destructor.
   *
   * @note Added to increase coverage.
   */
  ~UnderlineInfo();

  float mMaxHeight;    ///< The maximum height.
  float mMaxThickness; ///< The maximum underline's thickness.
  float mPosition;     ///< The underline's position of the character with the maximum underline's thickness.
};

/**
 * Stores underline info for each group of consecutive underlined characters.
 * It also stores some status used when traversing the whole text.
 */
struct TextUnderlineStatus
{
  /**
   * Default constructor.
   *
   * Initializes each member to its default.
   */
  TextUnderlineStatus();

  /**
   * Empty destructor.
   *
   * @note Added to increase coverage.
   */
  ~TextUnderlineStatus();

  std::vector<UnderlineInfo> mUnderlineInfo;            ///< Underline info for each group of consecutive underlined characters.
  std::size_t                mCharacterGlobalIndex;     ///< Global index (within the whole text) to current character.
  std::size_t                mLineGlobalIndex;          ///< Index to current laid out line. It takes into account the current layout configuration (is not the number of \n)
  bool                       mCurrentUnderlineStatus:1; ///< Whether current character is underlined.
};

/**
 * Stores layout information of the piece of a line.
 */
struct SubLineLayoutInfo
{
  /**
   * Default constructor.
   *
   * Initializes each member to its default.
   */
  SubLineLayoutInfo();

  /**
   * Empty destructor.
   *
   * @note Added to increase coverage.
   */
  ~SubLineLayoutInfo();

  float mLineLength;    ///< The length of the portion of the line which fits on the text-view width.
  float mMaxCharHeight; ///< The maximum height of all characters of the portion of line which fits on the text-view width.
  float mMaxAscender;   ///< The maximum ascender of all characters of the portion of line which fits on the text-view width.
};

/**
 * Calculates the layout info of the portion of the line which fits on the text-view width.
 *
 * @param[in] parentWidth Text-view width
 * @param[in] indices Indices to the word and character.
 * @param[in] lineLayoutInfo Layout info for the line.
 * @param[in] splitPolicy Whether a line is wraped by word, by character or by word and character.
 * @param[in] shrinkFactor Shrink factor used.
 * @param[out] layoutInfo Layout information of the part of the line which fits in the text-view width.
 */
void CalculateSubLineLayout( float parentWidth,
                             const TextViewProcessor::TextInfoIndices& indices,
                             const TextViewProcessor::LineLayoutInfo& lineLayoutInfo,
                             HorizontalWrapType splitPolicy,
                             float shrinkFactor,
                             SubLineLayoutInfo& layoutInfo );

/**
 * Calculates the \e x offset position for the whole text.
 *
 * @param[in] horizontalTextAlignment The horizontal alignment type.
 * @param[in] parentWidth The parent's width.
 * @param[in] wholeTextWidth The whole text's width.
 * @return The \e x position offset.
 */
float CalculateXoffset( Toolkit::Alignment::Type horizontalTextAlignment, float parentWidth, float wholeTextWidth );

/**
 * Calculates the \e y offset position for the whole text.
 *
 * @param[in] verticalTextAlignment The vertical alignment type.
 * @param[in] parentHeight The parent's height.
 * @param[in] wholeTextHeight The whole text's height.
 * @return The \e y position offset.
 */
float CalculateYoffset( Toolkit::Alignment::Type verticalTextAlignment, float parentHeight, float wholeTextHeight );

/**
 * Calculates the \e x offset position for one line.
 *
 * @param[in] justification The line justification type.
 * @param[in] wholeTextWidth The whole text's width.
 * @param[in] lineLength The line's length.
 * @return The \e x position offset.
 *
 */
float CalculateJustificationOffset( Toolkit::TextView::LineJustification justification, float wholeTextWidth, float lineLength );

/**
 * Whether text-actor is visible for Fade and Ellipsize exceed policies.
 *
 * It does different visibility tests according the given parameter \e type. @see VisibilityTestType.
 *
 * @param[in] position Position of the character.
 * @param[in] size Size of the character.
 * @param[in] parentSize Parent's size.
 * @param[in] type One of FULLY_VISIBLE, PARTIALLY_VISIBLE, PARTIALLY_VISIBLE_WIDTH or PARTIALLI_VISIBLE_HEIGHT.
 * @return \e true if is visible.
 */
bool IsVisible( const Vector3& position, const Size& size, const Size& parentSize, VisibilityTestType type );

/**
 * Calculates the coeficients of the rect equation for the two given points.
 *
 * @param[in] p0 A point of the rect.
 * @param[in] p1 Another point of the rect.
 *
 * @return The \e gradient is returned in the \e x member and the \e constant \e term is returned in the \e y value.
 */
Vector2 CalculateRectParameters( const Vector2& p0, const Vector2& p1 );

/**
 * Aligns the whole text within the text-view.
 *
 * @param[in] layoutParameters The layout parameters.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void UpdateAlignment( const TextView::LayoutParameters& layoutParameters,
                      TextView::RelayoutData& relayoutData );

/**
 * Calculates the bearing for the given character.
 *
 * @param[in,out] characterLayoutInfo Character layout info.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void CalculateBearing( TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                       TextView::RelayoutData& relayoutData );

/**
 * Updates the character's layout info table.
 *
 * This table is used to pass the size, the position and other layout info to other controls/actors.
 *
 * @param[in,out] minMaxXY The boundary box of the whole text.
 * @param[in,out] wordLayoutInfo Word layout info.
 * @param[in,out] characterLayoutInfo Character layout info.
 * @param[in,out] relayoutParameters Temporary layout parameters.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void UpdateLayoutInfoTable( Vector4& minMaxXY,
                            TextViewProcessor::WordLayoutInfo& wordLayoutInfo,
                            TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                            RelayoutParameters& relayoutParameters,
                            TextView::RelayoutData& relayoutData );

/**
 * Calculates the text-actor visibility and fade parameters.
 *
 * @param[in] layoutParameters The layout parameters.
 * @param[in] characterLayoutInfo Character layout info.
 * @param[in,out] relayoutParameters Temporary layout parameters.
 * @param[in,out] fadeParameters Temporary fade parameters.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void CalculateVisibilityForFade( const Internal::TextView::LayoutParameters& layoutParameters,
                                 TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                                 RelayoutParameters& relayoutParameters,
                                 FadeParameters& fadeParameters,
                                 TextView::RelayoutData& relayoutData );

/**
 * Calculates the text-actor visibility and creates eliipsize text-actors.
 *
 * @param[in] layoutParameters The layout parameters.
 * @param[in] characterLayoutInfo Character layout info.
 * @param[in,out] ellipsizeParameters Temporary ellipsize parameters.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void CalculateVisibilityForEllipsize( const Internal::TextView::LayoutParameters& layoutParameters,
                                      TextViewProcessor::CharacterLayoutInfo& characterLayoutInfo,
                                      EllipsizeParameters& ellipsizeParameters,
                                      TextView::RelayoutData& relayoutData );

/**
 * Creates the actors needed for the ellipsized text.
 *
 * @param[in,out] ellipsizeParameters Temporary ellipsize parameters.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void CreateEllipsizeTextActor( const EllipsizeParameters& ellipsizeParameters,
                               TextView::RelayoutData& relayoutData );

/**
 *
 */
void EllipsizeLine( const TextView::LayoutParameters& layoutParameters,
                    EllipsizeParameters& ellipsizeParameters,
                    TextView::RelayoutData& relayoutData );

/**
 * Traverse all text data structure setting its visibility to true.
 *
 * @param[in,out] relayoutData The text-view's data structures.
 */
void SetTextVisible( TextView::RelayoutData& relayoutData );

/**
 * Calculates the visibility and fade parameters.
 *
 * @param[in] layoutParameters The layout parameters.
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in,out] relayoutData The text-view's data structures.
 */
void UpdateVisibilityForFade( const TextView::LayoutParameters& layoutParameters,
                              const TextView::VisualParameters& visualParameters,
                              TextView::RelayoutData& relayoutData );
/**
 * Calculates the visibility for text ellipsize.
 *
 * @param[in] layoutParameters The layout parameters.
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in,out] relayoutData The text-view's data structures.
 */
void UpdateVisibilityForEllipsize( const TextView::LayoutParameters& layoutParameters,
                                   const TextView::VisualParameters& visualParameters,
                                   TextView::RelayoutData& relayoutData );
/**
 * Calculates the visibility and fade parameters.
 *
 * @param[in] layoutParameters The layout parameters.
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in,out] relayoutData The text-view's data structures.
 */
void UpdateVisibility( const TextView::LayoutParameters& layoutParameters,
                       const TextView::VisualParameters& visualParameters,
                       TextView::RelayoutData& relayoutData );

/**
 * Traverse all text updating text-actor handles with new size, position, ...
 *
 * @param[in] visualParameters Some visual parameters (fade, sort modifier and blending).
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info.
 */
void UpdateTextActorInfo( const TextView::VisualParameters& visualParameters,
                          TextView::RelayoutData& relayoutData );

/**
 * Traverses the whole text and for each piece of underlined text,
 * it calculates the maximum thickness and the position of that particular piece of underlined text.
 *
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 */
void CalculateUnderlineInfo( TextView::RelayoutData& relayoutData );

/**
 * Traverses the whole text and for each piece of underlined text,
 * it sets the previously calculated maximum thickness and the position of that particular piece of underlined text.
 *
 * @param[in,out] relayoutData Natural size (metrics), layout, text-actor info and conversion from visual to logical order and vice versa (for RTL text).
 */
void SetUnderlineInfo( TextView::RelayoutData& relayoutData );

/**
 * Remove renderable-actor from the text-view.
 *
 * @param[in,out] textView The text-view.
 * @param[in] glyphActors renderable-actors to be removed from the text-view.
 */
void RemoveGlyphActors( Actor textView,
                        const std::vector<RenderableActor>& glyphActors );

/**
 * Inserts the text-actors into the text-view and/or the text-actor's list.
 *
 * @param[in] relayoutOperationMask Whether the text-actors should be added into the text-view, the list of text-actors or in both.
 * @param[in,out] textView The text-view.
 * @param[in,out] relayoutData The text-view's data structures.
 */
void InsertToTextView( TextView::RelayoutOperationMask relayoutOperationMask,
                       Actor textView,
                       TextView::RelayoutData& relayoutData );

/**
 * Retrieves a new glyph-actor from the cache of text-actors or creating a new one if it's empty.
 *
 * @param[in] text The text-actor's text.
 * @param[in] style The text-actor's style.
 * @param[in] cache The cache of text-actors.
 *
 * @return a RenderableActor with the text's glyph.
 */
RenderableActor CreateGlyphActor( const Text& text, const TextStyle& style, TextActorCache& cache );

} // namespace TextViewRelayout

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_RELAYOUT_UTILITIES_H__
