//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali-toolkit/internal/builder/json-parser-state.h>

// EXTERNAL
#include <algorithm>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

// true if character represent a digit
inline bool IsDigit(char c)
{
  return (c >= '0' && c <= '9');
}

// convert string to integer
bool StringToInteger(const char *first, const char *last, int& out)
{
  int sign = 1;
  if (first != last)
  {
    if (*first == '-')
    {
      sign = -1;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }
  }

  // json error for int starting with zero
  if( 0 == (*first - '0') && (first+1 != last))
  {
    return false;
  }

  int result = 0;
  for (; first != last && IsDigit(*first); ++first)
  {
    result = 10 * result + (*first - '0');
  }
  out = result * sign;

  if(first != last)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// convert hexadecimal string to unsigned integer
bool HexStringToUnsignedInteger(const char *first, const char *last, unsigned int& out)
{
  unsigned int result = 0;
  for (; first != last; ++first)
  {
    int digit;
    if (IsDigit(*first))
    {
      digit = *first - '0';
    }
    else if (*first >= 'a' && *first <= 'f')
    {
      digit = *first - 'a' + 10;
    }
    else if (*first >= 'A' && *first <= 'F')
    {
      digit = *first - 'A' + 10;
    }
    else
    {
      break;
    }
    result = 16 * result + digit;
  }
  out = result;

  if(first != last)
  {
    return false;
  }
  else
  {
    return true;
  }
}

// convert string to floating point
bool StringToFloat(const char* first, const char* last, float& out)
{
  // sign
  float sign = 1;
  if (first != last)
  {
    if (*first == '-')
    {
      sign = -1;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }
  }

  // integer part
  float result = 0;
  for (; first != last && IsDigit(*first); ++first)
  {
    result = 10 * result + (*first - '0');
  }

  // fraction part
  if (first != last && *first == '.')
  {
    ++first;

    float inv_base = 0.1f;
    for (; first != last && IsDigit(*first); ++first)
    {
      result += (*first - '0') * inv_base;
      inv_base *= 0.1f;
    }
  }

  // result w\o exponent
  result *= sign;

  // exponent
  bool exponent_negative = false;
  int exponent = 0;
  if (first != last && (*first == 'e' || *first == 'E'))
  {
    ++first;

    if (*first == '-')
    {
      exponent_negative = true;
      ++first;
    }
    else if (*first == '+')
    {
      ++first;
    }

    if(first == last || !IsDigit(*first))
    {
      return false;
    }

    for (; first != last && IsDigit(*first); ++first)
    {
      exponent = 10 * exponent + (*first - '0');
    }
  }

  if (exponent)
  {
    float power_of_ten = 10;
    for (; exponent > 1; exponent--)
    {
      power_of_ten *= 10;
    }

    if (exponent_negative)
    {
      result /= power_of_ten;
    }
    else
    {
      result *= power_of_ten;
    }
  }

  out = result;

  if(first != last)
  {
    return false;
  }
  else
  {
    return true;
  }
}


bool IsNumber(char c)
{
  bool ret = false;
  switch( c )
  {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      ret = true;
      break;
    }
    default:
    {
      ret = false;
    }
  }
  return ret;
}

} // anon namespace


JsonParserState::JsonParserState(TreeNode* _root)
  : mRoot(_root), mCurrent(_root),
    mErrorDescription(NULL), mErrorNewLine(0), mErrorColumn(0), mErrorPosition(0),
    mNumberOfParsedChars(0), mNumberOfCreatedNodes(0), mFirstParse(false),
    mState(STATE_START)
{
  if(_root == NULL)
  {
    mFirstParse = true;
  }
}

TreeNode* JsonParserState::CreateNewNode(const char* name, TreeNode::NodeType type)
{
  TreeNode* node = NULL;

  node = TreeNodeManipulator::NewTreeNode();
  TreeNodeManipulator modifyNew(node);
  modifyNew.SetType(type);
  modifyNew.SetName(name);
  if(mRoot == NULL)
  {
    mRoot    = node;
    mCurrent = TreeNodeManipulator(mRoot);
  }
  else
  {
    mCurrent.AddChild(node);
    mCurrent = modifyNew;
  }

  ++mNumberOfCreatedNodes;

  return node;

}

