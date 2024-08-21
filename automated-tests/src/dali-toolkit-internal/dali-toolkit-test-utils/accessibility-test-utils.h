#ifndef __DALI_TOOLKIT_ACCESSIBILITY_TEST_UTILS__
#define __DALI_TOOLKIT_ACCESSIBILITY_TEST_UTILS__

#include <dali/devel-api/adaptor-framework/accessibility.h>

namespace Dali
{
namespace Accessibility
{
void                                                                           TestEnableSC(bool b);
std::vector<Address>                                                           TestGetChildren(const Address& adr);
std::string                                                                    TestGetName(const Address& adr);
std::string                                                                    TestGetDescription(const Address& adr);
uint32_t                                                                       TestGetRole(const Address& adr);
std::string                                                                    TestGetRoleName(const Address& adr);
Address                                                                        TestGetParent(const Address& adr);
std::string                                                                    TestGetLocalizedRoleName(const Address& adr);
std::array<uint32_t, 2>                                                        TestGetStates(const Address& adr);
std::unordered_map<std::string, std::string>                                   TestGetAttributes(const Address& adr);
bool                                                                           TestDoGesture(const Address& adr, Dali::Accessibility::Gesture type, int32_t xBeg, int32_t xEnd, int32_t yBeg, int32_t yEnd, Dali::Accessibility::GestureState state, uint32_t eventTime);
std::vector<std::tuple<uint32_t, std::vector<Dali::Accessibility::Address> > > TestGetRelationSet(const Address& adr);
Address                                                                        TestGetChildAtIndex(const Address& adr, int index);
ComponentLayer                                                                 TestGetLayer(const Address& adr);
int                                                                            TestGetIndexInParent(const Address& adr);
bool                                                                           TestGrabFocus(const Address& adr);
bool                                                                           TestGrabHighlight(const Address& adr);
bool                                                                           TestClearHighlight(const Address& adr);
std::tuple<int32_t, int32_t, int32_t, int32_t>                                 TestGetExtents(const Address& adr, Dali::Accessibility::CoordinateType coordinateType);
int                                                                            TestGetMdiZOrder(const Address& adr);
double                                                                         TestGetAlpha(const Address& adr);
void                                                                           PrintTree(const Address& root, size_t depth = 0);
bool                                                                           Find(const std::vector<std::string>& collection, const std::string& key);
std::string                                                                    TestGetActionName(const Address& adr, size_t index);
std::string                                                                    TestGetLocalizedActionName(const Address& adr, size_t index);
size_t                                                                         TestGetActionCount(const Address& adr);
bool                                                                           TestDoAction(const Address& adr, size_t index);
bool                                                                           TestDoAction(const Address& adr, const std::string& name);
std::string                                                                    TestGetActionKeyBinding(const Address& adr, size_t index);
std::string                                                                    TestGetActionDescription(const Address& adr, size_t index);
void                                                                           TestResetMoveOutedCalled();
bool                                                                           TestGetMoveOutedCalled();
bool                                                                           TestPropertyChangeCalled();
} // namespace Accessibility
} // namespace Dali

#endif //__DALI_TOOLKIT_ACCESSIBILITY_TEST_UTILS__
