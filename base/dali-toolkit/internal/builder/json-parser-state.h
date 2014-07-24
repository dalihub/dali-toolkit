#ifndef __DALI_JSON_PARSE_STATE_H__
#define __DALI_JSON_PARSE_STATE_H__

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

#include <dali/public-api/common/dali-common.h>

#include <dali-toolkit/public-api/builder/tree-node.h>

#include <dali-toolkit/internal/builder/tree-node-manipulator.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{

/*
 * A safer std::advance()
 */
template <typename IteratorType,typename EndIteratorType>
inline int AdvanceIter(IteratorType& iter, EndIteratorType& end, int n)
{
  for(int i =0; i < n; ++i)
  {
    if(iter == end)
    {
      return n - i;
    }
    ++iter;
  }
  return n;
}

/*
 * Maintains parser state machine
 *
 * If a NULL root node is passed in the constructor then a faster non merging parse is performed (the first pass).
 * Otherwise the json tree is merged (and requires slower searching)
 */
class JsonParserState
{
public:
  /*
   * Constructor
   * @param tree Tree to start with, pass NULL if no existing tree
   */
  explicit JsonParserState(TreeNode* tree);

  /*
   * Parse json source
   * The source is modified in place
   * @param source The vector buffer to parse
   * @return true if parsed successfully
   */
  bool ParseJson(VectorChar& source);

  /*
   * Get the root node
   * @return The root TreeNode
   */
  TreeNode* GetRoot();

  /*
   * Get the error description of the last parse
   * @return The error description or NULL if no error
   */
  const char* GetErrorDescription() { return mErrorDescription; }

  /*
   * Get the error line number
   * @return The line number of the error
   */
  int GetErrorLineNumber() { return mErrorNewLine; }

  /*
   * Get the error column
   * @return The error column
   */
  int GetErrorColumn() { return mErrorColumn; }

  /*
   * Get the error position
   * @return The error position
   */
  int GetErrorPosition() { return mErrorPosition; }

  /*
   * Get the size of the string data that has been parsed
   * @return The size of string data
   */
  int GetParsedStringSize() { return mNumberOfParsedChars; };

  /*
   * Get the number of nodes created
   * @return The number of nodes
   */
  int GetCreatedNodeCount() { return mNumberOfCreatedNodes; };

private:
  VectorCharIter mIter;                ///< Current position
  VectorCharIter mStart;               ///< Start position
  VectorCharIter mEnd;                 ///< End of buffer being parsed
  TreeNode* mRoot;                     ///< Root node created
  TreeNodeManipulator mCurrent;        ///< The Current modifiable node
  const char* mErrorDescription;       ///< The error description if set
  int mErrorNewLine;                   ///< The error line number
  int mErrorColumn;                    ///< The error column
  int mErrorPosition;                  ///< The error position
  int mNumberOfParsedChars;            ///< The size of string data
  int mNumberOfCreatedNodes;           ///< The number of nodes created
  bool mFirstParse;                    ///< Flag if first parse

  /*
   * The current parse state
   */
  enum State
  {
    STATE_START,
    STATE_OBJECT,
    STATE_KEY,
    STATE_VALUE,
    STATE_END,
  };

  State mState;

  // inhibited copy construct and assignment
  JsonParserState(const JsonParserState&);
  const JsonParserState& operator=(const JsonParserState&);

  /*
   * Parse over white space
   * Increments the current position
   * @return true if no parse errors
   */
  bool ParseWhiteSpace();

  /*
   * Parse over a number, setting the current node if found
   * Increments the current position. Sets error data if parse error.
   * @return true if found, false if parse error
   */
  bool ParseNumber();

  /*
   * Parse over a symbol
   * Increments the current position. Sets error data if parse error.
   * @return true if found, false if parse error
   */
  bool ParseSymbol(const std::string& symbol);

  /*
   * Parse over 'true' symbol, setting the current node if found
   * Increments the current position. Sets error data if parse error.
   * @return true if found, false if parse error
   */
  bool ParseTrue();

  /*
   * Parse over 'false' symbol, setting the current node if found
   * Increments the current position. Sets error data if parse error.
   * @return true if found, false if parse error
   */
  bool ParseFalse();

  /*
   * Parse over 'null' symbol, setting the current node if found
   * Increments the current position. Sets error data if parse error.
   * @return true if found, false if parse error
   */
  bool ParseNULL();

  /*
   * Parse over a string from the current position and insert escaped
   * control characters in place in the string and a null terminator.
   * This function works from and modifes the current buffer position.
   * @return the start of the null terminated string
   */
  char* EncodeString();

  /*
   * Create a new node with name and type
   */
  TreeNode* CreateNewNode(const char* name, TreeNode::NodeType type);

  /*
   * Create a new node if first parse, else check if the node already
   * exists and set it to a new type
   */
  TreeNode* NewNode(const char* name, TreeNode::NodeType type);

  /*
   * Set error meta data
   * @returns always false.
   */
  bool Error(const char* description);

  /*
   * Reset state for another parse
   */
  void Reset();

  /*
   * Set current to its parent
   * @return true if we had a parent, false and error otherwise
   */
  inline bool UpToParent()
  {
    if(NULL == mCurrent.GetParent())
    {
      return Error("Attempt to walk up above root");
    }
    mCurrent = TreeNodeManipulator( mCurrent.GetParent() );
    return true;
  }

  /*
   * Get the current character
   */
  inline char Char()
  {
    return *mIter;
  }

  /*
   * @return True if there are at least n character left
   */
  inline bool AtLeast(int n)
  {
    // The standard suggests vector.end() can be decremented as
    //   iter v.back() { *--v.end() }
    // (ISO/IEC 14882:2003 C++ Standard 23.1.1/12 – Sequences)
    return (mEnd - mIter) > n;
  }

  /*
   * @return True if at the end of the data to parse
   */
  inline bool AtEnd()
  {
    return mEnd == mIter;
  }

  /*
   * Advance current position by n characters or stop at mEnd
   */
  inline void Advance(int n)
  {
    int c = AdvanceIter(mIter, mEnd, n);
    mErrorPosition += c;
    mErrorColumn   += c;
  }

  /*
   * Advance by n charaters and return true if we reached the end
   */
  inline bool AdvanceEnded(int n)
  {
    int c = AdvanceIter(mIter, mEnd, n);
    mErrorPosition += c;
    mErrorColumn   += c;
    return mEnd == mIter;
  }

  /*
   * Advance by at least n characters (stopping at mEnd) and skip any whitespace after n.
   */
  inline void AdvanceSkipWhiteSpace(int n)
  {
    int c = AdvanceIter(mIter, mEnd, n);
    mErrorPosition += c;
    mErrorColumn   += c;
    static_cast<void>( ParseWhiteSpace() );
  }

  /*
   * Increment new line counters
   */
  inline void NewLine()
  {
    ++mErrorNewLine;
    mErrorColumn = 0;
  }

};


} // namespace Internal

} // namespace Toolkit

} // namespace Dali


#endif // header
