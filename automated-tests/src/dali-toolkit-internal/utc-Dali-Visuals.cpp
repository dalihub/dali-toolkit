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
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/wireframe/wireframe-visual.h>

using namespace Dali::Toolkit::Internal;

namespace
{

class DummyWireframeVisual : public WireframeVisual
{
public:

  DummyWireframeVisual( VisualFactoryCache& factoryCache )
  : WireframeVisual( factoryCache )
  {}

  virtual ~DummyWireframeVisual()
  {}

  void DoSetProperty( Dali::Property::Index index, const Dali::Property::Value& propertyValue )
  {
    WireframeVisual::DoSetProperty( index, propertyValue );
  }

  Dali::Property::Value DoGetProperty( Dali::Property::Index index )
  {
    return WireframeVisual::DoGetProperty( index );
  }
};

} // namespace

int UtcDaliWireframeVisual(void)
{
  // The goal of this test case is to cover the WireframeVisual::DoSetProperty() and
  // WireframeVisual::DoGetProperty() which are unreachable from the public API.

  tet_infoline( " UtcDaliWireframeVisual" );

  VisualFactoryCache* visualFactoryCache = new VisualFactoryCache();

  DummyWireframeVisual visual( *visualFactoryCache );

  visual.DoSetProperty( Dali::Toolkit::Visual::Property::TYPE, Dali::Toolkit::Visual::WIREFRAME );
  Dali::Property::Value value = visual.DoGetProperty( Dali::Toolkit::Visual::Property::TYPE );

  tet_result(TET_PASS);
  END_TEST;
}
