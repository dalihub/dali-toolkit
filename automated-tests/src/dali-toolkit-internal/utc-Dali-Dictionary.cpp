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
 */

#include <dali-toolkit-test-suite-utils.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/internal/builder/dictionary.h>
using namespace Dali::Toolkit::Internal;


std::string_view test_keys[20] =
{
  "testkey0", "testkey1", "testkey2", "testkey3", "testkey4", "testkey5", "testkey6", "testkey7", "testkey8", "testkey9",
  "testkey10", "testkey11", "testkey12", "testkey13", "testkey14", "testkey15", "testkey16", "testkey17", "testkey18", "testkey19"
};

std::string_view testKeys[20] =
{
  "TestKey0", "TestKey1", "TestKey2", "TestKey3", "TestKey4", "TestKey5", "TestKey6", "TestKey7", "TestKey8", "TestKey9",
  "TestKey10", "TestKey11", "TestKey12", "TestKey13", "TestKey14", "TestKey15", "TestKey16", "TestKey17", "TestKey18", "TestKey19"
};


int UtcDaliBuilderDictionaryNew(void)
{
  Dictionary<int> dictionary;

  DictionaryKeys keys;
  dictionary.GetKeys(keys);

  DALI_TEST_CHECK( keys.empty() );
  DALI_TEST_EQUALS( keys.size(), 0, TEST_LOCATION );
  END_TEST;
}


int UtcDaliBuilderDictionaryAdd1(void)
{
  Dictionary<int> dictionary;

  for(int i=0; i<10; i++)
  {
    char buffer[16];
    sprintf(buffer, "testkey%d",i);
    bool added=false;
    if(i%2 == 0)
    {
      added = dictionary.Add(std::string(buffer), i);
    }
    else
    {
      added = dictionary.Add(buffer, i);
    }
    DALI_TEST_EQUALS(added, true, TEST_LOCATION);
  }

  DictionaryKeys keys;
  dictionary.GetKeys(keys);

  DALI_TEST_EQUALS( keys.size(), 10, TEST_LOCATION );
  for(int i=0; i<10; i++)
  {
    char buffer[16];
    sprintf(buffer, "testkey%d",i);
    auto iter = std::find(keys.begin(), keys.end(), std::string(buffer));
    DALI_TEST_CHECK(iter != keys.end());
  }

  END_TEST;
}

int UtcDaliBuilderDictionaryAdd2(void)
{
  Dictionary<int> dictionary;

  for(int i=0; i<10; i++)
  {
    char buffer[16];
    sprintf(buffer, "testkey%d",i);
    bool added=false;
    if(i%2 == 0)
    {
      added = dictionary.Add(std::string(buffer), i);
    }
    else
    {
      added = dictionary.Add(buffer, i);
    }
    DALI_TEST_EQUALS(added, true, TEST_LOCATION);
  }

  DictionaryKeys keys;
  dictionary.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 10, TEST_LOCATION );

  bool added = dictionary.Add("testkey5", 1);
  DALI_TEST_EQUALS(added, false, TEST_LOCATION);
  DALI_TEST_EQUALS(*dictionary.Find("testkey5"), 5, TEST_LOCATION);

  dictionary.Clear();
  DALI_TEST_EQUALS(dictionary.Begin()==dictionary.End(), true, TEST_LOCATION);
  dictionary.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliBuilderDictionaryRemoveP(void)
{
  Dictionary<int> dictionary;

  for(int i=0; i<10; i++)
  {
    bool added=false;
    added = dictionary.Add(std::string(testKeys[i]), i);
    DALI_TEST_EQUALS(added, true, TEST_LOCATION);
  }

  DictionaryKeys keys;
  dictionary.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 10, TEST_LOCATION );

  for(int i=0; i<10; i++)
  {
    if(i%2==0)
    {
      dictionary.Remove(test_keys[i]); // Should fail (case sensitive)
    }
    else
    {
      dictionary.Remove(testKeys[i]);
    }
  }
  dictionary.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 5, TEST_LOCATION );

  dictionary.Clear();
  DALI_TEST_EQUALS(dictionary.Begin()==dictionary.End(), true, TEST_LOCATION);
  dictionary.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 0, TEST_LOCATION );

  END_TEST;
}


