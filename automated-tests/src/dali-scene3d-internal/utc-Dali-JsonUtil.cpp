/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// Enable debug log for test coverage
#define DEBUG_ENABLED 1

#include "dali-scene3d/internal/loader/json-util.h"
#include "dali-toolkit/devel-api/builder/json-parser.h"
#include <dali-test-suite-utils.h>
#include <string>

#define STRINGIFY(x) #x

using namespace Dali;
using namespace Dali::Toolkit;
using namespace Dali::Scene3D::Loader;

namespace
{
const std::string TEST_JSON =
  "{ \"int\": 17834,"
  "\"float\": 3.1415628,"
  "\"bool\": true,"
  "\"null\": null,"
  "\"string\": \"hello\","
  "\"floatArray\": [ 0.0, 0.25, 1.0, 0.75 ],"
  "\"intArray\": [ 1, 2, 3, 5, 7, 11, 13, -1, -5 ],"
  "\"mixedArray\": [ 1.99, \"the\", 6, \"brown\", \"fox\" ],"
  "\"stringArray\": [ \"lorem\", \"ipsum\", \"dolor\", \"sic\", \"amet\" ],"
  "\"object\": { \"duration\": 4.0, \"delay\": 1.0 },"
  "\"rgb\": [ 0.5, 0.8, 0.25 ],"
  "\"disambiguatedFloat\": { \"type\": \"float\", \"value\": 15.8 },"
  "\"rotation1\": { \"type\": \"rotation\", \"value\": [ 15.0, 90.0, -45.0 ] },"
  "\"rotation2\": { \"type\": \"rotation\", \"value\": [ 0.707, 0.0, 0.707, 0.0 ] },"
  "\"matrix\": [ 1.0, 0.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 1.0, 2.0, 3.0, 4.0 ],"
  "\"matrix3\": [ 2.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, 1.0, 2.0 ],"
  "\"vector2\": [ 2.0, 1.0 ]"
  "}";

template <typename T>
struct Item
{
  std::string name;
  bool readResult;
  T result;
};

struct Document
{
  Document(const std::string& json)
  : parser(JsonParser::New())
  {
    //DALI_TEST_CHECK(parser.Parse(json));
    if (!parser.Parse(json))
    {
      auto error = parser.GetErrorDescription();
      printf("Error: %s in {%d, %d}\n", error.c_str(), parser.GetErrorLineNumber(), parser.GetErrorColumn());
    }
    root = parser.GetRoot();
  }

  JsonParser parser;
  const TreeNode* root;
};

template <typename T, size_t N>
bool CompareArrays(const T (&array)[N], const T* p, T epsilon = T(0))
{
  for (auto& i: array)
  {
    if (std::abs(i - *p) > epsilon)
    {
      printf("Element %d mismatched.\n", int32_t(std::distance(array, &i)));
      return false;
    }
    ++p;
  }
  return true;
}

}

int UtcDaliJsonUtilReadBool(void)
{
  bool value = false;
  DALI_TEST_CHECK(!ReadBool(nullptr, value));
  DALI_TEST_EQUAL(value, false); // unchanged

  Document doc{ TEST_JSON };

  for (auto& i : {
    Item<bool>{ "bool",       true, true },
    Item<bool>{ "int",        false, true }, // value unchanged
    Item<bool>{ "float",      false, true },
    Item<bool>{ "null",       false, true },
    Item<bool>{ "floatArray", false, true },
    Item<bool>{ "intArray",   false, true },
    Item<bool>{ "object",     false, true },
  })
  {
    bool readResult = ReadBool(doc.root->GetChild(i.name), value);
    DALI_TEST_EQUAL(readResult, i.readResult);
    if (readResult)
    {
      DALI_TEST_EQUAL(value, i.result);
    }
  }

  END_TEST;
}

