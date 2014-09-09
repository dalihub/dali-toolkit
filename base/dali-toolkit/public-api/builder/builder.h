#ifndef __DALI_TOOLKIT_UIBUILDER_H__
#define __DALI_TOOLKIT_UIBUILDER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// INTERNAL INCLUDES
#include <dali/dali.h>

namespace Dali DALI_IMPORT_API
{

namespace Toolkit
{

namespace Internal DALI_INTERNAL
{
class Builder;
}

typedef std::map<std::string, Property::Value> PropertyValueMap;

/**
 * This class provides the ability to load and style an actor tree from a string representation.
 *
 * The following is an example in JSON.
 *
 * @code
 *
 *  {
 *    "templates": // are named instantiable actor trees
 *    {
 *      "default-text":
 *      {
 *        "type":"TextActor",
 *        "font":"",
 *        "parent-origin":[0.5,0.5,0],
 *        "scale": [50,50,1]
 *      }
 *    },
 *    "styles": // are named property sets applied to actor trees
 *    {
 *     "my-style":
 *      {
 *        "size": [10,10,1] // root properties applied to a given root actor
 *        "actors":         // properties applied to actors found by name from root
 *        {
 *          "ok":           // properties for an actor named "ok"
 *          {
 *            "scale":[5,5,1],
 *          },
 *          "cancel":
 *          {
 *            "scale":[50,50,1],
 *          }
 *       },
 *      },
 *    },
 *    "stage":
 *    [
 *      {
 *        "type":"default-text",
 *        "text":"Hello World",
 *        "position":[0,0,0]
 *      },
 *    ]
 *  }
 *
 * @endcode
 *
 * The following shows a method to load the json file.
 * @code
 * Builder builder = Builder::New();
 * std::string json_data(ReadFile("layout.json"));
 * builder.LoadFromString(json_data);
 * @endcode
 * Examples
 * - Load all actors in the "stage" section to the root layer
 * @code
 * builder.AddActors( Stage::GetCurrent().GetRootLayer() );
 * @endcode
 *
 * - Create an actor tree from the "templates" section
 * @code
 * TextActor actor = TextActor::DownCast( builder.Create( "default-text" ) );
 * @endcode
 *
 * - Style an actor tree from the "styles" section
 * @code
 * builder.ApplyStyle( "my-style", actor );
 * @endcode
 *
 * - Create an actor tree from json
 * @code
 * TextActor actor = TextActor::DownCast( builder.CreateFromJson("{\"type\":\"TextActor\",\"font\":\"\",\"scale\":[50,50,1]}") );
 * @endcode
 *
 * - Apply a style to an actor tree from json
 * @code
 * builder.ApplyFromJson( textActor, ("{\"scale\":[5,5,1]}") );
 * @endcode
 *
 */

class Builder : public BaseHandle
 {
 public:
   /**
    * Create an Builder handle; this can be initialised with Builder::New()
    * Calling member functions with an uninitialised handle is not allowed.
    */
  Builder();

  /**
   * Creates an Builder object.
   * @return A handle to the Builder control.
   */
  static Builder New();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Builder();

  /**
   * UI string data format
   */
  enum UIFormat
  {
    JSON,                 ///< String is JSON
  };

  /**
   * Loads a string representation of an actor tree into memory.
   * The Actor is not automatically added to the stage.
   * This function will raise an exception for parse and logical structure errors.
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @param data A string represenation of an Actor tree
   * @param format The string representation format ie JSON
   */
  void LoadFromString( const std::string& data, UIFormat format = JSON );

  /**
   * @brief Adds user defined constants to all future style template or animation expansions
   *
   * e.g.
   *   PropertyValueMap map;
   *   map["IMAGE_DIRECTORY"] = "/usr/share/images";
   *   builder.AddConstants( map );
   *
   * @pre The Builder has been initialized.
   * @param map The user defined constants used in template expansions.
   */
  void AddConstants( const PropertyValueMap& map );

