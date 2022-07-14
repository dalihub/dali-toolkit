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

#include "dali-scene3d/internal/loader/json-reader.h"
#include <dali-test-suite-utils.h>
#include <string>

using namespace Dali;

#define JSON_STRING(x) #x, strlen(#x)

int UtcDaliJsonReaderStrCmp(void)
{
  json_string_s jstr[] {
    { JSON_STRING(hello) },
    { JSON_STRING(hellew) },
  };
  DALI_TEST_EQUAL(json::StrCmp(jstr[0], "hello"), 0);
  DALI_TEST_EQUAL(json::StrCmp(jstr[1], "hello"), 'e' - 'o');

  END_TEST;
}

int UtcDaliJsonReaderValidateThrow(void)
{
  json_value_s jval { nullptr, json_type_array };
  DALI_TEST_THROWS(json::Validate(jval, json_type_object), std::runtime_error);
  json::Validate(jval, json_type_array);

  END_TEST;
}

int UtcDaliJsonReaderFindObjectChild(void)
{
  json_string_s jkey{ JSON_STRING(fudgeFactor) };
  json_number_s jActualValue{ JSON_STRING(5.2) };
  json_value_s jvalue { &jActualValue, json_type_number };
  json_object_element_s jobjelem { &jkey, &jvalue, nullptr };

  json_object_s jobj{ &jobjelem, 1 };

  DALI_TEST_EQUAL(json::FindObjectChild(jkey.string, jobj), &jvalue);
  DALI_TEST_EQUAL(json::FindObjectChild("fudgeFactory", jobj), static_cast<json_value_s*>(nullptr));

  END_TEST;
}
