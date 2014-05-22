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

#include <dali-toolkit/public-api/controls/page-turn-view/page-factory.h>

namespace Dali
{

namespace Toolkit
{

const std::string PageFactory::ACTOR_HITTABLE( "actor-hittable" );

PageFactory::PageFactory( )
: mNeedOffscreenRendering( false )
{
}

PageFactory::~PageFactory()
{
}

void PageFactory::EnableOffscreenRendering(  )
{
  mNeedOffscreenRendering = true;
}

bool PageFactory::IsOffscreenRenderingNeeded()
{
  return mNeedOffscreenRendering;
}

void PageFactory::SetActorHittability( Actor actor, bool hittable )
{
  // Create actor focusable property if not already created.
  Property::Index propertyActorHittable = actor.GetPropertyIndex(ACTOR_HITTABLE);
  if(propertyActorHittable == Property::INVALID_INDEX && hittable)
  {
    propertyActorHittable = actor.RegisterProperty(ACTOR_HITTABLE, true);
  }
  else
  {
    actor.SetProperty( propertyActorHittable, hittable );
  }
}

bool PageFactory::GetActorHittability( Actor actor )
{
  bool hittable = false;

  Property::Index propertyActorHittable = actor.GetPropertyIndex(ACTOR_HITTABLE);
  if(propertyActorHittable != Property::INVALID_INDEX)
  {
    hittable = actor.GetProperty<bool>( propertyActorHittable );
  }

  return hittable;
}

PageFactory::RefreshSignal& PageFactory::PageRefreshSignal()
{
  return mPageRefreshSignal;
}

void PageFactory::EmitPageRefreshSignal( int pageId )
{
  mPageRefreshSignal.Emit( pageId );
}

} // namespace Toolkit

} // namespace Dali
