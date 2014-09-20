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
#include <dali/public-api/text/text.h>

namespace Dali
{

class TextStyle;

namespace Toolkit
{

namespace Internal
{

namespace TextProcessor
{
// Forward declarations.
struct BidirectionalParagraphInfo;
struct BidirectionalLineInfo;
} // namespace TextProcessor

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
  std::size_t mCharacterParagraphIndex; ///< The character index within the paragraph.
};

/**
 * Stores gradient info.
 *
 * Used to fade in/out text-actors.
 */
struct GradientInfo
{
  /**
   * Default constructor.
   *
   * Initializes all members to their default values.
   */
  GradientInfo();

  /**
   * Default destructor.
   */
  ~GradientInfo();

  /**
   * Copy constructor
   */
  GradientInfo( const GradientInfo& info );

  /**
   * Assignment operator.
   */
  GradientInfo& operator=( const GradientInfo& info );

  Vector4                     mGradientColor;  ///< Gradient color.
  Vector2                     mStartPoint;     ///< Gradient start point.
  Vector2                     mEndPoint;       ///< Gradient end point.
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

  // Metrics of the glyph.
  Size        mSize;                ///< Height of the font and advance (the horizontal distance from the origin of the current character and the next one).
  float       mBearing;             ///< Vertical distance from the baseline to the top of the glyph's boundary box.
  float       mAscender;            ///< Distance from the base line to the top of the line.
  float       mUnderlineThickness;  ///< The underline's thickness.
  float       mUnderlinePosition;   ///< The underline's position.

  // Position and alignment offset. It depends on the lay-out.
  Vector3     mPosition;            ///< Position within the text-view
  Vector2     mOffset;              ///< Alignment and justification offset.

  RenderableActor mGlyphActor;      ///< Handle to a text-actor.
  float           mColorAlpha;      ///< Alpha component for the initial text color when text is faded.
  GradientInfo*   mGradientInfo;    ///< Stores gradient info.

  bool            mIsVisible:1;     ///< Whether the text-actor is visible.
  bool            mSetText:1;       ///< Whether a new text needs to be set in the text-actor.
  bool            mSetStyle:1;      ///< Whether a new style needs to be set in the text-actor.
  bool            mIsColorGlyph:1;  ///< Whether this character is an emoticon.
  bool            mIsRightToLeft:1; ///< Whether this character is right to left.
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
  std::size_t                       mFirstCharacter;            ///< Index to the first character of the word within the paragraph.
  CharacterLayoutInfoContainer      mCharactersLayoutInfo;      ///< Layout info for all characters.
};
typedef std::vector<WordLayoutInfo> WordLayoutInfoContainer;

/**
 * Stores the reordered layout for right to left text.
 */
struct RightToLeftParagraphLayout
{
  RightToLeftParagraphLayout()
  : mWordsLayoutInfo(),
    mText(),
    mTextStyles(),
    mPreviousLayoutCleared( false )
  {
  }

  WordLayoutInfoContainer mWordsLayoutInfo;         ///< Layout info for all words.
  Text                    mText;                    ///< Stores the text.
  Vector<TextStyle*>      mTextStyles;              ///< Stores the style per each character.
  bool                    mPreviousLayoutCleared:1; ///< Whether the previous right to left layout has been cleared.

  /**
   * Clears the word layout vector, the text and the vector of styles.
   */
  void Clear();
};

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
   * Clears all words, deletes all text styles, the paragraph bidirectional info and all bidirectional infor for each line.
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

  /**
   * Deletes the bidirectional info for each line.
   */
  void ClearBidirectionalInfo();

private:

  /**
   * Deletes all text styles.
   */
  void ClearStyles();

public:
  Size                                          mSize;                       ///< Size of the paragraph.
  float                                         mAscender;                   ///< Max of all ascenders of all words.
  float                                         mLineHeightOffset;           ///< Line height offset.
  std::size_t                                   mFirstCharacter;             ///< Index to the first character of the paragraph.
  std::size_t                                   mNumberOfCharacters;         ///< Stores the number of characters.
  WordLayoutInfoContainer                       mWordsLayoutInfo;            ///< Layout info for all words.
  Text                                          mText;                       ///< Stores the text.
  Vector<TextStyle*>                            mTextStyles;                 ///< Stores the style per each character.
  RightToLeftParagraphLayout*                   mRightToLeftLayout;          ///< Stores the reordered layout for the paragraph.
  TextProcessor::BidirectionalParagraphInfo*    mBidirectionalParagraphInfo; ///< Contains bidirectional info for the whole paragraph. Set to NULL if the paragraph has left to right characters only.
  Vector<TextProcessor::BidirectionalLineInfo*> mBidirectionalLinesInfo;     ///< Contains bidirectional info for each laid-out line.
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
   * Defualt destructor.
   *
   * Clears the paragraph vector, the ellipsis text and deletes all ellipsis styles.
   */
  ~TextLayoutInfo();

  /**
   * Copy constructor.
   */
  TextLayoutInfo( const TextLayoutInfo& text );

  /**
   * Assignment operator.
   */
  TextLayoutInfo& operator=( const TextLayoutInfo& text );

private:
  /**
   * Deletes all the ellipsis text styles.
   */
  void ClearStyles();

public:
  Size                         mWholeTextSize;        ///< width and height of the whole text.
  float                        mMaxWordWidth;         ///< maximum width between all words.
  float                        mMaxItalicsOffset;     ///< When rendering text-view in offscreen an extra width offset is needed to prevent italic characters to be cut if they are in the right edge.
  std::size_t                  mNumberOfCharacters;   ///< Stores the number of characters.
  ParagraphLayoutInfoContainer mParagraphsLayoutInfo; ///< Layout information for all paragraphs.
  WordLayoutInfo               mEllipsizeLayoutInfo;  ///< Layout information for the ellipsize text.
  Dali::Text                   mEllipsisText;         ///< The ellipsis text.
  Vector<TextStyle*>           mEllipsisTextStyles;   ///< Stores the style per each character of the ellipsis text.
};

} // namespace TextViewProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_TEXT_VIEW_PROCESSOR_TYPES_H__
