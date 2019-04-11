/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/text/markup-processor.h>

// EXTERNAL INCLUDES
#include <climits>  // for ULONG_MAX
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/markup-processor-color.h>
#include <dali-toolkit/internal/text/markup-processor-embedded-item.h>
#include <dali-toolkit/internal/text/markup-processor-font.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/xhtml-entities.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

namespace
{
// HTML-ISH tag and attribute constants.
// Note they must be lower case in order to make the comparison to work
// as the parser converts all the read tags to lower case.
const std::string XHTML_COLOR_TAG("color");
const std::string XHTML_FONT_TAG("font");
const std::string XHTML_B_TAG("b");
const std::string XHTML_I_TAG("i");
const std::string XHTML_U_TAG("u");
const std::string XHTML_SHADOW_TAG("shadow");
const std::string XHTML_GLOW_TAG("glow");
const std::string XHTML_OUTLINE_TAG("outline");
const std::string XHTML_ITEM_TAG("item");

const char LESS_THAN      = '<';
const char GREATER_THAN   = '>';
const char EQUAL          = '=';
const char QUOTATION_MARK = '\'';
const char SLASH          = '/';
const char BACK_SLASH     = '\\';
const char AMPERSAND      = '&';
const char HASH           = '#';
const char SEMI_COLON     = ';';
const char CHAR_ARRAY_END = '\0';
const char HEX_CODE       = 'x';

const char WHITE_SPACE    = 0x20;        // ASCII value of the white space.

// Range 1 0x0u < XHTML_DECIMAL_ENTITY_RANGE <= 0xD7FFu
// Range 2 0xE000u < XHTML_DECIMAL_ENTITY_RANGE <= 0xFFFDu
// Range 3 0x10000u < XHTML_DECIMAL_ENTITY_RANGE <= 0x10FFFFu
const unsigned long XHTML_DECIMAL_ENTITY_RANGE[] = { 0x0u, 0xD7FFu, 0xE000u, 0xFFFDu, 0x10000u, 0x10FFFFu };

const unsigned int MAX_NUM_OF_ATTRIBUTES =  5u; ///< The font tag has the 'family', 'size' 'weight', 'width' and 'slant' attrubutes.
const unsigned int DEFAULT_VECTOR_SIZE   = 16u; ///< Default size of run vectors.

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_MARKUP_PROCESSOR");
#endif

/**
 * @brief Struct used to retrieve the style runs from the mark-up string.
 */
struct StyleStack
{
  typedef VectorBase::SizeType RunIndex;

  Vector<RunIndex>  stack;    ///< Use a vector as a style stack. Stores the indices pointing where the run is stored inside the logical model.
  unsigned int topIndex; ///< Points the top of the stack.

  StyleStack()
  : stack(),
    topIndex( 0u )
  {
    stack.Resize( DEFAULT_VECTOR_SIZE );
  }

  void Push( RunIndex index )
  {
    // Check if there is space inside the style stack.
    const VectorBase::SizeType size = stack.Count();
    if( topIndex >= size )
    {
      // Resize the style stack.
      stack.Resize( 2u * size );
    }

    // Set the run index in the top of the stack.
    *( stack.Begin() + topIndex ) = index;

    // Reposition the pointer to the top of the stack.
    ++topIndex;
  }

