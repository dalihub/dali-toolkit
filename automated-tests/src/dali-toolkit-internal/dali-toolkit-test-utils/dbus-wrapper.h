#ifndef DALI_INTERNAL_ACCESSIBILITY_BRIDGE_DBUS_H
#define DALI_INTERNAL_ACCESSIBILITY_BRIDGE_DBUS_H

/*
 * Copyright 2019  Samsung Electronics Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// EXTERNAL INCLUDES
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

// INTERNAL INCLUDES
//#include <dali/public-api/adaptor-framework/accessibility.h>
//#include <dali/internal/accessibility/bridge/optional.h>

#include <dali/devel-api/adaptor-framework/accessibility.h>

#define ATSPI_PREFIX_PATH "/org/a11y/atspi/accessible/"
#define ATSPI_NULL_PATH "/org/a11y/atspi/null"

//// BART QUICK HACK
#define bart_assert(x) \
  do                   \
  {                    \
  } while(0)

struct _Eina_Value;

/// \cond
struct ObjectPath
{
  std::string value;
};

struct DBusWrapper
{
  virtual ~DBusWrapper() = default;

  enum class ConnectionType
  {
    SYSTEM,
    SESSION
  };

#define DEFINE_TYPE(name, eldbus_name, unref_call) \
  struct name                                      \
  {                                                \
    virtual ~name() = default;                     \
  };                                               \
  using name##Ptr     = std::shared_ptr<name>;     \
  using name##WeakPtr = std::weak_ptr<name>;

  DEFINE_TYPE(Connection, Eldbus_Connection, )
  DEFINE_TYPE(MessageIter, Eldbus_Message_Iter, eldbus_message_iter_container_close(Value))
  DEFINE_TYPE(Message, Eldbus_Message, eldbus_message_unref(Value))
  DEFINE_TYPE(Proxy, Eldbus_Proxy, eldbus_proxy_unref(Value))
  DEFINE_TYPE(Object, Eldbus_Object, eldbus_object_unref(Value))
  DEFINE_TYPE(Pending, Eldbus_Pending, )
  DEFINE_TYPE(EventPropertyChanged, Eldbus_Proxy_Event_Property_Changed, )

#undef DEFINE_TYPE
  virtual ConnectionPtr eldbus_address_connection_get_impl(const std::string& addr) = 0;

#define eldbus_message_iter_arguments_append_impl_basic_impl(type_set, type_get, sig)                 \
  virtual void eldbus_message_iter_arguments_append_impl(const MessageIterPtr& it, type_set src) = 0; \
  virtual bool eldbus_message_iter_get_and_next_impl(const MessageIterPtr& it, type_get& dst)    = 0;
#define eldbus_message_iter_arguments_append_impl_basic(type, sig) \
  eldbus_message_iter_arguments_append_impl_basic_impl(type, type, sig)

  eldbus_message_iter_arguments_append_impl_basic(uint8_t, y)
    eldbus_message_iter_arguments_append_impl_basic(uint16_t, q)
      eldbus_message_iter_arguments_append_impl_basic(uint32_t, u)
        eldbus_message_iter_arguments_append_impl_basic(uint64_t, t)
          eldbus_message_iter_arguments_append_impl_basic(int16_t, n)
            eldbus_message_iter_arguments_append_impl_basic(int32_t, i)
              eldbus_message_iter_arguments_append_impl_basic(int64_t, x)
                eldbus_message_iter_arguments_append_impl_basic(double, d)
                  eldbus_message_iter_arguments_append_impl_basic(bool, b)
                    eldbus_message_iter_arguments_append_impl_basic_impl(const std::string&, std::string, s)
                      eldbus_message_iter_arguments_append_impl_basic_impl(const ObjectPath&, ObjectPath, o)

#undef eldbus_message_iter_arguments_append_impl_basic
#undef eldbus_message_iter_arguments_append_impl_basic_impl

                        virtual MessageIterPtr eldbus_message_iter_container_new_impl(const MessageIterPtr& it, int type, const std::string& sig) = 0;
  virtual MessageIterPtr eldbus_message_iter_get_and_next_by_type_impl(const MessageIterPtr& it, int type)                                        = 0;
  virtual MessageIterPtr eldbus_message_iter_get_impl(const MessagePtr& it, bool write)                                                           = 0;
  virtual MessagePtr     eldbus_proxy_method_call_new_impl(const ProxyPtr& proxy, const std::string& funcName)                                    = 0;
  virtual MessagePtr     eldbus_proxy_send_and_block_impl(const ProxyPtr& proxy, const MessagePtr& msg)                                           = 0;
  virtual bool           eldbus_message_error_get_impl(const MessagePtr& msg, std::string& name, std::string& text)                               = 0;
  virtual std::string    eldbus_message_signature_get_impl(const MessagePtr& msg)                                                                 = 0;

  using SendCallback                                                                                                                                             = std::function<void(const MessagePtr& msg)>;
  virtual PendingPtr    eldbus_proxy_send_impl(const ProxyPtr& proxy, const MessagePtr& msg, const SendCallback& callback)                                       = 0;
  virtual std::string   eldbus_proxy_interface_get_impl(const ProxyPtr&)                                                                                         = 0;
  virtual void          eldbus_proxy_signal_handler_add_impl(const ProxyPtr& proxy, const std::string& member, const std::function<void(const MessagePtr&)>& cb) = 0;
  virtual std::string   eldbus_message_iter_signature_get_impl(const MessageIterPtr& iter)                                                                       = 0;
  virtual MessagePtr    eldbus_message_method_return_new_impl(const MessagePtr& msg)                                                                             = 0;
  virtual MessagePtr    eldbus_message_error_new_impl(const MessagePtr& msg, const std::string& err, const std::string& txt)                                     = 0;
  virtual PendingPtr    eldbus_connection_send_impl(const ConnectionPtr& conn, const MessagePtr& msg)                                                            = 0;
  virtual MessagePtr    eldbus_message_signal_new_impl(const std::string& path, const std::string& iface, const std::string& name)                               = 0;
  virtual MessagePtr    eldbus_message_ref_impl(const MessagePtr& msg)                                                                                           = 0;
  virtual ConnectionPtr eldbus_connection_get_impl(ConnectionType type)                                                                                          = 0;
  virtual std::string   eldbus_connection_unique_name_get_impl(const ConnectionPtr& conn)                                                                        = 0;
  virtual ObjectPtr     eldbus_object_get_impl(const ConnectionPtr& conn, const std::string& bus, const std::string& path)                                       = 0;
  virtual ProxyPtr      eldbus_proxy_get_impl(const ObjectPtr& obj, const std::string& interface)                                                                = 0;
  virtual ProxyPtr      eldbus_proxy_copy_impl(const ProxyPtr& ptr)                                                                                              = 0;
  virtual void          eldbus_name_request_impl(const ConnectionPtr&, const std::string&)
  {
  } // no-op
  virtual void eldbus_name_release_impl(const ConnectionPtr&, const std::string&)
  {
  } // no-op

  class StringStorage
  {
  public:
    struct char_ptr_deleter
    {
      void operator()(char* p)
      {
        free(p);
      }
    };
    std::vector<std::unique_ptr<char, char_ptr_deleter>> storage;

    const char* add(const char* txt)
    {
      auto ptr = strdup(txt);
      storage.push_back(std::unique_ptr<char, char_ptr_deleter>(ptr));
      return storage.back().get();
    }
    const char* add(const std::string& txt)
    {
      return add(txt.c_str());
    }
  };

  struct CallId
  {
    static std::atomic<unsigned int> LastId;
    unsigned int                     id = ++LastId;
  };
  struct MethodInfo
  {
    CallId                                                                     id;
    std::string                                                                memberName;
    std::vector<std::pair<std::string, std::string>>                           in, out; // _Eldbus_Arg_Info
    std::function<DBusWrapper::MessagePtr(const DBusWrapper::MessagePtr& msg)> callback;
  };
  struct SignalInfo
  {
    CallId                                           id;
    std::string                                      memberName;
    std::vector<std::pair<std::string, std::string>> args;
    unsigned int                                     uniqueId;
  };
  struct PropertyInfo
  {
    CallId                                                                                               setterId, getterId;
    std::string                                                                                          memberName, typeSignature;
    std::function<std::string(const DBusWrapper::MessagePtr&src, const DBusWrapper::MessageIterPtr&dst)> getCallback, setCallback;
  };
  struct SignalId
  {
    CallId id;

    SignalId() = default;
    SignalId(CallId id)
    : id(id)
    {
    }
  };
  virtual void        add_interface_impl(bool fallback, const std::string& pathName, const ConnectionPtr& connection, std::vector<std::function<void()>>& destructors, const std::string& interfaceName, std::vector<MethodInfo>& dscrMethods, std::vector<PropertyInfo>& dscrProperties, std::vector<SignalInfo>& dscrSignals) = 0;
  virtual void        add_property_changed_event_listener_impl(const ProxyPtr& proxy, const std::string& interface, const std::string& name, std::function<void(const _Eina_Value*)> cb)                                                                                                                                        = 0;
  static DBusWrapper* Installed();
  static void         Install(std::unique_ptr<DBusWrapper>);

  StringStorage Strings;
};

namespace detail
{
enum class MethodType
{
  Method,
  Getter,
  Setter
};
}

namespace std
{
template<>
struct hash<std::tuple<std::string, std::string, std::string>>
{
  size_t operator()(const std::tuple<std::string, std::string, std::string>& a) const
  {
    auto   a1 = std::hash<std::string>()(std::get<0>(a));
    auto   a2 = std::hash<std::string>()(std::get<1>(a));
    auto   a3 = std::hash<std::string>()(std::get<2>(a));
    size_t v  = a1;
    v         = (v * 11400714819323198485llu) + a2;
    v         = (v * 11400714819323198485llu) + a3;
    return v;
  }
};
template<>
struct hash<std::tuple<std::string, std::string, std::string, detail::MethodType>>
{
  size_t operator()(const std::tuple<std::string, std::string, std::string, detail::MethodType>& a) const
  {
    auto   a1 = std::hash<std::string>()(std::get<0>(a));
    auto   a2 = std::hash<std::string>()(std::get<1>(a));
    auto   a3 = std::hash<std::string>()(std::get<2>(a));
    auto   a4 = static_cast<size_t>(std::get<3>(a));
    size_t v  = a1;
    v         = (v * 11400714819323198485llu) + a2;
    v         = (v * 11400714819323198485llu) + a3;
    v         = (v * 11400714819323198485llu) + a4;
    return v;
  }
};
template<>
struct hash<std::tuple<std::string, std::string, unsigned int>>
{
  size_t operator()(const std::tuple<std::string, std::string, unsigned int>& a) const
  {
    auto   a1 = std::hash<std::string>()(std::get<0>(a));
    auto   a2 = std::hash<std::string>()(std::get<1>(a));
    auto   a3 = std::get<2>(a);
    size_t v  = a1;
    v         = (v * 11400714819323198485llu) + a2;
    v         = (v * 11400714819323198485llu) + a3;
    return v;
  }
};
} // namespace std
namespace detail
{
template<typename T>
struct DBusSigImpl
{
  enum
  {
    value = 0,
    end   = 0
  };
};
template<typename T>
struct DBusSig
{
  enum
  {
    value = DBusSigImpl<typename std::decay<T>::type>::value,
    end   = DBusSigImpl<typename std::decay<T>::type>::end
  };
};
template<typename T, typename Q, size_t I, size_t S>
struct IndexFromTypeTupleImpl
{
  enum
  {
    value = std::is_same<typename std::decay<typename std::tuple_element<I, T>::type>::type, Q>::value ? I : IndexFromTypeTupleImpl<T, Q, I + 1, S>::value
  };
};
template<typename T, typename Q, size_t S>
struct IndexFromTypeTupleImpl<T, Q, S, S>
{
  enum
  {
    value = S
  };
};
template<typename T, typename Q>
struct IndexFromTypeTuple
{
  enum
  {
    value = IndexFromTypeTupleImpl<T, typename std::decay<Q>::type, 0, std::tuple_size<T>::value>::value
  };
};
template<typename T, typename = void>
struct Encoder;
template<size_t I, size_t S, typename... ARGS>
struct EncoderTuple;
} // namespace detail
struct TestDBusWrapper : public DBusWrapper
{
  using MethodType = detail::MethodType;
  ConnectionPtr                                                                                                                   connection;
  std::unordered_map<std::tuple<std::string, std::string, std::string, MethodType>, std::function<MessagePtr(const MessagePtr&)>> testMethods;
  std::unordered_map<std::tuple<std::string, std::string, std::string, MethodType>, std::function<MessagePtr(const MessagePtr&)>> daliMethods;
  std::unordered_map<std::tuple<std::string, std::string, unsigned int>, std::string>                                             daliSignalsMap;
  std::unordered_map<std::tuple<std::string, std::string, std::string>, std::function<void(const MessagePtr&)>>                   daliSignals;
  std::unordered_map<std::tuple<std::string, std::string, std::string>, std::function<void(const _Eina_Value*)>>                  propertyChangeListeners;

  std::vector<std::function<void()>> asyncCalls;

  template<typename T>
  struct Variant
  {
    T value;
    Variant() = default;
    Variant(T t)
    : value(std::move(t))
    {
    }
  };
  template<typename... ARGS>
  void Encode(const MessagePtr& m, const std::tuple<ARGS...>& args)
  {
    auto iter = eldbus_message_iter_get_impl(m, true);
    detail::EncoderTuple<0, sizeof...(ARGS), ARGS...>::encode(*this, iter, args);
  }
  template<typename... ARGS>
  void Decode(const MessagePtr& m, std::tuple<ARGS...>& args)
  {
    auto iter = eldbus_message_iter_get_impl(m, false);
    detail::EncoderTuple<0, sizeof...(ARGS), ARGS...>::decode(*this, args, iter);
  }
  MessagePtr newMessage(const std::string& path, const std::string& interface, const std::string& name, bool reply);
  MessagePtr newReplyMessage(const MessagePtr& m);

  template<typename... ARGS>
  void fromTestEmitSignal(std::string path, const std::string& interface, const std::string& name, const std::tuple<ARGS...>& args)
  {
    if(path.empty()) throw error{};
    if(path[0] != '/') path = "/org/a11y/atspi/accessible/" + path;
    auto msg = newMessage(path, interface, name, false);
    Encode(msg, args);
    auto it = daliSignals.find(std::tuple<std::string, std::string, std::string>{path, interface, name});
    if(it == daliSignals.end()) throw error{};
    it->second(msg);
  }
  static std::shared_ptr<_Eina_Value> createEinaValue(bool);
  template<typename T>
  void fromTestChangeProperty(std::string path, const std::string& interface, const std::string& name, T new_value)
  {
    auto v = createEinaValue(new_value);
    if(path.empty()) throw error{};
    if(path[0] != '/') path = "/org/a11y/atspi/accessible/" + path;
    auto it = propertyChangeListeners.find(std::tuple<std::string, std::string, std::string>{path, interface, name});
    if(it == propertyChangeListeners.end()) throw error{};
    it->second(v.get());
  }
  template<typename... ARGS1, typename... ARGS2>
  std::tuple<ARGS1...> fromTestCall(const std::string& path, const std::string& interface, const std::string& name, const std::tuple<ARGS2...>& args)
  {
    auto msg = newMessage(path, interface, name, false);
    Encode(msg, args);
    auto        res = call(daliMethods, "daliMethods", msg, MethodType::Method);
    std::string a, b;
    if(eldbus_message_error_get_impl(res, a, b)) throw error{} << "call to " << path << " " << interface << " " << name << " failed: " << a << ": " << b;
    std::tuple<ARGS1...> tmp;
    Decode(res, tmp);
    return tmp;
  }
  template<typename T>
  T fromTestGet(const std::string& path, const std::string& interface, const std::string& name)
  {
    auto        msg = newMessage(path, interface, name, false);
    auto        res = call(daliMethods, "daliMethods", msg, MethodType::Getter);
    std::string a, b;
    if(eldbus_message_error_get_impl(res, a, b)) throw error{} << "call to " << path << " " << interface << " " << name << " failed: " << a << ": " << b;
    std::tuple<T> tmp;
    Decode(res, tmp);
    return std::move(std::get<0>(tmp));
  }
  template<typename... ARGS1, typename T>
  std::tuple<ARGS1...> fromTestSet(const std::string& path, const std::string& interface, const std::string& name, const T& arg)
  {
    auto msg = newMessage(path, interface, name, false);
    Encode(msg, TestDBusWrapper::Variant<T>{arg});
    auto        res = call(daliMethods, "daliMethods", msg, MethodType::Setter);
    std::string a, b;
    if(eldbus_message_error_get_impl(res, a, b)) throw error{} << "call to " << path << " " << interface << " " << name << " failed: " << a << ": " << b;
    std::tuple<ARGS1...> tmp;
    Decode(res, tmp);
    return tmp;
  }
  template<typename... ARGS1>
  void voidTestCall(const std::string& path, const std::string& interface, const std::string& name, const std::tuple<ARGS1...>& args)
  {
    auto msg = newMessage(path, interface, name, false);
    Encode(msg, args);
    auto        res = call(daliMethods, "daliMethods", msg, MethodType::Method);
    std::string a, b;
    if(eldbus_message_error_get_impl(res, a, b)) throw error{} << "call to " << path << " " << interface << " " << name << " failed: " << a << ": " << b;
  }

  TestDBusWrapper();
  ~TestDBusWrapper();

  class error : public std::exception
  {
    std::shared_ptr<std::ostringstream> temp = std::make_shared<std::ostringstream>();
    mutable std::string                 text;

  public:
    error() = default;

    template<typename T>
    error& operator<<(T&& t)
    {
      *temp << std::forward<T>(t);
      return *this;
    }
    const char* what() const noexcept override
    {
      text = temp->str();
      return text.c_str();
    }
  };
#define DEFINE_TYPE(name) \
  struct name##Impl;      \
  static name##Impl* get(const name##Ptr&);
  DEFINE_TYPE(Connection)
  DEFINE_TYPE(Proxy)
  DEFINE_TYPE(Object)
  DEFINE_TYPE(Message)
  DEFINE_TYPE(MessageIter)
#undef DEFINE_TYPE

  class Element;
  using ElementList  = std::list<Element>;
  using ElementMap   = std::map<std::string, std::string>;
  using ElementTuple = std::tuple<
    uint8_t,
    uint16_t,
    uint32_t,
    uint64_t,
    int16_t,
    int32_t,
    int64_t,
    double,
    bool,
    std::string,
    ObjectPath,
    ElementList>;

  class Element
  {
    ElementTuple data;
    char         signature_ = 0, end_ = 0, index_ = 0;
    template<typename T>
    void set(T&& t, char signature = detail::DBusSig<T>::value)
    {
      signature_ = signature;
      end_       = detail::DBusSig<T>::end;
      index_     = static_cast<char>(detail::IndexFromTypeTuple<ElementTuple, T>::value);
      get<T>()   = std::forward<T>(t);
    }

  public:
    template<typename T>
    Element(T&& t, typename std::enable_if<detail::DBusSig<T>::value != 0>::type* = nullptr)
    {
      set(std::forward<T>(t));
    }
    Element(ElementList t, int signature)
    {
      set(std::move(t), static_cast<char>(signature));
    }

    char signature() const
    {
      return signature_;
    }
    char end() const
    {
      return end_;
    }
    char index() const
    {
      return index_;
    }
    bool isContainer() const
    {
      return index_ == detail::IndexFromTypeTuple<ElementTuple, ElementList>::value;
    }
    template<typename T, typename = typename std::enable_if<detail::DBusSig<T>::value != 0>::type>
    bool is() const
    {
      return index_ == detail::IndexFromTypeTuple<ElementTuple, T>::value;
    }
    template<typename T, typename = typename std::enable_if<detail::DBusSig<T>::value != 0>::type>
    const T& get() const
    {
      if(!is<T>()) throw error{};
      return std::get<detail::IndexFromTypeTuple<ElementTuple, T>::value>(data);
    }
    template<typename T, typename = typename std::enable_if<detail::DBusSig<T>::value != 0>::type>
    T& get()
    {
      if(!is<T>()) throw error{};
      return std::get<detail::IndexFromTypeTuple<ElementTuple, T>::value>(data);
    }
  };

  ConnectionPtr eldbus_address_connection_get_impl(const std::string& addr) override;

#define eldbus_message_iter_arguments_append_impl_basic_impl(type_set, type_get, sig)              \
  void eldbus_message_iter_arguments_append_impl(const MessageIterPtr& it, type_set src) override; \
  bool eldbus_message_iter_get_and_next_impl(const MessageIterPtr& it, type_get& dst) override;
#define eldbus_message_iter_arguments_append_impl_basic(type, sig) \
  eldbus_message_iter_arguments_append_impl_basic_impl(type, type, sig)

  eldbus_message_iter_arguments_append_impl_basic(uint8_t, y)
    eldbus_message_iter_arguments_append_impl_basic(uint16_t, q)
      eldbus_message_iter_arguments_append_impl_basic(uint32_t, u)
        eldbus_message_iter_arguments_append_impl_basic(uint64_t, t)
          eldbus_message_iter_arguments_append_impl_basic(int16_t, n)
            eldbus_message_iter_arguments_append_impl_basic(int32_t, i)
              eldbus_message_iter_arguments_append_impl_basic(int64_t, x)
                eldbus_message_iter_arguments_append_impl_basic(double, d)
                  eldbus_message_iter_arguments_append_impl_basic(bool, b)
                    eldbus_message_iter_arguments_append_impl_basic_impl(const std::string&, std::string, s)
                      eldbus_message_iter_arguments_append_impl_basic_impl(const ObjectPath&, ObjectPath, o)

#undef eldbus_message_iter_arguments_append_impl_basic
#undef eldbus_message_iter_arguments_append_impl_basic_impl

                        MessageIterPtr eldbus_message_iter_container_new_impl(const MessageIterPtr& it, int type, const std::string& sig) override;
  MessageIterPtr eldbus_message_iter_get_and_next_by_type_impl(const MessageIterPtr& it, int type) override;
  MessageIterPtr eldbus_message_iter_get_impl(const MessagePtr& it, bool write) override;
  MessagePtr     eldbus_proxy_method_call_new_impl(const ProxyPtr& proxy, const std::string& funcName) override;
  MessagePtr     eldbus_proxy_send_and_block_impl(const ProxyPtr& proxy, const MessagePtr& msg) override;
  bool           eldbus_message_error_get_impl(const MessagePtr& msg, std::string& name, std::string& text) override;
  std::string    eldbus_message_signature_get_impl(const MessagePtr& msg) override;
  PendingPtr     eldbus_proxy_send_impl(const ProxyPtr& proxy, const MessagePtr& msg, const SendCallback& callback) override;
  std::string    eldbus_proxy_interface_get_impl(const ProxyPtr&) override;
  void           eldbus_proxy_signal_handler_add_impl(const ProxyPtr& proxy, const std::string& member, const std::function<void(const MessagePtr&)>& cb) override;
  std::string    eldbus_message_iter_signature_get_impl(const MessageIterPtr& iter) override;
  MessagePtr     eldbus_message_method_return_new_impl(const MessagePtr& msg) override;
  MessagePtr     eldbus_message_error_new_impl(const MessagePtr& msg, const std::string& err, const std::string& txt) override;
  PendingPtr     eldbus_connection_send_impl(const ConnectionPtr& conn, const MessagePtr& msg) override;
  MessagePtr     eldbus_message_signal_new_impl(const std::string& path, const std::string& iface, const std::string& name) override;
  MessagePtr     eldbus_message_ref_impl(const MessagePtr& msg) override;
  ConnectionPtr  eldbus_connection_get_impl(ConnectionType type) override;
  std::string    eldbus_connection_unique_name_get_impl(const ConnectionPtr& conn) override;
  ObjectPtr      eldbus_object_get_impl(const ConnectionPtr& conn, const std::string& bus, const std::string& path) override;
  ProxyPtr       eldbus_proxy_get_impl(const ObjectPtr& obj, const std::string& interface) override;
  ProxyPtr       eldbus_proxy_copy_impl(const ProxyPtr& ptr) override;
  void           add_property_changed_event_listener_impl(const ProxyPtr& proxy, const std::string& interface, const std::string& name, std::function<void(const _Eina_Value*)> cb) override;
  void           add_interface_impl(bool fallback, const std::string& pathName, const ConnectionPtr& connection, std::vector<std::function<void()>>& destructors, const std::string& interfaceName, std::vector<MethodInfo>& dscrMethods, std::vector<PropertyInfo>& dscrProperties, std::vector<SignalInfo>& dscrSignals) override;
  static bool    completed(const MessageIterPtr& iter);

private:
  MessagePtr call(std::unordered_map<std::tuple<std::string, std::string, std::string, MethodType>, std::function<MessagePtr(const MessagePtr&)>>& mp, const std::string& name, const MessagePtr& msg, MethodType type);
};

namespace detail
{
template<>
struct DBusSigImpl<uint8_t>
{
  enum
  {
    value = 'y',
    end   = 0
  };
};
template<>
struct DBusSigImpl<uint16_t>
{
  enum
  {
    value = 'q',
    end   = 0
  };
};
template<>
struct DBusSigImpl<uint32_t>
{
  enum
  {
    value = 'u',
    end   = 0
  };
};
template<>
struct DBusSigImpl<uint64_t>
{
  enum
  {
    value = 't',
    end   = 0
  };
};
template<>
struct DBusSigImpl<int16_t>
{
  enum
  {
    value = 'n',
    end   = 0
  };
};
template<>
struct DBusSigImpl<int32_t>
{
  enum
  {
    value = 'i',
    end   = 0
  };
};
template<>
struct DBusSigImpl<int64_t>
{
  enum
  {
    value = 'x',
    end   = 0
  };
};
template<>
struct DBusSigImpl<double>
{
  enum
  {
    value = 'd',
    end   = 0
  };
};
template<>
struct DBusSigImpl<bool>
{
  enum
  {
    value = 'b',
    end   = 0
  };
};
template<>
struct DBusSigImpl<std::string>
{
  enum
  {
    value = 's',
    end   = 0
  };
};
template<>
struct DBusSigImpl<ObjectPath>
{
  enum
  {
    value = 'o',
    end   = 0
  };
};
template<>
struct DBusSigImpl<TestDBusWrapper::ElementList>
{
  enum
  {
    value = '(',
    end   = ')'
  };
};
template<>
struct DBusSigImpl<TestDBusWrapper::ElementMap>
{
  enum
  {
    value = '{',
    end   = '}'
  };
};

template<typename T>
struct Encoder<T, decltype(TestDBusWrapper().eldbus_message_iter_arguments_append_impl(TestDBusWrapper::MessageIterPtr(), T()))>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const T& src)
  {
    w.eldbus_message_iter_arguments_append_impl(tgt, src);
  }
  static void decode(TestDBusWrapper& w, T& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    if(!w.eldbus_message_iter_get_and_next_impl(src, tgt)) throw TestDBusWrapper::error{};
  }
};
template<typename T>
struct Encoder<T, typename std::enable_if<std::is_enum<T>::value>::type>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const T& src)
  {
    Encoder<typename std::underlying_type<T>::type>::encode(w, tgt, static_cast<typename std::underlying_type<T>::type>(src));
  }
  static void decode(TestDBusWrapper& w, T& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    typename std::underlying_type<T>::type v = 0;
    Encoder<typename std::underlying_type<T>::type>::decode(w, v, src);
    tgt = static_cast<T>(v);
  }
};
template<typename T>
struct Encoder<const T>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const T& src)
  {
    Encoder<T>::encode(w, tgt, src);
  }
  static void decode(TestDBusWrapper& w, T& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    Encoder<T>::decode(w, tgt, src);
  }
};
template<typename T>
struct Encoder<const T&>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const T& src)
  {
    Encoder<T>::encode(w, tgt, src);
  }
  static void decode(TestDBusWrapper& w, T& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    Encoder<T>::decode(w, tgt, src);
  }
};
template<typename T>
struct Encoder<T&>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const T& src)
  {
    Encoder<T>::encode(w, tgt, src);
  }
  static void decode(TestDBusWrapper& w, T& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    Encoder<T>::decode(w, tgt, src);
  }
};
template<size_t I, size_t S, typename... ARGS>
struct EncoderTuple
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const std::tuple<ARGS...>& src)
  {
    Encoder<typename std::tuple_element<I, std::tuple<ARGS...>>::type>::encode(w, tgt, std::get<I>(src));
    EncoderTuple<I + 1, S, ARGS...>::encode(w, tgt, src);
  }
  static void decode(TestDBusWrapper& w, std::tuple<ARGS...>& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    Encoder<typename std::tuple_element<I, std::tuple<ARGS...>>::type>::decode(w, std::get<I>(tgt), src);
    EncoderTuple<I + 1, S, ARGS...>::decode(w, tgt, src);
  }
};
template<size_t S, typename... ARGS>
struct EncoderTuple<S, S, ARGS...>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const std::tuple<ARGS...>& src)
  {
  }
  static void decode(TestDBusWrapper& w, std::tuple<ARGS...>& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
  }
};
template<typename... ARGS>
struct Encoder<std::tuple<ARGS...>>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const std::tuple<ARGS...>& src)
  {
    auto var = w.eldbus_message_iter_container_new_impl(tgt, 'r', "");
    EncoderTuple<0, sizeof...(ARGS), ARGS...>::encode(w, var, src);
  }
  static void decode(TestDBusWrapper& w, std::tuple<ARGS...>& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    auto s = w.eldbus_message_iter_get_and_next_by_type_impl(src, 'r');
    if(!s) throw TestDBusWrapper::error{};
    EncoderTuple<0, sizeof...(ARGS), ARGS...>::decode(w, tgt, s);
  }
};
template<typename A, typename B>
struct Encoder<std::pair<A, B>>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const std::pair<A, B>& src, char type = 'r')
  {
    auto var = w.eldbus_message_iter_container_new_impl(tgt, type, "");
    Encoder<A>::encode(w, var, src.first);
    Encoder<B>::encode(w, var, src.second);
  }
  static void decode(TestDBusWrapper& w, std::pair<A, B>& tgt, const TestDBusWrapper::MessageIterPtr& src, char type = 'r')
  {
    auto s = w.eldbus_message_iter_get_and_next_by_type_impl(src, type);
    if(!s) throw TestDBusWrapper::error{};
    Encoder<A>::decode(w, tgt.first, s);
    Encoder<B>::decode(w, tgt.second, s);
  }
};
template<typename T>
struct Encoder<TestDBusWrapper::Variant<T>>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const TestDBusWrapper::Variant<T>& src)
  {
    //w.eldbus_message_iter_arguments_append_impl(tgt, src);
    auto var = w.eldbus_message_iter_container_new_impl(tgt, 'v', "");
    Encoder<T>::encode(w, var, src.value);
  }
  static void decode(TestDBusWrapper& w, TestDBusWrapper::Variant<T>& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    auto s = w.eldbus_message_iter_get_and_next_by_type_impl(src, 'v');
    if(!s) throw TestDBusWrapper::error{};
    Encoder<T>::decode(w, tgt.value, s);
  }
};
template<typename T>
struct Encoder<std::vector<T>>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const std::vector<T>& src)
  {
    auto var = w.eldbus_message_iter_container_new_impl(tgt, 'a', "");
    for(auto& q : src)
      Encoder<T>::encode(w, var, q);
  }
  static void decode(TestDBusWrapper& w, std::vector<T>& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    auto s = w.eldbus_message_iter_get_and_next_by_type_impl(src, 'a');
    if(!s) throw TestDBusWrapper::error{};
    while(!TestDBusWrapper::completed(s))
    {
      tgt.push_back({});
      Encoder<T>::decode(w, tgt.back(), s);
    }
  }
};
template<typename K, typename V>
struct Encoder<std::unordered_map<K, V>>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const std::unordered_map<K, V>& src)
  {
    auto var = w.eldbus_message_iter_container_new_impl(tgt, 'a', "");
    for(auto& q : src)
    {
      Encoder<typename std::unordered_map<K, V>::value_type>::encode(w, var, q, 'e');
    }
  }
  static void decode(TestDBusWrapper& w, std::unordered_map<K, V>& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    auto s = w.eldbus_message_iter_get_and_next_by_type_impl(src, 'a');
    if(!s) throw TestDBusWrapper::error{};
    while(!TestDBusWrapper::completed(s))
    {
      std::pair<K, V> tmp;
      Encoder<std::pair<K, V>>::decode(w, tmp, s, 'e');
      tgt.insert(std::move(tmp));
    }
  }
};
template<typename T, size_t I>
struct Encoder<std::array<T, I>>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const std::array<T, I>& src)
  {
    auto var = w.eldbus_message_iter_container_new_impl(tgt, 'a', "");
    for(auto& q : src)
      Encoder<T>::encode(w, var, q);
  }
  static void decode(TestDBusWrapper& w, std::array<T, I>& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    auto s = w.eldbus_message_iter_get_and_next_by_type_impl(src, 'a');
    if(!s) throw TestDBusWrapper::error{};
    size_t i = 0;
    while(!TestDBusWrapper::completed(s))
    {
      if(i >= tgt.size()) throw TestDBusWrapper::error{};
      Encoder<T>::decode(w, tgt[i], s);
      ++i;
    }
    if(i != tgt.size()) throw TestDBusWrapper::error{};
  }
};
template<>
struct Encoder<Dali::Accessibility::Address>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const Dali::Accessibility::Address& src)
  {
    Encoder<std::tuple<std::string, ObjectPath>>::encode(w, tgt, std::tuple<std::string, ObjectPath>{src.GetBus(), ObjectPath{"/org/a11y/atspi/accessible/" + src.GetPath()}});
  }
  static void decode(TestDBusWrapper& w, Dali::Accessibility::Address& tgt, const TestDBusWrapper::MessageIterPtr& src)
  {
    std::tuple<std::string, ObjectPath> tmp;
    Encoder<std::tuple<std::string, ObjectPath>>::decode(w, tmp, src);
    static const size_t prefixSize = std::string{"/org/a11y/atspi/accessible/"}.size();
    tgt                            = {std::move(std::get<0>(tmp)), std::get<1>(tmp).value.substr(prefixSize)};
  }
};
template<>
struct Encoder<const char*>
{
  static void encode(TestDBusWrapper& w, const TestDBusWrapper::MessageIterPtr& tgt, const char* src)
  {
    Encoder<std::string>::encode(w, tgt, src);
  }
};
} // namespace detail
/// \endcond

#define DBUS_DEBUG(...)                                \
  do                                                   \
  {                                                    \
    DBus::debugPrint(__FILE__, __LINE__, __VA_ARGS__); \
  } while(0)

#define DBUS_W DBusWrapper::Installed()

/**
 * @brief Template based, single file, wrapper library around eldbus for DBUS based communication.
 *
 * Main motivation was missing asynchronous calls in AT-SPI library and difficulties,
 * when using eldbus from C++.
 *
 * The library:
 * - takes care of marshalling arguments to and from DBUS calls.
 * - allows synchronous and asynchronous calls.
 * - allows synchronous and asynchronous listeners on signals.
 * - manages all involved objects' lifetimes.
 * - errors are passed as optional-alike objects, no exceptions are used.
 * - allows setting additional debug-print function for more details about
 *   what's going on
 *
 * DBUS's method signatures (and expected return values) are specified as template argument,
 * using functor syntax. For example:
 * \code{.cpp}
 * auto dbus = DBusClient{ ... };
 * auto v = dbus.method<std::tuple<int, float>(float, float, std::string)>("foo").call(1.0f, 2.0f, "qwe");
 * \endcode
 * means (synchronous) call on dbus object, which takes three arguments (thus making call signature \b dds)
 * of types float, float and string (float will be automatically converted to double).
 * Expected return value is std::tuple<int, float>, which gives signature <B>(id)</B> - std::tuple acts
 * as struct container. Returned value v will be of type ValueOrError<std::tuple<int, float>>.\n
 * Slightly different (asynchronous) example:
 * \code{.cpp}
 * auto dbus = DBusClient{ ... };
 * std::function<void(ValueOrError<int, float>)> callback;
 * dbus.method<ValueOrError<int, float>(float, float, std::string)>("foo").asyncCall(callback, 1.0f, 2.0f, "qwe");
 * \endcode
 * Now the call takes the same arguments and has the same signature. But expected values are different -
 * now the signature is simply \b id. ValueOrError acts in this case as placeholder for list of values,
 * which DBUS allows as return data. The call itself is asynchronous - instead of expecting reply
 * you need to pass a callback, which will be called either with received data and error message.
 *
 * Library is not thread-safe, the same object shouldn't be called from different threads without
 * synchronization. There's no guarantee, that callbacks will be executed on the same thread.
 */
