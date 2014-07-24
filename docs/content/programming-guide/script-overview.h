/*! \page script-overview Scripting Overview
 *
 * Dali has scripting support to
 *
 * <ul>
 *   <li>Provide a mechanism to allow custom controls in scripting.
 *   <li>Support layouts using JSON.
 *   <li>Support a dynamic Javascript runtime.
 * </ul>
 *
 * This is accessed via the Dali script external application which wraps Dali with a scripting engine. For example
 *
 * @code
 * daliscript hello-world.js
 * @endcode
 *
 * <h1 class="pg">A Mechanism To Allow Custom Controls in Scripting</h1>
 *
 * <h2 class="pg">The TypeRegistry</h2>
 *
 * The TypeRegistry allows class 'types' to register themselves as creatable from a scripting environment.
 *
 * Custom controls can register a creation function using class run time type information (RTTI).
 *
 * The RTTI typeid provides Dali with a unique name to register the type. In this registration the creation function is responsible for creating new instances of the custom class.
 *
 * Signals can be added to this type registration with a signal connection function.
 *
 * Actions can be similarly added with an action function.
 *
 *
 * <h2 class="pg">Non Animatable Properies</h2>
 *
 * The property system has non animatable properties that can be used by the scripting runtime to set actor attributes.
 *
 * Custom controls can register properties for scripting access. The custom control is notified of a non animatable property value change.
 *
 *
 * <h2 class="pg">A Javascript example</h2>
 *
 * A Javascript runtime wrapping Dali and the V8 Javacript engine is being developed to allow the creation of pure Javascript applications.
 *
 * ie 'daliscript helloworld.js'.
 *
 * This example shows how a Javacript file relates to the TypeRegistry and Property system.
 *
 * @code
 * // Creation
 * // This line looks for a type registered as "MyActor" and calls its creation function
 * var custom = new MyActor();
 *
 * // Property access
 * // This line finds a property called "alpha" and Sets with SetProperty(index, Property::Value(2.0))
 * // If the property is non animatable it calls OnPropertySet(Property::Value(2.0))
 * custom.alpha = 2.0;
 *
 * // NB: non animatable properties can be strings
 * custom.text = "a label";
 *
 * // Actions
 * // This line finds the action function registered as "SelectPage" and calls its with a list of arguments
 * // (NB: arguments are currently limited to non aggregate types ie no list, maps or objects)
 * custom.SelectPage("one");
 *
 * // Signals
 * // This line finds the signal registered as "touched" and sets the "OnTouch" callback function
 * custom.signals.touched = OnTouch;
 *
 * // OnTouch could have been previously defined as
 * function OnTouch(name)
 * {
 *   custom.text = name
 * }
 * @endcode
 *
 * <h1 class="pg">Supporting Layouts Using JSON</h1>
 *
 * The builder in Dali Toolkit provides a means to define layouts using the JSON file format.
 *
 * This format defines a text representation for key value pairs and lists of data. Lists and Maps can hold the fundamental Javascript data types of string, number(float/int), true, false, nil.
 *
 * *** Current Status
 *
 * Currently the builder supports internal Toolkit and Dali controls.
 *
 * *** Next Iteration
 *
 * The builder will be improved to make use of the TypeRegistry and non animatable properties and allow Custom Controls to be added into Scripting.
 *
 * This means the current JSON format will alter slightly (for example; properties will not be defined as a tree but as one level below the actor definition)
 *
 * An actor tree defined in JSON will be retrievable as a "Buildable" class instance.
 *
 * This buildable class allows the creation of the actor tree. It will also aid resource managment as a buildable can store the layout representation and unload resources when off stage (reconstructing the object when its added back onto the stage).
 *
 * <h1 class="pg">Supporting A Javascript Runtime</h1>
 *
 * As a separate project an application will be available that can execute Javascript.
 *
 * This application will provide a wrapping layer between V8 and Dali and allow a natural interface to the Javascript developer.
 *
 *
 *
 *
 *
 *
 *
 *
 */
