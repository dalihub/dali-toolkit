//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-effect-impl.h>
#include <dali-toolkit/internal/controls/scrollable/scroll-view/scroll-view-impl.h>

using namespace Dali;

namespace Dali
{

namespace Toolkit
{

namespace Internal
{

ScrollViewEffect::ScrollViewEffect()
: mScrollViewImpl(NULL)
{
}

ScrollViewEffect::~ScrollViewEffect()
{
}

void ScrollViewEffect::Attach(Toolkit::ScrollView& scrollView)
{
  DALI_ASSERT_ALWAYS( (!mScrollViewImpl) && "Already attached to a ScrollView" );

  mScrollViewImpl = &GetImpl(scrollView);

  OnAttach(scrollView);
}

void ScrollViewEffect::Detach(Toolkit::ScrollView& scrollView)
{
  DALI_ASSERT_ALWAYS( (mScrollViewImpl) && "Already detached from ScrollView" );
  DALI_ASSERT_ALWAYS( (&GetImpl(scrollView) == mScrollViewImpl) && "Effect attached to a different ScrollView");

  OnDetach(scrollView);

  mScrollViewImpl = NULL;
}

Toolkit::ScrollView ScrollViewEffect::GetScrollView()
{
  DALI_ASSERT_ALWAYS(mScrollViewImpl);

  return DownCast<Toolkit::ScrollView>( mScrollViewImpl->Self() );
}

} // namespace Internal

} // namespace Toolkit

} // namespace Dali