  /**
   * @brief Adds or modifies a user defined constant to all future style template or animation expansions
   *
   * e.g.
   * @code
   * builder.AddConstant( "IMAGE_DIRECTORY", "/usr/share/images" );
   * @endcode
   *
   * @pre The Builder has been initialized.
   * @param key The constant name to add or update
   * @param value The new value for the constant.
   */
  void AddConstant( const std::string& key, const Property::Value& value );

  /**
   * @brief Gets all currently defined constants.
   *
   * e.g.
   * @code
   * PropertyValueMap map = builder.GetConstants(); // get copy of current constants
   * map["IMAGE_DIRECTORY"] = "/usr/share/images";  // make modification
   * builder.AddConstants( map );                   // write back changes
   * @endcode
   *
   * @pre The Builder has been initialized.
   * @return A reference to the currently defined constants.
   */
  const PropertyValueMap& GetConstants() const;

  /**
   * @brief Gets a currently defined constant, or returns Property::INVALID
   *
   * e.g.
   * @code
   * PropertyValueMap map = builder.GetConstants(); // get copy of current constants
   * map["IMAGE_DIRECTORY"] = "/usr/share/images";  // make modification
   * builder.AddConstants( map );                   // write back changes
   * @endcode
   *
   * @pre The Builder has been initialized.
   * @param key The constant name to search for.
   */
  const Property::Value& GetConstant( const std::string& key ) const;

  /**
   * Creates an animation from the set of known animations
   * e.g.
   *   Animation a = builder.CreateAnimation( "wobble");
   *
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @pre The animationName exists in the animations section of the data representation
   * @param animationName The animation name to create
   * @returns The base handle of the created object
   */
  Animation CreateAnimation( const std::string& animationName );

  /**
   * @brief Creates an animation from the set of known animations with user defined constants
   *
   * e.g.
   *   PropertyValueMap map;
   *   map["ACTOR"] = actor.GetName();       // replaces '{ACTOR} in the template
   *   Animation a = builder.CreateAnimation( "wobble");
   *
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @pre The animationName exists in the animations section of the data representation
   * @pre The map contains all the constant expansions in the style template
   * @param animationName The animation name to create
   * @param map The user defined constants used in style template expansion.
   * @returns The base handle of the created object
   */
  Animation CreateAnimation( const std::string& animationName, const PropertyValueMap& map );

  /**
   * @brief Creates an animation from the set of known animations.
   *
   * The animation is applied to a specific actor.
   * e.g.
   *   Actor myInstance = builder.Create( "template-actor-tree" )
   *   Animation a = builder.CreateAnimation( "wobble", myInstance );
   *
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @pre The animationName exists in the animations section of the data representation
   * @param animationName The animation name to create
   * @param sourceActor The starting point in an actor tree, from which to look for property owners
   * @returns The base handle of the created object
   */
  Animation CreateAnimation( const std::string& animationName, Dali::Actor sourceActor );

  /**
   * @brief Creates an animation from the set of known animations with user defined constants
   *
   * The animation is applied to a specific actor.
   * e.g.
   *   PropertyValueMap map;
   *   map["ACTOR"] = actor.GetName();       // replaces '{ACTOR} in the template
   *   Actor myInstance = builder.Create( "template-actor-tree" )
   *   Animation a = builder.CreateAnimation( "wobble", myInstance);
   *
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @pre The animationName exists in the animations section of the data representation
   * @pre The map contains all the constant expansions in the style template
   * @param animationName The animation name to create
   * @param map The user defined constants used in style template expansion.
   * @param sourceActor The starting point in an actor tree, from which to look for property owners
   * @returns The base handle of the created object
   */
  Animation CreateAnimation( const std::string& animationName, const PropertyValueMap& map, Dali::Actor sourceActor );

  /**
   * @brief Creates an object (e.g. an actor) from the set of known style templates
   *
   * e.g.
   *   mActor.Add( Actor::DownCast(builder.Create( "default-text")) );
   *
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @pre The templateName exists in the templates section of the data representation
   *      and contains 'type' property used to create the object.
   * @param templateName The template to apply in creation.
   * @returns The base handle of the created object
   */
  BaseHandle Create( const std::string& templateName );

