/*! \page boost-function Boost function usage
<h2 class="pg">Signal handler functions</h2>
 boost::function is mostly used to define callback function types in Dali.
 For example in text-actor.h:
 \code
class TextActor : public RenderableActor
{
public:
   ...
   typedef boost::function<void (TextActor)> TextCallbackType;
   typedef Signal<TextSignalType, TextCallbackType> TextSignal;

public:
     TextSignal SignalTextAvailable();
    ...
 }
 \endcode

 \p "boost::function<void (TextActor)>" specifies a function type which has no return value and takes \p TextActor as an argument.
 The application can set a function of this type to be called when the text available signal is emitted.

 <h2 class="pg">Specifying custom functions (eg. sorting)</h2>
 Dali::Layer::SetSortFunction() is an example where the developer should use a boost function in order to specify the sorting algorithm.\n
 This API accepts both standalone functions or class methods.\n
 Standalone functions can be simply given as:
 \code
 static float TestSortFunction(const Vector3& position, float sortModifier)
 {
   // do something
 }

 void TestApp::SetRootSort()
 {
   Layer root = Stage::GetCurrent().GetLayer( 0 );
   root.SetSortFunction(TestSortFunction);
 }
 \endcode
 For member functions \b binding needs to be done:
 \code
 float TestApp::TestSortFunction(const Vector3& position, float sortModifier)
 {
   // do something
 }

 void TestApp::SetRootSort()
 {
   Layer root = Stage::GetCurrent().GetLayer( 0 );
   root.SetSortFunction(boost::bind(&TestApp::TestSortFunction, this));
 }
 \endcode
 \n\n
 For more information please see <a href="http://www.boost.org/doc/">the boost project documentation</a>.

 */