int UtcDaliJsonUtilReadInt(void)
{
  int value = 0xbadbeef;
  DALI_TEST_CHECK(!ReadInt(nullptr, value));
  DALI_TEST_EQUAL(value, 0xbadbeef);

  Document doc{ TEST_JSON };

  for (auto& i : {
    Item<int>{ "bool",       false, 0xbadbeef }, // unchanged from initial
    Item<int>{ "int",        true, 17834 },
    Item<int>{ "float",      true, 3 },
    Item<int>{ "null",       false, 3 },
    Item<int>{ "floatArray", false, 3 },
    Item<int>{ "intArray",   false, 3 },
    Item<int>{ "object",     false, 3 },
  })
  {
    bool readResult = ReadInt(doc.root->GetChild(i.name), value);
    DALI_TEST_EQUAL(readResult, i.readResult);
    if (readResult)
    {
      DALI_TEST_EQUAL(value, i.result);
    }
  }

  END_TEST;
}

int UtcDaliJsonUtilReadFloat(void)
{
  float value = 10.101f;
  DALI_TEST_CHECK(!ReadFloat(nullptr, value));
  DALI_TEST_EQUAL(value, 10.101f);

  Document doc{ TEST_JSON };

  for (auto& i : {
    Item<float>{ "bool",       false, 10.101f }, // unchanged from initial
    Item<float>{ "int",        true, 17834.f },
    Item<float>{ "float",      true, 3.1415628f },
    Item<float>{ "null",       false, 3.1415628f },
    Item<float>{ "floatArray", false, 3.1415628f },
    Item<float>{ "intArray",   false, 3.1415628f },
    Item<float>{ "object",     false, 3.1415628f },
  })
  {
    bool readResult = ReadFloat(doc.root->GetChild(i.name), value);
    DALI_TEST_EQUAL(readResult, i.readResult);
    if (readResult)
    {
      DALI_TEST_EQUAL(value, i.result);
    }
  }

  END_TEST;
}

int UtcDaliJsonUtilNumericalArrays(void)
{
  Document doc{ TEST_JSON };

  DALI_TEST_EQUAL(4u, GetNumericalArraySize(doc.root->GetChild("floatArray")));
  DALI_TEST_EQUAL(9u, GetNumericalArraySize(doc.root->GetChild("intArray")));
  DALI_TEST_EQUAL(1u, GetNumericalArraySize(doc.root->GetChild("mixedArray")));

  END_TEST;
}

int UtcDaliJsonUtilReadVectorInt(void)
{
  DALI_TEST_CHECK(!ReadVector(nullptr, static_cast<int*>(nullptr), 0));

  Document doc{ TEST_JSON };

  int ints[9];
  DALI_TEST_CHECK(ReadVector(doc.root->GetChild("floatArray"), ints, 4u));
  DALI_TEST_CHECK(CompareArrays<int>({ 0, 0, 1, 0 }, ints));

  DALI_TEST_CHECK(ReadVector(doc.root->GetChild("intArray"), ints, 9u));
  DALI_TEST_CHECK(CompareArrays<int>({ 1, 2, 3, 5, 7, 11, 13, -1, -5 }, ints));

  DALI_TEST_CHECK(ReadVector(doc.root->GetChild("mixedArray"), ints, 1u));
  DALI_TEST_CHECK(CompareArrays<int>({ 1 }, ints));

  END_TEST;
}

int UtcDaliJsonUtilReadVectorFloat(void)
{
  DALI_TEST_CHECK(!ReadVector(nullptr, static_cast<int*>(nullptr), 0));

  Document doc{ TEST_JSON };

  constexpr float e = 1e-6f;
  float floats[9];
  DALI_TEST_CHECK(ReadVector(doc.root->GetChild("floatArray"), floats, 4u));
  DALI_TEST_CHECK(CompareArrays<float>({ 0.f, 0.25f, 1.f, 0.75f }, floats, e));

  DALI_TEST_CHECK(ReadVector(doc.root->GetChild("intArray"), floats, 9u));
  DALI_TEST_CHECK(CompareArrays<float>({ 1.f, 2.f, 3.f, 5.f, 7.f, 11.f, 13.f, -1.f, -5.f }, floats, e));

  DALI_TEST_CHECK(ReadVector(doc.root->GetChild("mixedArray"), floats, 1u));
  DALI_TEST_CHECK(CompareArrays<float>({ 1.99f }, floats, e));

  END_TEST;
}

