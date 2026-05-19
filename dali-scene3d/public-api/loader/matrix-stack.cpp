/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali-scene3d/public-api/loader/matrix-stack.h>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/loader/utils.h>

namespace Dali::Scene3D::Loader
{
MatrixStack::MatrixStack()
{
  mStack.Reserve(16);
}

bool MatrixStack::IsEmpty() const
{
  return mStack.Empty();
}

void MatrixStack::Push(const Matrix& model)
{
  if(mStack.Empty())
  {
    mStack.PushBack(model);
  }
  else
  {
    Matrix m{false};
    Matrix::Multiply(m, model, mStack.Back());
    mStack.PushBack(m);
  }
}

const Matrix& MatrixStack::Top() const
{
  return mStack.Back();
}

void MatrixStack::Pop()
{
  DALI_ASSERT_ALWAYS(mStack.Count() > 0);
  mStack.Erase(mStack.End() - 1);
}

void MatrixStack::PopAll()
{
  mStack.Clear();
}

} // namespace Dali::Scene3D::Loader