TreeNode* JsonParserState::NewNode(const char* name, TreeNode::NodeType type)
{
  TreeNode* node = NULL;

  if(mFirstParse)
  {
    node = CreateNewNode(name, type);
  }
  else
  {
    // a merging parse

    if(name)
    {
      const TreeNode* found = mCurrent.GetChild(name);
      if( NULL != found )
      {
        node = const_cast<TreeNode*>(found);
      }
    }
    else
    {
      // if root node
      if( mCurrent.GetParent() == NULL )
      {
        node = mRoot;
      }
    }

    if(node)
    {
      // walk tree and deallocate children as were replacing this node
      TreeNodeManipulator modify(node);

      modify.SetName(name);
      // Set the type of the existing node, this may remove children where necessary
      // (changing from container type to value type)
      modify.SetType(type);

      mCurrent = modify;
    }
    else
    {
      // if not found then create new
      node = CreateNewNode(name, type);
    }
  }

  return node;
}

TreeNode* JsonParserState::GetRoot()
{
  return mRoot;
}

bool JsonParserState::Error(const char* description)
{
  mErrorDescription = description;
  return false;
}

bool JsonParserState::ParseWhiteSpace()
{
  bool c_comment   = false;
  bool cpp_comment = false;

  // skip white space
  char nextChar = 0;
  while(1)
  {
    char c = Char();

    if(c == '\xA')
    {
      NewLine();
    }

    if( AtLeast(2) )
    {
      nextChar = mIter[1];
    }
    else
    {
      nextChar = 0;
    }

    if( cpp_comment )
    {
      if( '\n' == c )
      {
        cpp_comment = false;
        Advance(1);
        continue; // rather than carry on as comments may be back to back
      }
    }
    else if( !c_comment && (c == '/' && nextChar == '/') )
    {
      cpp_comment = true;
    }

    if( c_comment )
    {
      if( c == '*' && nextChar == '/' )
      {
        c_comment = false;
        Advance(2);
        continue;
      }
    }
    else if( !cpp_comment && (c == '/' && nextChar == '*') )
    {
      c_comment = true;
    }

    if( ! (c_comment || cpp_comment) )
    {
      if( ! (c == '\x20' || c == '\x9' || c == '\xD' || c == '\xA' ) )
      {
        break;
      }
    }

    if( AdvanceEnded(1) )
    {
      break;
    }

  } // while(1)

  return true;
} // ParseWhiteSpace

bool JsonParserState::ParseSymbol(const std::string& symbol)
{
  if( AtLeast( symbol.size() ) )
  {
    for(int i = 0; i < static_cast<int>( symbol.size() ); ++i)
    {
      if(*mIter != symbol[i])
      {
        return false;
      }
      Advance(1);
    }
    return true;
  }
  else
  {
    return false;
  }
}

bool JsonParserState::ParseTrue()
{
  if( ParseSymbol("true") )
  {
    mCurrent.SetInteger(1);
    mCurrent.SetType(TreeNode::BOOLEAN);
    return true;
  }
  else
  {
    return Error("Unexpected character; expected symbol ie 'true'");
  }
}

bool JsonParserState::ParseFalse()
{
  if( ParseSymbol("false") )
  {
    mCurrent.SetInteger(0);
    mCurrent.SetType(TreeNode::BOOLEAN);
    return true;
  }
  else
  {
    return Error("Unexpected character; expected symbol ie 'false'");
  }
}

bool JsonParserState::ParseNULL()
{
  if( ParseSymbol("null") )
  {
    mCurrent.SetType(TreeNode::IS_NULL);
    return true;
  }
  else
  {
    return Error("Unexpected character; expected symbol ie 'null'");
  }
}

