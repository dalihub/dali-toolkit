/*! \page script-hello Scripting Hello World
 *
 * <h2 class="pg">Hello World - JSON layout</h2>
 *
 * The following JSON code is the minimum required to put the sentence "Hello World" on the screen.
 *
 * @code
 * {
 *  // a tree of actors
 *  "stage": [{
 *    "name": "text-label",
 *    "type": "TextLabel",
 *    "text": "Hello World",
 *    "parent-origin": "CENTER"
 *  }]
 * }
 * @endcode
 *
 * The following c++ code loads the JSON file
 *
 * @code
 * Builder builder = Builder::New();
 *
 * std::string json_data(ReadFile("layout.json"));
 *
 * builder.LoadFromString(json_data);
 *
 * Actor actor = builder.GetActor("text-label");
 *
 * Stage::GetCurrent().Add(actor);
 * @endcode
 *
 * <h2 class="pg">Hello World - Javascript</h2>
 *
 * Hello world can also be executed via Javascript.
 *
 * The Dali script application is needed to run the Javascript which provides a Javascript runtime and an interface to Dali.
 *
 * @code
 * daliscript hello-world.js
 * @endcode
 *
 * The TextLabel control to display Hello World can be constructed using Javascript dot notation accessing Dali Actor Properties.
 *
 * @code
 * var textLabel = Dali.TextLabel();
 *
 * textLabel.text          = "Hello World";
 * textLabel.font-family   = "FreeSans";
 * textLabel.font-style    = "Regular";
 * textLabel.parent-origin = "CENTER";
 *
 * Dali.Run();
 * @endcode
 *
 */


