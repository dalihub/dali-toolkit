/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/debug.h>
#include <climits> // for ULONG_MAX
#include <functional>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/text/character-set-conversion.h>
#include <dali-toolkit/internal/text/markup-processor-anchor.h>
#include <dali-toolkit/internal/text/markup-processor-background.h>
#include <dali-toolkit/internal/text/markup-processor-color.h>
#include <dali-toolkit/internal/text/markup-processor-embedded-item.h>
#include <dali-toolkit/internal/text/markup-processor-font.h>
#include <dali-toolkit/internal/text/markup-processor-helper-functions.h>
#include <dali-toolkit/internal/text/markup-processor-span.h>
#include <dali-toolkit/internal/text/markup-processor-strikethrough.h>
#include <dali-toolkit/internal/text/markup-processor-underline.h>
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
const std::string XHTML_ANCHOR_TAG("a");
const std::string XHTML_BACKGROUND_TAG("background");
const std::string XHTML_SPAN_TAG("span");
const std::string XHTML_STRIKETHROUGH_TAG("s");
const std::string XHTML_PARAGRAPH_TAG("p");

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

const char WHITE_SPACE = 0x20; // ASCII value of the white space.
const char NEW_LINE    = 0x0A; // ASCII value of the newline.

// Range 1 0x0u < XHTML_DECIMAL_ENTITY_RANGE <= 0xD7FFu
// Range 2 0xE000u < XHTML_DECIMAL_ENTITY_RANGE <= 0xFFFDu
// Range 3 0x10000u < XHTML_DECIMAL_ENTITY_RANGE <= 0x10FFFFu
const unsigned long XHTML_DECIMAL_ENTITY_RANGE[] = {0x0u, 0xD7FFu, 0xE000u, 0xFFFDu, 0x10000u, 0x10FFFFu};

const unsigned int MAX_NUM_OF_ATTRIBUTES = 5u;  ///< The font tag has the 'family', 'size' 'weight', 'width' and 'slant' attrubutes.
const unsigned int DEFAULT_VECTOR_SIZE   = 16u; ///< Default size of run vectors.

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, true, "LOG_MARKUP_PROCESSOR");
#endif

typedef VectorBase::SizeType RunIndex;

/**
 * @brief Struct used to retrieve the style runs from the mark-up string.
 */
template<typename StyleStackType>
struct StyleStack
{
  Vector<StyleStackType> stack;    ///< Use a vector as a style stack.
  unsigned int           topIndex; ///< Points the top of the stack.

  StyleStack()
  : stack(),
    topIndex(0u)
  {
    stack.Resize(DEFAULT_VECTOR_SIZE);
  }

  void Push(StyleStackType item)
  {
    // Check if there is space inside the style stack.
    const VectorBase::SizeType size = stack.Count();
    if(topIndex >= size)
    {
      // Resize the style stack.
      stack.Resize(2u * size);
    }

    // Set the item in the top of the stack.
    *(stack.Begin() + topIndex) = item;

    // Reposition the pointer to the top of the stack.
    ++topIndex;
  }

  StyleStackType Pop()
  {
    // Pop the top of the stack.
    --topIndex;
    return *(stack.Begin() + topIndex);
  }
};

/**
 * @brief Struct used to retrieve spans from the mark-up string.
 */
struct Span
{
  RunIndex colorRunIndex;
  RunIndex fontRunIndex;
  bool     isColorDefined;
  bool     isFontDefined;
};

/**
 * @brief Initializes a font run description to its defaults.
 *
 * @param[in,out] fontRun The font description run to initialize.
 */
void Initialize(FontDescriptionRun& fontRun)
{
  fontRun.characterRun.characterIndex     = 0u;
  fontRun.characterRun.numberOfCharacters = 0u;
  fontRun.familyName                      = NULL;
  fontRun.familyLength                    = 0u;
  fontRun.weight                          = TextAbstraction::FontWeight::NORMAL;
  fontRun.width                           = TextAbstraction::FontWidth::NORMAL;
  fontRun.slant                           = TextAbstraction::FontSlant::NORMAL;
  fontRun.size                            = 0u;
  fontRun.familyDefined                   = false;
  fontRun.weightDefined                   = false;
  fontRun.widthDefined                    = false;
  fontRun.slantDefined                    = false;
  fontRun.sizeDefined                     = false;
}

/**
 * @brief Initializes a color run description to its defaults.
 *
 * @param[in,out] colorRun The font description run to initialize.
 */