bool JsonParserState::ParseNumber()
{
  mCurrent.SetType( TreeNode::INTEGER );

  VectorCharIter first = mIter;
  char c = Char();

  if( !(c == '-' || IsNumber(c) ) )
  {
    return Error("Number must start with '-' or 0-9");
  }

  while ( IsNumber(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-' )
  {
    if (c == '.' || c == 'e' || c == 'E')
    {
      mCurrent.SetType( TreeNode::FLOAT );
    }
    Advance(1);
    c = Char();
  }

  if( mCurrent.GetType() == TreeNode::INTEGER )
  {
    int i = 0;
    if( StringToInteger(&(*first), &(*mIter), i ) )
    {
      mCurrent.SetInteger(i);
    }
    else
    {
      return Error("Bad integer number");
    }
  }

  if(mCurrent.GetType() == TreeNode::FLOAT)
  {
    float f = 0.f;
    if( StringToFloat(&(*first), &(*mIter), f) )
    {
      mCurrent.SetFloat(f);
    }
    else
    {
      return Error("Bad float number");
    }
  }

  return (mCurrent.GetType() == TreeNode::INTEGER)  || (mCurrent.GetType() == TreeNode::FLOAT);
}

char* JsonParserState::EncodeString()
{
  int substitution = 0;
  VectorCharIter first = mIter;
  VectorCharIter last  = mIter;

  while (*mIter)
  {
    if (static_cast<unsigned char>(*mIter) < '\x20')
    {
      static_cast<void>( Error("Control characters not allowed in strings") );
      return NULL;
    }
    else if (*mIter == '\\' && AtLeast(2))
    {
      switch (*(mIter+1))
      {
        case '"':
        {
          *last = '"';
          break;
        }
        case '\\':
        {
          *last = '\\';
          break;
        }
        case '/':
        {
          *last = '/';
          break;
        }
        case 'b':
        {
          *last = '\b';
          break;
        }
        case 'f':
        {
          *last = '\f';
          break;
        }
        case 'n':
        {
          *last = '\n';
          break;
        }
        case 'r':
        {
          *last = '\r';
          break;
        }
        case 't':
        {
          *last = '\t';
          break;
        }
        case 'u':
        {
          unsigned int codepoint;
          if( !AtLeast(6) )
          {
            static_cast<void>( Error("Bad unicode codepoint; not enough characters") );
            return NULL;
          }
          if ( !HexStringToUnsignedInteger(&(*(mIter + 2)), &(*(mIter + 6)), codepoint) )
          {
            static_cast<void>( Error("Bad unicode codepoint") );
            return NULL;
          }

          if (codepoint <= 0x7F)
          {
            *last = (char)codepoint;
          }
          else if (codepoint <= 0x7FF)
          {
            *last++ = (char)(0xC0 | (codepoint >> 6));
            *last = (char)(0x80 | (codepoint & 0x3F));
          }
          else if (codepoint <= 0xFFFF)
          {
            *last++ = (char)(0xE0 | (codepoint >> 12));
            *last++ = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            *last = (char)(0x80 | (codepoint & 0x3F));
          }

          Advance(4);
          break;
        } // case 'u' unicode

        default:
        {
          static_cast<void>( Error("Unrecognized escape sequence") );
          return NULL;
        }
      }

      ++last;
      Advance(2);
    }
    else if (*mIter == '{')
    {
      if((0 == substitution) && (*last != '\\'))
      {
        substitution = 1;
      }
      *last++ = *mIter;
      Advance(1);
    }
    else if (*mIter == '}')
    {
      if(substitution)
      {
        substitution++;
      }
      *last++ = *mIter;
      Advance(1);
    }
    else if (*mIter == '"')
    {
      *last = 0;
      Advance(1);
      break;
    }
    else
    {
      *last++ = *mIter;
      Advance(1);
    }

  } // while(*mIter)

  mNumberOfParsedChars += last - first;
  mNumberOfParsedChars += 1 ; // null terminator

  mCurrent.SetSubstitution( substitution > 1 );

  // return true;
  return &(*first);

} // ParseString()

bool JsonParserState::ParseJson(VectorChar& source)
{
  Reset();

  if( 0 == source.size() )
  {
    return Error("Empty source buffer to parse");
  }

  mIter = source.begin();
  mEnd  = source.end();

  char* name = NULL;
  char currentChar   = 0;
  char lastCharacter = 0;

  if( !ParseWhiteSpace() )
  {
    return false;
  }

  while(mIter != mEnd)
  {
    lastCharacter = currentChar;
    currentChar = Char();

    switch( mState )
    {
      case STATE_START:
      {
        if( '{' == currentChar )
        {
          NewNode(name, TreeNode::OBJECT);
          mState = STATE_OBJECT;
        }
        else if( '[' == currentChar )
        {
          NewNode(name, TreeNode::ARRAY);
          mState = STATE_VALUE;
        }
        else
        {
          return Error("Json must start with object {} or array []");
        }

        AdvanceSkipWhiteSpace(1);
        break;
      }
      case STATE_OBJECT:
      {
        if( '}' == currentChar )
        {
          if( !UpToParent() )
          {
            return false;
          }
          mState = STATE_VALUE;
        }
        else if ( '"' == currentChar )
        {
          mState = STATE_KEY;
        }
        else
        {
          return Error("Unexpected character");
        }

        AdvanceSkipWhiteSpace(1);
        break;
      }
      case STATE_KEY:
      {
        name = EncodeString();
        if( NULL == name )
        {
          return false;
        }
        if( !ParseWhiteSpace() )
        {
          return false;
        }
        if( ':' != Char())
        {
          return Error("Expected ':'");
        }
        if( !ParseWhiteSpace() )
        {
          return false;
        }
        mState = STATE_VALUE;

        AdvanceSkipWhiteSpace(1);
        break;
      }
      case STATE_VALUE:
      {
        if( '"' == currentChar )
        {
          Advance(1);
          NewNode(name, TreeNode::STRING);
          if( char* value = EncodeString() )
          {
            mCurrent.SetString(value);
          }
          else
          {
            return false;
          }
          if( !UpToParent() )
          {
            return false;
          }
          AdvanceSkipWhiteSpace(0);
        }
        else if( IsNumber(currentChar) || currentChar == '-' )
        {
          NewNode(name, TreeNode::IS_NULL);
          if( !ParseNumber() )
          {
            return false;
          }
          if( !UpToParent() )
          {
            return false;
          }
          AdvanceSkipWhiteSpace(0);
        }
        else if( '{' == currentChar )
        {
          NewNode(name, TreeNode::OBJECT);
          mState = STATE_OBJECT;
          AdvanceSkipWhiteSpace(1);
        }
        else if( '}' == currentChar )
        {
          if(',' == lastCharacter)
          {
            return Error("Expected a value");
          }

          if(mCurrent.GetType() != TreeNode::OBJECT)
          {
            return Error("Mismatched array definition");
          }

          if(mCurrent.GetParent() == NULL)
          {
            mState = STATE_END;
          }
          else
          {
            if( !UpToParent() )
            {
              return false;
            }
          }
          AdvanceSkipWhiteSpace(1);
        }
        else if( '[' == currentChar )
        {
          NewNode(name, TreeNode::ARRAY);
          mState = STATE_VALUE;
          AdvanceSkipWhiteSpace(1);
        }
        else if( ']' == currentChar )
        {
          if(',' == lastCharacter)
          {
            return Error("Expected a value");
          }

          if(mCurrent.GetType() != TreeNode::ARRAY)
          {
            return Error("Mismatched braces in object definition");
          }

          if(mCurrent.GetParent() == NULL)
          {
            mState = STATE_END;
          }
          else
          {
            if( !UpToParent() )
            {
              return false;
            }
          }
          AdvanceSkipWhiteSpace(1);
        }
        else if( 't' == currentChar )
        {
          NewNode(name, TreeNode::BOOLEAN);
          if( !ParseTrue() )
          {
            return false;
          }
          if( !UpToParent() )
          {
            return false;
          }
          AdvanceSkipWhiteSpace(0);
        }
        else if( 'n' == currentChar )
        {
          NewNode(name, TreeNode::IS_NULL);
          if( !ParseNULL() )
          {
            return false;
          }
          if( !UpToParent() )
          {
            return false;
          }
          AdvanceSkipWhiteSpace(0);
        }
        else if( 'f' == currentChar)
        {
          NewNode(name, TreeNode::BOOLEAN);
          if( !ParseFalse() )
          {
            return false;
          }
          if( !UpToParent() )
          {
            return false;
          }
          AdvanceSkipWhiteSpace(0);
        }
        else if( ',' == currentChar )
        {
          if( 0 == mCurrent.Size() )
          {
            return Error("Missing Value");
          }

          if(mCurrent.GetType() == TreeNode::OBJECT)
          {
            mState = STATE_OBJECT; // to get '"' in '"key":val'
          }
          else if(mCurrent.GetType() == TreeNode::ARRAY)
          {
            mState = STATE_VALUE; // array so just get next value
          }
          else
          {
            return Error("Unexpected character");
          }
          AdvanceSkipWhiteSpace(1);
        }
        else
        {
          return Error("Unexpected character");
        }

        name = NULL;

        break;
      } // case STATE_VALUE
      case STATE_END:
      {
        return Error("Unexpected character. Json must have one object or array at its root");
        break;
      }
    } // switch(mState)

  } // while(1)

  //
  if( mState != STATE_END )
  {
    return Error("Unexpected termination character");
  }

  mIter = source.end();

  return true;

} // ParseJson


void JsonParserState::Reset()
{
  mCurrent = TreeNodeManipulator(mRoot);

  mErrorDescription   = NULL;
  mErrorNewLine       = 0;
  mErrorColumn        = 0;
  mErrorPosition      = 0;
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