namespace DBus
{
/// \cond
class DBusServer;
class DBusClient;
class DBusInterfaceDescription;

/**
 * @brief Formats debug message and calls debug printer (if any) with it
 */
void debugPrint(const char* file, size_t line, const char* format, ...);

/**
 * @brief Sets debug printer callback, which will be called with debug messages
 *
 * Callback will be called in various moments of DBus activity. First value passed to callback
 * is pointer to text, second it's length. Text is ended with 0 (not counted towards it's size),
 * user can safely printf it.
 */
void setDebugPrinter(std::function<void(const char*, size_t)>);

struct Error
{
  std::string message;

  Error() = default;
  Error(std::string msg)
  : message(std::move(msg))
  {
    bart_assert(!message.empty());
  }
};

struct Success
{
};
/// \endcond

/**
 * @brief Value representing data, that came from DBUS or error message
 *
 * Object of this class either helds series of values (of types ARGS...)
 * or error message. This object will be true in boolean context, if has data
 * and false, if an error occured.
 * It's valid to create ValueOrError object with empty argument list or void:
 * \code{.cpp}
 * ValueOrError<> v1;
 * ValueOrError<void> v2;
 * \endcode
 * Both mean the same - ValueOrError containing no real data and being a marker,
 * wherever operation successed or failed and containing possible error message.
 */
template<typename... ARGS>
class ValueOrError
{
public:
  /**
   * @brief Empty constructor. Valid only, if all ARGS types are default constructible.
   */
  ValueOrError() = default;

