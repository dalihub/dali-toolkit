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
#include <dali-toolkit/internal/text/text-view.h>

// EXTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

struct View::Impl
{
  VisualModelPtr mVisualModel;
};

View::View()
: mImpl( NULL )
{
  mImpl = new View::Impl();
}

View::~View()
{
  delete mImpl;
}

void View::SetVisualModel( VisualModelPtr visualModel )
{
  mImpl->mVisualModel = visualModel;
}

const Vector4& View::GetTextColor() const
{
  if ( mImpl->mVisualModel )
  {
    VisualModel& model = *mImpl->mVisualModel;
    return model.GetTextColor();
  }
  return Vector4::ZERO;
}

const Vector2& View::GetShadowOffset() const
{
  if ( mImpl->mVisualModel )
  {
    VisualModel& model = *mImpl->mVisualModel;
    return model.GetShadowOffset();
  }
  return Vector2::ZERO;
}

const Vector4& View::GetShadowColor() const
{
  if ( mImpl->mVisualModel )
  {
    VisualModel& model = *mImpl->mVisualModel;
    return model.GetShadowColor();
  }
  return Vector4::ZERO;
}

const Vector4& View::GetUnderlineColor() const
{
  if ( mImpl->mVisualModel )
  {
    VisualModel& model = *mImpl->mVisualModel;
    return model.GetUnderlineColor();
  }
  return Vector4::ZERO;
}

bool View::IsUnderlineEnabled() const
{
  if ( mImpl->mVisualModel )
  {
    VisualModel& model = *mImpl->mVisualModel;
    return model.IsUnderlineEnabled();
  }
  return false;
}

float View::GetUnderlineHeight() const
{
  if ( mImpl->mVisualModel )
  {
    VisualModel& model = *mImpl->mVisualModel;
    return model.GetUnderlineHeight();
  }
  return 0.0f;
}

Length View::GetNumberOfGlyphs() const
{
  if( mImpl->mVisualModel )
  {
    VisualModel& model = *mImpl->mVisualModel;

    Length glyphCount = model.GetNumberOfGlyphs();
    Length positionCount = model.GetNumberOfGlyphPositions();

    DALI_ASSERT_DEBUG( positionCount <= glyphCount && "Invalid glyph positions in Model" );

    return (positionCount < glyphCount) ? positionCount : glyphCount;
  }

  return 0;
}

void View::GetGlyphs( GlyphInfo* glyphs,
                      GlyphIndex glyphIndex,
                      Length numberOfGlyphs ) const
{
  if( mImpl->mVisualModel )
  {
    mImpl->mVisualModel->GetGlyphs( glyphs, glyphIndex, numberOfGlyphs );
  }
}

void View::GetGlyphPositions( Vector2* glyphPositions,
                              GlyphIndex glyphIndex,
                              Length numberOfGlyphs ) const
{
  if( mImpl->mVisualModel )
  {
    mImpl->mVisualModel->GetGlyphPositions( glyphPositions, glyphIndex, numberOfGlyphs );
  }
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
