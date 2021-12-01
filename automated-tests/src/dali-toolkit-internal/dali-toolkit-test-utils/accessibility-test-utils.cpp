#include <automated-tests/src/dali-toolkit-internal/dali-toolkit-test-utils/accessibility-test-utils.h>
#include <dali-toolkit-test-suite-utils.h>
#include <dali/devel-api/common/stage.h>
#include <dali/devel-api/adaptor-framework/accessibility-impl.h>
#include "dbus-wrapper.h"

namespace Dali
{
namespace Accessibility
{
  using MethodType = TestDBusWrapper::MethodType;
  using MessagePtr = DBusWrapper::MessagePtr;

  static bool gMoveOutedCalled = false;

  void TestEnableSC(bool b)
  {
    static bool firstTime = true;
    if (b && firstTime)
    {
      firstTime = false;
      auto bridge = Accessibility::Bridge::GetCurrentBridge();
      Dali::Stage stage = Dali::Stage::GetCurrent();
      auto accessible = Accessibility::Accessible::Get( stage.GetRootLayer(), true );
      bridge->AddTopLevelWindow( accessible );
      bridge->SetApplicationName( "TestApp" );
      bridge->Initialize();

      static bool ScreenReaderEnabled = false;
      static bool IsEnabled = false;

      auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());

      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/bus", "org.a11y.Status", "ScreenReaderEnabled", MethodType::Getter}] = [wr](const MessagePtr &m) -> MessagePtr {
          auto reply = wr->newReplyMessage(m);
          wr->Encode(reply, std::tuple<TestDBusWrapper::Variant<bool>>{ ScreenReaderEnabled });
          return reply;
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/bus", "org.a11y.Status", "IsEnabled", MethodType::Getter}] = [wr](const MessagePtr &m) -> MessagePtr {
          auto reply = wr->newReplyMessage(m);
          wr->Encode(reply, std::tuple<TestDBusWrapper::Variant<bool>>{ IsEnabled });
          return reply;
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/bus", "org.a11y.Bus", "GetAddress", MethodType::Method}] = [wr](const MessagePtr &m) -> MessagePtr {
          auto reply = wr->newReplyMessage(m);
          wr->Encode(reply, std::tuple<const char*>{ "bus" });
          return reply;
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible/root", "org.a11y.atspi.Socket", "Embed", MethodType::Method}] = [wr](const MessagePtr &m) -> MessagePtr {
          auto reply = wr->newReplyMessage(m);
          wr->Encode(reply, std::tuple<Address>{ {"bus", "root"} });
          return reply;
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Object", "PropertyChange", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Object", "StateChanged", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Object", "BoundsChanged", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Object", "ActiveDescendantChanged", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Object", "TextChanged", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Object", "TextCaretMoved", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Object", "MoveOuted", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          gMoveOutedCalled = true;
          return wr->newReplyMessage(m);
      };

      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Window", "Activate", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
      wr->testMethods[std::tuple<std::string, std::string, std::string, MethodType>{"/org/a11y/atspi/accessible", "org.a11y.atspi.Event.Window", "Deactivate", MethodType::Method}] =
      [wr](const MessagePtr &m) -> MessagePtr {
          return wr->newReplyMessage(m);
      };
    }
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    wr->fromTestChangeProperty("/org/a11y/bus", "org.a11y.Status", "ScreenReaderEnabled", b);
    wr->fromTestChangeProperty("/org/a11y/bus", "org.a11y.Status", "IsEnabled", b);
  }

  std::vector<Address> TestGetChildren(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall<std::vector<Address>>(adr.GetPath(), "org.a11y.atspi.Accessible", "GetChildren", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  std::string TestGetName(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto name = wr->fromTestGet<std::string>(adr.GetPath(), "org.a11y.atspi.Accessible", "Name");
    return name;
  }

  std::string TestGetDescription(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto description = wr->fromTestGet<std::string>(adr.GetPath(), "org.a11y.atspi.Accessible", "Description");
    return description;
  }

  uint32_t TestGetRole(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall<uint32_t>(adr.GetPath(), "org.a11y.atspi.Accessible", "GetRole", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  std::string TestGetRoleName(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall<std::string>(adr.GetPath(), "org.a11y.atspi.Accessible", "GetRoleName", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  Address TestGetParent(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestGet< Address >(adr.GetPath(), "org.a11y.atspi.Accessible", "Parent");
    return chs;
  }

  std::string TestGetLocalizedRoleName(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall<std::string>(adr.GetPath(), "org.a11y.atspi.Accessible", "GetLocalizedRoleName", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  std::array< uint32_t, 2 > TestGetStates(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall<std::array< uint32_t, 2 >>(adr.GetPath(), "org.a11y.atspi.Accessible", "GetState", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  std::unordered_map< std::string, std::string > TestGetAttributes(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall<std::unordered_map< std::string, std::string >>(adr.GetPath(), "org.a11y.atspi.Accessible", "GetAttributes", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  bool TestDoGesture(const Address &adr, Dali::Accessibility::Gesture type, int32_t xBeg, int32_t xEnd, int32_t yBeg, int32_t yEnd, Dali::Accessibility::GestureState state, uint32_t eventTime)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< bool >(adr.GetPath(), "org.a11y.atspi.Accessible", "DoGesture",
        std::tuple< Dali::Accessibility::Gesture, int32_t, int32_t, int32_t, int32_t, Dali::Accessibility::GestureState, uint32_t >(type, xBeg, xEnd, yBeg, yEnd, state, eventTime ));
    return std::move(std::get<0>(chs));
  }

  std::vector< std::tuple< uint32_t, std::vector< Dali::Accessibility::Address > > > TestGetRelationSet(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< std::vector< std::tuple< uint32_t, std::vector< Dali::Accessibility::Address > > > >(adr.GetPath(), "org.a11y.atspi.Accessible", "GetRelationSet", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  Address TestGetChildAtIndex(const Address &adr, int index)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< Address >(adr.GetPath(), "org.a11y.atspi.Accessible", "GetChildAtIndex", std::tuple< int >( index ));
    return std::move(std::get<0>(chs));
  }

  ComponentLayer TestGetLayer(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< Dali::Accessibility::ComponentLayer >(adr.GetPath(), "org.a11y.atspi.Component", "GetLayer", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  int TestGetIndexInParent(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< int >(adr.GetPath(), "org.a11y.atspi.Accessible", "GetIndexInParent", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  bool TestGrabFocus(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< bool >(adr.GetPath(), "org.a11y.atspi.Component", "GrabFocus", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  bool TestGrabHighlight(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< bool >(adr.GetPath(), "org.a11y.atspi.Component", "GrabHighlight", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  bool TestClearHighlight(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< bool >(adr.GetPath(), "org.a11y.atspi.Component", "ClearHighlight", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  std::tuple< int32_t, int32_t, int32_t, int32_t > TestGetExtents(const Address &adr, Dali::Accessibility::CoordinateType coordinateType)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< std::tuple< int32_t, int32_t, int32_t, int32_t > >(adr.GetPath(), "org.a11y.atspi.Component", "GetExtents", std::make_tuple(static_cast<uint32_t>(coordinateType)));
    return std::move(std::get<0>(chs));
  }

  int TestGetMdiZOrder(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< int16_t >(adr.GetPath(), "org.a11y.atspi.Component", "GetMDIZOrder", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  double TestGetAlpha(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< double >(adr.GetPath(), "org.a11y.atspi.Component", "GetAlpha", std::tuple<>());
    return std::move(std::get<0>(chs));
  }

  std::string TestGetActionName(const Address &adr, size_t index)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< std::string >(adr.GetPath(), "org.a11y.atspi.Action", "GetName", std::tuple< int32_t >( index ));
    return std::move(std::get<0>(chs));
  }

  std::string TestGetLocalizedActionName(const Address &adr, size_t index)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< std::string >(adr.GetPath(), "org.a11y.atspi.Action", "GetLocalizedName", std::tuple< int32_t >( index ));
    return std::move(std::get<0>(chs));
  }

  size_t TestGetActionCount(const Address &adr)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto count = wr->fromTestGet< int32_t >(adr.GetPath(), "org.a11y.atspi.Action", "NActions");
    return count;
  }

  bool TestDoAction(const Address &adr, size_t index)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< bool >(adr.GetPath(), "org.a11y.atspi.Action", "DoAction", std::tuple< int32_t >( index ));
    return std::move(std::get<0>(chs));
  }

  bool TestDoAction(const Address &adr, const std::string& name)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< bool >(adr.GetPath(), "org.a11y.atspi.Action", "DoActionName", std::tuple< std::string >( name ));
    return std::move(std::get<0>(chs));
  }

  std::string TestGetActionKeyBinding(const Address &adr, size_t index)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< std::string >(adr.GetPath(), "org.a11y.atspi.Action", "GetKeyBinding", std::tuple< int32_t >( index ));
    return std::move(std::get<0>(chs));
  }

  std::string TestGetActionDescription(const Address &adr, size_t index)
  {
    auto wr = static_cast<TestDBusWrapper*>(DBusWrapper::Installed());
    auto chs = wr->fromTestCall< std::string >(adr.GetPath(), "org.a11y.atspi.Action", "GetDescription", std::tuple< int32_t >( index ));
    return std::move(std::get<0>(chs));
  }

  void TestResetMoveOutedCalled ()
  {
    gMoveOutedCalled = false;
  }

  bool TestGetMoveOutedCalled ()
  {
    return gMoveOutedCalled;
  }

  void PrintTree(const Address &root, size_t depth)
  {
    auto name = TestGetName(root);
    printf("%10s", root.GetPath().c_str());
    for(unsigned int i = 0; i < depth; ++i) printf("  ");
    printf("%s\n", name.c_str());
    auto chs = TestGetChildren(root);
    for(auto &c : chs)
    {
      PrintTree(c, depth + 1);
    }
  }

  bool Find(const std::vector< std::string > &collection, const std::string &key)
  {
    for(auto& it : collection)
    {
      if(it == key)
      {
        return true;
      }
    }
    return false;
  }

} // namespace Accessibility
} // namespace Dali