  RunIndex Pop()
  {
    // Pop the top of the stack.
    --topIndex;
    return *( stack.Begin() + topIndex );
  }
};

/**
 * @brief Initializes a font run description to its defaults.
 *
 * @param[in,out] fontRun The font description run to initialize.
 */
void Initialize( FontDescriptionRun& fontRun )
{
  fontRun.characterRun.characterIndex = 0u;
  fontRun.characterRun.numberOfCharacters = 0u;
  fontRun.familyName = NULL;
  fontRun.familyLength = 0u;
  fontRun.weight = TextAbstraction::FontWeight::NORMAL;
  fontRun.width = TextAbstraction::FontWidth::NORMAL;
  fontRun.slant = TextAbstraction::FontSlant::NORMAL;
  fontRun.size = 0u;
  fontRun.familyDefined = false;
  fontRun.weightDefined = false;
  fontRun.widthDefined = false;
  fontRun.slantDefined = false;
  fontRun.sizeDefined = false;
}

/**
 * @brief Splits the tag string into the tag name and its attributes.
 *
 * The attributes are stored in a vector in the tag.
 *
 * @param[in,out] tag The tag.
 */
void ParseAttributes( Tag& tag )
{
  if( tag.buffer == NULL )
  {
    return;
  }

  tag.attributes.Resize( MAX_NUM_OF_ATTRIBUTES );

  // Find first the tag name.
  bool isQuotationOpen = false;

  const char* tagBuffer = tag.buffer;
  const char* const tagEndBuffer = tagBuffer + tag.length;
  tag.length = 0u;
  for( ; tagBuffer < tagEndBuffer; ++tagBuffer )
  {
    const char character = *tagBuffer;
    if( WHITE_SPACE < character )
    {
      ++tag.length;
    }
    else
    {
      // Stops counting the length of the tag when a white space is found.
      // @note a white space is the WHITE_SPACE character and anything below as 'tab', 'return' or 'control characters'.
      break;
    }
  }
  SkipWhiteSpace( tagBuffer, tagEndBuffer );

  // Find the attributes.
  unsigned int attributeIndex = 0u;
  const char* nameBuffer = NULL;
  const char* valueBuffer = NULL;
  Length nameLength = 0u;
  Length valueLength = 0u;

  bool addToNameValue = true;
  Length numberOfWhiteSpace = 0u;
  for( ; tagBuffer < tagEndBuffer; ++tagBuffer )
  {
    const char character = *tagBuffer;
    if( ( WHITE_SPACE >= character ) && !isQuotationOpen )
    {
      if( NULL != valueBuffer )
      {
        // Remove white spaces at the end of the value.
        valueLength -= numberOfWhiteSpace;
      }

      if( ( NULL != nameBuffer ) && ( NULL != valueBuffer ) )
      {
        // Every time a white space is found, a new attribute is created and stored in the attributes vector.
        Attribute& attribute = *( tag.attributes.Begin() + attributeIndex );
        ++attributeIndex;

        attribute.nameBuffer = nameBuffer;
        attribute.valueBuffer = valueBuffer;
        attribute.nameLength = nameLength;
        attribute.valueLength = valueLength;

        nameBuffer = NULL;
        valueBuffer = NULL;
        nameLength = 0u;
        valueLength = 0u;

        addToNameValue = true; // next read characters will be added to the name.
      }
    }
    else if( EQUAL == character ) // '='
    {
      addToNameValue = false; // next read characters will be added to the value.
      SkipWhiteSpace( tagBuffer, tagEndBuffer );
    }
    else if( QUOTATION_MARK == character ) // '\''
    {
      // Do not add quotation marks to neither name nor value.
      isQuotationOpen = !isQuotationOpen;

      if( isQuotationOpen )
      {
        ++tagBuffer;
        SkipWhiteSpace( tagBuffer, tagEndBuffer );
        --tagBuffer;
      }
    }
    else
    {
      // Adds characters to the name or the value.
      if( addToNameValue )
      {
        if( NULL == nameBuffer )
        {
          nameBuffer = tagBuffer;
        }
        ++nameLength;
      }
      else
      {
        if( isQuotationOpen )
        {
          if( WHITE_SPACE >= character )
          {
            ++numberOfWhiteSpace;
          }
          else
          {
            numberOfWhiteSpace = 0u;
          }
        }
        if( NULL == valueBuffer )
        {
          valueBuffer = tagBuffer;
        }
        ++valueLength;
      }
    }
  }

  if( NULL != valueBuffer )
  {
    // Remove white spaces at the end of the value.
    valueLength -= numberOfWhiteSpace;
  }

  if( ( NULL != nameBuffer ) && ( NULL != valueBuffer ) )
  {
    // Checks if the last attribute needs to be added.
    Attribute& attribute = *( tag.attributes.Begin() + attributeIndex );
    ++attributeIndex;

    attribute.nameBuffer = nameBuffer;
    attribute.valueBuffer = valueBuffer;
    attribute.nameLength = nameLength;
    attribute.valueLength = valueLength;
  }

  // Resize the vector of attributes.
  tag.attributes.Resize( attributeIndex );
}

/**
 * @brief It parses a tag and its attributes if the given iterator @e it is pointing at a tag beginning.
 *
 * @param[in,out] markupStringBuffer The mark-up string buffer. It's a const iterator pointing the current character.
 * @param[in] markupStringEndBuffer Pointer to one character after the end of the mark-up string buffer.
 * @param[out] tag The tag with its attributes.
 *
 * @return @e true if the iterator @e it is pointing a mark-up tag. Otherwise @e false.
 */
bool IsTag( const char*& markupStringBuffer,
            const char* const markupStringEndBuffer,
            Tag& tag )
{
  bool isTag = false;
  bool isQuotationOpen = false;
  bool attributesFound = false;
  tag.isEndTag = false;
  bool isPreviousLessThan = false;
  bool isPreviousSlash = false;

  const char character = *markupStringBuffer;
  if( LESS_THAN == character ) // '<'
  {
    tag.buffer = NULL;
    tag.length = 0u;
    isPreviousLessThan = true;

    // if the iterator is pointing to a '<' character, then check if it's a mark-up tag is needed.
    ++markupStringBuffer;
    if( markupStringBuffer < markupStringEndBuffer )
    {
      SkipWhiteSpace( markupStringBuffer, markupStringEndBuffer );

      for( ; ( !isTag ) && ( markupStringBuffer < markupStringEndBuffer ); ++markupStringBuffer )
      {
        const char character = *markupStringBuffer;

        if( !isQuotationOpen && ( SLASH == character ) ) // '/'
        {
          if (isPreviousLessThan)
          {
            tag.isEndTag = true;
          }
          else
          {
            // if the tag has a '/' it may be an end tag.
            isPreviousSlash = true;
          }

          isPreviousLessThan = false;
          if( ( markupStringBuffer + 1u < markupStringEndBuffer ) && ( WHITE_SPACE >= *( markupStringBuffer + 1u ) ) )
          {
            ++markupStringBuffer;
            SkipWhiteSpace( markupStringBuffer, markupStringEndBuffer );
            --markupStringBuffer;
          }
        }
        else if( GREATER_THAN == character ) // '>'
        {
          isTag = true;
          if (isPreviousSlash)
          {
            tag.isEndTag = true;
          }

          isPreviousSlash = false;
          isPreviousLessThan = false;
        }
        else if( QUOTATION_MARK == character )
        {
          isQuotationOpen = !isQuotationOpen;
          ++tag.length;

          isPreviousSlash = false;
          isPreviousLessThan = false;
        }
        else if( WHITE_SPACE >= character ) // ' '
        {
          // If the tag contains white spaces then it may have attributes.
          if( !isQuotationOpen )
          {
            attributesFound = true;
          }
          ++tag.length;
        }
        else
        {
          if( NULL == tag.buffer )
          {
            tag.buffer = markupStringBuffer;
          }

          // If it's not any of the 'special' characters then just add it to the tag string.
          ++tag.length;

          isPreviousSlash = false;
          isPreviousLessThan = false;
        }
      }
    }

    // If the tag string has white spaces, then parse the attributes is needed.
    if( attributesFound )
    {
      ParseAttributes( tag );
    }
  }

  return isTag;
}

/**
 * @brief Returns length of XHTML entity by parsing the text. It also determines if it is XHTML entity or not.
 *
 * @param[in] markupStringBuffer The mark-up string buffer. It's a const iterator pointing the current character.
 * @param[in] markupStringEndBuffer Pointing to end of mark-up string buffer.
 *
 * @return Length of markupText in case of XHTML entity otherwise return 0.
 */
unsigned int GetXHTMLEntityLength( const char*& markupStringBuffer,
                                   const char* const markupStringEndBuffer )
{
  char character = *markupStringBuffer;
  if( AMPERSAND == character ) // '&'
  {
    // if the iterator is pointing to a '&' character, then check for ';' to find end to XHTML entity.
    ++markupStringBuffer;
    if( markupStringBuffer < markupStringEndBuffer )
    {
      unsigned int len = 1u;
      for( ; markupStringBuffer < markupStringEndBuffer ; ++markupStringBuffer )
      {
        character = *markupStringBuffer;
        ++len;
        if( SEMI_COLON == character ) // ';'
        {
          // found end of XHTML entity
          ++markupStringBuffer;
          return len;
        }
        else if( ( AMPERSAND == character ) || ( BACK_SLASH == character ) || ( LESS_THAN == character ))
        {
          return 0;
        }
      }
    }
  }
  return 0;
}

/**
 * @brief It parses a XHTML string which has hex/decimal entity and fill its corresponging utf-8 string.
 *
 * @param[in] markupText The mark-up text buffer.
 * @param[out] utf-8 text Corresponding to markup Text
 *
 * @return true if string is successfully parsed otherwise false
 */
bool XHTMLNumericEntityToUtf8 ( const char* markupText, char* utf8 )
{
  bool result = false;

  if( NULL != markupText )
  {
    bool isHex = false;

    // check if hex or decimal entity
    if( ( CHAR_ARRAY_END != *markupText ) && ( HEX_CODE == *markupText ) )
    {
      isHex = true;
      ++markupText;
    }

    char* end = NULL;
    unsigned long l = strtoul( markupText, &end, ( isHex ? 16 : 10 ) );  // l contains UTF-32 code in case of correct XHTML entity

    // check for valid XHTML numeric entities (between '#' or "#x" and ';')
    if( ( l > 0 ) && ( l < ULONG_MAX ) && ( *end == SEMI_COLON ) ) // in case wrong XHTML entity is set eg. "&#23abcdefs;" in that case *end will be 'a'
    {
      /* characters XML 1.1 permits */
      if( ( ( XHTML_DECIMAL_ENTITY_RANGE[0] < l ) && ( l <= XHTML_DECIMAL_ENTITY_RANGE[1] ) ) ||
        ( ( XHTML_DECIMAL_ENTITY_RANGE[2] <= l ) && ( l <= XHTML_DECIMAL_ENTITY_RANGE[3] ) ) ||
        ( ( XHTML_DECIMAL_ENTITY_RANGE[4] <= l ) && ( l <= XHTML_DECIMAL_ENTITY_RANGE[5] ) ) )
      {
        // Convert UTF32 code to UTF8
        Utf32ToUtf8( reinterpret_cast<const uint32_t* const>( &l ), 1, reinterpret_cast<uint8_t*>( utf8 ) );
        result = true;
       }
    }
  }
  return result;
}

} // namespace

