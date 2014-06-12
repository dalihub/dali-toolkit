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

// HEADER INCLUDE
#include <dali-toolkit/public-api/markup-processor/markup-processor.h>

// INTERNAL INCLUDES
#include <dali/public-api/text/text.h>


// EXTERNAL INCLUDES
#include <stack>
#include <sstream>

namespace Dali
{

namespace Toolkit
{

namespace MarkupProcessor
{

namespace
{
const std::string WEB_COLOR_TOKEN( "#" );
const std::string HEX_COLOR_TOKEN( "0x" );
const std::string ALPHA_ONE( "FF" );

const std::string BLACK_COLOR( "black" );
const std::string WHITE_COLOR( "white" );
const std::string RED_COLOR( "red" );
const std::string GREEN_COLOR( "green" );
const std::string BLUE_COLOR( "blue" );
const std::string YELLOW_COLOR( "yellow" );
const std::string MAGENTA_COLOR( "magenta" );
const std::string CYAN_COLOR( "cyan" );
const std::string TRANSPARENT_COLOR( "transparent" );

const std::string XHTML_B_TAG("b");
const std::string XHTML_I_TAG("i");
const std::string XHTML_U_TAG("u");
const std::string XHTML_BR_TAG("br");
const std::string XHTML_FONT_TAG("font");
const std::string XHTML_SHADOW_TAG("shadow");
const std::string XHTML_GLOW_TAG("glow");
const std::string XHTML_OUTLINE_TAG("outline");
const std::string XHTML_SMOOTH_EDGE_TAG("smooth");
const std::string XHTML_SIZE_PROPERTY("size");
const std::string XHTML_COLOR_PROPERTY("color");
const std::string XHTML_FACE_PROPERTY("face");
const std::string XHTML_STYLE_PROPERTY("style");
const std::string XHTML_PARAM_PROPERTY("param");
const std::string XHTML_PARAM_X_PROPERTY("paramx");
const std::string XHTML_PARAM_Y_PROPERTY("paramy");

const char LESS_THAN( '<' );
const char GREATER_THAN( '>' );
const char EQUAL( '=' );
const char QUOTATION_MARK( '\'');
const char LINE_SEPARATOR_CR( 0x0D ); // Carriage return character  CR
const char LINE_SEPARATOR_LF( 0x0A ); // New line character         LF
const char SLASH( '/' );
const char BACK_SLASH( '\\' );

const std::string LINE_SEPARATOR_LF_STRING(1 , LINE_SEPARATOR_LF); ///< a string with 1 line separator character
/**
 * Stores a property pair: name, value.
 */
struct Property
{
  Property( const std::string& n, const std::string& v )
  : name( n ),
    value( v )
  {}

