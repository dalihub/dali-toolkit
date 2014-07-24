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

// CLASS HEADER
#include <dali-toolkit/internal/builder/json-parser-impl.h>
// EXTERNAL
#include <cstring>

#include <dali-toolkit/internal/builder/tree-node-manipulator.h>
#include <dali-toolkit/internal/builder/json-parser-state.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

namespace
{

const char ERROR_DESCRIPTION_NONE[] = "No Error";

template <typename IteratorType,typename EndIteratorType>
inline IteratorType Advance(IteratorType& iter, EndIteratorType& end, int n)
{
  for(int i =0; i < n; ++i)
  {
    ++iter;
  }
  return iter;
}

} // anon namespace


JsonParser::JsonParser()
  : mRoot(NULL),
    mErrorDescription(ERROR_DESCRIPTION_NONE),
    mErrorPosition(0),
    mErrorLine(0),
    mErrorColumn(0),
    mNumberOfChars(0),
    mNumberOfNodes(0)
{
}

JsonParser::JsonParser(const TreeNode& tree)
  : mRoot(NULL),
    mErrorDescription(ERROR_DESCRIPTION_NONE),
    mErrorPosition(0),
    mErrorLine(0),
    mErrorColumn(0),
    mNumberOfChars(0),
    mNumberOfNodes(0)
{
  mRoot = TreeNodeManipulator::Copy( tree, mNumberOfNodes, mNumberOfChars );

  mSources.push_back( VectorChar( (sizeof(char) * mNumberOfChars) ) );

  VectorChar& buffer = mSources.back();

  VectorCharIter start = buffer.begin();

  TreeNodeManipulator modify(mRoot);

  modify.MoveStrings(start, buffer.end());
}

JsonParser::~JsonParser()
{
  if(mRoot)
  {
    TreeNodeManipulator modify(mRoot);
    modify.RemoveChildren();
    delete mRoot;
    mRoot = NULL;
  }
}

int JsonParser::Parse(const std::string& source)
{
  mSources.push_back( VectorChar(source.begin(), source.end()) );

  JsonParserState parserState(mRoot);

  if( parserState.ParseJson(mSources.back()) )
  {
    mRoot = parserState.GetRoot();

    mNumberOfChars += parserState.GetParsedStringSize();
    mNumberOfNodes += parserState.GetCreatedNodeCount();

    mErrorDescription   = ERROR_DESCRIPTION_NONE;
    mErrorPosition      = 0;
    mErrorLine          = 0;
    mErrorColumn        = 0;
  }
  else
  {
    mErrorDescription   = parserState.GetErrorDescription();
    if(NULL == mErrorDescription)
    {
      mErrorDescription = ERROR_DESCRIPTION_NONE;
    }
    mErrorPosition      = parserState.GetErrorPosition();
    mErrorLine          = parserState.GetErrorLineNumber();
    mErrorColumn        = parserState.GetErrorColumn();
  }

  return mRoot != NULL;
}


const TreeNode* JsonParser::GetRoot() const
{
  return mRoot;
}

bool JsonParser::ParseError() const
{
  return mErrorDescription != ERROR_DESCRIPTION_NONE;
}

int JsonParser::GetErrorPosition() const
{
  return mErrorPosition;
}

std::string JsonParser::GetErrorDescription() const
{
  return std::string(mErrorDescription);
}

int JsonParser::GetErrorLineNumber() const
{
  return mErrorLine;
}

int JsonParser::GetErrorColumn() const
{
  return mErrorColumn;
}

void JsonParser::Pack(void)
{
  mSources.push_back( VectorChar( (sizeof(char) * mNumberOfChars) ) );

  VectorChar& buffer = mSources.back();

  VectorCharIter start = buffer.begin();

  TreeNodeManipulator modify(mRoot);

  modify.MoveStrings(start, buffer.end());

  mSources.erase( mSources.begin(), --mSources.end() );
}

void JsonParser::Write(std::ostream& output, int indent) const
{
  TreeNodeManipulator modify(mRoot);
  modify.Write(output, indent);
}


} // namespace Internal

} // namespace Toolkit

} // namespace Dali
