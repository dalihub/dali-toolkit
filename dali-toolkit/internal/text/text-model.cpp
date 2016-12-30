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

// CLASS HEADER
#include <dali-toolkit/internal/text/text-model.h>

namespace Dali
{

namespace Toolkit
{

namespace Text
{

ModelPtr Model::New()
{
  return ModelPtr( new Model() );
}

const Size& Model::GetControlSize() const
{
  return mVisualModel->mControlSize;
}

const Size& Model::GetLayoutSize() const
{
  return mVisualModel->GetLayoutSize();
}

const Vector2& Model::GetScrollPosition() const
{
  return mScrollPosition;
}

Layout::HorizontalAlignment Model::GetHorizontalAlignment() const
{
  return mHorizontalAlignment;
}

Layout::VerticalAlignment Model::GetVerticalAlignment() const
{
  return mVerticalAlignment;
}

bool Model::IsTextElideEnabled() const
{
  return mElideEnabled;
}

Length Model::GetNumberOfLines() const
{
  return mVisualModel->mLines.Count();
}

const LineRun* const Model::GetLines() const
{
  return mVisualModel->mLines.Begin();
}

Length Model::GetNumberOfGlyphs() const
{
  return mVisualModel->mGlyphs.Count();
}

const GlyphInfo* const Model::GetGlyphs() const
{
  return mVisualModel->mGlyphs.Begin();
}

const Vector2* const Model::GetLayout() const
{
  return mVisualModel->mGlyphPositions.Begin();
}

const Vector4* const Model::GetColors() const
{
  return mVisualModel->mColors.Begin();
}

const ColorIndex* const Model::GetColorIndices() const
{
  return mVisualModel->mColorIndices.Begin();
}

const Vector4& Model::GetDefaultColor() const
{
  return mVisualModel->mTextColor;
}

Model::Model()
: mLogicalModel(),
  mVisualModel(),
  mScrollPosition(),
  mHorizontalAlignment( Layout::HORIZONTAL_ALIGN_BEGIN ),
  mVerticalAlignment( Layout::VERTICAL_ALIGN_TOP ),
  mAlignmentOffset( 0.0f ),
  mElideEnabled( false )
{
  mLogicalModel = LogicalModel::New();
  mVisualModel = VisualModel::New();
}

Model::~Model()
{
}

} // namespace Text

} // namespace Toolkit

} // namespace Dali