  /**
   * @brief Value constructor.
   *
   * This will be initialized as success with passed in values.
   */
  ValueOrError(ARGS... t)
  : value(std::move(t)...)
  {
  }

  /**
   * @brief Alternative Value constructor.
   *
   * This will be initialized as success with passed in values.
   */
  ValueOrError(std::tuple<ARGS...> t)
  : value(std::move(t))
  {
  }

  /**
   * @brief Error constructor. This will be initialized as failure with given error message.
   */
  ValueOrError(Error e)
  : error(std::move(e))
  {
    bart_assert(!error.message.empty());
  }

  /**
   * @brief bool operator.
   *
   * Returns true, if operation was successful (getValues member is callable), or false
   * when operation failed (getError is callable).
   */
  explicit operator bool() const
  {
    return error.message.empty();
  }

  /**
   * @brief Returns error message object.
   *
   * Returns object containing error message associated with the failed operation.
   * Only callable, if operation actually failed, otherwise will bart_assert.
   */
  const Error& getError() const
  {
    return error;
  }

  /**
   * @brief Returns modifiable tuple of held data.
   *
   * Returns reference to the internal tuple containing held data.
   * User can modify (or move) data safely.
   * Only callable, if operation actually successed, otherwise will bart_assert.
   */
  std::tuple<ARGS...>& getValues()
  {
    bart_assert(*this);
    return value;
  }

