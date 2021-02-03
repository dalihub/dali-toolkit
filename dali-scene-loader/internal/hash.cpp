/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include "dali-scene-loader/internal/hash.h"

namespace Dali
{
namespace SceneLoader
{
Hash::Hash(uint64_t initial)
: mValue(initial)
{
}

Hash& Hash::Add(bool b)
{
  mValue = Concatenate(b ? 0 : 1);
  return *this;
}

Hash& Hash::Add(int32_t i)
{
  mValue = Concatenate(i);
  return *this;
}

Hash& Hash::Add(uint32_t i)
{
  mValue = Concatenate(i);
  return *this;
}

Hash& Hash::Add(uint64_t i)
{
  mValue = Concatenate(i);
  return *this;
}

Hash& Hash::Add(float f)
{
  return AddObjectBytes(f);
}

Hash& Hash::Add(const char* cStr)
{
  return Add(cStr, strlen(cStr));
}

Hash& Hash::Add(const char* cStr, size_t len)
{
  auto i0 = reinterpret_cast<const uint8_t*>(cStr);
  return AddBytes(i0, i0 + len);
}

Hash& Hash::Add(const std::string& str)
{
  auto i0 = reinterpret_cast<const uint8_t*>(str.c_str());
  return AddBytes(i0, i0 + str.size());
}

Hash& Hash::AddBytes(const uint8_t* i0, const uint8_t* i1)
{
  while(i0 != i1)
  {
    mValue = Concatenate(*i0);
    ++i0;
  }
  return *this;
}

Hash::operator uint64_t() const
{
  return mValue;
}

uint64_t Hash::Concatenate(uint64_t value)
{
  return mValue * 31 + value;
}

} // namespace SceneLoader
} // namespace Dali