void Initialize(ColorRun& colorRun)
{
  colorRun.characterRun.characterIndex     = 0u;
  colorRun.characterRun.numberOfCharacters = 0u;
}

/**
 * @brief Initializes a underlined character run to its defaults.
 *
 * @param[in,out] underlinedCharacterRun The underelined character run to initialize.
 */
void Initialize(UnderlinedCharacterRun& underlinedCharacterRun)
{
  underlinedCharacterRun.characterRun.characterIndex     = 0u;
  underlinedCharacterRun.characterRun.numberOfCharacters = 0u;
}

/**
 * @brief Initializes a span to its defaults.
 *
 * @param[in,out] span The span to be initialized.
 */
void Initialize(Span& span)
{
  span.colorRunIndex  = 0u;
  span.isColorDefined = false;
  span.fontRunIndex   = 0u;
  span.isFontDefined  = false;
}

/**
 * @brief Initializes a strikethrough character run to its defaults.
 *
 * @param[in,out] strikethroughCharacterRun The strikethrough character run to initialize.
 */
void Initialize(StrikethroughCharacterRun& strikethroughCharacterRun)
{
  strikethroughCharacterRun.characterRun.characterIndex     = 0u;
  strikethroughCharacterRun.characterRun.numberOfCharacters = 0u;
  strikethroughCharacterRun.isColorSet                      = false;
}

/**
 * @brief Initializes a  bounded-paragraph character run to its defaults.
 *
 * @param[in,out] boundedParagraphRun The bounded paragraphRun run to initialize.
 */
void Initialize(BoundedParagraphRun& boundedParagraphRun)
{
  boundedParagraphRun.characterRun.characterIndex     = 0u;
  boundedParagraphRun.characterRun.numberOfCharacters = 0u;
}

/**
 * @brief Splits the tag string into the tag name and its attributes.
 *
 * The attributes are stored in a vector in the tag.
 *
 * @param[in,out] tag The tag.
 */
