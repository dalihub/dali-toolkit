/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include "dummy-visual.h"

#include <dali-toolkit/internal/visuals/visual-factory-cache.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

DummyVisualPtr DummyVisual::New( const Property::Map& properties )
{
  VisualFactoryCache* factoryCache = new VisualFactoryCache(false);

  DummyVisualPtr dummyVisualPtr( new DummyVisual( *factoryCache ) );

  return dummyVisualPtr;
}

DummyVisual::DummyVisual( VisualFactoryCache& factoryCache )
: Visual::Base( factoryCache, Visual::FittingMode::FILL, Toolkit::Visual::Type::COLOR ),
  mActionCounter( 0 )
{
}

void DummyVisual::DoCreatePropertyMap( Property::Map& map ) const
{
  // Implement if required
}

void DummyVisual::DoCreateInstancePropertyMap( Property::Map& map ) const
{
  // Implement if required
}

void DummyVisual::DoSetProperties( const Property::Map& propertyMap )
{
  // Implement if required
}

void DummyVisual::OnSetTransform()
{
  // Implement if required
}

void DummyVisual::DoSetOnStage( Actor& actor )
{
  // Implement if required
}

void DummyVisual::OnDoAction( const Property::Index actionName, const Property::Value& attributes )
{
  if ( DummyVisual::TEST_ACTION == actionName )
  {
    mActionCounter++;  // GetActionCounter can be used to test for this.
  }
  // Further Actions can be added here
}

unsigned int DummyVisual::GetActionCounter() const
{
  return mActionCounter;
}

void DummyVisual::ResetActionCounter()
{
  mActionCounter = 0;
}

} // Internal

} // namespace Toolkit

} // namespace Dali
