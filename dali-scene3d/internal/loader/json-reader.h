#ifndef DALI_SCENE3D_LOADER_JSON_READER_H_
#define DALI_SCENE3D_LOADER_JSON_READER_H_
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
#include <algorithm>
#include <cstring>
#include <map>
#include <memory>
#include <sstream>
#include <string_view>

// INTERNAL INCLUDES
#include <dali-scene3d/third-party/json.h> // TODO : Since license issue, We shoud replace this thing as <dali-toolkit/devel-api/builder/json-parser.h>

namespace json
{
/**
 * @brief Helper for freeing the memory allocated by json_parse()
 */
struct json_value_deleter
{
  void operator()(json_value_s* p) const
  {
    free(p);
  }
};
typedef std::unique_ptr<json_value_s, json_value_deleter> unique_ptr;

/**
 * @brief Case sensitive comparison of json_string_s and c-string.
 * @return difference in first different character, or 0 if the strings are identical.
 */
int StrCmp(const json_string_s& js, const char* s);

/**
 * @brief Case sensitive comparison of json_string_s and std::string.
 * @return difference in first different character, or 0 if the strings are identical.
 */
int StrCmp(const json_string_s& js, const std::string& s);

/**
 * @brief Convenience function to compare json_string_s and other supported string type,
 *  in swapped order.
 */
template<typename String>
inline int StrCmp(String& s, const json_string_s& js)
{
  return -StrCmp(js, s);
}

/**
 * @brief Checks @a jv to be the given @a type, otherwise a std::runtime_error is thrown.
 */
void Validate(const json_value_s& jv, json_type_e type);

namespace detail
{
/**
 * @brief Compile-time type-enum mapping.
 */
template<typename T>
struct Type2Enum
{
};

#define TYPE2ENUM(x)             \
  template<>                     \
  struct Type2Enum<json_##x##_s> \
  {                              \
    enum                         \
    {                            \
      VALUE = json_type_##x      \
    };                           \
  };

TYPE2ENUM(object)
TYPE2ENUM(array)
TYPE2ENUM(string)
TYPE2ENUM(number)
#undef TYPE2ENUM
} // namespace detail

/**
 * @brief Casts the payload of a json_value_s to the given type.
 */
template<typename Out>
inline const Out& Cast(const json_value_s& j)
{
  Validate(j, static_cast<json_type_e>(detail::Type2Enum<typename std::decay<Out>::type>::VALUE));
  return *static_cast<const Out*>(j.payload);
}

/**
 * @brief Casts the payload of a json_value_s to the given type.
 * @note std::runtime_error is thrown if the value is not the given type.
 */
template<typename Out>
inline Out& Cast(json_value_s& j)
{
  Validate(j, static_cast<json_type_e>(detail::Type2Enum<typename std::decay<Out>::type>::VALUE));
  return *static_cast<Out*>(j.payload);
}

/**
 * @brief Helper function to find a child element of @a obj mapped to @a key.
 * @return Pointer to the element, or nullptr if it could not be found.
 */
json_value_s* FindObjectChild(const std::string& key, json_object_s& obj);

/**
 * @brief Helper functions for reading various value types.
 */
struct Read
{
  static bool Boolean(const json_value_s& j)
  {
    if(j.type == json_type_true)
    {
      return true;
    }
    else if(j.type == json_type_false)
    {
      return false;
    }
    else // try to interpret a numeric value.
    {
      return Number<int>(j) != 0;
    }
  }

  template<typename T>
  static T Number(const json_value_s& j)
  {
    auto&             jn = Cast<const json_number_s>(j);
    std::stringstream ss;
    for(auto i0 = jn.number, i1 = i0 + jn.number_size; i0 != i1; ++i0)
    {
      ss.put(*i0);
    }

    T result;
    if(ss >> result)
    {
      return result;
    }
    throw std::runtime_error("Failed to convert value to number");
  }

  template<typename E>
  static E Enum(const json_value_s& j)
  {
    size_t number = Number<size_t>(j);
    return static_cast<E>(number);
  }

  static std::string_view StringView(const json_value_s& j)
  {
    auto& js = Cast<json_string_s>(j);
    return std::string_view(js.string, js.string_size);
  }

  static std::string String(const json_value_s& j)
  {
    auto& js = Cast<const json_string_s>(j);
    return std::string(js.string, js.string_size);
  }

  template<typename T, T (*readElement)(const json_value_s&)>
  static std::vector<T> Array(const json_value_s& j)
  {
    auto&          ja = Cast<const json_array_s>(j);
    std::vector<T> result;
    result.reserve(ja.length);
    auto i = ja.start;
    while(i)
    {
      result.push_back(std::move(readElement(*i->value)));
      i = i->next;
    }
    return result;
  }
};

/**
 * @brief Core class for object properties.
 */
struct PropertyCore
{
protected:
  explicit PropertyCore(const std::string& key)
  : mKey(key)
  {
  }

  const std::string& GetKey() const
  {
    return mKey;
  }

private:
  std::string mKey;
};

/**
 * @brief Base class for the properties of a type T.
 */
template<typename T>
struct PropertyBase : PropertyCore
{
  using PropertyCore::GetKey;

  explicit PropertyBase(const std::string& key)
  : PropertyCore(key)
  {
  }