  /**
   * @brief Returns const tuple of held data.
   *
   * Returns const reference to the internal tuple containing held data.
   * Only callable, if operation actually successed, otherwise will bart_assert.
   */
  const std::tuple<ARGS...>& getValues() const
  {
    bart_assert(*this);
    return value;
  }

protected:
  /// \cond
  std::tuple<ARGS...> value;
  Error               error;
  /// \endcond
};

/// \cond
template<>
class ValueOrError<>
{
public:
  ValueOrError() = default;
  ValueOrError(std::tuple<> t)
  {
  }
  ValueOrError(Error e)
  : error(std::move(e))
  {
    bart_assert(!error.message.empty());
  }

  explicit operator bool() const
  {
    return error.message.empty();
  }
  const Error& getError() const
  {
    return error;
  }
  std::tuple<>& getValues()
  {
    bart_assert(*this);
    static std::tuple<> t;
    return t;
  }
  std::tuple<> getValues() const
  {
    bart_assert(*this);
    return {};
  }

protected:
  Error error;
};

template<>
class ValueOrError<void>
{
public:
  ValueOrError() = default;
  ValueOrError(Success)
  {
  }
  ValueOrError(Error e)
  : error(std::move(e))
  {
    bart_assert(!error.message.empty());
  }

  explicit operator bool() const
  {
    return error.message.empty();
  }
  const Error& getError() const
  {
    return error;
  }
  std::tuple<>& getValues()
  {
    bart_assert(*this);
    static std::tuple<> t;
    return t;
  }
  std::tuple<> getValues() const
  {
    bart_assert(*this);
    return {};
  }

protected:
  Error error;
};

using ObjectPath = ObjectPath;

/// \endcond

/**
 * @brief Class used to marshall DBUS's variant type
 *
 * Minimalistic class, that allows user to specify DBUS variant type
 * as argument or return value. You need to pass real type hidden under variant as
 * template type \b A. At this point library doesn't allow to expected one of few classes
 * as return data in variant. So for example user can't specify method call, which on return
 * expects DBUS variant holding either string or int.
 */
template<typename A>
struct EldbusVariant
{
  A value;
};

/**
 * @brief Namespace for private, internal functions and classes
 */
namespace detail
{
/// \cond
template<typename T, typename = void>
struct signature;
template<typename... ARGS>
struct signature<std::tuple<ARGS...>>;
template<typename A, typename B>
struct signature<std::pair<A, B>>;
template<typename A>
struct signature<std::vector<A>>;
template<typename A, size_t N>
struct signature<std::array<A, N>>;
template<typename A, typename B>
struct signature<std::unordered_map<A, B>>;
template<typename A, typename B>
struct signature<std::map<A, B>>;
/// \endcond

/**
 * @brief Signature class for marshalling uint8 type.
 */
template<>
struct signature<uint8_t>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "uint8_t";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "y";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, uint8_t v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, uint8_t& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

/**
 * @brief Signature class for marshalling uint16 type.
 */
template<>
struct signature<uint16_t>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "uint16_t";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "q";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, uint16_t v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, uint16_t& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

/**
 * @brief Signature class for marshalling uint32 type.
 */
template<>
struct signature<uint32_t>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "uint32_t";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "u";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, uint32_t v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, uint32_t& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

/**
 * @brief Signature class for marshalling uint64 type.
 */
template<>
struct signature<uint64_t>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "uint64_t";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "t";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, uint64_t v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, uint64_t& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

/**
 * @brief Signature class for marshalling int16 type.
 */
template<>
struct signature<int16_t>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "int16_t";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "n";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, int16_t v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, int16_t& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

/**
 * @brief Signature class for marshalling int32 type.
 */
template<>
struct signature<int32_t>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "int32_t";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "i";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, int32_t v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, int32_t& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

/**
 * @brief Signature class for marshalling int64 type.
 */
template<>
struct signature<int64_t>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "int64_t";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "x";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, int64_t v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, int64_t& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

/**
 * @brief Signature class for marshalling double type.
 */
template<>
struct signature<double>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "double";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "d";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, double v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, double& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, float& v2)
  {
    double v = 0;
    auto   r = DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
    v2       = static_cast<float>(v);
    return r;
  }
};

/**
 * @brief Signature class for marshalling float type.
 */
template<>
struct signature<float>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "float";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "d";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, float v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, double& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, float& v2)
  {
    double v = 0;
    auto   r = DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
    v2       = static_cast<float>(v);
    return r;
  }
};

/**
 * @brief Signature class for marshalling boolean type.
 */
template<>
struct signature<bool>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "bool";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "b";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, bool v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, bool& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

template<typename T>
struct signature<T, typename std::enable_if<std::is_enum<T>::value, void>::type>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "enum";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return signature<typename std::underlying_type<T>::type>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, T v)
  {
    signature<typename std::underlying_type<T>::type>::set(iter, static_cast<int64_t>(v));
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, T& v)
  {
    typename std::underlying_type<T>::type q = 0;
    if(!signature<typename std::underlying_type<T>::type>::get(iter, q))
      return false;

    v = static_cast<T>(q);
    return true;
  }
};

/**
 * @brief Signature class for marshalling string type.
 *
 * Both (const) char * and std::string types are accepted as value to send.
 * Only std::string is accepted as value to receive.
 */
template<>
struct signature<std::string>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "string";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "s";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::string& v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  // /**
  //  * @brief Marshals value v as marshalled type into message
  //  */
  //  static void set( const DBusWrapper::MessageIterPtr &iter, const char* v )
  //  {
  //    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  //  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::string& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }
};

template<>
struct signature<ObjectPath>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "path";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "o";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::string& v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, ObjectPath{v});
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const ObjectPath& v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const char* v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, ObjectPath{v});
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, ObjectPath& v)
  {
    return DBUS_W->eldbus_message_iter_get_and_next_impl(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::string& v)
  {
    ObjectPath q;
    if(!DBUS_W->eldbus_message_iter_get_and_next_impl(iter, q)) return false;
    v = std::move(q.value);
    return true;
  }
};

/**
 * @brief Signature class for marshalling (const) char * type.
 *
 * Both (const) char * and std::string types are accepted as value to send.
 * You can't use (const) char * variable type to receive value.
 */
template<>
struct signature<char*>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "string";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "s";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::string& v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, v);
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const char* v)
  {
    DBUS_W->eldbus_message_iter_arguments_append_impl(iter, std::string{v});
  }
};
/// \cond

template<size_t INDEX, typename A, typename... ARGS>
struct signature_tuple_element_type_helper
{
  using type = typename signature_tuple_element_type_helper<INDEX - 1, ARGS...>::type;
};

template<typename A, typename... ARGS>
struct signature_tuple_element_type_helper<0, A, ARGS...>
{
  using type = A;
};
/// \endcond

