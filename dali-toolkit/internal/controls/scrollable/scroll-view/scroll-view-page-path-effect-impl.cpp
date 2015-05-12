/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-page-path-effect-impl.h>

//INTERNAL INCLUDES
#include <dali-toolkit/public-api/controls/scrollable/scroll-view/scroll-view.h>

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ScrollViewPagePathEffect::ScrollViewPagePathEffect(Path path, const Vector3& forward, Dali::Property::Index inputPropertyIndex, const Vector3& pageSize, unsigned int pageCount)
:mPageSize(pageSize),
 mInputPropertyIndex(inputPropertyIndex),
 mPageCount(pageCount)
{
  //Create path constrainer
  mPathConstrainer = Dali::PathConstrainer::New();
  mPathConstrainer.SetProperty( PathConstrainer::Property::FORWARD, forward );

  Dali::Property::Value pointsProperty = path.GetProperty(Path::Property::POINTS);
  mPathConstrainer.SetProperty( PathConstrainer::Property::POINTS, pointsProperty );

  pointsProperty = path.GetProperty(Path::Property::CONTROL_POINTS);
  mPathConstrainer.SetProperty( PathConstrainer::Property::CONTROL_POINTS, pointsProperty );

  //Create linear constrainer
  pointsProperty = Property::Value(Property::ARRAY);
  pointsProperty.AppendItem(0.0f);
  pointsProperty.AppendItem(1.0f);
  pointsProperty.AppendItem(0.0f);
  mLinearConstrainer = Dali::LinearConstrainer::New();
  mLinearConstrainer.SetProperty( LinearConstrainer::Property::VALUE, pointsProperty );
}

ScrollViewPagePathEffect::~ScrollViewPagePathEffect()
{
}

void ScrollViewPagePathEffect::ApplyToPage( Actor page, unsigned int pageOrder )
{
  float pageHalfSize = mPageSize.x * 0.5f;
  Vector2 range = Vector2( pageHalfSize - (pageHalfSize*pageOrder),  -pageHalfSize - (pageHalfSize*pageOrder) );
  Vector2 wrap  = Vector2( range.x, -pageHalfSize*(mPageCount-2) + range.y);

  Toolkit::ScrollView scrollView = GetScrollView();

  //Position
  mPathConstrainer.Apply( Dali::Property( page, Dali::Actor::Property::POSITION ),
                          Dali::Property( scrollView, mInputPropertyIndex),
                          range, wrap
                        );

  //Rotation
  mPathConstrainer.Apply( Dali::Property( page, Dali::Actor::Property::ORIENTATION ),
                          Dali::Property( scrollView, mInputPropertyIndex ),
                          range, wrap
                        );

  //Alpha
  mLinearConstrainer.Apply( Dali::Property( page, Dali::Actor::Property::COLOR_ALPHA ),
                            Dali::Property( scrollView, mInputPropertyIndex ),
                            range, wrap
                          );

}

void ScrollViewPagePathEffect::OnAttach(Toolkit::ScrollView& scrollView)
{
}

void ScrollViewPagePathEffect::OnDetach(Toolkit::ScrollView& scrollView)
{
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
