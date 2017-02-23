#ifndef DALI_TOOLKIT_INTERNAL_BUILDER_DICTIONARY_H
#define DALI_TOOLKIT_INTERNAL_BUILDER_DICTIONARY_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

namespace Dali
{
extern bool CaseInsensitiveStringCompare( const std::string& a, const std::string& b );

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


typedef std::vector<std::string> DictionaryKeys;
inline void Merge( DictionaryKeys& toDict, const DictionaryKeys& fromDict )
{
  for( DictionaryKeys::const_iterator fromIter = fromDict.begin(); fromIter != fromDict.end(); ++fromIter )
  {
    const std::string& fromKey = (*fromIter);
    DictionaryKeys::iterator toIter = std::find( toDict.begin(), toDict.end(), fromKey );
    if( toIter == toDict.end() )
    {
      toDict.push_back( fromKey );
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
    EntryType entry;
    Element( const std::string&name, EntryType entry )
    : key( name ),
      entry( entry )
    {
    }
  };
  typedef std::vector<Element> Elements;
  Elements container;

public:
  /**
   * Only allow const iteration over the dictionary
   */
  typedef typename Elements::const_iterator iterator;

  /**
   * Constructor
   */
  Dictionary<EntryType>()
  {
  }

  /**
   * Add a key value pair to the dictionary.
   * If the entry does not already exist, add it to the dictionary
   * using a shallow copy
   */
  bool Add( const std::string& name, const EntryType& entry )
  {
    for( typename Elements::iterator iter = container.begin(); iter != container.end(); ++iter )
    {
      if( iter->key == name )
      {
        return false;
      }
    }
    container.push_back( Element(name, entry) );
    return true;
  }

  /**
   * Add a key-value pair to the dictionary
   * If the entry does not already exist, add it to the dictionary
   * (shallow copy)
   */
  bool Add( const char* name, const EntryType& entry )
  {
    bool result=false;
    if( name != NULL )
    {
      std::string theName(name);
      result=Add(theName, entry);
    }
    return result;
  }

  /**
   * Remove a key value pair from the dictionary.
   */
  void Remove( const std::string& name )
  {
    for( typename Elements::iterator iter = container.begin(); iter != container.end(); ++iter )
    {
      if( iter->key == name )
      {
        container.erase( iter );
        break;
      }
    }
  }

  /**
   * Remove a key value pair from the dictionary.
   */
  void Remove( const char* name )
  {
    if( name != NULL )
    {
      std::string theName(name);
      Remove(theName);
    }
  }

  void Merge( const Dictionary<EntryType>& dictionary )
  {
    for( typename Elements::const_iterator fromIter = dictionary.container.begin(); fromIter != dictionary.container.end(); ++fromIter )
    {
      bool found=false;
      for( typename Elements::iterator toIter = container.begin(); toIter != container.end(); ++toIter )
      {
        if( fromIter->key == toIter->key )
        {
          found=true;
          toIter->entry = fromIter->entry;
        }
      }
      if( !found )
      {
        container.push_back( Element(fromIter->key, fromIter->entry) );
      }
    }
  }

  /**
   * Find the element in the dictionary pointed at by key, and
   * insensitive search, and return a const pointer to it, or NULL
   */
  const EntryType* FindConst( const std::string& key ) const
  {
    if( ! key.empty() )
    {
      for( typename Elements::const_iterator iter = container.begin(); iter != container.end(); ++iter )
      {
        if( Dali::CaseInsensitiveStringCompare(iter->key, key ))
        {
          const EntryType* result = &(iter->entry);
          return result;
        }
      }
    }
    return NULL;
  }

  /**
   * Find the element in the dictionary pointed at by key using a case
   * insensitive search, and return a non-const pointer to it, or NULL
   */
  EntryType* Find( const std::string& key ) const
  {
    EntryType* result = NULL;
    if( ! key.empty() )
    {
      for( typename Elements::const_iterator iter = container.begin(); iter != container.end(); ++iter )
      {
        if( Dali::CaseInsensitiveStringCompare(iter->key, key ))
        {
          // Const cast because of const_iterator. const_iterator because, STL.
          result = const_cast<EntryType*>(&(iter->entry));
        }
      }
    }
    return result;
  }

  /**
   * Find the element in the dictionary pointed at by key using a case
   * insensitive search, and return a const pointer to it, or NULL
   */
  const EntryType* FindConst( const char* key ) const
  {
    if( key != NULL )
    {
      std::string theKey(key);
      return FindConst( theKey );
    }
    return NULL;
  }

  /**
   * Find the element in the dictionary pointed at by key using a case
   * insensitive search, and return a non-const pointer to it, or NULL
   */
  EntryType* Find( const char* key ) const
  {
    if( key != NULL )
    {
      std::string theKey(key);
      return Find( theKey );
    }
    return NULL;
  }
  /**
   * Return an iterator pointing at the first entry in the dictionary
   */
  typename Elements::const_iterator Begin() const
  {
    return container.begin();
  }

  /**
   * Return an iterator pointing past the last entry in the dictionary
   */
  typename Elements::const_iterator End() const
  {
    return container.end();
  }

  void GetKeys( DictionaryKeys& keys ) const
  {
    keys.clear();
    for( typename Elements::const_iterator iter = container.begin(); iter != container.end(); ++iter )
    {
      keys.push_back( (*iter).key );
    }
  }

  void Clear()
  {
    container.clear();
  }
};



}//Internal
}//Toolkit
}//Dali

#endif // DALI_TOOLKIT_INTERNAL_BUILDER_DICTIONARY_H
