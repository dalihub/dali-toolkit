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
#include <dali-toolkit/public-api/builder/json-parser.h>

#include <memory.h>
#include <functional>
#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/builder/json-parser-impl.h>

namespace Dali
{

namespace Toolkit
{

JsonParser JsonParser::New()
{
  Internal::JsonParser* internal = new Internal::JsonParser();
  return JsonParser(internal);
}

JsonParser JsonParser::New(const TreeNode& tree)
{
  Internal::JsonParser* internal = new Internal::JsonParser(tree);
  return JsonParser(internal);
}

JsonParser::JsonParser()
{
}

JsonParser::~JsonParser()
{
}

JsonParser DownCast( BaseHandle handle )
{
  return JsonParser( dynamic_cast<Internal::JsonParser*>(handle.GetObjectPtr()) );
}

int JsonParser::Parse(const std::string& source)
{
  return GetImplementation(*this).Parse(source);
}

void JsonParser::Pack(void)
{
  return GetImplementation(*this).Pack();
}

const TreeNode* JsonParser::GetRoot() const
{
  return GetImplementation(*this).GetRoot();
}

bool JsonParser::ParseError() const
{
  return GetImplementation(*this).ParseError();
}

int JsonParser::GetErrorPosition() const
{
  return GetImplementation(*this).GetErrorPosition();
}

std::string JsonParser::GetErrorDescription() const
{
  return GetImplementation(*this).GetErrorDescription();
}

int JsonParser::GetErrorLineNumber() const
{
  return GetImplementation(*this).GetErrorLineNumber();
}

int JsonParser::GetErrorColumn() const
{
  return GetImplementation(*this).GetErrorColumn();
}

void JsonParser::Write(std::ostream& output, int indent) const
{
  return GetImplementation(*this).Write(output, indent);
}

JsonParser::JsonParser(Internal::JsonParser* internal)
  : BaseHandle(internal)
{
}

} // namespace toolkit

} // namespace Dali

