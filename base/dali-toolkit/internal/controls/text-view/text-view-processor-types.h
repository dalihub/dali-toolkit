#ifndef __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_TYPES_H__
#define __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_TYPES_H__

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
#include <dali/public-api/actors/renderable-actor.h>
#include <dali-toolkit/public-api/markup-processor/markup-processor.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextViewProcessor
{

/**
 * Whether the text is a new paragraph character '\n', a white space or normal text.
 */
enum TextSeparatorType
{
  ParagraphSeparator,
  WordSeparator,
  NoSeparator
};

/**
 * Whether to clear the text of the text-actors when text is removed.
 */
enum TextOperationOnRemove
{
  CLEAR_TEXT,
  KEEP_TEXT
};


/**
 * Stores text info indices.
 */
struct TextInfoIndices
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  TextInfoIndices();

  /**
   * Constructor.
   */
  TextInfoIndices( std::size_t paragraphIndex,
                   std::size_t wordIndex,
                   std::size_t characterIndex );

  /**
   * Equality operator.
   * @param [in] indices The text-info indices to be compared.
   *
   * @return \e true if both indices are equal.
   */
  bool operator==( const TextInfoIndices& indices ) const;

  std::size_t mParagraphIndex;          ///< The paragraph index within the text.
  std::size_t mWordIndex;               ///< The word index within the paragraph.
  std::size_t mCharacterIndex;          ///< The character index within the word.
};

/**
 * Layout information for a character.
 * It stores the position, size and ascender of its respective text-actor.
 */
struct CharacterLayoutInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  CharacterLayoutInfo();

  /**
   * Default destructor.
   *
   * Deletes the gradient info.
   */
  ~CharacterLayoutInfo();

  /**
   * Copy constructor.
   */
  CharacterLayoutInfo( const CharacterLayoutInfo& character );

  /**
   * Assignment operator.
   */
  CharacterLayoutInfo& operator=( const CharacterLayoutInfo& character );

  // Natural size (metrics) of the glyph.
  float       mHeight;             ///< Natural height of the character.
  float       mAdvance;            ///< Natural horizontal distance from origin of current character and the next one.
  float       mBearing;            ///< Natural vertical distance from the baseline to the top of the glyph's bbox.

  // Size of the text-actor (may be modified by a scale factor).
  Vector3     mPosition;           ///< Position within the text-view
  Vector2     mOffset;             ///< Alignment and justification offset.
  Size        mSize;               ///< Size of this character.
  float       mAscender;           ///< Distance from the base line to the top of the line.
  float       mUnderlineThickness; ///< The underline's thickness.
  float       mUnderlinePosition;  ///< The underline's position.

  RenderableActor             mGlyphActor;     ///< Handle to a text-actor.
  MarkupProcessor::StyledText mStyledText;     ///< Stores the text and its style.
  float                       mColorAlpha;     ///< Alpha component for the initial text color when text is faded.
  Vector4                     mGradientColor;  ///< Gradient color.
  Vector2                     mStartPoint;     ///< Gradient start point.
  Vector2                     mEndPoint;       ///< Gradient end point.

  bool                        mIsVisible:1;    ///< Whether the text-actor is visible.
  bool                        mSetText:1;      ///< Whether a new text needs to be set in the text-actor.
  bool                        mSetStyle:1;     ///< Whether a new style needs to be set in the text-actor.
  bool                        mIsColorGlyph:1; ///< Whether this character is an emoticon.
};
typedef std::vector<CharacterLayoutInfo> CharacterLayoutInfoContainer;

/**
 * Layout information for a word.
 */
struct WordLayoutInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  WordLayoutInfo();

  /**
   * Default destructor.
   *
   * Clears all characters.
   */
  ~WordLayoutInfo();

  /**
   * Copy constructor.
   */
  WordLayoutInfo( const WordLayoutInfo& word );

  /**
   * Assignment operator.
   */
  WordLayoutInfo& operator=( const WordLayoutInfo& word );

  Size                              mSize;                      ///< Size of the word.
  float                             mAscender;                  ///< Max of all ascenders of all characters.
  TextSeparatorType                 mType;                      ///< Whether this word is a word separator, a line separator or is not a separator.
  CharacterLayoutInfoContainer      mCharactersLayoutInfo;      ///< Layout info for all characters.
};
typedef std::vector<WordLayoutInfo> WordLayoutInfoContainer;

/**
 * Layout information for a paragraph.
 */
struct ParagraphLayoutInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  ParagraphLayoutInfo();

  /**
   * Default destructor.
   *
   * Clears all words and deletes all text styles.
   */
  ~ParagraphLayoutInfo();

  /**
   * Copy constructor.
   */
  ParagraphLayoutInfo( const ParagraphLayoutInfo& paragraph );

  /**
   * Assignment operator.
   */
  ParagraphLayoutInfo& operator=( const ParagraphLayoutInfo& paragraph );

  Size                                          mSize;                       ///< Size of the paragraph.
  float                                         mAscender;                   ///< Max of all ascenders of all words.
  float                                         mLineHeightOffset;           ///< Line height offset.
  std::size_t                                   mNumberOfCharacters;         ///< Stores the number of characters.
  WordLayoutInfoContainer                       mWordsLayoutInfo;            ///< Layout info for all words.
};
typedef std::vector<ParagraphLayoutInfo> ParagraphLayoutInfoContainer;

/**
 * Layout information for the whole text.
 */
struct TextLayoutInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  TextLayoutInfo();

  /**
   * Copy constructor.
   */
  TextLayoutInfo( const TextLayoutInfo& text );

  /**
   * Assignment operator.
   */
  TextLayoutInfo& operator=( const TextLayoutInfo& text );

  Size                         mWholeTextSize;        ///< width and height of the whole text.
  float                        mMaxWordWidth;         ///< maximum width between all words.
  float                        mMaxItalicsOffset;     ///< When rendering text-view in offscreen an extra width offset is needed to prevent italic characters to be cut if they are in the right edge.
  std::size_t                  mNumberOfCharacters;   ///< Stores the number of characters.
  ParagraphLayoutInfoContainer mParagraphsLayoutInfo; ///< Layout information for all paragraphs.
  WordLayoutInfo               mEllipsizeLayoutInfo;  ///< Layout information for the ellipsize text.
};

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_TYPES_H__
