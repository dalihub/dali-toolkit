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

#include <iostream>
#include <stdlib.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>

using namespace Dali;
using namespace Dali::Toolkit;

void utc_json_parser_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_json_parser_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

std::string ReplaceQuotes(const std::string &in_s)
{
  std::string s(in_s);
  // wrong as no embedded quote but had regex link problems
  std::replace(s.begin(), s.end(), '\'', '"');
  return s;
}

void CompareTrees(const TreeNode& a, const TreeNode& b)
{
  DALI_TEST_CHECK( a.GetType() == b.GetType() );

  DALI_TEST_CHECK( a.Size() == b.Size() );

  if( a.GetName() )
  {
    DALI_TEST_CHECK( std::string( a.GetName() ) == std::string( b.GetName() ) );
  }

  DALI_TEST_CHECK( a.HasSubstitution() == b.HasSubstitution() );

  switch( a.GetType() )
  {
    case TreeNode::OBJECT:
    case TreeNode::ARRAY:
    {
      for( TreeNode::ConstIterator aiter = a.CBegin(), biter = b.CBegin();
           aiter != a.CEnd() && biter != b.CEnd(); ++aiter, ++biter )
      {
        CompareTrees( (*aiter).second, (*biter).second );
      }
      break;
    }
    case TreeNode::STRING:
    {
      DALI_TEST_CHECK( std::string( a.GetString() ) == std::string( b.GetString() ) );
      break;
    }
    case TreeNode::FLOAT:
    {
      DALI_TEST_CHECK( a.GetFloat() == b.GetFloat() );
      break;
    }
    case TreeNode::INTEGER:
    {
      DALI_TEST_CHECK( a.GetInteger() == b.GetInteger());
      break;
    }
    case TreeNode::BOOLEAN:
    {
      DALI_TEST_CHECK( a.GetBoolean() == b.GetBoolean() );
      break;
    }
    default:
    {
      break;
    }
  }
}


}


int UtcDaliJsonParserMethod01(void)
{
  ToolkitTestApplication application;

  tet_infoline("JSON basic test");

  std::string s1( ReplaceQuotes(
"{ \
  'string':'value2', \
  'integer':2, \
  'float':2.0, \
  'boolean':true, \
  'nil':null, \
  'array':[1,2,3], \
  'object':{'key':'value'} \
}"));

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
  END_TEST;
}

int UtcDaliJsonParserMethod02(void)
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
  END_TEST;
}


int UtcDaliJsonParserMethod03(void)
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
  END_TEST;
}

int UtcDaliJsonParserMethod04(void)
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
          'alphaFunction': 'BOUNCE',                                    \
          'timePeriod': { 'duration': 2.5 }                            \
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
      'endAction':'DISCARD'                                            \
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
  END_TEST;
}

int UtcDaliJsonParserMethod05(void)
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
          'alphaFunction': 'BOUNCE',                                    \
          'timePeriod': { 'duration': 2.5 }                            \
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
  END_TEST;
}

namespace
{

static const int NUMBER_OK_TESTS = 36;
const char *TEST_OK[NUMBER_OK_TESTS] = {
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
}


int UtcDaliJsonParserMethod06(void)
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
  END_TEST;
}

namespace
{

static const int NUMBER_FAIL_TESTS = 34;
const char *TEST_FAIL[] = {
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
  "[{'no comma':1} {'b:2}]",
  "{'extra comma':1,}",
};
}

int UtcDaliJsonParserMethod07(void)
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
  END_TEST;
}

int UtcDaliJsonParserMethod08(void)
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
  END_TEST;
}

int UtcDaliJsonParserMethod09(void)
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
  END_TEST;
}

int UtcDaliJsonParserMethod10(void)
{
  ToolkitTestApplication application;

  tet_infoline("JSON empty data");

  std::string s1( "" );

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  DALI_TEST_CHECK(parser.ParseError());

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliJsonParserMethod11(void)
{
  ToolkitTestApplication application;
  tet_infoline("JSON tree copy");

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
          'alphaFunction': 'BOUNCE',                                    \
          'timePeriod': { 'duration': 2.5 }                            \
        }                                                               \
      ]                                                                 \
    }                                                                   \
  }                                                                     \
}                                                                       \
"));

  JsonParser parser = JsonParser::New();

  parser.Parse( s1 );

  JsonParser parser2 = JsonParser::New(*parser.GetRoot());

  DALI_TEST_CHECK(parser.GetRoot());
  DALI_TEST_CHECK(parser2.GetRoot());

  CompareTrees( *parser.GetRoot(), *parser2.GetRoot() );

  tet_result(TET_PASS);
  END_TEST;
}


int UtcDaliJsonParserMerge1(void)
{
  ToolkitTestApplication application;
  tet_infoline("JSON tree merge");

  std::string s1( ReplaceQuotes("                                       \
{                                                                       \
  'styles':                                                             \
  {                                                                     \
    'button':                                                           \
    {                                                                   \
      'backgroundColor':[0.8, 0.0, 1.0, 1.0],                           \
      'foregroundColor':[1, 1, 1, 1]                                    \
    }                                                                   \
  }                                                                     \
}                                                                       \
"));

  JsonParser parser = JsonParser::New();
  JsonParser testParser = JsonParser::New();

  testParser.Parse( s1 );

  parser.Parse( s1 );
  parser.Parse( s1 ); // Merge the tree into itself. The value array should not grow.

  DALI_TEST_CHECK(parser.GetRoot());

  CompareTrees( *parser.GetRoot(), *testParser.GetRoot() );

  END_TEST;
}
