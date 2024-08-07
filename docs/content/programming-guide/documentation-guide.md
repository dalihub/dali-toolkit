<!--
/**-->

# Writing documentation for the DALi programming guide  {#documentationguide}

For documentation, please follow these guidelines:

 - Create a mark down file (.md) using GitHub Flavoured Markdown https://help.github.com/articles/github-flavored-markdown/
 - Put it into the following folder: dali-toolkit/docs/content/programming-guide/
 - Include code samples for C++ in the mark down.
 - See script-overview.md overview in dali-toolkit/docs/content/programming-guide for an example
 - For DOXYGEN to link to the mark down it currently needs a reference {hash myfile}
  

## Images
 Doxygen copies all images in to the same folder as the HTML generated pages so you can just reference it as follows:

 ~~~
![ ](screen-shot.png)                  // required for Doxygen

The space between the brackets is the alternative text. This means you will never see a broken image symbol.
~~~
  
## Example

Please have a look at the numerous markdown files to see the header and footer requirements.

You can add tags to your headings as follows:
~~~{.md}
# MyChapter {#my-chapter}
~~~
Which will allow you to link to this section as follows:
~~~{.md}
[Go To MyChapter](@ref my-chapter)
~~~

Code blocks can be enclosed within 2 blocks of 3 tildes(~).

You can even specify your language type, for example:
~~~{.md}
~~~{.cpp}
...


*/
