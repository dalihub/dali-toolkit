/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <toolkit-event-thread-callback.h>
#include <toolkit-text-utils.h>

#include <dali-toolkit/internal/visuals/image-visual-shader-feature-builder.h>

#include <../dali-toolkit/dali-toolkit-test-utils/dummy-control.h>
#include <dummy-visual.h>

using namespace Dali;
using namespace Toolkit;

int UtcImageVisualShaderFeatureBuilderSetGetProperty01(void)
{
  ToolkitTestApplication application;
  tet_infoline("Register an ImageVisual and and perform an Action on Visual directly");
  auto featureBuilder = Dali::Toolkit::Internal::ImageVisualShaderFeatureBuilder()
                          .EnableTextureAtlas(Dali::Toolkit::Internal::ImageVisualShaderFeature::TextureAtlas::ENABLED)
                          .ApplyDefaultTextureWrapMode(true);

  std::string vertexPrefixList;
  std::string fragmentPrefixList;
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);

  DALI_TEST_EQUALS(vertexPrefixList, std::string(""), TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefixList, std::string("#define ATLAS_DEFAULT_WARP\n"), TEST_LOCATION);

  featureBuilder = featureBuilder.ApplyDefaultTextureWrapMode(false);

  vertexPrefixList.clear();
  fragmentPrefixList.clear();
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);

  DALI_TEST_EQUALS(vertexPrefixList, std::string(""), TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefixList, std::string("#define ATLAS_CUSTOM_WARP\n"), TEST_LOCATION);
  END_TEST;
}

int UtcImageVisualShaderFeatureBuilderSetGetProperty02(void)
{
  ToolkitTestApplication application;
  tet_infoline("Register an ImageVisual and and perform an Action on Visual directly");
  auto featureBuilder = Dali::Toolkit::Internal::ImageVisualShaderFeatureBuilder()
                          .EnableRoundedCorner(true)
                          .EnableBorderline(true)
                          .EnableAlphaMaskingOnRendering(true)
                          .EnableYuvToRgb(true, false);

  std::string vertexPrefixList;
  std::string fragmentPrefixList;
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);

  std::string vertexPrefixListResult;
  vertexPrefixListResult += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  vertexPrefixListResult += "#define IS_REQUIRED_BORDERLINE\n";
  vertexPrefixListResult += "#define IS_REQUIRED_ALPHA_MASKING\n";

  std::string fragmentPrefixListResult;
  fragmentPrefixListResult += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  fragmentPrefixListResult += "#define IS_REQUIRED_BORDERLINE\n";
  fragmentPrefixListResult += "#define IS_REQUIRED_ALPHA_MASKING\n";

  DALI_TEST_EQUALS(vertexPrefixList, vertexPrefixListResult, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefixList, fragmentPrefixListResult, TEST_LOCATION);

  featureBuilder = featureBuilder.EnableAlphaMaskingOnRendering(false);

  vertexPrefixList.clear();
  fragmentPrefixList.clear();
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);

  vertexPrefixListResult.clear();
  vertexPrefixListResult += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  vertexPrefixListResult += "#define IS_REQUIRED_BORDERLINE\n";

  fragmentPrefixListResult.clear();
  fragmentPrefixListResult += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  fragmentPrefixListResult += "#define IS_REQUIRED_BORDERLINE\n";
  fragmentPrefixListResult += "#define IS_REQUIRED_YUV_TO_RGB\n";

  DALI_TEST_EQUALS(vertexPrefixList, vertexPrefixListResult, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefixList, fragmentPrefixListResult, TEST_LOCATION);

  featureBuilder = featureBuilder.EnableYuvToRgb(true, true);

  vertexPrefixList.clear();
  fragmentPrefixList.clear();
  featureBuilder.GetVertexShaderPrefixList(vertexPrefixList);
  featureBuilder.GetFragmentShaderPrefixList(fragmentPrefixList);

  fragmentPrefixListResult.clear();
  fragmentPrefixListResult += "#define IS_REQUIRED_ROUNDED_CORNER\n";
  fragmentPrefixListResult += "#define IS_REQUIRED_BORDERLINE\n";
  fragmentPrefixListResult += "#define IS_REQUIRED_UNIFIED_YUV_AND_RGB\n";

  DALI_TEST_EQUALS(vertexPrefixList, vertexPrefixListResult, TEST_LOCATION);
  DALI_TEST_EQUALS(fragmentPrefixList, fragmentPrefixListResult, TEST_LOCATION);
  END_TEST;
}
