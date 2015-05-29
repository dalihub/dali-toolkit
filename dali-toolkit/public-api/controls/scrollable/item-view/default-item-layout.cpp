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

// HEADER
#include <dali-toolkit/public-api/controls/scrollable/item-view/default-item-layout.h>

// INTERNAL INCLUDES
#include <dali-toolkit/internal/controls/scrollable/item-view/depth-layout.h>
#include <dali-toolkit/internal/controls/scrollable/item-view/grid-layout.h>
#include <dali-toolkit/internal/controls/scrollable/item-view/spiral-layout.h>

namespace Dali
{

namespace Toolkit
{

namespace DefaultItemLayout
{

ItemLayoutPtr New( Type type )
{
  ItemLayoutPtr itemLayout;

  switch ( type )
  {
    case DEPTH:
    {
      itemLayout = Internal::DepthLayout::New();
      break;
    }

    case GRID:
    {
      itemLayout = Internal::GridLayout::New();
      break;
    }

    case LIST:
    {
      Internal::GridLayoutPtr layout = Internal::GridLayout::New();
      layout->SetNumberOfColumns( 1 );
      itemLayout = layout;
      break;
    }

    case SPIRAL:
    {
      itemLayout = Internal::SpiralLayout::New();
      break;
    }
  }

  return itemLayout;
}

} // namespace DefaultItemLayout

} // namespace Toolkit

} // namespace Dali