void ProcessMarkupString( const std::string& markupString, MarkupProcessData& markupProcessData )
{
  DALI_LOG_INFO( gLogFilter, Debug::Verbose, "markupString: %s\n", markupString.c_str() );

  // Reserve space for the plain text.
  const Length markupStringSize = markupString.size();
  markupProcessData.markupProcessedText.reserve( markupStringSize );

  // Stores a struct with the index to the first character of the run, the type of run and its parameters.
  StyleStack styleStack;

  // Points the next free position in the vector of runs.
  StyleStack::RunIndex colorRunIndex = 0u;
  StyleStack::RunIndex fontRunIndex = 0u;

  // Give an initial default value to the model's vectors.
  markupProcessData.colorRuns.Reserve( DEFAULT_VECTOR_SIZE );
  markupProcessData.fontRuns.Reserve( DEFAULT_VECTOR_SIZE );

  // Get the mark-up string buffer.
  const char* markupStringBuffer = markupString.c_str();
  const char* const markupStringEndBuffer = markupStringBuffer + markupStringSize;

  Tag tag;
  CharacterIndex characterIndex = 0u;
  for( ; markupStringBuffer < markupStringEndBuffer; )
  {
    tag.attributes.Clear();
    if( IsTag( markupStringBuffer,
               markupStringEndBuffer,
               tag ) )
    {
      if( TokenComparison( XHTML_COLOR_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new color run.
          ColorRun colorRun;
          colorRun.characterRun.numberOfCharacters = 0u;

          // Set the start character index.
          colorRun.characterRun.characterIndex = characterIndex;

          // Fill the run with the attributes.
          ProcessColorTag( tag, colorRun );

          // Push the color run in the logical model.
          markupProcessData.colorRuns.PushBack( colorRun );

          // Push the index of the run into the stack.
          styleStack.Push( colorRunIndex );

          // Point the next color run.
          ++colorRunIndex;
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
          ColorRun& colorRun = *( markupProcessData.colorRuns.Begin() + styleStack.Pop() );
          colorRun.characterRun.numberOfCharacters = characterIndex - colorRun.characterRun.characterIndex;
        }
      } // <color></color>
      else if( TokenComparison( XHTML_I_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new font run.
          FontDescriptionRun fontRun;
          Initialize( fontRun );

          // Fill the run with the parameters.
          fontRun.characterRun.characterIndex = characterIndex;
          fontRun.slant = TextAbstraction::FontSlant::ITALIC;
          fontRun.slantDefined = true;

          // Push the font run in the logical model.
          markupProcessData.fontRuns.PushBack( fontRun );

          // Push the index of the run into the stack.
          styleStack.Push( fontRunIndex );

          // Point the next free font run.
          ++fontRunIndex;
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
          FontDescriptionRun& fontRun = *( markupProcessData.fontRuns.Begin() + styleStack.Pop() );
          fontRun.characterRun.numberOfCharacters = characterIndex - fontRun.characterRun.characterIndex;
        }
      } // <i></i>
      else if( TokenComparison( XHTML_U_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new underline run.
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
        }
      } // <u></u>
      else if( TokenComparison( XHTML_B_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new font run.
          FontDescriptionRun fontRun;
          Initialize( fontRun );

          // Fill the run with the parameters.
          fontRun.characterRun.characterIndex = characterIndex;
          fontRun.weight = TextAbstraction::FontWeight::BOLD;
          fontRun.weightDefined = true;

          // Push the font run in the logical model.
          markupProcessData.fontRuns.PushBack( fontRun );

          // Push the index of the run into the stack.
          styleStack.Push( fontRunIndex );

          // Point the next free font run.
          ++fontRunIndex;
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
          FontDescriptionRun& fontRun = *( markupProcessData.fontRuns.Begin() + styleStack.Pop() );
          fontRun.characterRun.numberOfCharacters = characterIndex - fontRun.characterRun.characterIndex;
        }
      } // <b></b>
      else if( TokenComparison( XHTML_FONT_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new font run.
          FontDescriptionRun fontRun;
          Initialize( fontRun );

          // Fill the run with the parameters.
          fontRun.characterRun.characterIndex = characterIndex;

          ProcessFontTag( tag, fontRun );

          // Push the font run in the logical model.
          markupProcessData.fontRuns.PushBack( fontRun );

          // Push the index of the run into the stack.
          styleStack.Push( fontRunIndex );

          // Point the next free font run.
          ++fontRunIndex;
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
          FontDescriptionRun& fontRun = *( markupProcessData.fontRuns.Begin() + styleStack.Pop() );
          fontRun.characterRun.numberOfCharacters = characterIndex - fontRun.characterRun.characterIndex;
        }
      } // <font></font>
      else if( TokenComparison( XHTML_SHADOW_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new shadow run.
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
        }
      } // <shadow></shadow>
      else if( TokenComparison( XHTML_GLOW_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new glow run.
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
        }
      } // <glow></glow>
      else if( TokenComparison( XHTML_OUTLINE_TAG, tag.buffer, tag.length ) )
      {
        if( !tag.isEndTag )
        {
          // Create a new outline run.
        }
        else
        {
          // Pop the top of the stack and set the number of characters of the run.
        }
      } // <outline></outline>
      else if (TokenComparison(XHTML_ITEM_TAG, tag.buffer, tag.length))
      {
        if (tag.isEndTag)
        {
          // Create an embedded item instance.
          EmbeddedItem item;
          item.characterIndex = characterIndex;
          ProcessEmbeddedItem(tag, item);

          markupProcessData.items.PushBack(item);

          // Insert white space character that will be replaced by the item.
          markupProcessData.markupProcessedText.append( 1u, WHITE_SPACE );
          ++characterIndex;
        }
      }
    }  // end if( IsTag() )
    else if( markupStringBuffer < markupStringEndBuffer )
    {
      unsigned char character = *markupStringBuffer;
      const char* markupBuffer = markupStringBuffer;
      unsigned char count = GetUtf8Length( character );
      char utf8[8];

      if( ( BACK_SLASH == character ) && ( markupStringBuffer + 1u < markupStringEndBuffer ) )
      {
        // Adding < , >  or & special character.
        const unsigned char nextCharacter = *( markupStringBuffer + 1u );
        if( ( LESS_THAN == nextCharacter ) || ( GREATER_THAN == nextCharacter ) || ( AMPERSAND == nextCharacter ) )
        {
          character = nextCharacter;
          ++markupStringBuffer;

          count = GetUtf8Length( character );
          markupBuffer = markupStringBuffer;
        }
      }
      else   // checking if conatins XHTML entity or not
      {
        const unsigned int len =  GetXHTMLEntityLength( markupStringBuffer, markupStringEndBuffer);

        // Parse markupStringTxt if it contains XHTML Entity between '&' and ';'
        if( len > 0 )
        {
          char* entityCode = NULL;
          bool result = false;
          count = 0;

          // Checking if XHTML Numeric Entity
          if( HASH == *( markupBuffer + 1u ) )
          {
            entityCode = &utf8[0];
            // markupBuffer is currently pointing to '&'. By adding 2u to markupBuffer it will point to numeric string by skipping "&#'
            result = XHTMLNumericEntityToUtf8( ( markupBuffer + 2u ), entityCode );
          }
          else    // Checking if XHTML Named Entity
          {
            entityCode = const_cast<char*> ( NamedEntityToUtf8( markupBuffer, len ) );
            result = ( entityCode != NULL );
          }
          if ( result )
          {
            markupBuffer = entityCode; //utf8 text assigned to markupBuffer
            character = markupBuffer[0];
          }
          else
          {
            DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Not valid XHTML entity : (%.*s) \n", len, markupBuffer );
            markupBuffer = NULL;
          }
        }
        else    // in case string conatins Start of XHTML Entity('&') but not its end character(';')
        {
          if( character == AMPERSAND )
          {
            markupBuffer = NULL;
            DALI_LOG_INFO( gLogFilter, Debug::Verbose, "Not Well formed XHTML content \n" );
          }
        }
      }

      if( markupBuffer != NULL )
      {
        const unsigned char numberOfBytes = GetUtf8Length( character );
        markupProcessData.markupProcessedText.push_back( character );

        for( unsigned char i = 1u; i < numberOfBytes; ++i )
        {
          ++markupBuffer;
          markupProcessData.markupProcessedText.push_back( *markupBuffer );
        }

        ++characterIndex;
        markupStringBuffer += count;
      }
    }
  }

  // Resize the model's vectors.
  if( 0u == fontRunIndex )
  {
    markupProcessData.fontRuns.Clear();
  }
  else
  {
    markupProcessData.fontRuns.Resize( fontRunIndex );
  }

  if( 0u == colorRunIndex )
  {
    markupProcessData.colorRuns.Clear();
  }
  else
  {
    markupProcessData.colorRuns.Resize( colorRunIndex );

#ifdef DEBUG_ENABLED
    for( unsigned int i=0; i<colorRunIndex; ++i )
    {
      ColorRun& run = markupProcessData.colorRuns[i];
      DALI_LOG_INFO( gLogFilter, Debug::Verbose, "run[%d] index: %d, length: %d, color %f,%f,%f,%f\n", i, run.characterRun.characterIndex, run.characterRun.numberOfCharacters, run.color.r, run.color.g, run.color.b, run.color.a );
    }
#endif
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