int UtcDaliBuilderDictionaryRemoveN(void)
{
  Dictionary<int> dictionary;

  for(int i=0; i<10; i++)
  {
    bool added=false;
    added = dictionary.Add(std::string(testKeys[i]), i);
    DALI_TEST_EQUALS(added, true, TEST_LOCATION);
  }

  DictionaryKeys keys;
  dictionary.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 10, TEST_LOCATION );

  dictionary.Remove("randomkey");
  dictionary.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 10, TEST_LOCATION );

  END_TEST;
}


int UtcDaliBuilderDictionaryMerge1(void)
{
  // Test that "overlapping" dicts merge into 1 successfully
  Dictionary<int> dictionary1;

  for(int i=0; i<10; i++)
  {
    dictionary1.Add(std::string(test_keys[i]), i);
  }

  Dictionary<int> dictionary2;
  for(int i=0; i<20; i++)
  {
    dictionary2.Add(std::string(testKeys[i]), i);
  }

  dictionary1.Merge(dictionary2);
  DictionaryKeys keys;
  dictionary1.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 30, TEST_LOCATION ); // Now have 2 case versions of 10 keys :/ - broken by design?

  for(int i=0; i<20;++i)
  {
    // Check both cases of keys
    auto ptr1 = dictionary1.FindConst(test_keys[i]);
    auto ptr2 = dictionary1.FindConst(testKeys[i]);

    DALI_TEST_CHECK( nullptr != ptr1 );
    DALI_TEST_CHECK( nullptr != ptr2 );
  }

  END_TEST;
}

int UtcDaliBuilderDictionaryMerge2(void)
{
  // Test that non-overlapping dicts merge successfully
  Dictionary<int> dictionary1;
  for(int i=0; i<10; i++) // Add first 10 from lowercase keys
  {
    dictionary1.Add(std::string(test_keys[i]), i);
  }

  Dictionary<int> dictionary2;
  for(int i=10; i<20; i++) // add last 10 from capitalized keys
  {
    dictionary2.Add(std::string(testKeys[i]), i);
  }

  dictionary1.Merge(dictionary2);
  DictionaryKeys keys;
  dictionary1.GetKeys(keys);
  DALI_TEST_EQUALS( keys.size(), 20, TEST_LOCATION ); // check it's an amalgam of both

  for(int i=0; i<20;++i)
  {
    // Check both cases of keys
    DALI_TEST_CHECK( nullptr != dictionary1.FindConst(test_keys[i]));
    DALI_TEST_CHECK( nullptr != dictionary1.FindConst(testKeys[i]));
  }

  END_TEST;
}


template<typename EntryType>
struct TestElement
{
  std::string key;
  EntryType entry;
  TestElement(std::string name, EntryType entry)
  : key(std::move(name)),
    entry(std::move(entry))
  {
  }
};

int UtcDaliBuilderDictionaryFindP(void)
{
  // Test that non-overlapping dicts merge successfully
  Dictionary<int> dictionary;
  for(int i=0; i<10; i++) // Add first 10 from lowercase keys
  {
    dictionary.Add(std::string(test_keys[i]), i);
  }

  // Test that the entries can be directly modified
  for(int i=0; i<10; i++)
  {
    auto entryPtr = dictionary.Find(testKeys[i]);
    DALI_TEST_CHECK( entryPtr != nullptr );
    *entryPtr = i+10;
  }

  for(int i=0; i<10; ++i)
  {
    auto entryPtr = dictionary.Find(testKeys[i]);
    DALI_TEST_CHECK( entryPtr != nullptr );
    DALI_TEST_EQUALS( *entryPtr, i+10, TEST_LOCATION );
  }

  END_TEST;
}
