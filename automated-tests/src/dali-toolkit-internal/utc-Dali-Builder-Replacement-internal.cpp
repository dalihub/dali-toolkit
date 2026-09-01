/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>
#include <dali-toolkit/internal/builder/replacement.h>

using namespace Dali;
using namespace Dali::Toolkit;

using Dali::Toolkit::Internal::Replacement;

void utc_dali_builder_replacement_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_builder_replacement_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBuilderReplacementIsInsetsOptionalChildEmptyP(void)
{
  tet_infoline("Replacement::IsInsets(OptionalChild) must return an empty optional when there is no child node");

  Replacement replacement;

  OptionalChild  noChild;
  OptionalInsets result = replacement.IsInsets(noChild);
  DALI_TEST_CHECK(!result);

  END_TEST;
}

int UtcDaliBuilderReplacementIsInsetsOptionalChildPresentP(void)
{
  tet_infoline("Replacement::IsInsets(OptionalChild) must forward to IsInsets(const TreeNode&) when the child node exists");

  Dali::Toolkit::JsonParser parser = Dali::Toolkit::JsonParser::New();
  DALI_TEST_CHECK(parser.Parse("{ \"value\": [1, 2, 3, 4] }"));

  const TreeNode* root = parser.GetRoot();
  DALI_TEST_CHECK(root);

  const TreeNode* valueNode = root->GetChild("value");
  DALI_TEST_CHECK(valueNode);

  Replacement    replacement;
  OptionalChild  child(*valueNode);
  OptionalInsets result = replacement.IsInsets(child);
  DALI_TEST_CHECK(result);
  DALI_TEST_CHECK(*result == Insets(1.0f, 2.0f, 3.0f, 4.0f));

  END_TEST;
}
