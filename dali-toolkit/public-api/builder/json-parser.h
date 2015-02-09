#ifndef __DALI_JSON_PARSER_H__
#define __DALI_JSON_PARSER_H__

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

#include <string>
#include <vector>
#include <list>
#include <ostream>

// INTERNAL INCLUDES
#include <dali-toolkit/public-api/builder/tree-node.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class JsonParser;
}

/*
 * Parses JSON
 */
class DALI_IMPORT_API JsonParser : public BaseHandle
{
public:

  /*
   * Create new parser
   * @return JsonParser
   */
  static JsonParser New();

  /*
   * Create new parser from the given tree
   * This method will deep copy the given tree.
   * @return JsonParser
   */
  static JsonParser New(const TreeNode& tree);

  /*
   * Create empty handle
   */
  JsonParser();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~JsonParser();

  /**
   * Downcast an Object handle to JsonParser if it is a JsonParser.
   * @param[in] handle Handle to an object
   * @return A handle to a JsonParser or an uninitialized handle
   */
  static JsonParser DownCast( BaseHandle handle );

  /*
   * Parse the source and construct a node tree.
   * Subsequent calls to this function will merge the trees.
   * @param source The json source to parse
   * @return zero if parsed okay, otherwise an error.
   */
  int Parse(const std::string& source);

  /*
   * Optimize memory usage by packing strings
   */
  void Pack(void);

  /*
   * Get the tree root node
   */
  const TreeNode* GetRoot() const;

  /*
   * Get the parser error flag
   * @return true if there was a parse error
   */
  bool ParseError() const;

  /*
   * Get the last error position
   * @return The character position of the most recent Parse() error
   */
  int GetErrorPosition() const;

  /*
   * Get the last error description
   * @return A string description of the error
   */
  std::string GetErrorDescription() const;

  /*
   * Get the last error line number
   * @return the line number of the most recent Parse() error.
   */
  int GetErrorLineNumber() const;

  /*
   * Get the last error line number
   * @return the line number of the most recent Parse() error.
   */
  int GetErrorColumn() const;

  /*
   * Write to output stream with optional indent
   * @param output The stream to write to
   * @param indent The indent to use
   */
  void Write(std::ostream& output, int indent) const;

public: // Not intended for application developers

  /**
   * This constructor is used by Dali New() methods
   * @param [in] parser A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL JsonParser(Internal::JsonParser* parser);
};

} // namespace Toolkit

} // namespace Dali


#endif // __DALI_JSON_PARSER_H__
