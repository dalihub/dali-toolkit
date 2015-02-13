#ifndef __DALI_TOOLKIT_INTERNAL_BUILDER_OPTIONAL__
#define __DALI_TOOLKIT_INTERNAL_BUILDER_OPTIONAL__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
template <typename T>
struct OptionalTypes
{
  typedef T ValueType;
  typedef const T& ReturnType;
  static ReturnType Get(const ValueType& v) { return v; }
  static ValueType Set(const ReturnType v) { return v; }
  static bool Ok(const ValueType& v) { return true; }
};

template <typename T>
struct OptionalTypes<T*>
{
  typedef T* ValueType;
  typedef const T* ReturnType;
  static ReturnType Get(const ValueType v) { return v; }
  static ValueType Set(const ReturnType v) { return v; }
  static bool Ok(const ReturnType v) { return NULL != v; }
};

template <typename T>
struct OptionalTypes<T&>
{
  typedef T* ValueType;
  typedef const T& ReturnType;
  static ReturnType Get(const ValueType v) { return *v; }
  static ValueType Set(const ReturnType v) { return &v; }
  static bool Ok(const ReturnType v) { return true; }
};

template <typename T>
class OptionalValue
{
public:
  typedef void ( OptionalValue::*bool_type )() const;
  typedef typename OptionalTypes<T>::ReturnType ReturnType;
  typedef typename OptionalTypes<T>::ValueType ValueType;

  OptionalValue(): mOk(false), mValue() {}
  OptionalValue( T value ): mOk(OptionalTypes<T>::Ok(value)), mValue(OptionalTypes<T>::Set(value)) {}
  OptionalValue( bool b, T value ): mOk(b), mValue(OptionalTypes<T>::Set(value)) {}

  ReturnType operator *() const { return OptionalTypes<T>::Get(mValue); }

  // safe bool idiom
  operator bool_type() const {
    return mOk == true ? &OptionalValue::this_type_does_not_support_comparisons : 0;
  }

private:
  bool mOk;
  ValueType mValue;
  void this_type_does_not_support_comparisons() const {}
};

template <typename T, typename U>
bool operator==( const OptionalValue<T>& lhs, const OptionalValue<U>& rhs )
{
  lhs.this_type_does_not_support_comparisons();
  return false;
}

template <typename T, typename U>
bool operator!=( const OptionalValue<T>& lhs, const OptionalValue<U>& rhs )
{
  lhs.this_type_does_not_support_comparisons();
  return false;
}

#endif // header