void ParseAttributes(Tag& tag)
{
  if(tag.buffer == NULL)
  {
    return;
  }

  tag.attributes.Resize(MAX_NUM_OF_ATTRIBUTES);

  // Find first the tag name.
  bool isQuotationOpen = false;

  const char*       tagBuffer    = tag.buffer;
  const char* const tagEndBuffer = tagBuffer + tag.length;
  tag.length                     = 0u;
  for(; tagBuffer < tagEndBuffer; ++tagBuffer)
  {
    const char character = *tagBuffer;
    if(WHITE_SPACE < character)
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
  SkipWhiteSpace(tagBuffer, tagEndBuffer);

  // Find the attributes.
  unsigned int attributeIndex = 0u;
  const char*  nameBuffer     = NULL;
  const char*  valueBuffer    = NULL;
  Length       nameLength     = 0u;
  Length       valueLength    = 0u;

  bool   addToNameValue     = true;
  Length numberOfWhiteSpace = 0u;
  for(; tagBuffer < tagEndBuffer; ++tagBuffer)
  {
    const char character = *tagBuffer;
    if((WHITE_SPACE >= character) && !isQuotationOpen)
    {
      if(NULL != valueBuffer)
      {
        // Remove white spaces at the end of the value.
        valueLength -= numberOfWhiteSpace;
      }

      if((NULL != nameBuffer) && (NULL != valueBuffer))
      {
        // Every time a white space is found, a new attribute is created and stored in the attributes vector.
        Attribute& attribute = *(tag.attributes.Begin() + attributeIndex);
        ++attributeIndex;

        attribute.nameBuffer  = nameBuffer;
        attribute.valueBuffer = valueBuffer;
        attribute.nameLength  = nameLength;
        attribute.valueLength = valueLength;

        nameBuffer  = NULL;
        valueBuffer = NULL;
        nameLength  = 0u;
        valueLength = 0u;

        addToNameValue = true; // next read characters will be added to the name.
      }
    }
    else if(EQUAL == character) // '='
    {
      addToNameValue = false; // next read characters will be added to the value.
      SkipWhiteSpace(tagBuffer, tagEndBuffer);
    }
    else if(QUOTATION_MARK == character) // '\''
    {
      // Do not add quotation marks to neither name nor value.
      isQuotationOpen = !isQuotationOpen;

      if(isQuotationOpen)
      {
        ++tagBuffer;
        SkipWhiteSpace(tagBuffer, tagEndBuffer);
        --tagBuffer;
      }
    }
    else
    {
      // Adds characters to the name or the value.
      if(addToNameValue)
      {
        if(NULL == nameBuffer)
        {
          nameBuffer = tagBuffer;
        }
        ++nameLength;
      }
      else
      {
        if(isQuotationOpen)
        {
          if(WHITE_SPACE >= character)
          {
            ++numberOfWhiteSpace;
          }
          else
          {
            numberOfWhiteSpace = 0u;
          }
        }
        if(NULL == valueBuffer)
        {
          valueBuffer = tagBuffer;
        }
        ++valueLength;
      }
    }
  }

  if(NULL != valueBuffer)
  {
    // Remove white spaces at the end of the value.
    valueLength -= numberOfWhiteSpace;
  }

  if((NULL != nameBuffer) && (NULL != valueBuffer))
  {
    // Checks if the last attribute needs to be added.
    Attribute& attribute = *(tag.attributes.Begin() + attributeIndex);
    ++attributeIndex;

    attribute.nameBuffer  = nameBuffer;
    attribute.valueBuffer = valueBuffer;
    attribute.nameLength  = nameLength;
    attribute.valueLength = valueLength;
  }

  // Resize the vector of attributes.
  tag.attributes.Resize(attributeIndex);
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
bool IsTag(const char*&      markupStringBuffer,
           const char* const markupStringEndBuffer,
           Tag&              tag)
{
  bool isTag              = false;
  bool isQuotationOpen    = false;
  bool attributesFound    = false;
  tag.isEndTag            = false;
  bool isPreviousLessThan = false;
  bool isPreviousSlash    = false;

  const char character = *markupStringBuffer;
  if(LESS_THAN == character) // '<'
  {
    tag.buffer         = NULL;
    tag.length         = 0u;
    isPreviousLessThan = true;

    // if the iterator is pointing to a '<' character, then check if it's a mark-up tag is needed.
    ++markupStringBuffer;
    if(markupStringBuffer < markupStringEndBuffer)
    {
      SkipWhiteSpace(markupStringBuffer, markupStringEndBuffer);

      for(; (!isTag) && (markupStringBuffer < markupStringEndBuffer); ++markupStringBuffer)
      {
        const char character = *markupStringBuffer;

        if(!isQuotationOpen && (SLASH == character)) // '/'
        {
          if(isPreviousLessThan)
          {
            tag.isEndTag = true;
          }
          else
          {
            // if the tag has a '/' it may be an end tag.
            isPreviousSlash = true;
          }

          isPreviousLessThan = false;
          if((markupStringBuffer + 1u < markupStringEndBuffer) && (WHITE_SPACE >= *(markupStringBuffer + 1u)))
          {
            ++markupStringBuffer;
            SkipWhiteSpace(markupStringBuffer, markupStringEndBuffer);
            --markupStringBuffer;
          }
        }
        else if(GREATER_THAN == character) // '>'
        {
          isTag = true;
          if(isPreviousSlash)
          {
            tag.isEndTag = true;
          }

          isPreviousSlash    = false;
          isPreviousLessThan = false;
        }
        else if(QUOTATION_MARK == character)
        {
          isQuotationOpen = !isQuotationOpen;
          ++tag.length;

          isPreviousSlash    = false;
          isPreviousLessThan = false;
        }
        else if(WHITE_SPACE >= character) // ' '
        {
          // If the tag contains white spaces then it may have attributes.
          if(!isQuotationOpen)
          {
            attributesFound = true;
          }
          ++tag.length;
        }
        else
        {
          if(NULL == tag.buffer)
          {
            tag.buffer = markupStringBuffer;
          }

          // If it's not any of the 'special' characters then just add it to the tag string.
          ++tag.length;

          isPreviousSlash    = false;
          isPreviousLessThan = false;
        }
      }
    }

    // If the tag string has white spaces, then parse the attributes is needed.
    if(attributesFound)
    {
      ParseAttributes(tag);
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
unsigned int GetXHTMLEntityLength(const char*&      markupStringBuffer,
                                  const char* const markupStringEndBuffer)
{
  char character = *markupStringBuffer;
  if(AMPERSAND == character) // '&'
  {
    // if the iterator is pointing to a '&' character, then check for ';' to find end to XHTML entity.
    ++markupStringBuffer;
    if(markupStringBuffer < markupStringEndBuffer)
    {
      unsigned int len = 1u;
      for(; markupStringBuffer < markupStringEndBuffer; ++markupStringBuffer)
      {
        character = *markupStringBuffer;
        ++len;
        if(SEMI_COLON == character) // ';'
        {
          // found end of XHTML entity
          ++markupStringBuffer;
          return len;
        }
        else if((AMPERSAND == character) || (BACK_SLASH == character) || (LESS_THAN == character))
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
bool XHTMLNumericEntityToUtf8(const char* markupText, char* utf8)
{
  bool result = false;

  if(NULL != markupText)
  {
    bool isHex = false;

    // check if hex or decimal entity
    if((CHAR_ARRAY_END != *markupText) && (HEX_CODE == *markupText))
    {
      isHex = true;
      ++markupText;
    }

    char*         end = NULL;
    unsigned long l   = strtoul(markupText, &end, (isHex ? 16 : 10)); // l contains UTF-32 code in case of correct XHTML entity

    // check for valid XHTML numeric entities (between '#' or "#x" and ';')
    if((l > 0) && (l < ULONG_MAX) && (*end == SEMI_COLON)) // in case wrong XHTML entity is set eg. "&#23abcdefs;" in that case *end will be 'a'
    {
      /* characters XML 1.1 permits */
      if(((XHTML_DECIMAL_ENTITY_RANGE[0] < l) && (l <= XHTML_DECIMAL_ENTITY_RANGE[1])) ||
         ((XHTML_DECIMAL_ENTITY_RANGE[2] <= l) && (l <= XHTML_DECIMAL_ENTITY_RANGE[3])) ||
         ((XHTML_DECIMAL_ENTITY_RANGE[4] <= l) && (l <= XHTML_DECIMAL_ENTITY_RANGE[5])))
      {
        // Convert UTF32 code to UTF8
        Utf32ToUtf8(reinterpret_cast<const uint32_t* const>(&l), 1, reinterpret_cast<uint8_t*>(utf8));
        result = true;
      }
    }
  }
  return result;
}

/**
 * @brief Processes a particular tag for the required run (color-run, font-run or underlined-character-run).
 *
 * @tparam RunType Whether ColorRun , FontDescriptionRun or UnderlinedCharacterRun
 *
 * @param[in/out] runsContainer The container containing all the runs
 * @param[in/out] styleStack The style stack
 * @param[in] tag The tag we are currently processing
 * @param[in] characterIndex The current character index
 * @param[in/out] runIndex The run index
 * @param[in/out] tagReference The tagReference we should increment/decrement
 * @param[in] parameterSettingFunction This function will be called to set run specific parameters
 */
template<typename RunType>
void ProcessTagForRun(
  Vector<RunType>&                          runsContainer,
  StyleStack<RunIndex>&                     styleStack,
  const Tag&                                tag,
  const CharacterIndex                      characterIndex,
  RunIndex&                                 runIndex,
  int&                                      tagReference,
  std::function<void(const Tag&, RunType&)> parameterSettingFunction)
{
  if(!tag.isEndTag)
  {
    // Create a new run.
    RunType run;
    Initialize(run);

    // Fill the run with the parameters.
    run.characterRun.characterIndex = characterIndex;
    parameterSettingFunction(tag, run);

    // Push the run in the logical model.
    runsContainer.PushBack(run);

    // Push the index of the run into the stack.
    styleStack.Push(runIndex);

    // Point the next free run.
    ++runIndex;

    // Increase reference
    ++tagReference;
  }
  else
  {
    if(tagReference > 0)
    {
      // Pop the top of the stack and set the number of characters of the run.
      RunType& run                        = *(runsContainer.Begin() + styleStack.Pop());
      run.characterRun.numberOfCharacters = characterIndex - run.characterRun.characterIndex;
      --tagReference;
    }
  }
}

/**
 * @brief Processes the item tag
 *
 * @param[in/out] markupProcessData The markup process data
 * @param[in] tag The current tag
 * @param[in/out] characterIndex The current character index
 */
void ProcessItemTag(
  MarkupProcessData& markupProcessData,
  const Tag          tag,
  CharacterIndex&    characterIndex)
{
  if(tag.isEndTag)
  {
    // Create an embedded item instance.
    EmbeddedItem item;
    item.characterIndex = characterIndex;
    ProcessEmbeddedItem(tag, item);

    markupProcessData.items.PushBack(item);

    // Insert white space character that will be replaced by the item.
    markupProcessData.markupProcessedText.append(1u, WHITE_SPACE);
    ++characterIndex;
  }
}

/**
 * @brief Processes the paragraph-tag
 *
 * @param[in/out] markupProcessData The markup process data
 * @param[in] tag The current tag
 * @param[in] isEndBuffer Whether the end of buffer
 * @param[in/out] characterIndex The current character index
 */
void ProcessParagraphTag(
  MarkupProcessData& markupProcessData,
  const Tag          tag,
  bool               isEndBuffer,
  CharacterIndex&    characterIndex)
{
  if((characterIndex > 0 &&
      markupProcessData.markupProcessedText[characterIndex - 1u] != NEW_LINE) &&
     (!(tag.isEndTag && isEndBuffer)))
  {
    // Insert new-line character at the start and end of paragraph.
    markupProcessData.markupProcessedText.append(1u, NEW_LINE);
    ++characterIndex;
  }
}

/**
 * @brief Processes the anchor tag
 *
 * @param[in/out] markupProcessData The markup process data
 * @param[in] tag The current tag
 * @param[in/out] characterIndex The current character index
 */
void ProcessAnchorTag(
  MarkupProcessData& markupProcessData,
  const Tag          tag,
  CharacterIndex&    characterIndex)
{
  if(!tag.isEndTag)
  {
    // Create an anchor instance.
    Anchor anchor;
    anchor.startIndex = characterIndex;
    anchor.endIndex   = 0u;
    ProcessAnchor(tag, anchor);
    markupProcessData.anchors.PushBack(anchor);
  }
  else
  {
    // Update end index.
    unsigned int count = markupProcessData.anchors.Count();
    if(count > 0)
    {
      markupProcessData.anchors[count - 1].endIndex = characterIndex;
    }
  }
}

/**
 * @brief Processes span tag for the color-run & font-run.
 *
 * @param[in] spanTag The tag we are currently processing
 * @param[in/out] spanStack The spans stack
 * @param[int/out] colorRuns The container containing all the color runs
 * @param[int/out] fontRuns The container containing all the font description runs
 * @param[in/out] colorRunIndex The color run index
 * @param[in/out] fontRunIndex The font run index
 * @param[in] characterIndex The current character index
 * @param[in] tagReference The tagReference we should increment/decrement
 */
void ProcessSpanForRun(
  const Tag&                  spanTag,
  StyleStack<Span>&           spanStack,
  Vector<ColorRun>&           colorRuns,
  Vector<FontDescriptionRun>& fontRuns,
  RunIndex&                   colorRunIndex,
  RunIndex&                   fontRunIndex,
  const CharacterIndex        characterIndex,
  int&                        tagReference)
{
  if(!spanTag.isEndTag)
  {
    // Create a new run.
    ColorRun colorRun;
    Initialize(colorRun);

    FontDescriptionRun fontRun;
    Initialize(fontRun);

    Span span;
    Initialize(span);

    // Fill the run with the parameters.
    colorRun.characterRun.characterIndex = characterIndex;
    fontRun.characterRun.characterIndex  = characterIndex;

    span.colorRunIndex = colorRunIndex;
    span.fontRunIndex  = fontRunIndex;

    ProcessSpanTag(spanTag, colorRun, fontRun, span.isColorDefined, span.isFontDefined);

    // Push the span into the stack.
    spanStack.Push(span);

    // Point the next free run.
    if(span.isColorDefined)
    {
      // Push the run in the logical model.
      colorRuns.PushBack(colorRun);
      ++colorRunIndex;
    }

    if(span.isFontDefined)
    {
      // Push the run in the logical model.
      fontRuns.PushBack(fontRun);
      ++fontRunIndex;
    }

    // Increase reference
    ++tagReference;
  }
  else
  {
    if(tagReference > 0)
    {
      // Pop the top of the stack and set the number of characters of the run.
      Span span = spanStack.Pop();

      if(span.isColorDefined)
      {
        ColorRun& colorRun                       = *(colorRuns.Begin() + span.colorRunIndex);
        colorRun.characterRun.numberOfCharacters = characterIndex - colorRun.characterRun.characterIndex;
      }

      if(span.isFontDefined)
      {
        FontDescriptionRun& fontRun             = *(fontRuns.Begin() + span.fontRunIndex);
        fontRun.characterRun.numberOfCharacters = characterIndex - fontRun.characterRun.characterIndex;
      }

      --tagReference;
    }
  }
}

/**
 * @brief Resizes the model's vectors
 *
 * @param[in/out] markupProcessData The markup process data
 * @param[in] fontRunIndex The font run index
 * @param[in] colorRunIndex The color run index
 * @param[in] underlinedCharacterRunIndex The underlined character run index
 * @param[in] backgroundRunIndex The background run index
 * @param[in] boundedParagraphRunIndex The bounded paragraph run index
 *
 */
void ResizeModelVectors(MarkupProcessData& markupProcessData,
                        const RunIndex     fontRunIndex,
                        const RunIndex     colorRunIndex,
                        const RunIndex     underlinedCharacterRunIndex,
                        const RunIndex     backgroundRunIndex,
                        const RunIndex     boundedParagraphRunIndex)
{
  markupProcessData.fontRuns.Resize(fontRunIndex);
  markupProcessData.colorRuns.Resize(colorRunIndex);
  markupProcessData.underlinedCharacterRuns.Resize(underlinedCharacterRunIndex);
  markupProcessData.backgroundColorRuns.Resize(backgroundRunIndex);
  markupProcessData.boundedParagraphRuns.Resize(boundedParagraphRunIndex);

#ifdef DEBUG_ENABLED
  for(unsigned int i = 0; i < colorRunIndex; ++i)
  {
    ColorRun& run = markupProcessData.colorRuns[i];
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "run[%d] index: %d, length: %d, color %f,%f,%f,%f\n", i, run.characterRun.characterIndex, run.characterRun.numberOfCharacters, run.color.r, run.color.g, run.color.b, run.color.a);
  }
#endif
}

/**
 * @brief Processes the markup string buffer
 *
 * @param[in/out] markupProcessData The markup process data
 * @param[in/out] markupStringBuffer The markup string buffer pointer
 * @param[in] markupStringEndBuffer The markup string end buffer pointer
 * @param[in/out] characterIndex The current character index
 */
void ProcessMarkupStringBuffer(
  MarkupProcessData& markupProcessData,
  const char*&       markupStringBuffer,
  const char* const  markupStringEndBuffer,
  CharacterIndex&    characterIndex)
{
  unsigned char character    = *markupStringBuffer;
  const char*   markupBuffer = markupStringBuffer;
  unsigned char count        = GetUtf8Length(character);
  char          utf8[8];

  if((BACK_SLASH == character) && (markupStringBuffer + 1u < markupStringEndBuffer))
  {
    // Adding < , >  or & special character.
    const unsigned char nextCharacter = *(markupStringBuffer + 1u);
    if((LESS_THAN == nextCharacter) || (GREATER_THAN == nextCharacter) || (AMPERSAND == nextCharacter))
    {
      character = nextCharacter;
      ++markupStringBuffer;

      count        = GetUtf8Length(character);
      markupBuffer = markupStringBuffer;
    }
  }
  else // checking if contains XHTML entity or not
  {
    const unsigned int len = GetXHTMLEntityLength(markupStringBuffer, markupStringEndBuffer);

    // Parse markupStringTxt if it contains XHTML Entity between '&' and ';'
    if(len > 0)
    {
      char* entityCode = NULL;
      bool  result     = false;
      count            = 0;

      // Checking if XHTML Numeric Entity
      if(HASH == *(markupBuffer + 1u))
      {
        entityCode = &utf8[0];
        // markupBuffer is currently pointing to '&'. By adding 2u to markupBuffer it will point to numeric string by skipping "&#'
        result = XHTMLNumericEntityToUtf8((markupBuffer + 2u), entityCode);
      }
      else // Checking if XHTML Named Entity
      {
        entityCode = const_cast<char*>(NamedEntityToUtf8(markupBuffer, len));
        result     = (entityCode != NULL);
      }
      if(result)
      {
        markupBuffer = entityCode; //utf8 text assigned to markupBuffer
        character    = markupBuffer[0];
      }
      else
      {
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Not valid XHTML entity : (%.*s) \n", len, markupBuffer);
        markupBuffer = NULL;
      }
    }
    else // in case string conatins Start of XHTML Entity('&') but not its end character(';')
    {
      if(character == AMPERSAND)
      {
        markupBuffer = NULL;
        DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Not Well formed XHTML content \n");
      }
    }
  }

  if(markupBuffer != NULL)
  {
    const unsigned char numberOfBytes = GetUtf8Length(character);
    markupProcessData.markupProcessedText.push_back(character);

    for(unsigned char i = 1u; i < numberOfBytes; ++i)
    {
      ++markupBuffer;
      markupProcessData.markupProcessedText.push_back(*markupBuffer);
    }

    ++characterIndex;
    markupStringBuffer += count;
  }
}

} // namespace

void ProcessMarkupString(const std::string& markupString, MarkupProcessData& markupProcessData)
{
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "markupString: %s\n", markupString.c_str());

  // Reserve space for the plain text.
  const Length markupStringSize = markupString.size();
  markupProcessData.markupProcessedText.reserve(markupStringSize);

  // Stores a struct with the index to the first character of the run, the type of run and its parameters.
  StyleStack<RunIndex> styleStack;

  // Stores a struct with the index to the first character of the color run & color font for the span.
  StyleStack<Span> spanStack;

  // Points the next free position in the vector of runs.
  RunIndex colorRunIndex                  = 0u;
  RunIndex fontRunIndex                   = 0u;
  RunIndex underlinedCharacterRunIndex    = 0u;
  RunIndex backgroundRunIndex             = 0u;
  RunIndex strikethroughCharacterRunIndex = 0u;
  RunIndex boundedParagraphRunIndex       = 0u;

  // check tag reference
  int colorTagReference      = 0u;
  int fontTagReference       = 0u;
  int iTagReference          = 0u;
  int bTagReference          = 0u;
  int uTagReference          = 0u;
  int backgroundTagReference = 0u;
  int spanTagReference       = 0u;
  int sTagReference          = 0u;
  int pTagReference          = 0u;

  // Give an initial default value to the model's vectors.
  markupProcessData.colorRuns.Reserve(DEFAULT_VECTOR_SIZE);
  markupProcessData.fontRuns.Reserve(DEFAULT_VECTOR_SIZE);
  markupProcessData.underlinedCharacterRuns.Reserve(DEFAULT_VECTOR_SIZE);
  markupProcessData.backgroundColorRuns.Reserve(DEFAULT_VECTOR_SIZE);

  // Get the mark-up string buffer.
  const char*       markupStringBuffer    = markupString.c_str();
  const char* const markupStringEndBuffer = markupStringBuffer + markupStringSize;

  Tag            tag;
  CharacterIndex characterIndex = 0u;
  for(; markupStringBuffer < markupStringEndBuffer;)
  {
    tag.attributes.Clear();
    if(IsTag(markupStringBuffer,
             markupStringEndBuffer,
             tag))
    {
      if(TokenComparison(XHTML_COLOR_TAG, tag.buffer, tag.length))
      {
        ProcessTagForRun<ColorRun>(
          markupProcessData.colorRuns, styleStack, tag, characterIndex, colorRunIndex, colorTagReference, [](const Tag& tag, ColorRun& run) { ProcessColorTag(tag, run); });
      } // <color></color>
      else if(TokenComparison(XHTML_I_TAG, tag.buffer, tag.length))
      {
        ProcessTagForRun<FontDescriptionRun>(
          markupProcessData.fontRuns, styleStack, tag, characterIndex, fontRunIndex, iTagReference, [](const Tag&, FontDescriptionRun& fontRun) {
            fontRun.slant        = TextAbstraction::FontSlant::ITALIC;
            fontRun.slantDefined = true;
          });
      } // <i></i>
      else if(TokenComparison(XHTML_U_TAG, tag.buffer, tag.length))
      {
        ProcessTagForRun<UnderlinedCharacterRun>(
          markupProcessData.underlinedCharacterRuns, styleStack, tag, characterIndex, underlinedCharacterRunIndex, uTagReference, [](const Tag& tag, UnderlinedCharacterRun& run) { ProcessUnderlineTag(tag, run); });
      } // <u></u>
      else if(TokenComparison(XHTML_B_TAG, tag.buffer, tag.length))
      {
        ProcessTagForRun<FontDescriptionRun>(
          markupProcessData.fontRuns, styleStack, tag, characterIndex, fontRunIndex, bTagReference, [](const Tag&, FontDescriptionRun& fontRun) {
            fontRun.weight        = TextAbstraction::FontWeight::BOLD;
            fontRun.weightDefined = true;
          });
      } // <b></b>
      else if(TokenComparison(XHTML_FONT_TAG, tag.buffer, tag.length))
      {
        ProcessTagForRun<FontDescriptionRun>(
          markupProcessData.fontRuns, styleStack, tag, characterIndex, fontRunIndex, fontTagReference, [](const Tag& tag, FontDescriptionRun& fontRun) { ProcessFontTag(tag, fontRun); });
      } // <font></font>
      else if(TokenComparison(XHTML_ANCHOR_TAG, tag.buffer, tag.length))
      {
        /* Anchor */
        ProcessAnchorTag(markupProcessData, tag, characterIndex);
        /* Color */
        ProcessTagForRun<ColorRun>(
          markupProcessData.colorRuns, styleStack, tag, characterIndex, colorRunIndex, colorTagReference, [](const Tag& tag, ColorRun& run) {
            run.color = Color::BLUE;
            ProcessColorTag(tag, run);
          });
        /* Underline */
        ProcessTagForRun<UnderlinedCharacterRun>(
          markupProcessData.underlinedCharacterRuns, styleStack, tag, characterIndex, underlinedCharacterRunIndex, uTagReference, [](const Tag& tag, UnderlinedCharacterRun& run) {
            run.properties.color = Color::BLUE;
            run.properties.colorDefined = true;
            ProcessUnderlineTag(tag, run);
          });
      } // <a href=https://www.tizen.org>tizen</a>
      else if(TokenComparison(XHTML_SHADOW_TAG, tag.buffer, tag.length))
      {
        // TODO: If !tag.isEndTag, then create a new shadow run.
        //       else Pop the top of the stack and set the number of characters of the run.
      } // <shadow></shadow>
      else if(TokenComparison(XHTML_GLOW_TAG, tag.buffer, tag.length))
      {
        // TODO: If !tag.isEndTag, then create a new glow run.
        //       else Pop the top of the stack and set the number of characters of the run.
      } // <glow></glow>
      else if(TokenComparison(XHTML_OUTLINE_TAG, tag.buffer, tag.length))
      {
        // TODO: If !tag.isEndTag, then create a new outline run.
        //       else Pop the top of the stack and set the number of characters of the run.
      } // <outline></outline>
      else if(TokenComparison(XHTML_ITEM_TAG, tag.buffer, tag.length))
      {
        ProcessItemTag(markupProcessData, tag, characterIndex);
      }
      else if(TokenComparison(XHTML_BACKGROUND_TAG, tag.buffer, tag.length))
      {
        ProcessTagForRun<ColorRun>(
          markupProcessData.backgroundColorRuns, styleStack, tag, characterIndex, backgroundRunIndex, backgroundTagReference, [](const Tag& tag, ColorRun& run) { ProcessBackground(tag, run); });
      }
      else if(TokenComparison(XHTML_SPAN_TAG, tag.buffer, tag.length))
      {
        ProcessSpanForRun(tag, spanStack, markupProcessData.colorRuns, markupProcessData.fontRuns, colorRunIndex, fontRunIndex, characterIndex, spanTagReference);
      }
      else if(TokenComparison(XHTML_STRIKETHROUGH_TAG, tag.buffer, tag.length))
      {
        ProcessTagForRun<StrikethroughCharacterRun>(
          markupProcessData.strikethroughCharacterRuns, styleStack, tag, characterIndex, strikethroughCharacterRunIndex, sTagReference, [](const Tag& tag, StrikethroughCharacterRun& run) { ProcessStrikethroughTag(tag, run); });
      } // <s></s>
      else if(TokenComparison(XHTML_PARAGRAPH_TAG, tag.buffer, tag.length))
      {
        ProcessParagraphTag(markupProcessData, tag, (markupStringBuffer == markupStringEndBuffer), characterIndex);
        ProcessTagForRun<BoundedParagraphRun>(
          markupProcessData.boundedParagraphRuns, styleStack, tag, characterIndex, boundedParagraphRunIndex, pTagReference, [](const Tag& tag, BoundedParagraphRun& run) {});
      } // <p></p>
    }   // end if( IsTag() )
    else if(markupStringBuffer < markupStringEndBuffer)
    {
      ProcessMarkupStringBuffer(markupProcessData, markupStringBuffer, markupStringEndBuffer, characterIndex);
    }
  }

  // Resize the model's vectors.
  ResizeModelVectors(markupProcessData, fontRunIndex, colorRunIndex, underlinedCharacterRunIndex, backgroundRunIndex, boundedParagraphRunIndex);
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
