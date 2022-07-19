#ifndef DALI_SCENE3D_LOADER_HASH_H_
#define DALI_SCENE3D_LOADER_HASH_H_
/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <cstdint>
#include <cstring>
#include <string>

namespace Dali
{
namespace Scene3D
{
namespace Loader
{
/**
 * @brief Rudimentary hash generator that follows a builder pattern.
 */
class Hash
{
public:
  static constexpr uint64_t DEFAULT_SEED = 61081;

  explicit Hash(uint64_t initial = DEFAULT_SEED);

  /**
   * @brief Applies a boolean to the hash.
   * @return Its updated self.
   */
  Hash& Add(bool b);

  /**
   * @brief Applies a signed 32-bit integer to the hash.
   * @return Its updated self.
   */
  Hash& Add(int32_t i);

  /**
   * @brief Applies an unsigned 32-bit integer to the hash.
   * @return Its updated self.
   */
  Hash& Add(uint32_t i);

  /**
   * @brief Applies an unsigned 64-bit integer to the hash.
   * @return Its updated self.
   */
  Hash& Add(uint64_t i);

  /**
   * @brief Applies a float to the hash.
   * @return Its updated self.
   */
  Hash& Add(float f);

  /**
   * @brief Applies a c-string to the hash.
   * @return Its updated self.
   */
  Hash& Add(const char* cStr);

  /**
   * @brief Applies a c-string @a cStr of @a len characters, to the hash.
   * @return Its updated self.
   */
  Hash& Add(const char* cStr, size_t len);

  /**
   * @brief Applies a string to the hash.
   * @return Its updated self.
   */
  Hash& Add(const std::string& str);

  /**
   * @brief Applies a series of bytes between @a i0 and @a i1 to the hash.
   * @return Its updated self.
   */
  Hash& AddBytes(const uint8_t* i0, const uint8_t* i1);

  /**
   * @brief Applies the bytes of an object @a value, to the hash.
   * @return Its updated self.
   */
  template<typename T>
  Hash& AddObjectBytes(const T& value);

  operator uint64_t() const;

private:
  uint64_t mValue;

  uint64_t Concatenate(uint64_t value);
};

template<typename T>
Hash& Hash::AddObjectBytes(const T& value)
{
  auto i0 = reinterpret_cast<const uint8_t*>(&value);
  auto i1 = i0 + sizeof(T);
  while(i0 != i1)
  {
    mValue = Concatenate(*i0);
    ++i0;
  }
  return *this;
}

} // namespace Loader
} // namespace Scene3D
} // namespace Dali

#endif // DALI_SCENE3D_LOADER_HASH_H_
