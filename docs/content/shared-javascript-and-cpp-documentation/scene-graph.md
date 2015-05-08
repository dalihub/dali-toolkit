# Scene Graph
## What is a scene graph?
From wikipedia...
  
A scene graph is a collection of nodes in a graph or tree structure.
A node may have many children but often only a single parent,
with the effect of a parent applied to all its child nodes;
an operation performed on a group automatically propagates
its effect to all of its members. In many programs, associating
a geometrical transformation matrix (see also transformation and matrix)
at each group level and concatenating such matrices together is an
efficient and natural way to process such operations. A common feature,
for instance, is the ability to group related shapes/objects into a
compound object that can then be moved, transformed, selected,
etc. as easily as a single object.

 ### How does this relate to the Dali public API?

 Actors are effectively nodes that receive input (touch events) and act as a
 container for draw-able elements (which are also nodes) and other actors.