/**
 * @brief Helper class to marshall tuples
 *
 * This class marshals all elements of the tuple value starting at the index INDEX
 * and incrementing. This class recursively calls itself with increasing INDEX value
 * until INDEX is equal to SIZE, where recursive calling ends.
 */
template<size_t INDEX, size_t SIZE, typename... ARGS>
struct signature_tuple_helper
{
  using current_type = typename signature_tuple_element_type_helper<INDEX, ARGS...>::type;

  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    if(INDEX + 1 >= SIZE)
      return signature<current_type>::name();
    return signature<current_type>::name() + ", " + signature_tuple_helper<INDEX + 1, SIZE, ARGS...>::name();
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return signature<current_type>::sig() + signature_tuple_helper<INDEX + 1, SIZE, ARGS...>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::tuple<ARGS...>& args)
  {
    signature<current_type>::set(iter, std::get<INDEX>(args));
    signature_tuple_helper<INDEX + 1, SIZE, ARGS...>::set(iter, args);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::tuple<ARGS...>& args)
  {
    return signature<current_type>::get(iter, std::get<INDEX>(args)) &&
           signature_tuple_helper<INDEX + 1, SIZE, ARGS...>::get(iter, args);
  }
};

/**
 * @brief Helper class to marshall tuples
 *
 * This class marks end of the tuple marshalling. Members of this class are called
 * when INDEX value is equal to SIZE.
 */
template<size_t SIZE, typename... ARGS>
struct signature_tuple_helper<SIZE, SIZE, ARGS...>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::tuple<ARGS...>& args)
  {
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::tuple<ARGS...>& args)
  {
    return true;
  }
};

/**
 * @brief Signature class for marshalling tuple of values
 *
 * This class marshalls tuple of values. This represents
 * DBUS struct typle, encoded with character 'r'
 */
template<typename... ARGS>
struct signature<std::tuple<ARGS...>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "tuple<" + signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::name() + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "(" + signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::sig() + ")";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::tuple<ARGS...>& args)
  {
    auto entry = DBUS_W->eldbus_message_iter_container_new_impl(iter, 'r', "");
    signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::set(entry, args);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::tuple<ARGS...>& args)
  {
    auto entry = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, 'r');
    if(!entry) return false;
    return signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::get(entry, args);
  }
};

/**
 * @brief Signature class for marshalling ValueOrError template type
 *
 * ValueOrError template type is used to marshall list of values passed to
 * DBUS (or received from) at the "top" level. For example ss(s) is represented as
 * \code{.cpp} ValueOrError<std::string, std::string, std::tuple<std::string>> \endcode
 * While (ss(s)) is represented as
 * \code{.cpp} std::tuple<std::string, std::string, std::tuple<std::string>> \endcode
 * or
 * \code{.cpp} ValueOrError<std::tuple<std::string, std::string, std::tuple<std::string>>> \endcode
 */
template<typename... ARGS>
struct signature<ValueOrError<ARGS...>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "ValueOrError<" + signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::name() + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const ValueOrError<ARGS...>& args)
  {
    signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::set(iter, args.getValues());
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, ValueOrError<ARGS...>& args)
  {
    return signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::get(iter, args.getValues());
  }
};

/**
 * @brief Signature class for marshalling ValueOrError<void> type
 */
template<>
struct signature<ValueOrError<void>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "ValueOrError<void>";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const ValueOrError<void>& args)
  {
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, ValueOrError<void>& args)
  {
    return true;
  }
};

/**
 * @brief Signature class for marshalling ValueOrError<> type
 */
template<>
struct signature<ValueOrError<>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "ValueOrError<>";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const ValueOrError<>& args)
  {
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, ValueOrError<>& args)
  {
    return true;
  }
};

/**
 * @brief Signature class for marshalling pair of types
 */
template<typename A, typename B>
struct signature<std::pair<A, B>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "pair<" + signature_tuple_helper<0, 2, A, B>::name() + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "(" + signature_tuple_helper<0, 2, A, B>::sig() + ")";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::pair<A, B>& ab, bool dictionary = false)
  {
    auto entry = DBUS_W->eldbus_message_iter_container_new_impl(iter, dictionary ? 'e' : 'r', "");
    signature_tuple_helper<0, 2, A, B>::set(entry, ab);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::pair<A, B>& ab)
  {
    auto entry = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, 'r');
    if(!entry)
    {
      entry = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, '{');
      if(!entry) return false;
    }

    std::tuple<A, B> ab_tmp;
    auto             z = signature_tuple_helper<0, 2, A, B>::get(entry, ab_tmp);
    if(z)
    {
      ab.first  = std::move(std::get<0>(ab_tmp));
      ab.second = std::move(std::get<1>(ab_tmp));
    }
    return z;
  }
};

/**
 * @brief Signature class for marshalling std::vector template type
 *
 * This marshals container's content as DBUS a ascii character type code.
 */
template<typename A>
struct signature<std::vector<A>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "vector<" + signature<A>::name() + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "a" + signature<A>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::vector<A>& v)
  {
    auto lst = DBUS_W->eldbus_message_iter_container_new_impl(iter, 'a', signature<A>::sig());
    bart_assert(lst);
    for(auto& a : v)
    {
      signature<A>::set(lst, a);
    }
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::vector<A>& v)
  {
    auto s = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, 'a');
    if(!s) return false;
    v.clear();
    A a;
    while(signature<A>::get(s, a))
      v.push_back(std::move(a));

    return true;
  }
};

/**
 * @brief Signature class for marshalling std::array template type
 *
 * This marshals container's content as DBUS a ascii character type code.
 */
template<typename A, size_t N>
struct signature<std::array<A, N>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "array<" + signature<A>::name() + ", " + std::to_string(N) + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "a" + signature<A>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::array<A, N>& v)
  {
    auto lst = DBUS_W->eldbus_message_iter_container_new_impl(iter, 'a', signature<A>::sig());
    bart_assert(lst);
    for(auto& a : v)
    {
      signature<A>::set(lst, a);
    }
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::array<A, N>& v)
  {
    auto s = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, 'a');
    if(!s)
      return false;
    for(auto& a : v)
    {
      if(!signature<A>::get(s, a))
        return false;
    }
    return true;
  }
};

/**
 * @brief Signature class for marshalling EldbusVariant type
 *
 * This marshals variant's content as DBUS v ascii character type code.
 */
template<typename A>
struct signature<EldbusVariant<A>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "variant<" + signature<A>::name() + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "v";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const EldbusVariant<A>& v)
  {
    set(iter, v.value);
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const A& v)
  {
    auto var = DBUS_W->eldbus_message_iter_container_new_impl(iter, 'v', signature<A>::sig());
    signature<A>::set(var, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, EldbusVariant<A>& v)
  {
    auto s = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, 'v');
    if(!s)
      return false;
    return signature<A>::get(s, v.value);
  }
};

/**
 * @brief Signature class for marshalling std::unordered_map template type
 *
 * This marshals container's content as DBUS {} ascii character type code.
 * Note, that library doesnt check, if the key is basic type, as DBUS
 * specification mandates.
 * User can always exchange std::unordered_map for std::map and the reverse.
 * User can receive such values as std::vector of std::pair<key, value> values.
 * Order of such values is unspecified.
 */
template<typename A, typename B>
struct signature<std::unordered_map<A, B>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "unordered_map<" + signature<A>::name() + ", " + signature<B>::name() + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "a{" + signature_tuple_helper<0, 2, A, B>::sig() + "}";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::unordered_map<A, B>& v)
  {
    auto sig = "{" + signature_tuple_helper<0, 2, A, B>::sig() + "}";
    auto lst = DBUS_W->eldbus_message_iter_container_new_impl(iter, 'a', sig);
    bart_assert(lst);
    for(auto& a : v)
    {
      signature<std::pair<A, B>>::set(lst, a, true);
    }
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::unordered_map<A, B>& v)
  {
    auto s = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, 'a');
    v.clear();
    if(!s)
      return false;
    std::pair<A, B> a;
    while(signature<std::pair<A, B>>::get(s, a))
      v.insert(std::move(a));
    return true;
  }
};

/**
 * @brief Signature class for marshalling std::unordered_map template type
 *
 * This marshals container's content as DBUS {} ascii character type code.
 * Note, that library doesnt check, if the key is basic type, as DBUS
 * specification mandates.
 * User can always exchange std::unordered_map for std::map and the reverse.
 * User can receive such values as std::vector of std::pair<key, value> values.
 * Order of such values is unspecified.
 */
template<typename A, typename B>
struct signature<std::map<A, B>>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "map<" + signature<A>::name() + ", " + signature<B>::name() + ">";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return "a{" + signature_tuple_helper<0, 2, A, B>::sig() + "}";
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const std::map<A, B>& v)
  {
    auto sig = "{" + signature_tuple_helper<0, 2, A, B>::sig() + "}";
    auto lst = DBUS_W->eldbus_message_iter_container_new_impl(iter, 'a', sig);
    bart_assert(lst);
    for(auto& a : v)
    {
      signature<std::pair<A, B>>::set(lst, a, true);
    }
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static bool get(const DBusWrapper::MessageIterPtr& iter, std::map<A, B>& v)
  {
    auto s = DBUS_W->eldbus_message_iter_get_and_next_by_type_impl(iter, 'a');
    if(!s)
      return false;
    std::pair<A, B> a;
    while(signature<std::pair<A, B>>::get(s, a))
      v.insert(std::move(a));
    return true;
  }
};

/**
 * @brief Signature helper class for marshalling const reference types
 */
