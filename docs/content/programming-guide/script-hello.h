/*! \page script-hello Scripting Hello World
 *
 * <h2 class="pg">Hello World - JSON layout</h2>
 *
 * The following JSON code is the minimum required to put the sentence "Hello World" on the screen.
 *
 * @code
 * {
 *     "fonts":
 *     {
 *         "freesans": {"name": "FreeSans", "point-size": 12.0, "weight": "WEIGHT_REGULAR" }
 *     },
 *     "actors":
 *     [
 *         {"name":"text-actor",
 *          "type":"Text",
 *          "text":"Hello World",
 *          "font":"freesans",
 *          "parent-origin":"CENTER"
 *         }
 *     ]
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
 * Actor actor = builder.GetActor("text-actor");
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
 * The TextActor control to display Hello World can be constructed using Javascript dot notation accessing Dali Actor Properties.
 *
 * @code
 * var textActor = Dali.TextActor();
 *
 * textActor.text          = "Hello World";
 * textActor.font          = "FreeSans";
 * textActor.font-weight   = "WEIGHT_REGULAR";
 * textActor.parent-origin = "CENTER";
 *
 * Dali.Run();
 * @endcode
 *
 */


