/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <cstdlib>

#include <dali-test-suite-utils.h>
// GTest fails to compile with "error: return-statement with a value, in function returning 'void'" error
// if using dali assert function so define new assert with returning void.
#define _ASSERT_H_
#define assert(exp) \
    if (!(exp)) { \
        fprintf(stderr, \
            "Assert fail in %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, \
            "Following expression is not true:\n" \
                "%s\n", #exp); \
        std::abort(); \
    }
#include <gtest/gtest.h>
#undef GTEST_HAS_DEATH_TEST
#include "yoga/YGFlexTest.cpp"
#include "yoga/YGAlignContentTest.cpp"
#include "yoga/YGComputedMarginTest.cpp"
#include "yoga/YGZeroOutLayoutRecursivlyTest.cpp"
#include "yoga/YGRoundingTest.cpp"
#include "yoga/YGTreeMutationTest.cpp"
#include "yoga/YGMeasureCacheTest.cpp"
#include "yoga/YGTraversalTest.cpp"
#include "yoga/YGAlignItemsTest.cpp"
#include "yoga/YGComputedPaddingTest.cpp"
#include "yoga/YGDimensionTest.cpp"
#include "yoga/YGDefaultValuesTest.cpp"
#include "yoga/YGMinMaxDimensionTest.cpp"
#include "yoga/YGPaddingTest.cpp"
#include "yoga/YGLoggerTest.cpp"
#include "yoga/YGAbsolutePositionTest.cpp"
#include "yoga/YGBorderTest.cpp"
#include "yoga/YGDirtiedTest.cpp"
#include "yoga/YGRoundingMeasureFuncTest.cpp"
#include "yoga/YGEdgeTest.cpp"
#include "yoga/YGAlignSelfTest.cpp"
#include "yoga/YGMeasureTest.cpp"
#include "yoga/YGFlexDirectionTest.cpp"
#include "yoga/YGHadOverflowTest.cpp"
#include "yoga/YGNodeChildTest.cpp"
#include "yoga/YGRoundingFunctionTest.cpp"
#include "yoga/YGPersistenceTest.cpp"
#include "yoga/YGPercentageTest.cpp"
#include "yoga/YGStyleTest.cpp"
#include "yoga/YGMarginTest.cpp"
#include "yoga/YGLayoutDiffingTest.cpp"
#include "yoga/YGBaselineFuncTest.cpp"
#include "yoga/YGAspectRatioTest.cpp"
#include "yoga/YGSizeOverflowTest.cpp"
#include "yoga/YGDirtyMarkingTest.cpp"
#include "yoga/YGMeasureModeTest.cpp"
#include "yoga/YGJustifyContentTest.cpp"
#include "yoga/YGInfiniteHeightTest.cpp"
#include "yoga/YGRelayoutTest.cpp"
#include "yoga/YGDisplayTest.cpp"
#include "yoga/YGFlexWrapTest.cpp"

//////////////////////////////////////////////////////////

int UtcDaliFlexboxLayoutTest(void)
{
  tet_infoline("UtcDaliFlexboxLayoutTest");
  int argc = 1;
  const char* argv = "yoga-gtest";
  testing::InitGoogleTest( &argc,  const_cast< char** >( &argv ) );

  // The test function is a 3rd party function that should return true if the test passes
  if( !RUN_ALL_TESTS() )
  {
    tet_result(TET_PASS);
  }
  else
  {
    tet_result(TET_FAIL);
  }

  END_TEST;
}
