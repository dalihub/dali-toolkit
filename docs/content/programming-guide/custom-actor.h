/*! \page custom-actor Custom Actor
 * The Dali::CustomActor is used as a base class for UI controls.  It is a proxy object to enable derived classes access
 * to a subset of the methods defined in the internal Actor class.
 *
 * Classes deriving from Custom Actor should follow the same design principle as the rest of the Dali API.
 *
 * One class of the new UI control should inherit from Dali::CustomActor, while a second should inherit
 * Dali::CustomActorImpl.  This implementation class contains a number of pure virtual methods that enable the new UI
 * control to respond to a variety of events such as touch and notification of being added to the stage.
 *
 * For example, if creating a new button widget called myNewButton, the user would create two classes, myNewButton which
 * derives from Dali::CustomActor and an implementation part myNewButtonImpl which would derive from Dali::CustomActorImpl.
 *
 * In the New() method for the myNewButton class, the user should then create a new instance of the myNewButtonImpl class
 * and pass this to the constructor of the myNewButton object.  Internally the connection will be made
 * between the new widget actor and Dali, thus allowing messages such as OnSizeSet to be received by the new actor.
 *
 * It is the responsibility of the implementation of the new UI control to implement the method bodies for the inherited
 * pure virtual methods from Dali::CustomActorImpl.  Obviously the application won't compile if the methods are not
 * overidden, but the user does not need to fill in the code for methods they don't want or need to use.
 *
 * The following code shows the static New() method from the implementation part of the TextView UI control:
 * \code
 * Dali::Toolkit::TextView TextView::New()
 * {
 *   // Create the implementation, temporarily owned on stack
 *   boost::intrusive_ptr< TextView > textView = new TextView;
 *
 *   // Pass ownership to CustomActor
 *   Dali::Toolkit::TextView handle( *textView );
 *
 *   // Second-phase init of the implementation
 *   // This can only be done after the CustomActor connection has been made...
 *   textView->Initialize();
 *
 *   return handle;
 * }
 * \endcode
 *
 * After the implementation object is created it is passed back to the basic Text View through the constructor,the
 * constructor uses this passed in object to initialise the internal implementation objects.
 *
 * After both the objects are created it calls an init method on the implementation which is used to initialise
 * objects.  THis is the preferred way to do things so to avoid errors in the constructors.
 *
 * If desired, the user can then use the myNewButtonImpl implementation class to handle only the callback message
 * handler methods, and do all the rest of their widget processing the the main myNewButton class.  Access to the
 * implementation class can be gained using the GetImpl(*this) method.  For example:
 *
 * \code
 * void TextView::SetFont(const Font newFont)
 * {
 *  GetImpl(*this).SetFont(newFont);
 * }
 * \endcode
 */
