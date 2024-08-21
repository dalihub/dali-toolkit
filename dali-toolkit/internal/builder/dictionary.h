#ifndef DALI_TOOLKIT_INTERNAL_BUILDER_DICTIONARY_H
#define DALI_TOOLKIT_INTERNAL_BUILDER_DICTIONARY_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/common/vector-wrapper.h>

#include <algorithm>
#include <string_view>

namespace Dali
{
extern bool CaseInsensitiveStringCompare(std::string_view a, std::string_view b);

namespace Toolkit
{
namespace Internal
{
/**
 * The Dictionary template class enables a means of storing key-value
 * pairs where the keys are strings and the value can be a complex
 * type.
 *
 * It enables lookup of keys via case-insensitive match.
 */

using DictionaryKeys = std::vector<std::string>;

inline void Merge(DictionaryKeys& toDict, const DictionaryKeys& fromDict)
{
  for(const auto& element : fromDict)
  {
    auto iter = std::find(toDict.cbegin(), toDict.cend(), element);
    if(iter == toDict.cend())
    {
      toDict.push_back(element);
    }
  }
}

template<typename EntryType>
class Dictionary
{
private:
  /**
   * Element is a key-value pair
   */
  struct Element
  {
    std::string key;
    EntryType   entry;
    Element(std::string name, EntryType entry)
    : key(std::move(name)),
      entry(std::move(entry))
    {
    }
  };
  using Elements = std::vector<Element>;
  Elements container;

  auto FindElementCaseInsensitive(std::string_view key) const
  {
    return std::find_if(
      Begin(), End(), [key](auto& e)
      { return Dali::CaseInsensitiveStringCompare(e.key, key); });
  }

  auto FindElement(std::string_view key)
  {
    return std::find_if(container.begin(), container.end(), [key](auto& e)
                        { return bool(key == e.key); });
  }

public:
  /**
   * Only allow const iteration over the dictionary
   */
  using iterator = typename Elements::const_iterator;

  /**
   * Constructor
   */
  Dictionary() = default;

  /**
   * Add a key value pair to the dictionary.
   * If the entry does not already exist, add it to the dictionary
   */
  bool Add(std::string name, EntryType entry)
  {
    auto iter = FindElement(name);
    if(iter != End())
    {
      return false;
    }

    container.push_back(Element(std::move(name), std::move(entry)));
    return true;
  }

  /**
   * Add a key-value pair to the dictionary
   * If the entry does not already exist, add it to the dictionary
   */
  bool Add(const char* name, EntryType entry)
  {
    if(name != nullptr)
    {
      return Add(std::string(name), std::move(entry));
    }
    return false;
  }

  /**
   * Remove a key value pair from the dictionary.
   */
  void Remove(std::string_view name)
  {
    if(!name.empty())
    {
      auto iter = FindElement(name);

      if(iter != End())
      {
        container.erase(iter);
      }
    }
  }

  void Merge(const Dictionary<EntryType>& dictionary)
  {
    for(const auto& element : dictionary.container)
    {
      auto iter = FindElement(element.key);

      if(iter == End())
      {
        container.push_back(Element(element.key, element.entry));
      }
      else
      {
        iter->entry = element.entry;
      }
    }
  }

  /**
   * Find the element in the dictionary pointed at by key, and
   * insensitive search, and return a const pointer to it, or NULL
   */
  const EntryType* FindConst(std::string_view key) const
  {
    if(!key.empty())
    {
      auto iter = FindElementCaseInsensitive(key);

      if(iter != End())
      {
        return &(iter->entry);
      }
    }
    return nullptr;
  }

  /**
   * Find the element in the dictionary pointed at by key using a case
   * insensitive search, and return a non-const pointer to it, or NULL
   */
  EntryType* Find(std::string_view key) const
  {
    if(!key.empty())
    {
      auto iter = FindElementCaseInsensitive(key);

      if(iter != End())
      {
        return const_cast<EntryType*>(&(iter->entry));
      }
    }
    return nullptr;
  }

  iterator Begin() const
  {
    return container.cbegin();
  }

  /**
   * Return an iterator pointing past the last entry in the dictionary
   */
  iterator End() const
  {
    return container.cend();
  }

  void GetKeys(DictionaryKeys& keys) const
  {
    keys.clear();
    for(const auto& element : container)
    {
      keys.push_back(element.key);
    }
  }

  void Clear()
  {
    container.clear();
  }
};

} // namespace Internal
} // namespace Toolkit
} // namespace Dali

#endif // DALI_TOOLKIT_INTERNAL_BUILDER_DICTIONARY_H