int UtcDaliJsonUtilReadColor(void)
{
  Vector4 color;
  DALI_TEST_CHECK(!ReadColor(nullptr, color));

  Document doc{ TEST_JSON };
  DALI_TEST_CHECK(!ReadColor(doc.root->GetChild("bool"), color));
  DALI_TEST_CHECK(!ReadColor(doc.root->GetChild("int"), color));
  DALI_TEST_CHECK(!ReadColor(doc.root->GetChild("float"), color));
  DALI_TEST_CHECK(!ReadColor(doc.root->GetChild("string"), color));
  DALI_TEST_CHECK(!ReadColor(doc.root->GetChild("object"), color));

  constexpr float e = 1e-6f;
  DALI_TEST_CHECK(ReadColor(doc.root->GetChild("floatArray"), color));
  DALI_TEST_CHECK(CompareArrays<float>({ 0.f, 0.25f, 1.0f, 0.75f }, color.AsFloat(), e));

  DALI_TEST_CHECK(ReadColor(doc.root->GetChild("intArray"), color));
  DALI_TEST_CHECK(CompareArrays<float>({ 1.f, 2.f, 3.f, 5.f }, color.AsFloat(), e));

  DALI_TEST_CHECK(ReadColor(doc.root->GetChild("rgb"), color));
  DALI_TEST_CHECK(CompareArrays<float>({ .5f, .8f, .25f, 1.f }, color.AsFloat(), e));

  END_TEST;
}

int UtcDaliJsonUtilReadTimePeriod(void)
{
  TimePeriod value(60.f);
  DALI_TEST_CHECK(!ReadTimePeriod(nullptr, value));
  DALI_TEST_EQUAL(value.durationSeconds, 60.f);
  DALI_TEST_EQUAL(value.delaySeconds, 0.f);

  Document doc{ TEST_JSON };
  DALI_TEST_CHECK(ReadTimePeriod(doc.root->GetChild("object"), value));
  DALI_TEST_EQUAL(value.durationSeconds, 4.f);
  DALI_TEST_EQUAL(value.delaySeconds, 1.f);

  END_TEST;
}

int UtcDaliJsonUtilReadString(void)
{
  std::string value = "bye";
  DALI_TEST_CHECK(!ReadString(nullptr, value));
  DALI_TEST_EQUAL(value, "bye");

  Document doc{ TEST_JSON };

  for (auto& i : {
    Item<std::string>{ "bool",       false, "bye"}, // unchanged from initial
    Item<std::string>{ "int",        false, "bye"},
    Item<std::string>{ "float",      false, "bye"},
    Item<std::string>{ "null",       false, "bye"},
    Item<std::string>{ "string",     true, "hello"},
    Item<std::string>{ "floatArray", false, "hello"}, // unchanged
    Item<std::string>{ "object",     false, "hello"},
  })
  {
    bool readResult = ReadString(doc.root->GetChild(i.name), value);
    DALI_TEST_EQUAL(readResult, i.readResult);
    if (readResult)
    {
      DALI_TEST_EQUAL(value, i.result);
    }
  }

  END_TEST;
}

int UtcDaliJsonUtilReadStringVector(void)
{
  std::vector<std::string> strings;
  DALI_TEST_CHECK(!ReadStringVector(nullptr, strings));
  DALI_TEST_CHECK(strings.empty());

  Document doc{ TEST_JSON };

  DALI_TEST_CHECK(!ReadStringVector(doc.root->GetChild("floatArray"), strings));
  DALI_TEST_CHECK(strings.empty());

  DALI_TEST_CHECK(!ReadStringVector(doc.root->GetChild("intArray"), strings));
  DALI_TEST_CHECK(strings.empty());

  DALI_TEST_CHECK(!ReadStringVector(doc.root->GetChild("mixedArray"), strings));
  DALI_TEST_CHECK(strings.empty());

  DALI_TEST_CHECK(ReadStringVector(doc.root->GetChild("stringArray"), strings));
  DALI_TEST_EQUAL(strings.size(), 5u);

  auto iStrings = strings.begin();
  for (auto& i : { "lorem", "ipsum", "dolor", "sic", "amet" })
  {
    DALI_TEST_EQUAL(*iStrings, i);
    ++iStrings;
  }

  END_TEST;
}

