<!--
/**-->

 # Hello World - JSON layout{#script-hello}

 The following JSON code is the minimum required to put the sentence "Hello World" on the screen.

~~~{.json}
{
 // a tree of actors
 "stage": [{
   "name": "text-label",
   "type": "TextLabel",
   "text": "Hello World",
   "parentOrigin": "CENTER"
 }]
}
~~~

 The following code loads the JSON file

~~~{.cpp}
 // C++
 Builder builder = Builder::New();

 std::string json_data(ReadFile("layout.json"));

 builder.LoadFromString(json_data);

 builder.AddActors( Stage::GetCurrent().GetRootLayer() );

 ~~~

 ~~~{.js}
 // JavaScript

 var builder = new dali.Builder();

 builder.loadFromFile("layout.json");

 builder.addActors( dali.stage.getRootLayer )

~~~
 ## Hello World - Javascript

 The DALi script application is needed to run the Javascript which provides a Javascript runtime and an interface to Dali.

~~~
 scripting.example hello-world.js
~~~

 The TextLabel control to display Hello World can be constructed using Javascript dot notation accessing DALi Actor Properties.

~~~{.js}
// JavaScript

 var textLabel = new dali.TextLabel();

 textLabel.text         = "Hello World";
 textLabel.fontFamily   = "FreeSans";
 textLabel.fontStyle    = "Regular";
 textLabel.parentOrigin = dali.CENTER;

 dali.stage.add( textLabel );
~~~

@class _Guide_Script_Hello_World

*/