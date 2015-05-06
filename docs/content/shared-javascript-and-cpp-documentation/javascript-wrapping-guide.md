/**
 *
# JavaScript wrapping guide  {#javascriptwrapping}

This guide outlines what files to modify when the DALi C++ public API changes.

## Background

We use Google's V8 to run JavaScript code.
https://developers.google.com/v8/get_started

#### Folder structure

There is a folder for each type of wrapped object.
  

The filename for a wrapped type are always object-wrapper.xxx
The filename for the static functions that forward calls to DALi are always object-api.xxx
  
The current file / folder structure is as follows:
  
![ ](../assets/img/javascript-wrapping-guide/folder-view.png)
![ ](folder-view.png)


## What to do when the DALi public API changes:

### New property has been added
- No code change required.
- It will be automatically avalable using the dot notation. E.g. actor.my_new_property = true;

### New property type has been added
- modify property-value-wrapper.h / .cpp to support the new type

### New function added to an object
- Add the function name to function table in my-object-wrapper.cpp
- Add the forwarding function to my-object-api.cpp/.h
- Ensure you have created YUIDOC documention above the function
  
![ ](../assets/img/javascript-wrapping-guide/adding-function.png)
![ ](adding-function.png)

### New object added

This is an example of wrapping a new DALi C++ object called Light.

- in dali-wrapper.cpp in ConstructorFunctionTable insert the constructor in the table.
  
![ ](../assets/img/javascript-wrapping-guide/constructors.png)
![ ](constructors.png)
  

Objects registered in this table can be created in JavaScript as follows:

~~~{.js}
var light = new dali.Light();
~~~

- Add the Light to the Type enum in BaseWrappedObject class.
  
![ ](../assets/img/javascript-wrapping-guide/base-wrapped-types.png)
![ ](base-wrapped-types.png)
  

-  Create the light-wrapper / light-api files
  
If Light inherits from Handle then use path-wrapper and path-api as a template to create light-wrapper and light-api
( inherits from HandleWrapper)
  
Otherwise use animation-wrapper and animation-api as a template ( inherts from BaseWrappedObject)



## Design
![ ](../assets/img/javascript-wrapping-guide/high-level-design.png)
![ ](high-level-design.png)


![ ](../assets/img/javascript-wrapping-guide/plugin-creation.png)
![ ](plugin-creation.png)


![ ](../assets/img/javascript-wrapping-guide/plugin-execution.png)
![ ](plugin-execution.png)

### Internals
In order to wrap DALi C++ objects in JavaScript, we use
hidden fields inside the JavaScript object.


  
| JavaScript Object                     |   _   |  C++ WrappedObject  (e.g. ImageWrapper)|
|---------------------------------------|-------|----------------------------------------|
| Hidden internal fields                |       |                                        |
| *Pointer to a     WrappedObject       | ----> |      Handle to a Dali::Image object    |
| Type of wrapped object (e.g. Image)   |       |                                        |
  

So if you call
~~~{.js}
var name = myActor.getId();
~~~
v8 will detect myActor is a wrapped object, and call getId() on that wrapped object.
The wrapped object, then forwards the command to the real DALi object.
  
Whenever we want to access functions / properties of that wrapped object, we unwrap it
to get access to the Dali object.
  
Each wrapped object registers with Dali garbage collector so they can be deleted
when Dali shuts down

@class _Guide_JavaScript_Wrapping
*/