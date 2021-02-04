
#ifndef DALI_TOOLKIT_INTERNAL_ROUND_ROBIN_CONTAINER_VIEW_H
#define DALI_TOOLKIT_INTERNAL_ROUND_ROBIN_CONTAINER_VIEW_H

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

// EXTERNAL INCLUDES
#include <cstddef>
#include <vector>

namespace Dali
{
namespace Toolkit
{
namespace Internal
{
/**
 * @brief RoundRobinContainerView is a view to a container that allows iterating through the elements cyclically.
 */
template<typename T>
class RoundRobinContainerView
{
public:
  using ContainerType = std::vector<T>;

  /**
   * @brief Constructs a new RoundRobinControlView with the given number elements using the provided factory.
   * @param[in] numberOfElements The number of elements in the container
   * @param[in] factory          Factory function of functor that will be used to create instances of the elements
   */
  template<typename FactoryType>
  RoundRobinContainerView(size_t numberOfElements, const FactoryType& factory)
  : mElements(),
    mNextIndex{}
  {
    mElements.reserve(numberOfElements);
    for(unsigned i = {}; i < numberOfElements; ++i)
    {
      mElements.push_back(factory());
    }
  }

  /**
   * @brief Reset the position of the iterator returned by GetNext() to the first element.
   */
  void Reset()
  {
    mNextIndex = 0u;
  }

  /**
   * @brief Returns the next element on the container.
   * @return Iterator for the next element
   */
  typename ContainerType::iterator GetNext()
  {
    SetValidNextIndex();

    return mElements.begin() + mNextIndex++;
  }

  /**
   * @brief Returns the iterator to the end of the container.
   *
   * Can be used to compare against GetNext() to check if the container is empty.
   *
   * @return The container end() element
   */
  typename ContainerType::const_iterator End() const
  {
    return mElements.cend();
  }

  // default members
  ~RoundRobinContainerView() = default;

  RoundRobinContainerView(const RoundRobinContainerView&) = delete;
  RoundRobinContainerView& operator=(const RoundRobinContainerView&) = delete;
  RoundRobinContainerView(RoundRobinContainerView&&)                 = default;
  RoundRobinContainerView& operator=(RoundRobinContainerView&&) = default;

private:
  /**
   * @brief Check the current index and reset if necessary.
   */
  void SetValidNextIndex()
  {
    if(mNextIndex >= mElements.size())
    {
      Reset();
    }
  }

private:
  ContainerType mElements;  //< container of elements
  size_t        mNextIndex; //< index to the next element to be viewed
};

} // namespace Internal

} // namespace Toolkit

} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_ROUND_ROBIN_CONTAINER_VIEW_H