template<typename A>
struct signature<const A&>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "const " + signature<A>::name() + "&";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return signature<A>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const A& v)
  {
    signature<A>::set(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static void get(const DBusWrapper::MessageIterPtr& iter, A& v)
  {
    signature<A>::get(iter, v);
  }
};

/**
 * @brief Signature helper class for marshalling reference types
 */
template<typename A>
struct signature<A&>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return signature<A>::name() + "&";
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return signature<A>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const A& v)
  {
    signature<A>::set(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static void get(const DBusWrapper::MessageIterPtr& iter, A& v)
  {
    signature<A>::get(iter, v);
  }
};

/**
 * @brief Signature helper class for marshalling const types
 */
template<typename A>
struct signature<const A>
{
  /**
   * @brief Returns name of type marshalled, for informative purposes
   */
  static std::string name()
  {
    return "const " + signature<A>::name();
  }

  /**
   * @brief Returns DBUS' signature of type marshalled
   */
  static std::string sig()
  {
    return signature<A>::sig();
  }

  /**
   * @brief Marshals value v as marshalled type into message
   */
  static void set(const DBusWrapper::MessageIterPtr& iter, const A& v)
  {
    signature<A>::set(iter, v);
  }

  /**
   * @brief Marshals value from marshalled type into variable v
   */
  static void get(const DBusWrapper::MessageIterPtr& iter, A& v)
  {
    signature<A>::get(iter, v);
  }
};

/// \cond
using CallId = DBusWrapper::CallId;

template<typename ValueType>
ValueType unpackValues(CallId callId, const DBusWrapper::MessagePtr& msg)
{
  auto      iter = DBUS_W->eldbus_message_iter_get_impl(msg, false);
  ValueType r;

  if(iter)
  {
    if(!signature<ValueType>::get(iter, r))
    {
      DBUS_DEBUG("ValueType is %s", signature<ValueType>::name().c_str());
      r = Error{"call " + std::to_string(callId.id) + ": failed to unpack values, got signature '" +
                DBUS_W->eldbus_message_signature_get_impl(msg) + "', expected '" + signature<ValueType>::sig() + "'"};
    }
  }
  else
  {
    r = Error{"call " + std::to_string(callId.id) + ": failed to get iterator"};
  }
  return r;
}

inline void packValues_helper(const DBusWrapper::MessageIterPtr&)
{
}

template<typename A, typename... ARGS>
void packValues_helper(const DBusWrapper::MessageIterPtr& iter, A&& a, ARGS&&... r)
{
  signature<A>::set(iter, std::forward<A>(a));
  packValues_helper(iter, std::forward<ARGS>(r)...);
}

template<typename... ARGS>
void packValues(CallId callId, const DBusWrapper::MessagePtr& msg, ARGS&&... r)
{
  auto iter = DBUS_W->eldbus_message_iter_get_impl(msg, true);
  packValues_helper(iter, std::forward<ARGS>(r)...);
}

template<typename>
struct ReturnType;
template<typename R, typename... ARGS>
struct ReturnType<R(ARGS...)>
{
  using type = R;
};

template<typename R, typename... ARGS>
struct ReturnType<std::function<R(ARGS...)>>
{
  using type = R;
};

template<int...>
struct sequence
{
};

template<int N, int... S>
struct sequence_gen : sequence_gen<N - 1, N - 1, S...>
{
};

template<int... S>
struct sequence_gen<0, S...>
{
  typedef sequence<S...> type;
};

template<typename C, typename... ARGS>
struct apply_helper
{
  const std::function<C>&    c;
  const std::tuple<ARGS...>& args;

  template<int... S>
  auto apply_2(sequence<S...>) const -> decltype(c(std::get<S>(args)...))
  {
    return c(std::get<S>(args)...);
  }
  auto apply_1() const -> decltype(apply_2(typename sequence_gen<sizeof...(ARGS)>::type()))
  {
    return apply_2(typename sequence_gen<sizeof...(ARGS)>::type());
  }
};

template<typename C, typename A, typename... ARGS>
struct apply_helper_2
{
  const std::function<C>&    c;
  const A&                   a;
  const std::tuple<ARGS...>& args;

  template<int... S>
  auto apply_2(sequence<S...>) const -> decltype(c(a, std::get<S>(args)...))
  {
    return c(a, std::get<S>(args)...);
  }
  auto apply_1() const -> decltype(apply_2(typename sequence_gen<sizeof...(ARGS)>::type()))
  {
    return apply_2(typename sequence_gen<sizeof...(ARGS)>::type());
  }
};

template<typename C, typename... ARGS>
auto apply(const std::function<C>& c, const std::tuple<ARGS...>& args) -> typename ReturnType<C>::type
{
  apply_helper<C, ARGS...> ah{c, args};
  return ah.apply_1();
}

template<typename C, typename D, typename... ARGS>
auto apply(const std::function<C>& c, const D& d, const std::tuple<ARGS...>& args) -> typename ReturnType<C>::type
{
  apply_helper_2<C, D, ARGS...> ah{c, d, args};
  return ah.apply_1();
}

struct ConnectionState
{
  DBusWrapper::ConnectionPtr connection;
  DBusWrapper::ObjectPtr     object;
  DBusWrapper::ProxyPtr      proxy, propertiesProxy;
};

template<typename RETTYPE, typename... ARGS>
RETTYPE call(CallId callId, const ConnectionState& connectionState, bool property, const std::string& funcName, const ARGS&... args)
{
  const auto& proxy = property ? connectionState.propertiesProxy : connectionState.proxy;
  if(!proxy)
  {
    DBUS_DEBUG("call %d: not initialized", callId.id);
    return Error{"not initialized"};
  }

  DBUS_DEBUG("call %d: calling '%s'", callId.id, funcName.c_str());
  auto msg = DBUS_W->eldbus_proxy_method_call_new_impl(proxy, funcName);
  if(!msg)
  {
    DBUS_DEBUG("call %d: failed", callId.id);
    return Error{"failed to create message"};
  }

  detail::packValues(callId, msg, args...);
  auto reply = DBUS_W->eldbus_proxy_send_and_block_impl(proxy, msg);
  DBUS_DEBUG("call %d: calling '%s' done", callId.id, funcName.c_str());
  if(!reply)
  {
    DBUS_DEBUG("call %d: failed", callId.id);
    return Error{"eldbus returned null as reply"};
  }
  std::string errname, errmsg;
  if(DBUS_W->eldbus_message_error_get_impl(reply, errname, errmsg))
  {
    DBUS_DEBUG("call %d: %s: %s", callId.id, errname.c_str(), errmsg.c_str());
    return Error{errname + ": " + errmsg};
  }
  DBUS_DEBUG("call %d: got reply with signature '%s'", callId.id, DBUS_W->eldbus_message_signature_get_impl(reply).c_str());
  return detail::unpackValues<RETTYPE>(callId, reply);
}

template<typename RETTYPE, typename... ARGS>
void asyncCall(CallId callId, const ConnectionState& connectionState, bool property, const std::string& funcName, std::function<void(RETTYPE)> callback, const ARGS&... args)
{
  const auto& proxy = property ? connectionState.propertiesProxy : connectionState.proxy;
  if(!proxy)
  {
    DBUS_DEBUG("call %d: not initialized", callId.id);
    callback(Error{"not initialized"});
    return;
  }

  auto msg = DBUS_W->eldbus_proxy_method_call_new_impl(proxy, funcName);
  if(!msg)
  {
    DBUS_DEBUG("call %d: failed", callId.id);
    callback(Error{"failed to create message"});
    return;
  }

  detail::packValues(callId, msg, args...);
  auto pending = DBUS_W->eldbus_proxy_send_impl(proxy, msg, [callback, callId, proxy](const DBusWrapper::MessagePtr& reply) {
    DBUS_DEBUG("call %d: calling done", callId.id);
    if(!reply)
    {
      DBUS_DEBUG("call %d: failed", callId.id);
      callback(Error{"eldbus returned null as reply"});
    }
    else
    {
      std::string errname, errmsg;
      if(DBUS_W->eldbus_message_error_get_impl(reply, errname, errmsg))
      {
        DBUS_DEBUG("call %d: %s: %s", callId.id, errname.c_str(), errmsg.c_str());
        callback(Error{errname + ": " + errmsg});
      }
      else
      {
        DBUS_DEBUG("call %d: got reply with signature '%s'", callId.id, DBUS_W->eldbus_message_signature_get_impl(reply).c_str());
        callback(detail::unpackValues<RETTYPE>(callId, reply));
      }
    }
  });
  if(pending)
  {
    DBUS_DEBUG("call %d: call sent", callId.id);
  }
  else
  {
    DBUS_DEBUG("call %d: failed to send call", callId.id);
    callback(Error{"failed to send call"});
  }
}

inline void displayDebugCallInfo(CallId callId, const std::string& funcName, const std::string& info, const std::string& interfaceName)
{
  DBUS_DEBUG("call %d: %s iname = %s fname = %s", callId.id, info.c_str(), interfaceName.c_str(), funcName.c_str());
}

inline void displayDebugCallInfoSignal(CallId callId, const std::string& funcName, const std::string& info, const std::string& interfaceName)
{
  DBUS_DEBUG("call %d: %s signal iname = %s fname = %s", callId.id, info.c_str(), interfaceName.c_str(), funcName.c_str());
}

inline void displayDebugCallInfoProperty(CallId callId, const std::string& funcName, std::string info, const std::string& interfaceName, const std::string& propertyName)
{
  DBUS_DEBUG("call %d: %s iname = %s pname = %s", callId.id, info.c_str(), interfaceName.c_str(), propertyName.c_str());
}

using StringStorage = DBusWrapper::StringStorage;

template<typename A, typename... ARGS>
struct EldbusArgGenerator_Helper
{
  static void add(std::vector<std::pair<std::string, std::string>>& r)
  {
    auto s   = r.size();
    auto sig = signature<A>::sig();
    bart_assert(!sig.empty());
    auto name = "p" + std::to_string(s + 1);
    r.push_back({std::move(sig), std::move(name)});
    EldbusArgGenerator_Helper<ARGS...>::add(r);
  }
};

template<>
struct EldbusArgGenerator_Helper<void>
{
  static void add(std::vector<std::pair<std::string, std::string>>&)
  {
  }
};

template<>
struct EldbusArgGenerator_Helper<ValueOrError<void>, void>
{
  static void add(std::vector<std::pair<std::string, std::string>>&)
  {
  }
};
template<>
struct EldbusArgGenerator_Helper<ValueOrError<>, void>
{
  static void add(std::vector<std::pair<std::string, std::string>>&)
  {
  }
};

template<typename... ARGS>
struct EldbusArgGenerator_Helper<std::tuple<ARGS...>>
{
  static void add(std::vector<std::pair<std::string, std::string>>& r)
  {
    EldbusArgGenerator_Helper<ARGS..., void>::add(r);
  }
};

template<typename RetType>
struct dbus_interface_return_type_traits
{
  using type = ValueOrError<RetType>;
};

template<typename... ARGS>
struct dbus_interface_return_type_traits<ValueOrError<ARGS...>>
{
  using type = ValueOrError<ARGS...>;
};

template<typename T>
struct dbus_interface_traits;
template<typename RetType, typename... ARGS>
struct dbus_interface_traits<RetType(ARGS...)>
{
  using Ret     = typename dbus_interface_return_type_traits<RetType>::type;
  using SyncCB  = std::function<Ret(ARGS...)>;
  using AsyncCB = std::function<void(std::function<void(Ret)>, ARGS...)>;
  using VEArgs  = ValueOrError<ARGS...>;
};

template<typename T>
struct EldbusArgGenerator_Args;
template<typename RetType, typename... ARGS>
struct EldbusArgGenerator_Args<RetType(ARGS...)>
{
  static std::string name()
  {
    return signature_tuple_helper<0, sizeof...(ARGS), ARGS...>::name();
  }
  static std::vector<std::pair<std::string, std::string>> get()
  {
    std::vector<std::pair<std::string, std::string>> tmp;
    EldbusArgGenerator_Helper<ARGS..., void>::add(tmp);
    return tmp;
  }
};

template<typename T>
struct EldbusArgGenerator_ReturnType;
template<typename RetType, typename... ARGS>
struct EldbusArgGenerator_ReturnType<RetType(ARGS...)>
{
  static std::string name()
  {
    return signature<RetType>::name();
  }
  static std::vector<std::pair<std::string, std::string>> get()
  {
    std::vector<std::pair<std::string, std::string>> tmp;
    EldbusArgGenerator_Helper<RetType, void>::add(tmp);
    return tmp;
  }
};

template<typename T>
struct EldbusArgGenerator_ReturnType;
template<typename... ARGS>
struct EldbusArgGenerator_ReturnType<void(ARGS...)>
{
  static std::string name()
  {
    return "";
  }
  static std::vector<std::pair<std::string, std::string>> get()
  {
    return {};
  }
};
/// \endcond
} // namespace detail

using ConnectionType = DBusWrapper::ConnectionType;

/**
 * @brief Class representing client's end of DBUS connection
 *
 * Allows calling (synchronous and asynchronos) methods on selected interface
 * Allows (synchronous and asynchronos) setting / getting properties.
 * Allows registering signals.
 */
class DBusClient
{
  /// \cond
  struct ConnectionInfo
  {
    std::string interfaceName, busName, pathName;
  };
  /// \endcond
public:
  /**
   * @brief Default constructor, creates non-connected object.
   */
  DBusClient() = default;

  /**
   * @brief Connects to dbus choosen by tp, using given arguments
   *
   * @param bus_name name of the bus to connect to
   * @param path_name object's path
   * @param interface_name interface name
   */
  DBusClient(std::string busName_, std::string pathName_, std::string interfaceName_, ConnectionType tp);

