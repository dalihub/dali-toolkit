/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali-toolkit/internal/visuals/visual-factory-resolve-url.h>

using namespace Dali::Toolkit::Internal;

int UtcDaliResolveUrlRegularImage(void)
{
  tet_infoline( "UtcDaliResolveUrl REGULAR_IMAGE" );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("foobar.jpeg"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("foobar.gif"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("foobar.PNG"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("foobar.Png123"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("foobar.Png1.23"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType(""), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType(" "), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("."), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("9"), TEST_LOCATION );

  END_TEST;
}

int UtcDaliResolveUrlSvg(void)
{
  tet_infoline( "UtcDaliResolveUrl SVG" );

  DALI_TEST_EQUALS( UrlType::SVG, ResolveUrlType("foobar.svg"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::SVG, ResolveUrlType("foobar.svg.svg"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::SVG, ResolveUrlType("foobar.svG"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::SVG, ResolveUrlType("foobar.SVG"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::SVG, ResolveUrlType(".SvG"), TEST_LOCATION );

  // SVGs aren't N-patch
  DALI_TEST_EQUALS( UrlType::SVG, ResolveUrlType("foobar.9.svg"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("svg.png"), TEST_LOCATION );

  // maybe controversial, but for now we expect the suffix to be exactly .svg
  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("svg.svg1"), TEST_LOCATION );

  END_TEST;
}

int UtcDaliResolveUrlNPatch(void)
{
  tet_infoline( "UtcDaliResolveUrl N_PATCH" );

  DALI_TEST_EQUALS( UrlType::N_PATCH, ResolveUrlType("foobar.9.gif"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::N_PATCH, ResolveUrlType("foobar.#.png"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::N_PATCH, ResolveUrlType("foobar.9.9.bmp"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::N_PATCH, ResolveUrlType("foobar.9.9.jpg[]=$$"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::N_PATCH, ResolveUrlType("foobar.9.#.#.9.wbpm123"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("svg.##.png"), TEST_LOCATION );

  DALI_TEST_EQUALS( UrlType::REGULAR_IMAGE, ResolveUrlType("svg.99.jpeg"), TEST_LOCATION );

  END_TEST;
}