int UtcDaliJsonUtilReadAndReturnPropertyValue(void)
{
  Document doc{ TEST_JSON };
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("disambiguatedFloat")).Get<float>(), 15.8f);

  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("rotation1")).Get<Quaternion>(),
    Quaternion(Radian(Degree(15.0)), Radian(Degree(90.0)), Radian(Degree(-45.f))));
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("rotation2")).Get<Quaternion>(),
    Quaternion(Vector4(0.707f, 0.f, 0.707f, 0.f)));

  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("bool")).Get<bool>(), true);
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("int")).Get<int32_t>(), 17834);

  const float floats[]{ 1.f, 0.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, 3.f, 0.f, 1.f, 2.f, 3.f, 4.f };
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("matrix")).Get<Matrix>(), Matrix(floats));
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("matrix3")).Get<Matrix3>(), Matrix3(floats[5],
    floats[6], floats[7], floats[8], floats[9], floats[10], floats[11], floats[12], floats[13]));
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("floatArray")).Get<Vector4>(),
    Vector4(0.f, .25f, 1.f, .75f));
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("rgb")).Get<Vector3>(),
    Vector3(.5f, .8f, .25f));
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("vector2")).Get<Vector2>(),
    Vector2(2.f, 1.f));
  DALI_TEST_EQUAL(ReadPropertyValue(*doc.root->GetChild("object")->GetChild("duration")).Get<float>(), 4.f );

  END_TEST;
}

namespace
{
template <typename T>
void CheckEqualityAs(Property::Value lhs, Property::Value rhs)
{
  DALI_TEST_EQUAL(lhs.Get<T>(), rhs.Get<T>());
}

}

int UtcDaliJsonUtilReadPropertyValue(void)
{
  struct TypeNameValue
  {
    Property::Type type;
    std::string name;
    Property::Value value;
    void(*compareFn)(Property::Value, Property::Value);
  };

  Document doc{ TEST_JSON };

  const float floats[]{ 1.f, 0.f, 0.f, 0.f, 0.f, 2.f, 0.f, 0.f, 0.f, 0.f, 3.f, 0.f, 1.f, 2.f, 3.f, 4.f };
  const TypeNameValue typeNameValues[] {
    { Property::BOOLEAN, "bool", true, CheckEqualityAs<bool> },
    { Property::FLOAT, "float", 3.1415628f, CheckEqualityAs<float> },
    { Property::INTEGER, "int", 17834, CheckEqualityAs<int32_t> },
    { Property::VECTOR2, "vector2", Vector2(2.f, 1.f), CheckEqualityAs<Vector2> },
    { Property::VECTOR3, "rgb", Vector3(.5f, .8f, .25f), CheckEqualityAs<Vector3> },
    { Property::VECTOR4, "floatArray", Vector4(.0f, .25f, 1.f, .75f), CheckEqualityAs<Vector4> },
    { Property::MATRIX3, "matrix3", Matrix3(2.f, 0.f, 0.f, 0.f, 0.f, 3.f, 0.f, 1.f, 2.f),
      CheckEqualityAs<Matrix3> },
    { Property::MATRIX, "matrix", Matrix(floats), CheckEqualityAs<Matrix> },
    { Property::RECTANGLE, "intArray", Rect<int>(1, 2, 3, 5), CheckEqualityAs<Rect<int>> },
    { Property::EXTENTS, "intArray", Extents(1, 2, 3, 5), CheckEqualityAs<Extents> },
  };
  for(auto& i: typeNameValues)
  {
    std::cout << i.value << std::endl;
    i.compareFn(ReadPropertyValue(i.type, *doc.root->GetChild(i.name)), i.value);
  }

  END_TEST;
}
