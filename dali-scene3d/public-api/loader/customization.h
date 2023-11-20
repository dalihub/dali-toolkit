#ifndef DALI_SCENE3D_LOADER_CUSTOMIZATION_STATE_H_
#define DALI_SCENE3D_LOADER_CUSTOMIZATION_STATE_H_
/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-scene3d/public-api/api.h>

namespace Dali::Scene3D::Loader
{
/**
 * @brief Offers a description of an aspect of the scene that can be customized:
 *     the number of options, and the name of the nodes that are registered
 *     for the tag, whose children will be shown / hidden based on selection.
 * @SINCE_2_0.7
 */
struct DALI_SCENE3D_API Customization
{
  using Tag        = std::string;
  using OptionType = uint32_t;

  /**
   * @brief A mapping of customizations to tags.
   * @SINCE_2_0.7
   */
  struct DALI_SCENE3D_API Map
  {
    Map();
    ~Map();

    /**
     * @brief Maps the given @a customization to the given @a tag, overwriting any previous mapping to the same tag.
     * @SINCE_2_0.7
     * @return Pointer to the inserted (or pre-existing) Customization instance.
     */
    Customization* Set(Tag tag, Customization&& customization);

    /**
     * @brief Attempts to retrieve a const Customization based on the given @a tag.
     * @SINCE_2_0.7
     */
    const Customization* Get(Tag tag) const;

    /**
     * @brief Attempts to retrieve a Customization based on the given @a tag.
     * @SINCE_2_0.7
     */
    Customization* Get(Tag tag);

    /**
     * @brief Returns the number of elements.
     * @SINCE_2_0.7
     */
    uint32_t Size() const;

    /**
     * @brief Removes every element from the Map.
     * @SINCE_2_0.7
     */
    void Clear();

  private:
    struct Impl;
    const std::unique_ptr<Impl> mImpl;
  };

  /**
   * @brief A mapping of choices - indices of children of customization nodes to use - to tags.
   * @SINCE_2_0.7
   */
  struct DALI_SCENE3D_API Choices
  {
    Choices();
    ~Choices();

    /**
     * @brief Maps the given @a option to the given @a tag, overwriting any previous mapping to the same tag.
     * @SINCE_2_0.7
     */
    void Set(Tag tag, OptionType option);

    /**
     * @brief Attempts to retrieve a Customization based on the given @a tag.
     * @SINCE_2_0.7
     * @return A Customization based on the given @a tag.
     * @note Returns NONE if @a tag is not known.
     */
    OptionType Get(Tag tag) const;

    /**
     * @brief Returns the number of elements.
     * @SINCE_2_0.7
     * @return The number of elements.
     */
    uint32_t Size() const;

    /**
     * @brief Removes every element from the underlying map.
     * @SINCE_2_0.7
     */
    void Clear();

  private:
    struct Impl;
    const std::unique_ptr<Impl> mImpl;
  };

  static const OptionType NONE = OptionType(-1);

  OptionType               numOptions = 0;
  std::vector<std::string> nodes; // to apply option to.
};

} // namespace Dali::Scene3D::Loader

#endif //DALI_SCENE3D_LOADER_CUSTOMIZATION_STATE_H_