  /**
   * @brief Connects to dbus using connection conn
   *
   * @param bus_name name of the bus to connect to
   * @param path_name object's path
   * @param interface_name interface name
   * @param conn connection object from getDBusConnectionByType call
   */
  DBusClient(std::string busName_, std::string pathName_, std::string interfaceName_, const DBusWrapper::ConnectionPtr& conn = {});

  /**
   * @brief Destructor object.
   *
   * All signals added will be disconnected.
   * All asynchronous calls will be cancelled, their callback's will be called
   * with failure message.
   */
  ~DBusClient()                 = default;
  DBusClient(const DBusClient&) = delete;
  DBusClient(DBusClient&&)      = default;

  DBusClient& operator=(DBusClient&&) = default;
  DBusClient& operator=(const DBusClient&) = delete;

  /**
   * @brief bool operator
   *
   * Returns true, if object is connected to DBUS
   */
  explicit operator bool() const
  {
    return bool(connectionState->proxy);
  }

  /**
   * @brief Helper class for calling a method
   *
   * Template type T defines both arguments sent to the method
   * and expected values. Receiving different values will be reported as
   * error. For example:
   * \code{.cpp} Method<int(float, float)> \endcode
   * defines method, which takes two arguments (two floats) and return
   * single value of type int.
   */
  template<typename T>
  struct Method
  {
    /// \cond
    using RetType = typename detail::dbus_interface_traits<T>::Ret;
    const detail::ConnectionState&  connectionState;
    std::string                     funcName;
    std::string                     info;
    std::shared_ptr<ConnectionInfo> connectionInfo;
    /// \endcond

    /**
     * @brief Executes synchronous call on DBUS's method
     *
     * The function returns ValueOrError<...> object, which
     * contains either received values or error message.
     *
     * @param args arguments to pass to the method
     */
    template<typename... ARGS>
    RetType call(const ARGS&... args)
    {
      detail::CallId callId;
      detail::displayDebugCallInfo(callId, funcName, info, connectionInfo->interfaceName);
      return detail::call<RetType>(callId, connectionState, false, funcName, args...);
    }

    /**
     * @brief Executes asynchronous call on DBUS's method
     *
     * The function calls callback with either received values or error message.
     *
     * @param callback callback functor, which will be called with return value(s) or error message
     * @param args arguments to pass to the method
     */
    template<typename... ARGS>
    void asyncCall(std::function<void(RetType)> callback, const ARGS&... args)
    {
      detail::CallId callId;
      detail::displayDebugCallInfo(callId, funcName, info, connectionInfo->interfaceName);
      detail::asyncCall<RetType>(callId, connectionState, false, funcName, std::move(callback), args...);
    }
  };

  /**
   * @brief Helper class for calling a property
   *
   * Template type T defines type of the value hidden under property.
   * Note, that library automatically wraps both sent and received value into
   * DBUS's wrapper type.
   */
  template<typename T>
  struct Property
  {
    /// \cond
    using RetType        = typename detail::dbus_interface_return_type_traits<T>::type;
    using VariantRetType = typename detail::dbus_interface_return_type_traits<EldbusVariant<T>>::type;
    const detail::ConnectionState&  connectionState;
    std::string                     propName;
    std::string                     info;
    std::shared_ptr<ConnectionInfo> connectionInfo;
    /// \endcond

    /**
     * @brief executes synchronous get on property
     *
     * The function returns ValueOrError<...> object, which
     * contains either received values or error message.
     */
    RetType get()
    {
      detail::CallId callId;
      detail::displayDebugCallInfoProperty(callId, "Get", info, connectionInfo->interfaceName, propName);
      auto z = detail::call<VariantRetType>(callId, connectionState, true, "Get", connectionInfo->interfaceName, propName);
      if(!z)
        return z.getError();
      return {std::get<0>(z.getValues()).value};
    }

    /**
     * @brief executes asynchronous get on property
     *
     * The function calls callback with either received values or error message.
     *
     * @param callback callback functor, which will be called with return value(s) or error message
     */
    void asyncGet(std::function<void(RetType)> callback)
    {
      detail::CallId callId;
      detail::displayDebugCallInfoProperty(callId, "Get", info, connectionInfo->interfaceName, propName);
      auto cc = [callback](VariantRetType reply) {
        if(reply)
          callback(std::move(std::get<0>(reply.getValues()).value));
        else
          callback(reply.getError());
      };
      detail::asyncCall<VariantRetType>(callId, connectionState, true, "Get", std::move(cc), connectionInfo->interfaceName, propName);
    }

    /**
     * @brief executes synchronous set on property
     *
     * The function returns ValueOrError<void> object, with
     * possible error message.
     */
    ValueOrError<void> set(const T& r)
    {
      detail::CallId callId;
      detail::displayDebugCallInfoProperty(callId, "Set", info, connectionInfo->interfaceName, propName);
      EldbusVariant<T> variantValue{std::move(r)};
      return detail::call<ValueOrError<void>>(callId, connectionState, true, "Set", connectionInfo->interfaceName, propName, variantValue);
    }

    /**
     * @brief executes asynchronous get on property
     *
     * The function calls callback with either received values or error message.
     *
     * @param callback callback functor, which will be called with return value(s) or error message
     */
    void asyncSet(std::function<void(ValueOrError<void>)> callback, const T& r)
    {
      detail::CallId callId;
      detail::displayDebugCallInfoProperty(callId, "Set", info, connectionInfo->interfaceName, propName);
      EldbusVariant<T> variantValue{std::move(r)};
      detail::asyncCall<ValueOrError<void>>(callId, connectionState, true, "Set", std::move(callback), connectionInfo->interfaceName, propName, variantValue);
    }
  };

  /**
   * @brief Constructs Property<...> object for calling properties
   *
   * The function constructs and returns proxy object for calling property.
   *
   * @param propName property name to set and / or query
   */
  template<typename PropertyType>
  Property<PropertyType> property(std::string propName)
  {
    return Property<PropertyType>{*connectionState, std::move(propName), info, connectionInfo};
  }

  /**
   * @brief Constructs Method<...> object for calling methods
   *
   * The function constructs and returns proxy object for calling method.
   *
   * @param funcName function name to call
   */
  template<typename MethodType>
  Method<MethodType> method(std::string funcName)
  {
    return Method<MethodType>{*connectionState, std::move(funcName), info, connectionInfo};
  }

  /**
   * @brief Registers notification callback, when property has changed
   *
   * The callback will be called with new value, when property's value has changed.
   * Note, that template type V must match expected type, otherwise undefined behavior will occur,
   * there's no check for this.
   */
  template<typename V>
  void addPropertyChangedEvent(std::string propertyName, std::function<void(V)> callback)
  {
    detail::CallId callId;
    detail::displayDebugCallInfoSignal(callId, propertyName, info, connectionInfo->interfaceName);
    DBUS_DEBUG("call %d: adding property", callId.id);
    auto& cI = this->connectionInfo;
    DBUS_W->add_property_changed_event_listener_impl(connectionState->proxy, cI->interfaceName, propertyName, [callback](const _Eina_Value* newValue) {
      V val = 0;
      if(!getFromEinaValue(newValue, &val))
      {
        DBUS_DEBUG("unable to get property's value");
        return;
      }
      callback(val);
    });
  }

  /**
   * @brief Registers callback on the DBUS' signal
   *
   * The function registers callback signalName. When signal comes, callback will be called.
   * Callback object will exists as long as signal is registered. You can unregister signal
   * by destroying DBusClient object.
   *
   * @param signalName name of the signal to register
   * @param callback callback to call
   */
  template<typename SignalType>
  void addSignal(std::string signalName, std::function<SignalType> callback)
  {
    detail::CallId callId;
    detail::displayDebugCallInfoSignal(callId, signalName, info, connectionInfo->interfaceName);
    DBUS_W->eldbus_proxy_signal_handler_add_impl(connectionState->proxy, signalName, [callId, callback, signalName](const DBusWrapper::MessagePtr& msg) -> void {
      std::string errname, aux;
      if(DBUS_W->eldbus_message_error_get_impl(msg, errname, aux))
      {
        DBUS_DEBUG("call %d: Eldbus error: %s %s", callId.id, errname.c_str(), aux.c_str());
        return;
      }
      DBUS_DEBUG("call %d: received signal with signature '%s'", callId.id, DBUS_W->eldbus_message_signature_get_impl(msg).c_str());
      using ParamsType = typename detail::dbus_interface_traits<SignalType>::VEArgs;
      auto params      = detail::unpackValues<ParamsType>(callId, msg);
      if(!params)
      {
        DBUS_DEBUG("call %d: failed: %s", callId.id, params.getError().message.c_str());
        return;
      }
      try
      {
        detail::apply(callback, params.getValues());
      }
      catch(...)
      {
        DBUS_DEBUG("unhandled exception");
        bart_assert(0);
      }
    });
  }

private:
  /// \cond
  std::unique_ptr<detail::ConnectionState> connectionState{new detail::ConnectionState};
  std::string                              info;
  std::shared_ptr<ConnectionInfo>          connectionInfo;

  static bool getFromEinaValue(const _Eina_Value* v, void* dst);
  /// \endcond
};

/**
 * @brief Helper class describing DBUS's server interface
 *
 */
class DBusInterfaceDescription
{
  friend class DBusServer;

public:
  /// \cond
  using MethodInfo   = DBusWrapper::MethodInfo;
  using SignalInfo   = DBusWrapper::SignalInfo;
  using PropertyInfo = DBusWrapper::PropertyInfo;
  using SignalId     = DBusWrapper::SignalId;
  /// \endcond

  /**
   * @brief Creates empty interface description with given name
   *
   * @param interfaceName name of the interface
   */
  DBusInterfaceDescription(std::string interfaceName);

  /**
   * @brief adds new, synchronous method to the interface
   *
   * When method memberName is called on DBUS, callback functor will be called
   * with values received from DBUS. callback won't be called, if method was
   * called with invalid signature. Value returned from functor (or error message)
   * will be marshalled back to the caller.
   *
   * Template type T defines both arguments sent to the method
   * and expected values. Receiving different values will be reported as
   * error. For example:
   * \code{.cpp} Method<int(float, float)> \endcode
   * defines method, which takes two arguments (two floats) and return
   * single value of type int.
   *
   * @param memberName name of the method
   * @param callback functor, which will be called
   */
  template<typename T>
  void addMethod(const std::string& memberName, typename detail::dbus_interface_traits<T>::SyncCB callback)
  {
    detail::CallId callId;
    MethodInfo     mi;
    methods.push_back(std::move(mi));
    auto& z      = methods.back();
    z.in         = detail::EldbusArgGenerator_Args<T>::get();
    z.out        = detail::EldbusArgGenerator_ReturnType<T>::get();
    z.memberName = memberName;
    DBUS_DEBUG("call %d: method %s, in %s, out %s", callId.id, memberName.c_str(), detail::EldbusArgGenerator_Args<T>::name().c_str(), detail::EldbusArgGenerator_ReturnType<T>::name().c_str());
    z.callback = construct<T>(callId, callback);
    z.id       = callId;
  }

