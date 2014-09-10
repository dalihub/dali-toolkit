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

// FILE HEADER
#include <dali-toolkit/internal/controls/text-view/text-processor-bidirectional-info.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace TextProcessor
{

BidirectionalParagraphInfo::BidirectionalParagraphInfo()
: mDirection( FRIBIDI_TYPE_ON ),
  mCharactersTypeBuffer(),
  mLevelsBuffer(),
  mLogicalUnicodeBuffer()
{
}

BidirectionalParagraphInfo::~BidirectionalParagraphInfo()
{
}

BidirectionalParagraphInfo::BidirectionalParagraphInfo( const BidirectionalParagraphInfo& info )
: mDirection( info.mDirection ),
  mCharactersTypeBuffer( info.mCharactersTypeBuffer ),
  mLevelsBuffer( info.mLevelsBuffer ),
  mLogicalUnicodeBuffer( info.mLogicalUnicodeBuffer )
{
}

BidirectionalParagraphInfo& BidirectionalParagraphInfo::operator=( const BidirectionalParagraphInfo& info )
{
  if( this != &info )
  {
    mDirection = info.mDirection;
    mCharactersTypeBuffer = info.mCharactersTypeBuffer;
    mLevelsBuffer = info.mLevelsBuffer;
    mLogicalUnicodeBuffer = info.mLogicalUnicodeBuffer;
  }

  return *this;
}

BidirectionalLineInfo::BidirectionalLineInfo()
: mCharacterParagraphIndex(),
  mNumberOfCharacters(),
  mText(),
  mVisualToLogicalMap(),
  mLogicalToVisualMap()
{
}

BidirectionalLineInfo::~BidirectionalLineInfo()
{
}

BidirectionalLineInfo::BidirectionalLineInfo( const BidirectionalLineInfo& info )
: mCharacterParagraphIndex( info.mCharacterParagraphIndex ),
  mNumberOfCharacters( info.mNumberOfCharacters ),
  mText( info.mText ),
  mVisualToLogicalMap( info.mVisualToLogicalMap ),
  mLogicalToVisualMap( info.mLogicalToVisualMap )
{
}

BidirectionalLineInfo& BidirectionalLineInfo::operator=( const BidirectionalLineInfo& info )
{
  if( this != &info )
  {
    mCharacterParagraphIndex = info.mCharacterParagraphIndex;
    mNumberOfCharacters = info.mNumberOfCharacters;
    mText = info.mText;
    mVisualToLogicalMap = info.mVisualToLogicalMap;
    mLogicalToVisualMap = info.mLogicalToVisualMap;
  }

  return *this;
}

bool BeginsRightToLeftCharacter( const Text& text )
{
  for( size_t i = 0u, length = text.GetLength(); i < length; ++i )
  {
    Character::CharacterDirection direction = text[i].GetCharacterDirection();
    if( direction != Character::Neutral )
    {
      return ( direction == Character::RightToLeft || direction == Character::RightToLeftWeak );
    }
  }

  return false;
}

bool ContainsRightToLeftCharacter( const Dali::Text& text )
{
  for( size_t i = 0u, length = text.GetLength(); i < length; ++i )
  {
    Character::CharacterDirection direction = ( text[i] ).GetCharacterDirection();
    if( ( Character::RightToLeft == direction ) || ( Character::RightToLeftWeak == direction ) )
    {
      return true;
    }
  }

  return false;
}

void ProcessBidirectionalText( Text& paragraph, BidirectionalParagraphInfo* info )
{
  if( paragraph.IsEmpty() )
  {
    // nothing to do if the paragraph is empty.
    return;
  }

  const std::size_t stringSize = paragraph.GetText().size();

  // Text buffer in logical order. Coded in unicode.
  info->mLogicalUnicodeBuffer.resize( stringSize + 1u, 0u );
  FriBidiChar* logicalUnicodeBufferPointer = &info->mLogicalUnicodeBuffer[0u];

  // Converts from utf8 to unicode.
  const std::size_t length = fribidi_charset_to_unicode( FRIBIDI_CHAR_SET_UTF8, paragraph.GetText().c_str(), stringSize, logicalUnicodeBufferPointer );

  // Character type buffer.
  info->mCharactersTypeBuffer.resize( length, 0u );

  // Levels buffer.
  info->mLevelsBuffer.resize( length, 0u );

  // Joining type buffer.
  std::vector<FriBidiJoiningType> joiningTypeBuffer;
  joiningTypeBuffer.resize( length, 0u );

  // Pointers to the buffers.
  FriBidiCharType* charactersTypeBufferPointer = &info->mCharactersTypeBuffer[0u];
  FriBidiLevel* levelsBufferPointer = &info->mLevelsBuffer[0u];
  FriBidiJoiningType* joiningTypeBufferPointer = &joiningTypeBuffer[0u];

  // Retrieves the type of each character.
  fribidi_get_bidi_types( logicalUnicodeBufferPointer, length, charactersTypeBufferPointer );

  // Retrieves the paragraph direction.
  info->mDirection = fribidi_get_par_direction( charactersTypeBufferPointer, length );

  // Retrieve the embedding levels.
  fribidi_get_par_embedding_levels( charactersTypeBufferPointer, length, &info->mDirection, levelsBufferPointer );

  // Retrieve the joining types.
  fribidi_get_joining_types( logicalUnicodeBufferPointer, length, joiningTypeBufferPointer );

  fribidi_join_arabic( charactersTypeBufferPointer, length, levelsBufferPointer, joiningTypeBufferPointer );

  const FriBidiFlags flags = FRIBIDI_FLAGS_DEFAULT | FRIBIDI_FLAGS_ARABIC;

  fribidi_shape( flags, levelsBufferPointer, length, joiningTypeBufferPointer, logicalUnicodeBufferPointer );

  std::vector<char> bidiTextConverted;

  bidiTextConverted.resize( length * 4u + 1u ); // Maximum bytes to represent one UTF-8 character is 6.
                                                // Currently Dali doesn't support this UTF-8 extension. Dali only supports 'regular' UTF-8 which has a maximum of 4 bytes per character.

  fribidi_unicode_to_charset( FRIBIDI_CHAR_SET_UTF8, logicalUnicodeBufferPointer, length, &bidiTextConverted[0] );

  paragraph = Text( &bidiTextConverted[0u] );
}

void ReorderLine( BidirectionalParagraphInfo* paragraphInfo,
                  BidirectionalLineInfo* lineInfo )
{
  const FriBidiFlags flags = FRIBIDI_FLAGS_DEFAULT | FRIBIDI_FLAGS_ARABIC;

  lineInfo->mVisualToLogicalMap.Resize( lineInfo->mNumberOfCharacters, 0u );
  lineInfo->mLogicalToVisualMap.Resize( lineInfo->mNumberOfCharacters, 0u );

  std::vector<FriBidiChar> visualUnicodeBuffer;
  visualUnicodeBuffer.insert( visualUnicodeBuffer.end(),
                              paragraphInfo->mLogicalUnicodeBuffer.begin() + lineInfo->mCharacterParagraphIndex,
                              paragraphInfo->mLogicalUnicodeBuffer.begin() + ( lineInfo->mCharacterParagraphIndex + lineInfo->mNumberOfCharacters ) );

  // Pointers to the buffers.
  FriBidiCharType* charactersTypeBufferPointer = &paragraphInfo->mCharactersTypeBuffer[lineInfo->mCharacterParagraphIndex];
  FriBidiLevel* levelsBufferPointer = &paragraphInfo->mLevelsBuffer[lineInfo->mCharacterParagraphIndex];
  FriBidiChar* visualUnicodeBufferPointer = &visualUnicodeBuffer[0u];
  FriBidiStrIndex* visualToLogicalMapPointer = &lineInfo->mVisualToLogicalMap[0u];

  // Initialize the visual to logical mapping table to the identity. Otherwise fribidi_reorder_line fails to retrieve a valid mapping table.
  for( std::size_t index = 0u; index < lineInfo->mNumberOfCharacters; ++index )
  {
    lineInfo->mVisualToLogicalMap[ index ] = index;
  }

  fribidi_reorder_line( flags,
                        charactersTypeBufferPointer,
                        lineInfo->mNumberOfCharacters,
                        0u,
                        paragraphInfo->mDirection,
                        levelsBufferPointer,
                        visualUnicodeBufferPointer,
                        visualToLogicalMapPointer );

  // Fill the logical to visual mapping table.
  for( std::size_t index = 0u; index < lineInfo->mNumberOfCharacters; ++index )
  {
    lineInfo->mLogicalToVisualMap[ lineInfo->mVisualToLogicalMap[ index ] ] = index;
  }

  std::vector<char> bidiTextConverted;

  bidiTextConverted.resize( lineInfo->mNumberOfCharacters * 4u + 1u ); // Maximum bytes to represent one UTF-8 character is 6.
                                                                       // Currently Dali doesn't support this UTF-8 extension.
                                                                       // Dali only supports 'regular' UTF-8 which has a maximum of 4 bytes per character.

  fribidi_unicode_to_charset( FRIBIDI_CHAR_SET_UTF8, visualUnicodeBufferPointer, lineInfo->mNumberOfCharacters, &bidiTextConverted[0u] );

  lineInfo->mText = Text( &bidiTextConverted[0u] );
}

} // namespace TextProcessor

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