  std::string name;
  std::string value;
};


/**
 * Compare if two strings are equal.
 *
 * The comparison is case insensitive.
 *
 * @param[in] string1 First of the two strings to be compared.
 * @param[in] string2 Second of the strings to be compared.
 *
 * @return \e true if both strings are equal (case insensitive).
 */
bool CaseInsensitiveComparison( const std::string& string1, const std::string& string2 )
{
  const std::size_t stringSize = string1.size();
  if( stringSize != string2.size() )
  {
    // Early return. Strings have different sizes.
    return false;
  }

  bool equal = true;
  for( std::size_t index = 0; equal && ( index < stringSize ); ++index )
  {
    if( std::toupper( *( string1.begin() + index ) ) != std::toupper( *( string2.begin() + index ) ) )
    {
      equal = false;
    }
  }

  return equal;
}

/**
 * Converts a string into a float value.
 * @param[in] floatStr A float packed inside a string.
 * @return The float value.
 */
float StringToFloat( const std::string& floatStr )
{
  float ret = 0.f;

  std::istringstream( floatStr ) >> ret;

  return ret;
}

/**
 * Converts a float into a string.
 * @param[in] value. A float.
 * @return The float packed into a string.
 */
std::string FloatToString( float value )
{
  std::ostringstream stream;
  stream << value;

  return stream.str();
}

/**
 * Converts a string into an hexadecimal unsigned int.
 * @param[in] uintStr An hexadecimal unsigned int packed inside a string.
 * @return The hexadecimal value.
 */
unsigned int StringToHex( const std::string& uintStr )
{
  unsigned int ret = 0;

  std::string str( uintStr );
  if( uintStr.size() <= 8 )
  {
    str.insert( 2, std::string( "ff" ) );
  }

  std::istringstream( str ) >> std::hex >> ret;

  return ret;
}

/**
 * Converts an hexadecimal value into a string.
 * @param[in] value. An hexadecimal value.
 * @return The hexadecimal value packed inside a string.
 */
std::string HexToString( unsigned int value )
{
  std::ostringstream stream;
  stream << std::hex << value;
  return stream.str();
}

/**
 * Converts an ARGB color packed in 4 byte unsigned int into a Vector4 color used in Dali.
 * @param[in] color An ARGB color packed in an unsigned int.
 * @param[out] retColor A Vector4 with the converted color.
 */
void UintColorToVector4( unsigned int color, Vector4& retColor )
{
  retColor.a = static_cast<float>( ( color & 0xFF000000 ) >> 24 ) / 255.f;
  retColor.r = static_cast<float>( ( color & 0x00FF0000 ) >> 16 ) / 255.f;
  retColor.g = static_cast<float>( ( color & 0x0000FF00 ) >> 8 ) / 255.f;
  retColor.b = static_cast<float>( color & 0x000000FF ) / 255.f;
}

/**
 * Converts an ARGB Vector4 color into a 4 byte packed inside an unsigned int.
 * @param[in] color A Vector4 with an ARGB color.
 * @return An unsigned int with the converted color.
 */
unsigned int Vector4ColorToUint( const Vector4& color )
{
  unsigned int retColor = 0;

  retColor = static_cast<unsigned int>( color.a * 255.f ) << 24;
  retColor += static_cast<unsigned int>( color.r * 255.f ) << 16;
  retColor += static_cast<unsigned int>( color.g * 255.f ) << 8;
  retColor += static_cast<unsigned int>( color.b * 255.f );

  return retColor;
}

/**
 * Converts a color packed inside a string into an ARGB Vector4 color.
 * The string color could be in hexadecimal ( 0xFF0000FF ), webcolor ( #0000FF or #00F ) or some constant values:
 * black, white, red, green, blue, yellow, magenta, cyan, transparent.
 * @param[in] colorStr A color packed inside a string..
 * @param[out] retColor A color packed inside a Vector4.
 */
void ColorStringToVector4( const std::string& colorStr, Vector4& retColor )
{
  std::string subStr1 = colorStr.substr( 0, 1 );
  std::string subStr2 = colorStr.substr( 0, 2 );

  if( WEB_COLOR_TOKEN == subStr1 )
  {
    std::string webColor( colorStr.begin() + 1, colorStr.end() );
    if( 3 == webColor.size() )                      // 3 component web color #F00 (red)
    {
      webColor.insert( 2, &( webColor[2] ), 1 );
      webColor.insert( 1, &( webColor[1] ), 1 );
      webColor.insert( 0, &( webColor[0] ), 1 );
      webColor.insert( 0, ALPHA_ONE );
    }
    else if( 6 == webColor.size() )                 // 6 component web color #FF0000 (red)
    {
      webColor.insert( 0, ALPHA_ONE );
    }
    webColor.insert( 0, HEX_COLOR_TOKEN );

    UintColorToVector4( StringToHex( webColor ), retColor );
  }
  else if( CaseInsensitiveComparison( HEX_COLOR_TOKEN, subStr2 ) )
  {
    UintColorToVector4( StringToHex( colorStr ), retColor );
  }
  else if( CaseInsensitiveComparison( BLACK_COLOR, colorStr ) )
  {
    retColor = Color::BLACK;
  }
  else if( CaseInsensitiveComparison( WHITE_COLOR, colorStr ) )
  {
    retColor = Color::WHITE;
  }
  else if( CaseInsensitiveComparison( RED_COLOR, colorStr ) )
  {
    retColor = Color::RED;
  }
  else if( CaseInsensitiveComparison( GREEN_COLOR, colorStr ) )
  {
    retColor = Color::GREEN;
  }
  else if( CaseInsensitiveComparison( BLUE_COLOR, colorStr ) )
  {
    retColor = Color::BLUE;
  }
  else if( CaseInsensitiveComparison( YELLOW_COLOR, colorStr ) )
  {
    retColor = Color::YELLOW;
  }
  else if( CaseInsensitiveComparison( MAGENTA_COLOR, colorStr ) )
  {
    retColor = Color::MAGENTA;
  }
  else if( CaseInsensitiveComparison( CYAN_COLOR, colorStr ) )
  {
    retColor = Color::CYAN;
  }
  else if( CaseInsensitiveComparison( TRANSPARENT_COLOR, colorStr ) )
  {
    retColor = Color::TRANSPARENT;
  }
}

/**
 * Converts a color packed inside a Vector4 into a string.
 * The string color could be in hexadecimal ( 0xFF0000FF ), webcolor ( #0000FF or #00F ) or some constant values:
 * black, white, red, green, blue, yellow, magenta, cyan, transparent.
 * @param[in] color A color packed inside a Vector4
 * @return The color packed inside a string.
 */
std::string Vector4ToColorString( const Vector4& color )
{
  std::string colorStr;

  if( Color::BLACK == color )
  {
    colorStr = BLACK_COLOR;
  }
  else if( Color::WHITE == color )
  {
    colorStr = WHITE_COLOR;
  }
  else if( Color::RED == color )
  {
    colorStr = RED_COLOR;
  }
  else if( Color::GREEN == color )
  {
    colorStr = GREEN_COLOR;
  }
  else if( Color::BLUE == color )
  {
    colorStr = BLUE_COLOR;
  }
  else if( Color::YELLOW == color )
  {
    colorStr = YELLOW_COLOR;
  }
  else if( Color::MAGENTA == color )
  {
    colorStr = MAGENTA_COLOR;
  }
  else if( Color::CYAN == color )
  {
    colorStr = CYAN_COLOR;
  }
  else if( Color::TRANSPARENT == color )
  {
    colorStr = TRANSPARENT_COLOR;
  }
  else
  {
    colorStr = HEX_COLOR_TOKEN + HexToString( Vector4ColorToUint( color ) );
  }

  return colorStr;
}

/**
 * Skips any unnecessary white space.
 * @param[in,out] it Iterator pointing to the current character of the markup string which is being parsed.
 * @param[in] endIt Iterator pointing to the end of the markup string.
 */
void SkipWhiteSpace( std::string::const_iterator& it, const std::string::const_iterator& endIt )
{
  bool found = false;
  for( ; ( !found ) && ( it != endIt ); ++it )
  {
    if( !isspace( *it ) )
    {
      found = true;
      --it;
    }
  }
}

/**
 * Adds a line separator 'LF' character to the given styled text array.
 * @param[in] style The current style.
 * @param[in,out] styledTextArray The given styled text array.
 */
void AddNewLineChar( const TextStyle& style, StyledTextArray& styledTextArray )
{
  const Text text( LINE_SEPARATOR_LF_STRING );
  styledTextArray.push_back( StyledText( text, style ) );
}

/**
 * Adds text to the given styled text array.
 * It splits the text in characters.
 * @param[in] textToBeStored The text to be stored.
 * @param[in] styleToBeStored The current style.
 * @param[in,out] styledTextArray The given styled text array.
 */
void AddText( const std::string& textToBeStored, const TextStyle& styleToBeStored, StyledTextArray& styledTextArray )
{
  const Text text( textToBeStored );
  for( size_t i = 0, length = text.GetLength(); i < length; ++i )
  {
    styledTextArray.push_back( StyledText( Text( text[i] ), styleToBeStored ) );
  }
}

/**
 * Splits the tag string into the tag name and its properties.
 * @param[in] tag The tag string with its pairs property, value.
 * @param[out] tagName The name of the tag.
 * @param[out] properties Vector of properties.
 */
void ParseProperties( const std::string& tag, std::string& tagName, std::vector<Property>& properties )
{
  // Find first the tag name.
  bool found = false;
  bool isQuotationOpen = false;
  std::string::const_iterator it, endIt;
  for( it = tag.begin(), endIt = tag.end(); ( !found ) && ( it != endIt ); ++it )
  {
    const char character( *it );
    if( !isspace( character ) )
    {
      tagName += character;
    }
    else
    {
      found = true;
    }
  }
  SkipWhiteSpace( it, endIt );

  // Find the properties.
  std::string name;
  std::string value;
  bool addToNameValue = true;
  for( ; it != endIt; ++it )
  {
    const char character( *it );
    if( isspace( character ) && !isQuotationOpen )
    {
      if( !name.empty() && !value.empty() )
      {
        // Every time a white space is found, a new property is created and stored in the properties vector.
        properties.push_back( Property( name, value ) );
        name .clear();
        value.clear();
        addToNameValue = true; // next read characters will be added to the name.
      }
    }
    else if( EQUAL == character ) // '='
    {
      addToNameValue = false; // next read characters will be added to the value.
      SkipWhiteSpace( it, endIt );
    }
    else if( QUOTATION_MARK == character ) // '\''
    {
      // Do not add quotation marks to neither name nor value.
      isQuotationOpen = !isQuotationOpen;
    }
    else
    {
      // Adds characters to the name or the value.
      if( addToNameValue )
      {
        name += character;
      }
      else
      {
        value += character;
      }
    }
  }
  if( !name.empty() && !value.empty() )
  {
    // Checks if the last property needs to be added.
    properties.push_back( Property( name, value ) );
  }
}

/**
 * It parses a tag and its properties if the given iterator \e it is pointing at a tag beginning.
 * @param[in,out] it Iterator pointing to the current character of the markup string which is being parsed.
 * @param[in] endIt Iterator pointing to the end of the markup string.
 * @param[out] tag Name of the tag.
 * @param[out] isEndTag Whether the tag is and end tag i.e </tag_name> or not.
 * @param[out] properties The vector with tag properties.
 * @return \e true if the iterator \e it is pointing a markup tag. Otherwise \e false.
 */
bool IsTag( std::string::const_iterator& it, const std::string::const_iterator& endIt, std::string& tag, bool& isEndTag, std::vector<Property>& properties )
{
  bool isTag = false;
  bool isQuotationOpen = false;
  bool propertiesFound = false;
  std::string tagString;

  const char character( *it );
  if( LESS_THAN == character ) // '<'
  {
    // if the iterator is pointing to a '<' character, then check if it's a markup tag is needed.
    ++it;
    if( it != endIt )
    {
      SkipWhiteSpace( it, endIt );

      for( ; ( !isTag ) && ( it != endIt ); ++it )
      {
        const char character( *it );

        if( SLASH == character ) // '/'
        {
          // if the tag has a '/' then it's an end or empty tag.
          isEndTag = true;

          if( ( it + 1 != endIt ) && ( isspace( *( it + 1 ) ) )  && ( !isQuotationOpen ) )
          {
            ++it;
            SkipWhiteSpace( it, endIt );
            --it;
          }
        }
        else if( GREATER_THAN == character ) // '>'
        {
          isTag = true;
        }
        else if(QUOTATION_MARK == character)
        {
          isQuotationOpen = !isQuotationOpen;
          tagString += character;
        }
        else if( isspace( character ) ) // ' '
        {
          // If the tag contains white spaces then it may have properties.
          if ( !isQuotationOpen )
          {
            propertiesFound = true;
          }
          tagString += character;
        }
        else
        {
          // If it's not any of the 'special' characters then just add it to the tag string.
          tagString += character;
        }
      }
      --it;
    }
  }

  // If the tag string has white spaces, then parse the properties is needed.
  if( propertiesFound )
  {
    ParseProperties( tagString, tag, properties );
  }
  else
  {
    tag = tagString;
  }

  return isTag;
}

} // namespace

void GetStyledTextArray( const std::string& markupString, StyledTextArray& styledTextArray, bool scanForMarkup )
{
  styledTextArray.clear();

  if ( !scanForMarkup )
  {
    const Text text( markupString );
    const std::size_t size = text.GetLength();

    styledTextArray.resize( size, StyledText( Text(), TextStyle() ) );

    std::size_t index = 0;
    for( StyledTextArray::iterator it = styledTextArray.begin(), endIt = styledTextArray.end(); it != endIt; ++it, ++index )
    {
      StyledText& styledText( *it );

      styledText.mText.Append( text[index] );
    }
    return;
  }

  TextStyle defaultStyle;
  std::stack<TextStyle> styleStack;

  styleStack.push( defaultStyle );
  TextStyle currentStyle = styleStack.top();
  std::string textToBeStored;
  TextStyle styleToBeStored( currentStyle );
  for( std::string::const_iterator it = markupString.begin(), endIt = markupString.end(); it != endIt; ++it )
  {
    std::string tag;
    bool isEndTag = false;
    std::vector<Property> tagProperties;
    if( IsTag( it, endIt, tag, isEndTag, tagProperties ) )
    {
      if( CaseInsensitiveComparison( XHTML_I_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          newStyle.SetItalics( true );
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <i></i>
      else if( CaseInsensitiveComparison( XHTML_U_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          newStyle.SetUnderline( true );
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <u></u>
      else if( CaseInsensitiveComparison( XHTML_B_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          newStyle.SetWeight( TextStyle::BOLD );
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <b></b>
      else if( CaseInsensitiveComparison( XHTML_BR_TAG, tag ) )
      {
        if( isEndTag )
        {
          AddText( textToBeStored, styleToBeStored, styledTextArray );
          AddNewLineChar( currentStyle, styledTextArray );
          textToBeStored.clear();
        }
      } // <br/>
      else if( CaseInsensitiveComparison( XHTML_FONT_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          for( std::vector<Property>::const_iterator it = tagProperties.begin(), endIt = tagProperties.end(); it != endIt; ++it )
          {
            const Property& property( *it );
            if( CaseInsensitiveComparison( XHTML_FACE_PROPERTY, property.name ) )
            {
              newStyle.SetFontName( property.value );
            }
            else if( CaseInsensitiveComparison( XHTML_STYLE_PROPERTY, property.name ) )
            {
              newStyle.SetFontStyle( property.value );
            }
            else if( CaseInsensitiveComparison( XHTML_COLOR_PROPERTY, property.name ) )
            {
              Vector4 color;
              ColorStringToVector4( property.value, color );
              newStyle.SetTextColor( color );
            }
            else if( CaseInsensitiveComparison( XHTML_SIZE_PROPERTY, property.name ) )
            {
              newStyle.SetFontPointSize( PointSize( StringToFloat( property.value ) ) );
            }
          }
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <font></font>
      else if( CaseInsensitiveComparison( XHTML_SHADOW_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          Vector4 color( TextStyle::DEFAULT_SHADOW_COLOR );
          Vector2 offset( TextStyle::DEFAULT_SHADOW_OFFSET );
          for( std::vector<Property>::const_iterator it = tagProperties.begin(), endIt = tagProperties.end(); it != endIt; ++it )
          {
            const Property& property( *it );
            if( CaseInsensitiveComparison( XHTML_PARAM_X_PROPERTY, property.name ) )
            {
              offset.x = StringToFloat( property.value );
            }
            else if( CaseInsensitiveComparison( XHTML_PARAM_Y_PROPERTY, property.name ) )
            {
              offset.y = StringToFloat( property.value );
            }
            else if( CaseInsensitiveComparison( XHTML_COLOR_PROPERTY, property.name ) )
            {
              ColorStringToVector4( property.value, color );
            }
          }
          newStyle.SetShadow( true, color, offset );
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <shadow></shadow>
      else if( CaseInsensitiveComparison( XHTML_GLOW_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          Vector4 color( TextStyle::DEFAULT_GLOW_COLOR );
          float intensity = TextStyle::DEFAULT_GLOW_INTENSITY;
          for( std::vector<Property>::const_iterator it = tagProperties.begin(), endIt = tagProperties.end(); it != endIt; ++it )
          {
            const Property& property( *it );
            if( CaseInsensitiveComparison( XHTML_PARAM_PROPERTY, property.name ) )
            {
              intensity = StringToFloat( property.value );
            }
            else if( CaseInsensitiveComparison( XHTML_COLOR_PROPERTY, property.name ) )
            {
              ColorStringToVector4( property.value, color );
            }
          }
          newStyle.SetGlow( true, color, intensity );
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <glow></glow>
      else if( CaseInsensitiveComparison( XHTML_OUTLINE_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          Vector4 color( TextStyle::DEFAULT_OUTLINE_COLOR );
          Vector2 thickness( TextStyle::DEFAULT_OUTLINE_THICKNESS );
          for( std::vector<Property>::const_iterator it = tagProperties.begin(), endIt = tagProperties.end(); it != endIt; ++it )
          {
            const Property& property( *it );
            if( CaseInsensitiveComparison( XHTML_PARAM_X_PROPERTY, property.name ) )
            {
              thickness.x = StringToFloat( property.value );
            }
            else if( CaseInsensitiveComparison( XHTML_PARAM_Y_PROPERTY, property.name ) )
            {
              thickness.y = StringToFloat( property.value );
            }
            else if( CaseInsensitiveComparison( XHTML_COLOR_PROPERTY, property.name ) )
            {
              ColorStringToVector4( property.value, color );
            }
          }
          newStyle.SetOutline( true, color, thickness );
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <outline></outline>
      else if( CaseInsensitiveComparison( XHTML_SMOOTH_EDGE_TAG, tag ) )
      {
        if( !isEndTag )
        {
          TextStyle newStyle( currentStyle );
          for( std::vector<Property>::const_iterator it = tagProperties.begin(), endIt = tagProperties.end(); it != endIt; ++it )
          {
            const Property& property( *it );
            if( CaseInsensitiveComparison( XHTML_PARAM_PROPERTY, property.name ) )
            {
              newStyle.SetSmoothEdge( StringToFloat( property.value ) );
            }
          }
          styleStack.push( newStyle );
          currentStyle = styleStack.top();
        }
        else
        {
          styleStack.pop();
          currentStyle = styleStack.top();
        }
      } // <smooth></smooth>
    } // end if( IsTag() )
    else
    {
      char character( *it );

      // Adding < or > special character.
      if( ( BACK_SLASH == character ) && ( it + 1 != endIt ) )
      {
        const char nextChar( *( it + 1 ) );
        if( ( LESS_THAN == nextChar ) || ( GREATER_THAN == nextChar ) )
        {
          character = nextChar;
          ++it;
        }
      }
      else if( ( LINE_SEPARATOR_CR == character ) && ( it + 1 != endIt ) )
      {
        if( LINE_SEPARATOR_LF == *( it + 1 ) )
        {
          character = LINE_SEPARATOR_LF;
          ++it;
        }
      }

      if( styleToBeStored != currentStyle )
      {
        if( !textToBeStored.empty() )
        {
          AddText( textToBeStored, styleToBeStored, styledTextArray );
          textToBeStored.clear();
        }
        styleToBeStored = currentStyle;
      }
      textToBeStored.insert( textToBeStored.end(), character );
    }
  }
  if( !textToBeStored.empty() )
  {
    AddText( textToBeStored, styleToBeStored, styledTextArray );
    textToBeStored.clear();
  }
}

void GetPlainString( const StyledTextArray& styledTextArray, std::string& plainString )
{
  // First step is put all simultaneous characters with same style together.
  for( StyledTextArray::const_iterator it = styledTextArray.begin(), endIt = styledTextArray.end(); it != endIt; ++it )
  {
    const StyledText& styledText( *it );
    plainString += styledText.mText.GetText();
  }
}

void GetMarkupString( const StyledTextArray& styledTextArray, std::string& markupString )
{
  const std::string WHITE_SPACE( " " );

  TextStyle previousStyle;
  StyledText newStyledText;
  StyledTextArray compressedStyledTextArray;

  markupString.clear();

  // First step is put all simultaneous characters with same style together.
  for( StyledTextArray::const_iterator it = styledTextArray.begin(), endIt = styledTextArray.end(); it != endIt; ++it )
  {
    const StyledText& styledText( *it );

    if( previousStyle != styledText.mStyle )
    {
      if( !newStyledText.mText.IsEmpty() )
      {
        compressedStyledTextArray.push_back( newStyledText );
      }
      newStyledText = StyledText();
      newStyledText.mStyle = styledText.mStyle;
    }

    if( !styledText.mText.IsEmpty() )
    {
      const char character = styledText.mText.GetText()[0];
      if( ( character == LESS_THAN ) || ( character == GREATER_THAN ) )
      {
        newStyledText.mText.Append( Text( std::string( &BACK_SLASH, 1 ) ) );
      }
    }

    newStyledText.mText.Append( styledText.mText );

    previousStyle = newStyledText.mStyle;
  }

  //Add the last characters.
  if( !newStyledText.mText.IsEmpty() )
  {
    compressedStyledTextArray.push_back( newStyledText );
  }

  // Write markup string.
  const std::string lineSeparatorStr( &LINE_SEPARATOR_LF );
  const Text lineSeparator( lineSeparatorStr );

  const TextStyle defaultStyle;
  for( StyledTextArray::const_iterator it = compressedStyledTextArray.begin(), endIt = compressedStyledTextArray.end(); it != endIt; ++it )
  {
    const StyledText& styledText( *it );

    bool isItalics = styledText.mStyle.IsItalicsEnabled();
    bool isBold = defaultStyle.GetWeight() != styledText.mStyle.GetWeight();
    bool isUnderline = styledText.mStyle.IsUnderlineEnabled();
    bool hasFontFace = defaultStyle.GetFontName() != styledText.mStyle.GetFontName();
    bool hasFontStyle = defaultStyle.GetFontStyle() != styledText.mStyle.GetFontStyle();
    bool hasFontSize = fabsf( defaultStyle.GetFontPointSize() - styledText.mStyle.GetFontPointSize() ) > GetRangedEpsilon( defaultStyle.GetFontPointSize(), styledText.mStyle.GetFontPointSize() );
    bool hasFontColor = defaultStyle.GetTextColor() != styledText.mStyle.GetTextColor();

    bool hasSmooth = fabsf( defaultStyle.GetSmoothEdge() - styledText.mStyle.GetSmoothEdge() ) > GetRangedEpsilon( defaultStyle.GetSmoothEdge(), styledText.mStyle.GetSmoothEdge() );
    bool hasShadowColor = defaultStyle.GetShadowColor() != styledText.mStyle.GetShadowColor();
    bool hasShadowParams = defaultStyle.GetShadowOffset() != styledText.mStyle.GetShadowOffset();
    bool hasGlowColor = defaultStyle.GetGlowColor() != styledText.mStyle.GetGlowColor();
    bool hasGlowParams = fabsf( defaultStyle.GetGlowIntensity() - styledText.mStyle.GetGlowIntensity() ) > GetRangedEpsilon( defaultStyle.GetGlowIntensity(), styledText.mStyle.GetGlowIntensity() );
    bool hasOutlineColor = defaultStyle.GetOutlineColor() != styledText.mStyle.GetOutlineColor();
    bool hasOutlineParams = defaultStyle.GetOutlineThickness() != styledText.mStyle.GetOutlineThickness();

    // Write font info.
    if( hasFontFace || hasFontStyle || hasFontSize || hasFontColor )
    {
      markupString += LESS_THAN + XHTML_FONT_TAG;

      if( hasFontFace )
      {
        markupString += WHITE_SPACE + XHTML_FACE_PROPERTY + EQUAL + QUOTATION_MARK + styledText.mStyle.GetFontName() + QUOTATION_MARK; // face=''
      }

      if( hasFontStyle )
      {
        markupString += WHITE_SPACE + XHTML_STYLE_PROPERTY + EQUAL + QUOTATION_MARK + styledText.mStyle.GetFontStyle() + QUOTATION_MARK; // style=''
      }

      if( hasFontSize )
      {
        markupString += WHITE_SPACE + XHTML_SIZE_PROPERTY + EQUAL + QUOTATION_MARK + FloatToString( styledText.mStyle.GetFontPointSize() ) + QUOTATION_MARK; // size=''
      }

      if( hasFontColor )
      {
        markupString += WHITE_SPACE + XHTML_COLOR_PROPERTY + EQUAL + QUOTATION_MARK + Vector4ToColorString( styledText.mStyle.GetTextColor() ) + QUOTATION_MARK; // color=''
      }

      markupString += GREATER_THAN;
    } // <font>

    // Write italics.
    if( isItalics )
    {
      markupString += LESS_THAN + XHTML_I_TAG + GREATER_THAN;
    } // <i>

    // Write bold.
    if( isBold )
    {
      markupString += LESS_THAN + XHTML_B_TAG + GREATER_THAN;
    } // <b>

    // Write underline.
    if( isUnderline )
    {
      markupString += LESS_THAN + XHTML_U_TAG + GREATER_THAN;
    } // <u>

    // Write smooth.
    if( hasSmooth )
    {
      markupString += LESS_THAN + XHTML_SMOOTH_EDGE_TAG + WHITE_SPACE + XHTML_PARAM_PROPERTY + EQUAL + QUOTATION_MARK + FloatToString( styledText.mStyle.GetSmoothEdge() ) + QUOTATION_MARK + GREATER_THAN;
    }

    // Write shadow.
    if( styledText.mStyle.IsShadowEnabled() )
    {
      markupString += LESS_THAN + XHTML_SHADOW_TAG;

      if( hasShadowColor )
      {
        markupString += WHITE_SPACE + XHTML_COLOR_PROPERTY + EQUAL + QUOTATION_MARK + Vector4ToColorString( styledText.mStyle.GetShadowColor() ) + QUOTATION_MARK;
      }

      if( hasShadowParams )
      {
        markupString += WHITE_SPACE + XHTML_PARAM_X_PROPERTY + EQUAL + QUOTATION_MARK + FloatToString( styledText.mStyle.GetShadowOffset().x ) + QUOTATION_MARK;
        markupString += WHITE_SPACE + XHTML_PARAM_Y_PROPERTY + EQUAL + QUOTATION_MARK + FloatToString( styledText.mStyle.GetShadowOffset().y ) + QUOTATION_MARK;
      }

      markupString += GREATER_THAN;
    }

    // Write glow.
    if( styledText.mStyle.IsGlowEnabled() )
    {
      markupString += LESS_THAN + XHTML_GLOW_TAG;

      if( hasGlowColor )
      {
        markupString += WHITE_SPACE + XHTML_COLOR_PROPERTY + EQUAL + QUOTATION_MARK + Vector4ToColorString( styledText.mStyle.GetGlowColor() ) + QUOTATION_MARK; // color=''
      }

      if( hasGlowParams )
      {
        markupString += WHITE_SPACE + XHTML_PARAM_PROPERTY + EQUAL + QUOTATION_MARK + FloatToString( styledText.mStyle.GetGlowIntensity() ) + QUOTATION_MARK; // param=''
      }

      markupString += GREATER_THAN;
    } // <glow>

    // Write outline.
    if( styledText.mStyle.IsOutlineEnabled() )
    {
      markupString += LESS_THAN + XHTML_OUTLINE_TAG;

      if( hasOutlineColor )
      {
        markupString += WHITE_SPACE + XHTML_COLOR_PROPERTY + EQUAL + QUOTATION_MARK + Vector4ToColorString( styledText.mStyle.GetOutlineColor() ) + QUOTATION_MARK; // color = ''
      }

      if( hasOutlineParams )
      {
        markupString += WHITE_SPACE + XHTML_PARAM_X_PROPERTY + EQUAL + QUOTATION_MARK + FloatToString( styledText.mStyle.GetOutlineThickness().x ) + QUOTATION_MARK; // paramx=''
        markupString += WHITE_SPACE + XHTML_PARAM_Y_PROPERTY + EQUAL + QUOTATION_MARK + FloatToString( styledText.mStyle.GetOutlineThickness().y ) + QUOTATION_MARK; // paramy=''
      }

      markupString += GREATER_THAN;
    } // <outline>

    // Write text.
    if( styledText.mText[0] == lineSeparator[0] )
    {
      markupString += LESS_THAN + XHTML_BR_TAG + WHITE_SPACE + SLASH + GREATER_THAN; // <br />
    }
    else
    {
      markupString += styledText.mText.GetText();
    }

    // Write outline close tag.
    if( styledText.mStyle.IsOutlineEnabled() )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_OUTLINE_TAG + GREATER_THAN ); // </outline>
    }

    // Write glow close tag.
    if( styledText.mStyle.IsGlowEnabled() )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_GLOW_TAG + GREATER_THAN ); // </glow>
    }

    // Write shadow close tag.
    if( styledText.mStyle.IsShadowEnabled() )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_SHADOW_TAG + GREATER_THAN ); // </shadow>
    }

    // Write smooth close tag.
    if( hasSmooth )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_SMOOTH_EDGE_TAG + GREATER_THAN ); // </smooth>
    }

    // Write underline close tag.
    if( isUnderline )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_U_TAG + GREATER_THAN ); // </u>
    }

    // Write bold close tag.
    if( isBold )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_B_TAG + GREATER_THAN ); // </b>
    }

    // Write italics close tag.
    if( isItalics )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_I_TAG + GREATER_THAN ); // </i>
    }

    // Write font close tag.
    if( hasFontFace || hasFontStyle || hasFontSize || hasFontColor )
    {
      markupString += LESS_THAN + ( SLASH + XHTML_FONT_TAG + GREATER_THAN ); // </font>
    }
  }
}

void SetTextStyle( StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask )
{
  if( !styledTextArray.empty() )
  {
    const size_t size = styledTextArray.size() - 1;
    SetTextStyleToRange( styledTextArray, style, mask, 0, size );
  }
}

void SetTextStyle( const Text& text, StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask )
{
  if( !text.IsEmpty() )
  {
    const size_t size = text.GetLength();

    for( size_t i = 0; i < size; ++i )
    {
      StyledText styledText;
      styledText.mText = Text( text[i] );
      styledText.mStyle = style;

      styledTextArray.push_back( styledText );
    }

    SetTextStyleToRange( styledTextArray, style, mask, 0, size - 1 );
  }
}

void SetTextStyleToRange( StyledTextArray& styledTextArray, const TextStyle& style, TextStyle::Mask mask, std::size_t begin, std::size_t end )
{
  const size_t size = styledTextArray.size();
  DALI_ASSERT_ALWAYS( begin < size );
  DALI_ASSERT_ALWAYS( end < size );

  for( StyledTextArray::iterator it = styledTextArray.begin() + std::min(begin, end), endIt = styledTextArray.begin() + std::max(begin, end) + 1; it != endIt; ++it )
  {
    StyledText& styledText( *it );

    styledText.mStyle.Copy( style, mask );
  } // for loop
}

} // namespace MarkupProcessor

} // namespace Toolkit

} // namespace Dali
