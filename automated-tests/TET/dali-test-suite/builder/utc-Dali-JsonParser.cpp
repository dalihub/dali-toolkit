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

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali-toolkit/public-api/builder/json-parser.h>

#include <dali-toolkit-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Toolkit;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

TEST_FUNCTION( UtcDaliJsonParserMethod01, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod02, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod03, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod04, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod05, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod06, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod07, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod08, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod09, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliJsonParserMethod10, NEGATIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

std::string ReplaceQuotes(const std::string &in_s)
{
  std::string s(in_s);
  // wrong as no embedded quote but had regex link problems
  std::replace(s.begin(), s.end(), '\'', '"');
  return s;
}


static void UtcDaliJsonParserMethod01()
{
  ToolkitTestApplication application;

  tet_infoline("JSON basic test");

  std::string s1( ReplaceQuotes("\
{                                         \
  'string':'value2',                      \
  'integer':2,                            \
  'float':2.0,                            \
  'boolean':true,                         \
  'nil':null,                             \
  'array':[1,2,3],                        \
  'object':{'key':'value'}                \
}                                         \
"));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  if(parser.ParseError())
  {
    std::cout << "Error: " << parser.GetErrorDescription() << std::endl;
    std::cout << "   at: " << parser.GetErrorLineNumber() << "(" << parser.GetErrorPosition() << ")" << std::endl;
  }

  DALI_TEST_CHECK(!parser.ParseError());

  const TreeNode* root = parser.GetRoot();

  DALI_TEST_CHECK(root);

  DALI_TEST_CHECK(root->Size());

  TreeNode::ConstIterator iter = root->CBegin();
  DALI_TEST_CHECK(iter != root->CEnd());

  const TreeNode* node = NULL;

  node = &((*iter).second);
  DALI_TEST_CHECK(node);
  DALI_TEST_CHECK(node->GetType() == TreeNode::STRING);
  DALI_TEST_CHECK(std::string((*iter).first) == std::string("string"));
  DALI_TEST_CHECK(std::string(node->GetString()) == std::string("value2"));

  ++iter;
  DALI_TEST_CHECK(iter != root->CEnd());
  node = &((*iter).second);
  DALI_TEST_CHECK(node);
  DALI_TEST_CHECK(node->GetType() == TreeNode::INTEGER);
  DALI_TEST_CHECK(std::string((*iter).first) == std::string("integer"));
  DALI_TEST_CHECK(node->GetInteger() == 2);

  ++iter;
  DALI_TEST_CHECK(iter != root->CEnd());
  node = &((*iter).second);
  DALI_TEST_CHECK(node);
  DALI_TEST_CHECK(node->GetType() == TreeNode::FLOAT);
  DALI_TEST_CHECK(std::string((*iter).first) == std::string("float"));
  DALI_TEST_CHECK(node->GetFloat() == 2.0);

  ++iter;
  DALI_TEST_CHECK(iter != root->CEnd());
  node = &((*iter).second);
  DALI_TEST_CHECK(node);
  DALI_TEST_CHECK(node->GetType() == TreeNode::BOOLEAN);
  DALI_TEST_CHECK(std::string((*iter).first) == std::string("boolean"));
  DALI_TEST_CHECK(node->GetBoolean());

  ++iter;
  DALI_TEST_CHECK(iter != root->CEnd());
  node = &((*iter).second);
  DALI_TEST_CHECK(node);
  DALI_TEST_CHECK(node->GetType() == TreeNode::IS_NULL);
  DALI_TEST_CHECK(std::string((*iter).first) == std::string("nil"));

  ++iter;
  DALI_TEST_CHECK(iter != root->CEnd());
  node = &((*iter).second);
  DALI_TEST_CHECK(node);
  DALI_TEST_CHECK(node->GetType() == TreeNode::ARRAY);
  DALI_TEST_CHECK(node->Size() == 3);
  TreeNode::ConstIterator iterArray = node->CBegin();

  DALI_TEST_CHECK(iterArray != node->CEnd());
  DALI_TEST_CHECK( ((*iterArray).second).GetType() == TreeNode::INTEGER);
  DALI_TEST_CHECK( (*iterArray).first == NULL );
  DALI_TEST_CHECK( ((*iterArray).second).GetInteger() == 1);

  ++iterArray;
  DALI_TEST_CHECK(iterArray != node->CEnd());
  DALI_TEST_CHECK( ((*iterArray).second).GetType() == TreeNode::INTEGER);
  DALI_TEST_CHECK( (*iterArray).first == NULL );
  DALI_TEST_CHECK( ((*iterArray).second).GetInteger() == 2);

  ++iterArray;
  DALI_TEST_CHECK(iterArray != node->CEnd());
  DALI_TEST_CHECK( ((*iterArray).second).GetType() == TreeNode::INTEGER);
  DALI_TEST_CHECK( (*iterArray).first == NULL );
  DALI_TEST_CHECK( ((*iterArray).second).GetInteger() == 3);

  ++iter;
  DALI_TEST_CHECK(iter != root->CEnd());
  node = &((*iter).second);
  DALI_TEST_CHECK(node);
  DALI_TEST_CHECK(node->GetType() == TreeNode::OBJECT);
  DALI_TEST_CHECK(node->Size() == 1);

  TreeNode::ConstIterator iterObject = node->CBegin();
  DALI_TEST_CHECK(iterObject != node->CEnd());
  DALI_TEST_CHECK( ((*iterObject).second).GetType() == TreeNode::STRING);
  DALI_TEST_CHECK( std::string((*iterObject).first) == std::string("key" ));
  DALI_TEST_CHECK( std::string(((*iterObject).second).GetString()) == std::string("value"));

  tet_result(TET_PASS);
}

static void UtcDaliJsonParserMethod02()
{
  ToolkitTestApplication application;

  tet_infoline("JSON Comments");

  std::string s1( ReplaceQuotes("         \
// some comments with empty line above  \n\
{                                         \
  // inline comments                    \n\
  'key':'value', // endline comments    \n\
  // more inline comments               \n\
  'key2':'value2'                         \
}                                         \
"));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  if(parser.ParseError())
  {
    std::cout << "Error: " << parser.GetErrorDescription() << std::endl;
    std::cout << "   at: " << parser.GetErrorLineNumber() << "(" << parser.GetErrorPosition() << ")" << std::endl;
  }

  DALI_TEST_CHECK(!parser.ParseError());

  const TreeNode* root = parser.GetRoot();

  DALI_TEST_CHECK(root);

  DALI_TEST_CHECK(root->Size());

  const TreeNode& node = (*root->CBegin()).second;

  DALI_TEST_CHECK(node.GetType() == TreeNode::STRING);

  DALI_TEST_CHECK(node.GetString() == std::string("value"));

  DALI_TEST_CHECK((*root->CBegin()).first == std::string("key"));

  tet_result(TET_PASS);
}


static void UtcDaliJsonParserMethod03()
{
  ToolkitTestApplication application;

  tet_infoline("JSON Empty line comment");

  std::string s1( ReplaceQuotes(
"/*\n" \
"c comment\n" \
"*/"\
"// next empty line comment\n"\
"//\n"\
"{\n"\
"  'key':'value'\n"\
"}\n"\
));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  if(parser.ParseError())
  {
    std::cout << "Error: " << parser.GetErrorDescription() << std::endl;
    std::cout << "   at: " << parser.GetErrorLineNumber() << "(" << parser.GetErrorPosition() << ")" << std::endl;
  }

  DALI_TEST_CHECK(!parser.ParseError());

  const TreeNode* root = parser.GetRoot();

  DALI_TEST_CHECK(root);

  DALI_TEST_CHECK(root->Size());

  const TreeNode& node = (*root->CBegin()).second;

  DALI_TEST_CHECK(node.GetType() == TreeNode::STRING);

  DALI_TEST_CHECK(node.GetString() == std::string("value"));

  DALI_TEST_CHECK((*root->CBegin()).first == std::string("key"));

  tet_result(TET_PASS);
}

static void UtcDaliJsonParserMethod04()
{
  ToolkitTestApplication application;

  tet_infoline("JSON Merge");

  std::string s1( ReplaceQuotes("                                       \
{                                                                       \
  'animations':                                                         \
  {                                                                     \
    'bump':                                                             \
    {                                                                   \
      'properties':                                                     \
      [                                                                 \
        {                                                               \
          'actor':'bump-image',                                         \
          'property':'uLightPosition',                                  \
          'value':[0.8, 0.0, -1.5],                                     \
          'alpha-function': 'BOUNCE',                                   \
          'time-period': { 'duration': 2.5 }                            \
        }                                                               \
      ]                                                                 \
    }                                                                   \
  }                                                                     \
}                                                                       \
"));

  std::string s2( ReplaceQuotes("                                       \
{                                                                       \
  'animations':                                                         \
  {                                                                     \
    'bump':                                                             \
    {                                                                   \
      'duration': 5.0,                                                  \
      'loop': true,                                                     \
      'end-action':'DISCARD'                                            \
    }                                                                   \
  }                                                                     \
}                                                                       \
"));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  if(parser.ParseError())
  {
    std::cout << "Error: " << parser.GetErrorDescription() << std::endl;
    std::cout << "   at: " << parser.GetErrorLineNumber() << "(" << parser.GetErrorPosition() << ")" << std::endl;
  }
  DALI_TEST_CHECK(!parser.ParseError());

  parser.Parse( s2 );

  if(parser.ParseError())
  {
    std::cout << "Error: " << parser.GetErrorDescription() << std::endl;
    std::cout << "   at: " << parser.GetErrorLineNumber() << "(" << parser.GetErrorPosition() << ")" << std::endl;
  }

  DALI_TEST_CHECK(!parser.ParseError());

  const TreeNode* root = parser.GetRoot();
  DALI_TEST_CHECK(root);

  const TreeNode *node = root->Find("bump");
  DALI_TEST_CHECK(node);

  DALI_TEST_CHECK(static_cast<int>(node->Size()) == 4);

  DALI_TEST_CHECK( node->GetChild("duration") );
  DALI_TEST_CHECK( node->GetChild("loop") );
  DALI_TEST_CHECK( node->GetChild("properties") );


  tet_result(TET_PASS);
}

static void UtcDaliJsonParserMethod05()
{
  ToolkitTestApplication application;

  tet_infoline("JSON Pack & Write");

  std::string s1( ReplaceQuotes("                                       \
{                                                                       \
  'animations':                                                         \
  {                                                                     \
    'bump':                                                             \
    {                                                                   \
      'properties':                                                     \
      [                                                                 \
        {                                                               \
          'actor':'bump-image',                                         \
          'property':'uLightPosition',                                  \
          'value':[0.8, 0.0, -1.5],                                     \
          'alpha-function': 'BOUNCE',                                   \
          'time-period': { 'duration': 2.5 }                            \
        }                                                               \
      ]                                                                 \
    }                                                                   \
  }                                                                     \
}                                                                       \
"));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  if(parser.ParseError())
  {
    std::cout << "Error: " << parser.GetErrorDescription() << std::endl;
    std::cout << "   at: " << parser.GetErrorLineNumber() << "(" << parser.GetErrorPosition() << ")" << std::endl;
  }
  DALI_TEST_CHECK(!parser.ParseError());

  std::stringstream a;
  parser.Write(a, 2);

  parser.Pack();

  std::stringstream b;
  parser.Write(b, 2);

  DALI_TEST_CHECK( a.str() == b.str() );

  tet_result(TET_PASS);
}


static const int NUMBER_OK_TESTS = 36;
char *TEST_OK[NUMBER_OK_TESTS] = {
  "{ 'hex': '\u0123\u4567\u89AB\uCDEF\uabcd\uef4A' }",
  "{ 'special': '`1~!@#$%^&*()_+-={:[,]}|;.</>?' }",
  "{ 'slash': '/ & \' }",
  "{'object with 1 member':['array with 1 element']}",
  "[{}, [], -42, true, false, null]",
  "{ 'integer': 1234567890 }",
  "{ 'integer': 1234567890 }",
  "{ 'real': -9876.543210 }",
  "{ 'e': 0.123456789e-12 }",
  "{ 'E': 1.234567890E+34 }",
  "{ '':  23456789012E66 }",
  "{ 'zero': 0 }",
  "{ 'one': 1 }",
  "{ 'space': ' ' }",
  "{ 'backslash': '\' }",
  "{ 'controls': '\\b\\f\\n\\r\\t' }",
  "{ 'alpha': 'abcdefghijklmnopqrstuvwyz' }",
  "{ 'ALPHA': 'ABCDEFGHIJKLMNOPQRSTUVWYZ' }",
  "{ 'digit': '0123456789' }",
  "{ '0123456789': 'digit' }",
  "{ 'true': true }",
  "{ 'false': false }",
  "{ 'null': null }",
  "{ 'array':[  ] }",
  "{ 'object':{  } }",
  "{ 'address': '1 Communication Centre. South Street' }",
  "{ 'url': 'http://www.JSON.org/' }",
  "{ 'comment': '// /* <!-- --' }",
  "{ '# -- --> */': ' ' }",
  "{ ' s p a c e d ' :[1,2 , 3,4 , 5        ,          6           ,7        ]}",
  "{ 'compact':[1,2,3,4,5,6,7]}",
  "{ 'quotes': '&#34; \\u0022 %22 0x22 034 &#x22;' }",
  "{ '\\uCAFE\\uBABE\\uAB98\\uFCDE\\ubcda\\uef4A\\b\\f\\n\\r\\t`1~!@#$%^&*()_+-=[]{}|;:': 'A key can be any string'}",
  "[ 0.5 ,98.6, 99.44,1066,1e1,0.1e1,1e-1,1e00,2e+00,2e-00, 'rosebud']",
  "{'JSON Test Pattern pass3': { 'The outermost value': 'must be an object or array.', 'In this test': 'It is an object.' } }",
  "[[[[[[[[[[[[[[[[[[['Not too deep']]]]]]]]]]]]]]]]]]]",
};


static void UtcDaliJsonParserMethod06()
{
  ToolkitTestApplication application;

  tet_infoline("JSON Parse Success");

  JsonParser parser = JsonParser::New();

  for(int i = 0; i < NUMBER_OK_TESTS; ++i)
  {
    parser = JsonParser::New();

    parser.Parse( ReplaceQuotes(TEST_OK[i]) );

    if(parser.ParseError())
    {
      tet_printf("Valid JSON parse test %d Failed", i);
      tet_printf("%s", ReplaceQuotes(TEST_OK[i]).c_str());

      tet_printf("JSON Error %d:%d: %s (%d)", parser.GetErrorLineNumber(), parser.GetErrorColumn(), parser.GetErrorDescription().c_str(), parser.GetErrorPosition());
    }

    DALI_TEST_CHECK(!parser.ParseError());
  }

  tet_result(TET_PASS);
}


static const int NUMBER_FAIL_TESTS = 32;
char *TEST_FAIL[] = {
  "[' tab\t   character  \t in\t string   ']",
  "['Extra close']]",
  "['Colon instead of comma': false]",
  "{'Numbers cannot have leading zeroes': 013}",
  "['Bad value', truth]",
  "['Illegal backslash escape: \017']",
  "['Bad value', truth]['Illegal backslash escape: \017']",
  "{'Comma instead if closing brace': true,",
  "{'Double colon':: null}",
  "{'Extra comma': true,}",
  "['Unclosed array'",
  "{'Illegal invocation': alert()}",
  "{'Missing colon' null}",
  "[0e]",
  "{unquoted_key: 'keys must be quoted'}",
  "'A JSON payload should be an object or array, not a string.'",
  "[\naked]",
  "{'Illegal expression': 1 + 2}",
  "{'Extra value after close': true} 'misplaced quoted value'",
  "[0e+]",
  "[+23456789012E66]",
  "['extra comma',]",
  "['Comma after the close'],",
  "['double extra comma',,]",
  "['Illegal backslash escape: \x15']",
  "['line\nbreak']",
  "{'Comma instead of colon', null}",
  "['mismatch'}",
  "['line\nbreak']",
  "[0e+-1]",
  "{'Numbers cannot be hex': 0x14}",
  "[   , '<-- missing value']",
};

static void UtcDaliJsonParserMethod07()
{
  ToolkitTestApplication application;

  tet_infoline("JSON Fail");

  JsonParser parser = JsonParser::New();

  for(int i = 0; i < NUMBER_FAIL_TESTS; ++i)
  {
    parser = JsonParser::New();

    parser.Parse( ReplaceQuotes(TEST_FAIL[i]) );

    if(!parser.ParseError())
    {
      tet_printf("Invalid JSON parse test %d Failed", i);
      tet_printf("%s", ReplaceQuotes(TEST_FAIL[i]).c_str());
      tet_printf("JSON Error %d:%d %s (%s)", parser.GetErrorLineNumber(), parser.GetErrorColumn(),
                 parser.GetErrorDescription().c_str(), parser.GetErrorPosition());
    }

    DALI_TEST_CHECK(parser.ParseError());
  }


  parser = JsonParser::New();

  parser.Parse( "['single quote']" );

  if(!parser.ParseError())
  {
    tet_printf("['single quote']");
  }

  DALI_TEST_CHECK(parser.ParseError());

  tet_result(TET_PASS);
}

static void UtcDaliJsonParserMethod08()
{
  ToolkitTestApplication application;

  tet_infoline("JSON error reporting");

  std::string s1( ReplaceQuotes("\
{                                         \n\
  'float':,],                             \n\
}                                         \n\
"));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  DALI_TEST_CHECK(parser.ParseError());

  DALI_TEST_CHECK(1  == parser.GetErrorLineNumber());
  DALI_TEST_CHECK(53 == parser.GetErrorPosition());
  DALI_TEST_CHECK(11 == parser.GetErrorColumn());

  tet_result(TET_PASS);
}

static void UtcDaliJsonParserMethod09()
{
  ToolkitTestApplication application;

  tet_infoline("JSON Pack()");

  std::string s1( ReplaceQuotes("\
{                                         \
  'string':'value2',                      \
  'integer':2,                            \
  'float':2.3,                            \
  'boolean':true,                         \
  'nil':null,                             \
  'array':[1,2,3],                        \
  'object':{'key':'value'}                \
}                                         \
"));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  std::stringstream ss1;
  parser.Write(ss1, 2);

  parser.Pack(); // Pack() moves strings

  std::stringstream ss2;
  parser.Write(ss2, 2);

  DALI_TEST_CHECK(ss1.str() == ss2.str());

  tet_result(TET_PASS);
}

static void UtcDaliJsonParserMethod10()
{
  ToolkitTestApplication application;

  tet_infoline("JSON basic test");

  std::string s1( "" );

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  DALI_TEST_CHECK(parser.ParseError());

  tet_result(TET_PASS);
}