  /**
   * @brief adds new, synchronous property to the interface
   *
   * When property memberName is called on DBUS, respective callback functor will be called
   * with values received from DBUS. callback won't be called, if method was
   * called with invalid signature. Value returned from functor (or error message)
   * will be marshalled back to the caller.
   *
   * Template type T defines type of the value hidden under property.
   * Note, that library automatically wraps both sent and received value into
   * DBUS's wrapper type.
   *
   * @param memberName name of the method
   * @param getter functor, which will be called when property is being read
   * @param setter functor, which will be called when property is being set
   */
  template<typename T>
  void addProperty(const std::string& memberName, std::function<ValueOrError<T>()> getter, std::function<ValueOrError<void>(T)> setter)
  {
    properties.push_back({});
    auto& z         = properties.back();
    z.memberName    = memberName;
    z.typeSignature = detail::signature<T>::sig();
    if(getter)
    {
      detail::CallId getterId;
      z.getterId = getterId;
      DBUS_DEBUG("call %d: property %s (get) type %s", getterId.id, memberName.c_str(), detail::signature<T>::name().c_str());
      z.getCallback = [=](const DBusWrapper::MessagePtr& src, const DBusWrapper::MessageIterPtr& dst) -> std::string {
        try
        {
          auto v = detail::apply(getter, std::tuple<>{});
          if(v)
          {
            detail::signature<T>::set(dst, std::get<0>(v.getValues()));
            DBUS_DEBUG("call %d: success", getterId.id);
            return "";
          }
          DBUS_DEBUG("call %d: failed: %s", getterId.id, v.getError().message.c_str());
          return v.getError().message;
        }
        catch(std::exception& e)
        {
          return std::string("unhandled exception (") + e.what() + ")";
        }
        catch(...)
        {
          return "unhandled exception";
        }
      };
    }
    if(setter)
    {
      detail::CallId setterId;
      z.setterId = setterId;
      DBUS_DEBUG("call %d: property %s (set) type %s", setterId.id, memberName.c_str(), detail::signature<T>::name().c_str());
      z.setCallback = [=](const DBusWrapper::MessagePtr& src, const DBusWrapper::MessageIterPtr& src_iter) -> std::string {
        std::tuple<T> value;
        auto          src_signature = DBUS_W->eldbus_message_iter_signature_get_impl(src_iter);
        if(detail::signature<T>::get(src_iter, std::get<0>(value)))
        {
          try
          {
            auto v = detail::apply(setter, std::move(value));
            if(v)
            {
              DBUS_DEBUG("call %d: success", setterId.id);
              return "";
            }
            DBUS_DEBUG("call %d: failed: %s", setterId.id, v.getError().message.c_str());
            return v.getError().message;
          }
          catch(std::exception& e)
          {
            return std::string("unhandled exception (") + e.what() + ")";
          }
          catch(...)
          {
            return "unhandled exception";
          }
        }
        DBUS_DEBUG("call %d: failed to unpack values, got signature '%s', expected '%s'", setterId.id, src_signature.c_str(), detail::signature<T>::sig().c_str());
        return "call " + std::to_string(setterId.id) + ": failed to unpack values, got signature '" +
               src_signature + "', expected '" + detail::signature<T>::sig() + "'";
      };
    }
  }

  /**
   * @brief adds new signal to the interface
   *
   * Template types ARGS defines values, which will be emited with the signal
   *
   * @param memberName name of the method
   */
  template<typename... ARGS>
  SignalId addSignal(const std::string& memberName)
  {
    detail::CallId callId;
    signals.push_back({});
    auto& z      = signals.back();
    z.memberName = memberName;
    z.args       = detail::EldbusArgGenerator_Args<void(ARGS...)>::get(DBUS_W->Strings);
    z.id         = callId;
    DBUS_DEBUG("call %d: signal %s", callId.id, memberName.c_str());
    return SignalId{callId};
  }

private:
  /// \cond
  std::vector<MethodInfo>   methods;
  std::vector<PropertyInfo> properties;
  std::vector<SignalInfo>   signals;
  std::string               interfaceName;

  template<typename T>
  std::function<DBusWrapper::MessagePtr(const DBusWrapper::MessagePtr& msg)> construct(detail::CallId                                    callId,
                                                                                       typename detail::dbus_interface_traits<T>::SyncCB callback)
  {
    using VEArgs = typename detail::dbus_interface_traits<T>::VEArgs;
    return [=](const DBusWrapper::MessagePtr& msg) -> DBusWrapper::MessagePtr {
      DBUS_DEBUG("call %d: entering", callId.id);
      DBusWrapper::MessagePtr ret  = {};
      auto                    args = detail::unpackValues<VEArgs>(callId, msg);
      if(args)
      {
        try
        {
          auto v = detail::apply(callback, std::move(args.getValues()));
          if(v)
          {
            DBUS_DEBUG("call %d: success", callId.id);
            ret = DBUS_W->eldbus_message_method_return_new_impl(msg);
            detail::packValues(callId, ret, v);
          }
          else
          {
            DBUS_DEBUG("call %d: failed: %s", callId.id, v.getError().message.c_str());
            ret = DBUS_W->eldbus_message_error_new_impl(msg, "org.freedesktop.DBus.Error.Failed", v.getError().message);
          }
        }
        catch(std::exception& e)
        {
          auto txt = std::string("unhandled exception (") + e.what() + ")";
          DBUS_DEBUG("call %d: failed: %s", callId.id, txt.c_str());
          ret = DBUS_W->eldbus_message_error_new_impl(msg, "org.freedesktop.DBus.Error.Failed", txt);
        }
        catch(...)
        {
          DBUS_DEBUG("call %d: failed: %s", callId.id, "unhandled exception");
          ret = DBUS_W->eldbus_message_error_new_impl(msg, "org.freedesktop.DBus.Error.Failed", "unhandled exception");
        }
      }
      else
      {
        std::ostringstream err;
        err << "expected signature '" << detail::signature<VEArgs>::sig() << "', got '" << DBUS_W->eldbus_message_signature_get_impl(msg) << "'";
        auto str = err.str();
        DBUS_DEBUG("call %d: failed: %s", callId.id, str.c_str());
        ret = DBUS_W->eldbus_message_error_new_impl(msg, "org.freedesktop.DBus.Error.InvalidArgs", str);
      }
      return ret;
    };
  }
  /// \endcond
};

/**
 * @brief Class representing server's end of DBUS connection
 *
 * Allows listening (synchronously and asynchronosly) on methods on selected interface
 * Allows listening (synchronously and asynchronosly) on setting / getting properties.
 * Allows emiting signals.
 */
class DBusServer
{
public:
  /**
   * @brief Constructs non-connected dbus server.
   */
  DBusServer() = default;

  /**
   * @brief Constructs dbus server on either system or user dbus connection.
   */
  DBusServer(ConnectionType tp);

  /**
   * @brief Constructs dbus server on connection from getDBusConnectionByType
   */
  DBusServer(const DBusWrapper::ConnectionPtr& conn);

  /**
   * @brief Destructor
   *
   * Destructor will properly destroy everything. Destructor will cancel
   * pending replies.
   */
  ~DBusServer() = default;

  DBusServer(const DBusServer&) = delete;
  DBusServer(DBusServer&&)      = default;

  DBusServer& operator=(DBusServer&&) = default;
  DBusServer& operator=(const DBusServer&) = delete;

  /**
   * @brief Registers interface on given path name
   *
   * @param pathName path object to register interface on.
   * @param dscr
   * @param fallback
   */
  void addInterface(const std::string& pathName, DBusInterfaceDescription& dscr, bool fallback = false);

  /**
   * @brief Gets bus name of the current connection (must be connected)
   */
  std::string getBusName() const;

  /**
   * @brief Returns connection object for this dbus server object
   *
   * @return connection object
   */
  DBusWrapper::ConnectionPtr getConnection();

  /**
   * @brief Emits signal
   *
   * Emits signal based only on data passed to the function
   *
   * @param signal identifier of the signal
   * @param args values to emit
   */
  template<typename... ARGS>
  void emit2(const std::string& path, const std::string& interfaceName, const std::string& signalName, const ARGS&... args)
  {
    auto           msg = DBUS_W->eldbus_message_signal_new_impl(path, interfaceName, signalName);
    detail::CallId id;
    detail::packValues(id, msg, args...);
    DBUS_W->eldbus_connection_send_impl(connection, msg);
  }

  /**
   * @brief Returns current object path, when handling call to property / method
   *
   * User can call this function from inside callback used to handle property / method calls.
   * It will retrieve object's path used in the call. Note, that in asynchronous handling
   * of those calls user need to retrieve and store the current object / current connection
   * as the value will change at the moment user's callback handling will exit. For example:
   * \code{.cpp}
   * DBusInterfaceDescription interface{"name"};
   * auto handler_later = [](std::function<void(void)> done_cb) {
   *   // process something later on
   *   DBusServer::getCurrentObjectPath(); // this will return empty string
   * };
   * interface.addAsyncMethod<void()>("m", [=](std::function<void(void)> done_cb) {
   *   DBusServer::getCurrentObjectPath(); // this will current object's path
   *
   *   // do some processing later on and call done_cb, when it's done
   *   register_to_call_sometime_later_on(std::move(done_cb));
   * };
   * \endcode
   */
  static std::string getCurrentObjectPath()
  {
    return currentObjectPath;
  }

  /**
   * @brief Returns current connection object, when handling call to property / method
   *
   * User can call this function from inside callback used to handle property / method calls.
   * It will retrieve object's path used in the call. Note, that in asynchronous handling
   * of those calls user need to retrieve and store the current object / current connection
   * as the value will change at the moment user's callback handling will exit. For example:
   * \code{.cpp}
   * DBusInterfaceDescription interface{"name"};
   * auto handler_later = [](std::function<void(void)> done_cb) {
   *   // process something later on
   *   DBusServer::getCurrentObjectPath(); // this will return empty string
   * };
   * interface.addAsyncMethod<void()>("m", [=](std::function<void(void)> done_cb) {
   *   DBusServer::getCurrentObjectPath(); // this will current object's path
   *
   *   // do some processing later on and call done_cb, when it's done
   *   register_to_call_sometime_later_on(std::move(done_cb));
   * };
   * \endcode
   */
  static const DBusWrapper::ConnectionPtr& getCurrentConnection()
  {
    return currentConnection;
  }

  /// \cond
  class CurrentObjectSetter
  {
  public:
    CurrentObjectSetter(DBusWrapper::ConnectionPtr con, std::string path)
    {
      currentObjectPath = std::move(path);
      currentConnection = std::move(con);
    }
    ~CurrentObjectSetter()
    {
      currentObjectPath = "";
      currentConnection = {};
    }
    CurrentObjectSetter(const CurrentObjectSetter&) = delete;
    CurrentObjectSetter(CurrentObjectSetter&&)      = delete;
    void operator=(const CurrentObjectSetter&) = delete;
    void operator=(CurrentObjectSetter&&) = delete;
  };
  /// \endcond

private:
  /// \cond
  DBusWrapper::ConnectionPtr connection;
  struct DestructorObject
  {
    std::vector<std::function<void()>> destructors;
    ~DestructorObject()
    {
      for(auto& a : destructors) a();
    }
  };

  std::unique_ptr<DestructorObject>              destructorObject{new DestructorObject()};
  static thread_local std::string                currentObjectPath;
  static thread_local DBusWrapper::ConnectionPtr currentConnection;

  /// \endcond
};

/// \cond
DBusWrapper::ConnectionPtr getDBusConnectionByType(ConnectionType tp);
DBusWrapper::ConnectionPtr getDBusConnectionByName(const std::string& name);
std::string                getConnectionName(const DBusWrapper::ConnectionPtr&);
/// \endcond
} // namespace DBus

/// \cond
namespace std
{
template<size_t INDEX, typename... ARGS>
inline auto get(DBus::ValueOrError<ARGS...>& v) -> decltype(std::get<INDEX>(v.getValues()))&
{
  return std::get<INDEX>(v.getValues());
}

template<size_t INDEX, typename... ARGS>
inline auto get(const DBus::ValueOrError<ARGS...>& v) -> decltype(std::get<INDEX>(v.getValues()))
{
  return std::get<INDEX>(v.getValues());
}
} // namespace std
/// \endcond

#endif // DALI_INTERNAL_ACCESSIBILITY_BRIDGE_DBUS_H
