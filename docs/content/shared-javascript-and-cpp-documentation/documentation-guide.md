/**
 *

# Writing documentation for the DALi programing guide  {#documentationguide}

To allow documentation to be shared between C++ and JavaScript, please follow these guidelines:

 - Create a mark down file (.md) using GitHub Flavoured Markdown https://help.github.com/articles/github-flavored-markdown/
 - Put it into the shared C++ / JavaScript documentation: dali-toolkit/docs/content/shared-javascript-and-cpp-documentation/~
 - Include code samples for both C++ and JavaScript in the mark down.
 - See script-overview.md overview in dali-toolkit/docs/content/shared-javascript-and-cpp-documentation for an example
 - For YUIDOC to parse the file it needs:
   - Enclosed in code comment block
   - Have a class tag with a description of the file
 - For DOXYGEN to link to the mark down it currently needs a reference {hash myfile}
  

#### Images
 Images are shared between both Doxygen and YUIDOC tools using a symbolic link.
 You need to link to the image twice in shared documentation.
 This is because YUIDOC stores images in a folder called assets/img/  which is relative to the HTML pages.
 Where as Doxygen copies all images in to the same folder as the HTML generated pages.

 ~~~
![ ](../assets/img/screen-shot.png)    // required for YUIDOC
![ ](screen-shot.png)                  // required for Doxygen

The space between the brackets is the alternative text. This means you will never see a broken image symbol.
~~~
  
## Example
![ ](../assets/img/example-documentation/example-code.png)
![ ](example-code.png)



#### Why use GitHub flavoured markdown?
 - Table support is good and language specific code blocks are easier to define ( javascript/C++).
 - Doxygen and YUIDOC both support it.


@class _Guide_Writing_DALi_Programming_Guide_Documentation

*/
