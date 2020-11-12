/*
* Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/public-api/matrix-stack.h"
#include "dali-scene-loader/public-api/utils.h"

namespace Dali
{
namespace SceneLoader
{

MatrixStack::MatrixStack()
{
  mStack.reserve(16);
}

bool MatrixStack::IsEmpty() const
{
  return mStack.empty();
}

void MatrixStack::Push(const Matrix& model)
{
  if (mStack.empty())
  {
    mStack.push_back(model);
  }
  else
  {
    Matrix m{ false };
    Matrix::Multiply(m, model, mStack.back());
    mStack.push_back(m);
  }
}

const Matrix& MatrixStack::Top() const
{
  return mStack.back();
}

void MatrixStack::Pop()
{
  DALI_ASSERT_ALWAYS(mStack.size() > 0);
  mStack.pop_back();
}

void MatrixStack::PopAll()
{
  mStack.clear();
}

}
}
