/*! \page script-howto Scripting HowTo
 *
 * <h2 class="pg">Scripting A Custom Control</h2>
 *
 * These steps must be taken to provide scripting access for your control.
 *
 * <ul>
 *   <li>Register your class Type.
 *   <li>Register Signals and Actions (optional).
 *   <li>Register properties (optional).
 * </ul>
 *
*
 * <h3 class="pg">Registering your Type, Signals and Actions</h3>
 *
 * As part of your <b>my-actor.cpp</b> a <em>static</em> "Dali::TypeRegistration" object is created to register MyActor for scripting.
 *
 * Functions for Creation, Signal Connection and Action are registered with this object.
 *
 * @code
 * namespace   // type registration
 * {
 *
 * // Register MyActor with base actor CustomActor and creation function CreateCustom
 * Dali::TypeRegistration mCustomType( typeid(MyActor), typeid(Dali::CustomActor), MyActor::Create );
 *
 * // Add a signal to the type registration
 * Dali::TypeSignalConnector signal1( mCustomType, "page-changed", MyActor::DoConnectSignalCustom);
 *
 * // Add an action to the type registration
 * Dali::TypeAction action1( mCustomType, "SelectPage", MyActor::DoActionCustom);
 *
 * }
 * @endcode
 *
 * The registered handling functions are also static. For example.
 *
 * @code
 * BaseHandle MyActor::Create(void)
 * {
 *   return MyActor::New();
 * }
 *
 * Dali::Connection MyActor::DoConnectSignalCustom(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor)
 * {
 *   Dali::Connection connection ;
 *
 *   MyActor* actor = dynamic_cast<MyActor*>(object);
 *
 *   if(actor && "page-changed" == signalName)
 *   {
 *     connection = return actor->PageChangedSignal().Connect( tracker, functor );
 *   }
 *
 *   return connection ;
 * }
 *
 * bool MyActor::DoActionCustom(BaseObject* object, const std::string& actionName, const PropertyValueContainer& attributes)
 * {
 *   bool actioned = false ;
 *
 *   MyActor* actor = dynamic_cast<MyActor*>(object) ;
 *
 *   if(actor && "SelectPage" == actionName)
 *   {
 *      actor->DoSelectPage() ;
 *      actioned = true;
 *   }
 *
 *   return actioned ;
 * }
 * @endcode
 *
 * <h3 class="pg">Providing Properties for scripting</h3>
 *
 * Properties can be registered by name to allow script access.
 *
 * A RegisterProperty() call with property attributes allows the custom class to register non animatable properties.
 *
 * @code
 * void MyActor::Initialize()
 * {
 *   // Register a non animatable and writeable property.
 *   mPropertyAlphaIndex = Self().RegisterProperty("alpha", 0.0f, Dali::Property::WRITEABLE);
 * }
 * @endcode
 *
 * If a non animatable property is set then the class is notified via the OnPropertySet virtual function.
 *
 * @code
 * void MyActor::OnPropertySet(Property::Index index, Property::Value propertyValue)
 * {
 *  if(index == mPropertyAlphaIndex)
 *  {
 *    SetAlpha(propertyValue.<float>Get());
 *  }
 * }
 *
 * @endcode
 *
 */
