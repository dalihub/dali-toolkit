#ifndef DALI_SCENE_LOADER_MATRIX_STACK_H_
#define DALI_SCENE_LOADER_MATRIX_STACK_H_
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

// INTERNAL INCLUDES
#include "dali-scene-loader/public-api/api.h"

// EXTERNAL INCLUDES
#include "dali/public-api/math/matrix.h"
#include "dali/public-api/common/vector-wrapper.h"

namespace Dali
{
namespace SceneLoader
{

/**
 * @brief A stack of matrices whereby each newly pushed matrix is stored
 *  after being multiplied by the previous one (if any).
 * @note Current implementation reserves space for 16 matrices.
 */
class DALI_SCENE_LOADER_API MatrixStack
{
public:
  MatrixStack();

  bool IsEmpty() const;
  void Push(const Matrix& model);
  const Matrix& Top() const;
  void Pop();
  void PopAll();  // clears the stack, but retains the storage.

private:
  std::vector<Matrix> mStack;
};

}
}

#endif //DALI_SCENE_LOADER_MATRIX_STACK_H_
