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
#include <dali-toolkit/devel-api/visuals/text-visual-properties.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>
#include <dali-toolkit/internal/visuals/visual-factory-cache.h>
#include <dali-toolkit/internal/visuals/wireframe/wireframe-visual.h>
#include <dali-toolkit/dali-toolkit.h>

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

int UtcDaliTextVisual(void)
{
  tet_infoline(" UtcDaliTextVisual");
  ToolkitTestApplication application;

  Stage stage = Stage::GetCurrent();

  Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
  control.SetParentOrigin( ParentOrigin::CENTER );

  Dali::Property::Map map;
  map[ Dali::Toolkit::Visual::Property::TYPE ] = Dali::Toolkit::DevelVisual::TEXT;
  map[ Dali::Toolkit::TextVisual::Property::ENABLE_MARKUP ] = true;
  std::string markupText( "<color value='blue'><font size='50'>H</font></color>ello <color value='blue'><font size='50'>w</font></color>orld" );
  map[ Dali::Toolkit::TextVisual::Property::TEXT ] = markupText;
  map[ Dali::Toolkit::TextVisual::Property::TEXT_COLOR ] = Dali::Vector4( 0.25f, 0.25f, 0.5f, 1.f );
  map[ Dali::Toolkit::TextVisual::Property::FONT_FAMILY ] = "TizenSansRegular";
  map[ Dali::Toolkit::TextVisual::Property::POINT_SIZE ] = 30.f;
  map[ Dali::Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT ] = "CENTER";
  map[ Dali::Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT ] = "CENTER";

  control.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );

  stage.Add( control );

  try
  {
    application.SendNotification();
    application.Render();
  }
  catch( ... )
  {
    tet_result(TET_FAIL);
  }

  // The renderer should be removed.
  control.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
  control.SetSize( 1.f, 0.f );

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( 0u, control.GetRendererCount(), TEST_LOCATION );

  tet_result(TET_PASS);
  END_TEST;
}
