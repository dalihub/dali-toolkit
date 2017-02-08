#ifndef __DALI_TOOLKIT_INTERNAL_BUILDER_H__
#define __DALI_TOOLKIT_INTERNAL_BUILDER_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <list>
#include <map>
#include <dali/public-api/common/stage.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali-toolkit/devel-api/builder/json-parser.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/internal/builder/builder-declarations.h>
#include <dali-toolkit/internal/builder/style.h>

// Warning messages usually displayed
#define DALI_SCRIPT_WARNING(format, args...) \
  DALI_LOG_WARNING("Script:" format, ## args)

// Info messages are usually debug build
#define DALI_SCRIPT_INFO(format, args...) \
  DALI_LOG_INFO(Dali::Toolkit::Internal::gFilterScript, Debug::General, "Script:" format, ## args)

// Info Verbose need to be swiched on in gFilterScript filter constructor (by default set to General)
#define DALI_SCRIPT_VERBOSE(format, args...) \
  DALI_LOG_INFO(Dali::Toolkit::Internal::gFilterScript, Debug::Verbose, "Script:" format, ## args)

namespace Dali
{

namespace Toolkit
{
  class TreeNode;
}

namespace Toolkit
{

namespace Internal
{

#if defined(DEBUG_ENABLED)
extern Dali::Integration::Log::Filter* gFilterScript;
#endif

class Builder;
class Replacement;

/**
 * @copydoc Toolkit::Builder
 */
class Builder : public Dali::BaseObject
{
public:

  Builder();

  /**
   * @copydoc Toolkit::Builder::LoadFromString
   */
  void LoadFromString( const std::string &data,
                       Dali::Toolkit::Builder::UIFormat rep = Dali::Toolkit::Builder::JSON );

  /**
   * @copydoc Toolkit::Builder::AddConstants
   */
  void AddConstants( const Property::Map& map );

  /**
   * @copydoc Toolkit::Builder::AddConstant
   */
  void AddConstant( const std::string& key, const Property::Value& value );

  /**
   * @copydoc Toolkit::Builder::GetConstants
   */
  const Property::Map& GetConstants() const;

  /**
   * @copydoc Toolkit::Builder::GetConstant
   */
  const Property::Value& GetConstant( const std::string& key ) const;

  /**
   * @copydoc Toolkit::Builder::CreateAnimation( const std::string& animationName );
   */
  Animation CreateAnimation( const std::string& animationName );

  /**
   * @copydoc Toolkit::Builder::CreateAnimation( const std::string& animationName, const Property::Map& map );
   */
  Animation CreateAnimation( const std::string& animationName, const Property::Map& map );

  /**
   * @copydoc Toolkit::Builder::CreateAnimation( const std::string&,Dali::Actor);
   */
  Animation CreateAnimation( const std::string& animationName, Dali::Actor sourceActor );

  /**
   * @copydoc Toolkit::Builder::CreateAnimation( const std::string&,const Property::Map&, Dali::Actor);
   */
  Animation CreateAnimation( const std::string& animationName, const Property::Map& map, Dali::Actor sourceActor );

  /**
   * @copydoc Toolkit::Builder::Create( const std::string& templateName );
   */
  BaseHandle Create( const std::string& templateName );

  /**
   * @copydoc Toolkit::Builder::Create( const std::string& templateName, const Property::Map& map );
   */
  BaseHandle Create( const std::string& templateName, const Property::Map& map );

  /**
   * @copydoc Toolkit::Builder::CreateFromJson( const std::string& json );
   */
  BaseHandle CreateFromJson( const std::string& json );

  /**
   * @copydoc Toolkit::Builder::ApplyFromJson( Handle& handle, const std::string& json );
   */
  bool ApplyFromJson(  Handle& handle, const std::string& json );

  /**
   * @copydoc Toolkit::Builder::ApplyStyle
   */
  bool ApplyStyle( const std::string& styleName, Handle& handle );

  /**
   * Lookup the stylename in builder. If it's found in the parse tree,
   * then return true.
   * @param[in] styleName The style name to search for
   * @return true if the stylename exists
   */
  bool LookupStyleName( const std::string& styleName );

  /**
   * Lookup the stylename in the recorded Styles - if it exists,
   * performs a shallow copy to the passed in style and returns true.
   * Otherwise it returns false.

   * @param[in] styleName The stylename to search for
   * @return A const pointer to the style object
   */
  const StylePtr GetStyle( const std::string& styleName );

  /**
   * @copydoc Toolkit::Builder::AddActors
   */
  void AddActors( Actor toActor );

  /**
   * @copydoc Toolkit::Builder::AddActors
   */
  void AddActors( const std::string &sectionName, Actor toActor );

  /**
   * @copydoc Toolkit::Builder::CreateRenderTask
   */
  void CreateRenderTask( const std::string &name );

  /**
   * @copydoc Toolkit::Builder::GetFrameBufferImage
   */
  FrameBufferImage GetFrameBufferImage( const std::string &name );

  /**
   * @copydoc Toolkit::Builder::GetFrameBufferImage
   */
  FrameBufferImage GetFrameBufferImage( const std::string &name, const Replacement& constant );

  /**
   * @copydoc Toolkit::Builder::GetPath
   */
  Path GetPath( const std::string &name );

  /**
   * @copydoc Toolkit::Builder::GetPathConstrainer
   */
  Dali::PathConstrainer GetPathConstrainer( const std::string& name );

  /*
   * Check if a given constrainer is of type PathConstrainer
   * @param[in] name The name of the constrainer
   * @return True if constainer is of type PathConstrainer, False otherwise
   *
   */
  bool IsPathConstrainer( const std::string& name );

  /**
   * @copydoc Toolkit::Builder::GetLinearConstrainer
   */
  Dali::LinearConstrainer GetLinearConstrainer( const std::string& name );

  /*
   * Check if a given constrainer is of type LinearConstrainer
   * @param[in] name The name of the constrainer
   * @return True if constainer is of type LinearConstrainer, False otherwise
   *
   */
  bool IsLinearConstrainer( const std::string& name );

  /**
   * @copydoc Toolkit::Builder::QuitSignal
   */
  Toolkit::Builder::BuilderSignalType& QuitSignal();

  /**
   * Emits the quit signal
   */
  void EmitQuitSignal();


protected:

  virtual ~Builder();

private:
  typedef std::vector<const char*> KeyStack;
  typedef std::vector< TreeNode::KeyNodePair > MappingsLut;
  typedef struct{ std::string name; Dali::LinearConstrainer linearConstrainer; } LinearConstrainerEntry;
  typedef std::vector<LinearConstrainerEntry> LinearConstrainerLut;
  typedef struct{ std::string name; Dali::PathConstrainer pathConstrainer; } PathConstrainerEntry;
  typedef std::vector<PathConstrainerEntry> PathConstrainerLut;
  typedef std::map<const std::string, Path> PathLut;
  typedef std::map<const std::string, FrameBufferImage> ImageLut;

private:
  // Undefined
  Builder(const Builder&);

  // Undefined
  Builder& operator=(const Builder& rhs);

  void LoadConstants( const TreeNode& root, Property::Map& intoMap );

  Animation CreateAnimation( const std::string& animationName,
                             const Replacement& replacement,
                             Dali::Actor        sourceActor );

  BaseHandle Create( const std::string& templateName,
                     const Replacement& constant );

  BaseHandle DoCreate( const TreeNode&    root,
                       const TreeNode&    node,
                       Actor              parent,
                       const Replacement& replacements );

  void SetupTask( RenderTask&              task,
                  const Toolkit::TreeNode& node,
                  const Replacement&       replacement );

  bool ApplyStyle( const std::string& styleName,
                   Handle&            handle,
                   const Replacement& replacement);

  void ApplyAllStyleProperties( const TreeNode&    root,
                                const TreeNode&    node,
                                Dali::Handle&      handle,
                                const Replacement& constant );

  void RecordStyles( const char*        styleName,
                     const TreeNode&    node,
                     Dali::Handle&      handle,
                     const Replacement& replacements );

  void RecordStyle( StylePtr           style,
                    const TreeNode&    node,
                    Dali::Handle&      handle,
                    const Replacement& replacements );

  void ApplyProperties( const TreeNode&    root,
                        const TreeNode&    node,
                        Dali::Handle&      handle,
                        const Replacement& constant );

  void ApplySignals( const TreeNode& root,
                     const TreeNode& node,
                     Dali::Handle& handle );

  void ApplyStylesByActor( const TreeNode&    root,
                           const TreeNode&    node,
                           Dali::Handle&      handle,
                           const Replacement& constant );

  void SetProperties( const TreeNode&    node,
                      Handle&            handle,
                      const Replacement& constant );

  bool MapToTargetProperty( Handle&            propertyObject,
                            const std::string& key,
                            const TreeNode&    node,
                            const Replacement& constant,
                            Property::Index&   index,
                            Property::Value&   value );

  /**
   * Find the key in the mapping table, if it's present, then generate
   * a property value for it (of the given type if available),
   * recursing as necessary, and stopping if any cycles are detected.
   *
   * @param[in] mappingRoot The JSON node containing the mappings
   * @param[in] theKey The key to search for
   * @param[in] propertyType The property type if known, or NONE
   * @param[in,out] value The string value to test and write back to.
   */
  bool GetPropertyMap( const TreeNode&  mappingRoot,
                       const char*      theKey,
                       Property::Type   propertyType,
                       Property::Value& value );

  void SetCustomProperties( const TreeNode&      node,
                            Handle&              handle,
                            const Replacement&   constant,
                            const std::string&   childName,
                            Property::AccessMode accessMode );

  /**
   * Find the key in the mapping table, if it's present, then generate
   * a property value for it (of the given type if available),
   * recursing as necessary, and stopping if any cycles are detected.
   *
   * @param[in] mappingRoot The JSON node containing the mappings
   * @param[in] theKey The key to search for
   * @param[in,out] keyStack the stack of visited keys
   * @param[in] propertyType The property type if known, or NONE
   * @param[in,out] value The string value to test and write back to.
   */
  bool RecursePropertyMap( const TreeNode&  mappingRoot,
                           KeyStack&        keyStack,
                           const char*      theKey,
                           Property::Type   propertyType,
                           Property::Value& value );


  /**
   * Tests if the value is a string delimited by <>. If it is, then it attempts to
   * change the value to the mapping from a matching key in the mappings table.
   * @param[in] mappingRoot The JSON node containing the mappings
   * @param[in,out] keyStack the stack of visited keys
   * @param[in,out] value The string value to test and write back to.
   * @return true if the value was converted, false otherwise.
   */
  bool ConvertChildValue( const TreeNode& mappingRoot,
                          KeyStack& keyStack,
                          Property::Value& value );

private:
  Toolkit::JsonParser                 mParser;
  ImageLut                            mFrameBufferImageLut;
  PathLut                             mPathLut;
  PathConstrainerLut                  mPathConstrainerLut;
  LinearConstrainerLut                mLinearConstrainerLut;
  SlotDelegate<Builder>               mSlotDelegate;
  Property::Map                       mReplacementMap;
  MappingsLut                         mCompleteMappings;
  Dictionary<StylePtr>                mStyles; // State based styles
  Toolkit::Builder::BuilderSignalType mQuitSignal;
};

} // namespace Internal

inline Internal::Builder& GetImpl(Dali::Toolkit::Builder& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<Internal::Builder&>(handle);
}

inline const Internal::Builder& GetImpl(const Dali::Toolkit::Builder& obj)
{
  DALI_ASSERT_ALWAYS(obj);

  const Dali::BaseObject& handle = obj.GetBaseObject();

  return static_cast<const Internal::Builder&>(handle);
}

} // namespace Toolkit

} // namespace Dali

#endif // __DALI_TOOLKIT_INTERNAL_BUILDER_H__