  virtual ~PropertyBase()
  {
  }

  virtual void Read(const json_value_s& j, T& obj) = 0;
};

/**
 * @brief Concrete property of an object to read into from JSON with a given function.
 */
template<class T, typename U>
struct Property : PropertyBase<T>
{
  using ReadFn        = U (*)(const json_value_s&);
  using MemberPtr     = U T::*;
  using SetterArgType = typename std::conditional<sizeof(U) <= sizeof(uintptr_t), U, const U&>::type;
  using Setter        = void (T::*)(SetterArgType);

  Property(const std::string& key, ReadFn read, MemberPtr ptr)
  : PropertyBase<T>(key),
    mRead(read),
    mAccessor(new DirectAccessor(ptr))
  {
  }

  Property(const std::string& key, ReadFn read, Setter setter)
  : PropertyBase<T>(key),
    mRead(read),
    mAccessor(new SetterAccessor(setter))
  {
  }

  ~Property()
  {
  }

  void Read(const json_value_s& j, T& obj) override
  {
    mAccessor->Set(mRead(j), obj);
  }

private:
  struct AccessorBase
  {
    virtual ~AccessorBase()
    {
    }

    virtual void Set(U value, T& obj) const = 0;
  };

  struct DirectAccessor : AccessorBase
  {
    DirectAccessor(MemberPtr ptr)
    : mPointer(ptr)
    {
    }

    void Set(U value, T& obj) const override
    {
      obj.*mPointer = std::move(value);
    }

    MemberPtr mPointer;
  };

  struct SetterAccessor : AccessorBase
  {
    SetterAccessor(Setter setter)
    : mSetter(setter)
    {
    }

    void Set(U value, T& obj) const override
    {
      (obj.*mSetter)(value);
    }

    Setter mSetter;
  };

  ReadFn                        mRead;
  std::unique_ptr<AccessorBase> mAccessor;
};

/**
 * @brief Helper function to make a Property for a member of type U, of object of type T.
 */
template<class T, typename U>
Property<T, U>* MakeProperty(const std::string& key, typename Property<T, U>::ReadFn readFn, U T::*ptr)
{
  return new Property<T, U>(key, readFn, ptr);
}

/**
 * @brief Core class for an object Reader.
 */
struct ReaderCore
{
protected:
  std::vector<void*> mProperties;

  ReaderCore() = default;

  ReaderCore(const ReaderCore& other) = delete;
  ReaderCore& operator=(const ReaderCore& other) = delete;

  ReaderCore(ReaderCore&& other) = default;
  ReaderCore& operator=(ReaderCore&& other) = default;
};

/**
 * @brief Object Reader template for reading into an object of a given type,
 *  with properties registered for the various members.
 */
template<typename T>
class Reader : protected ReaderCore
{
public:
  Reader() = default;

  Reader(const Reader<T>& other) = delete;
  Reader<T>& operator=(const Reader<T>& other) = delete;

  Reader(Reader<T>&& other) = default;
  Reader<T>& operator=(Reader&& other) = default;

  ~Reader()
  {
    for(auto& p : mProperties)
    {
      delete Cast(p);
    }
  }

  Reader<T>& Register(PropertyBase<T>& prop)
  {
    auto iInsert = std::lower_bound(mProperties.begin(), mProperties.end(), &prop, SortPredicate);
    if(iInsert == mProperties.end() || Cast(*iInsert)->GetKey() != prop.GetKey())
    {
      mProperties.insert(iInsert, &prop);
    }
    else
    {
      delete Cast(*iInsert);
      *iInsert = &prop;
    }
    return *this;
  }

  void Read(const json_object_s& jo, T& obj) const
  {
    auto i = jo.start;
    while(i)
    {
      auto iFind = std::lower_bound(mProperties.begin(), mProperties.end(), *i->name, FindPredicate);
      if(iFind != mProperties.end())
      {
        auto prop = Cast(*iFind);
        if(0 == StrCmp(*i->name, prop->GetKey()))
        {
          prop->Read(*i->value, obj);
        }
      }
      i = i->next;
    }
  }

private:
  static inline PropertyBase<T>* Cast(void* p)
  {
    return static_cast<PropertyBase<T>*>(p);
  }

  static bool SortPredicate(void* p, PropertyBase<T>* prop)
  {
    const auto prop0 = Cast(p);
    return prop0->GetKey().compare(prop->GetKey()) < 0;
  }

  static bool FindPredicate(void* p, const json_string_s& key)
  {
    const auto prop = Cast(p);
    return StrCmp(prop->GetKey(), key) < 0;
  }
};

/**
 * @brief Wraps a Reader<T> in a function usable as a Property<T>::ReadFn, i.e. to facilitate
 *  deserializing structures of nested objects.
 */
template<typename T>
struct ObjectReader
{
  static const Reader<T>* sReader;

  static T Read(const json_value_s& j)
  {
    T     result;
    auto& jo = Cast<json_object_s>(j);
    sReader->Read(jo, result);
    return result;
  }
};

template<typename T>
const Reader<T>* ObjectReader<T>::sReader = nullptr;

template<typename T>
void SetObjectReader(const Reader<T>& r)
{
  ObjectReader<T>::sReader = &r;
}

} // namespace json

#endif //DALI_SCENE3D_LOADER_JSON_READER_H_