  /**
   * @brief Creates an object from the style templates with user defined constants
   *
   * e.g.
   *   PropertyValueMap map;
   *   map["IMAGE_DIR"] = "/usr/share/images"; // replaces '{IMAGE_DIR} in the template
   *   mActor.Add( Actor::DownCast(builder.Create( "default-image", map) ) );
   *
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @pre The templateName exists in the templates section of the data representation
   *      and contains 'type' property used to create the object.
   * @param templateName The template used to create the object.
   * @param map The user defined constants used in template expansion.
   * @returns The base handle of the created object
   */
  BaseHandle Create( const std::string& templateName, const PropertyValueMap& map );

  /**
   * @brief Creates an object (e.g. an actor) from given json snippet
   *
   * e.g.
   *   Actor a = Actor::DownCast(builder.CreateFromJson( "{\"type\":\"TextActor\"}"));
   *
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @param json The json snippet used to create the object.
   * @returns The base handle of the created object if any
   */
  BaseHandle CreateFromJson( const std::string& json );

  /**
   * Apply a style (a collection of properties) to an actor.
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @param styleName The name of the set of style properties to set on the handle object.
   * @param handle Then handle of the object on which to set the properties.
   *
   * @return Return true if the style was found
   */
  bool ApplyStyle( const std::string& styleName, Handle& handle );

  /**
   * Apply a style (a collection of properties) to an actor from the given json snippet
   * @pre The Builder has been initialized.
   * @pre Preconditions have been met for creating dali objects ie Images, Actors etc
   * @param handle Then handle of the object on which to set the properties.
   * @param json The json snippet used to create the object.
   *
   * @return Return true if the json snippet was parsed
   */
  bool ApplyFromJson(  Handle& handle, const std::string& json );


  /**
   * Add the actor tree in the "stage" section to the actor toActor.
   * ie if the representation has a 'stage' section that contains a tree of
   * actors then
   *    builder.AddActors( Stage::GetCurrent().GetRootLayer() );
   * will create and add the actors to the stage root layer.
   * @param toActor The actor to add the created actors to
   */
  void AddActors( Actor toActor );

  /**
   * Adds actors in the sectionName to the actor toActor.
   * ie if the representation has a sectionName section that contains a tree of
   * actors then
   *    builder.AddActors( sectionName, Stage::GetCurrent().GetRootLayer() );
   * will create and add the actors to the stage root layer.
   * @param sectionName The section name to search for the actor tree
   * @param toActor The actor to add the created actors to
   */
  void AddActors( const std::string &sectionName, Actor toActor );

  /**
   * Create a render task set.
   * @pre The Builder has been initialized.
   * @param name The library name of the render task set.
   */
  void CreateRenderTask( const std::string &name );

  /**
   * Get or create ShaderEffect from the ShaderEffect instance library.
   * An empty handle is returned otherwise.
   * @pre The Builder has been initialized.
   * @param name The name of a ShaderEffect in the loaded representation
   * @return A handle to a ShaderEffect if found, otherwise empty
   */
  ShaderEffect GetShaderEffect( const std::string &name );

  /**
   * Get or create FrameBufferImage from the FrameBufferImage instance library.
   * An empty handle is returned otherwise.
   * @pre The Builder has been initialized.
   * @param name The name of a FrameBufferImage in the loaded representation
   * @return A handle to a FrameBufferImage if found, otherwise empty
   */
  FrameBufferImage GetFrameBufferImage( const std::string &name );

  // Signals

  /**
   * @brief Builder signal type
   */
  typedef SignalV2< void () > Signal;

  /**
   * @brief Signal emitted when a quit action is requested by the builder.
   */
  Signal& QuitSignal();

private:
  Builder(Internal::Builder *impl);

}; // class Builder

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_UIBUILDER_H__
