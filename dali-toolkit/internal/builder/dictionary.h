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
   * Find the element in the dictionary pointed at by key, and
   * return a pointer to it, or NULL.
   */
  EntryType* Find( const std::string& key ) const
  {
    EntryType* result=NULL;

    if( ! key.empty() )
    {
      for( typename Elements::iterator iter = container.begin(); iter != container.end(); ++iter )
      {
        if( iter->key == key )
        {
          result = &(iter->entry);
          break;
        }
      }
    }
    return result;
  }

  /**
   * Find the element in the dictionary pointed at by key, and
   * return a pointer to it, or NULL
   */
  EntryType* Find( const char* key ) const
  {
    if( key != NULL )
    {
      std::string theKey(key);
      return Find(theKey);
    }
    return NULL;
  }

  /**
   * Find the element in the dictionary pointed at by key using a case
   * insensitive search, and return a const pointer to it, or NULL
   */
  const EntryType* FindCaseInsensitiveC( const std::string& key ) const
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
  EntryType* FindCaseInsensitive( const std::string& key ) const
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
  const EntryType* FindCaseInsensitiveC( const char* key ) const
  {
    if( key != NULL )
    {
      std::string theKey(key);
      return FindCaseInsensitiveC( theKey );
    }
    return NULL;
  }

  /**
   * Find the element in the dictionary pointed at by key using a case
   * insensitive search, and return a non-const pointer to it, or NULL
   */
  EntryType* FindCaseInsensitive( const char* key ) const
  {
    if( key != NULL )
    {
      std::string theKey(key);
      return FindCaseInsensitive( theKey );
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
};



}//Internal
}//Toolkit
}//Dali

#endif // DALI_TOOLKIT_INTERNAL_BUILDER_DICTIONARY_H
