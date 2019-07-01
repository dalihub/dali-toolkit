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


*/
