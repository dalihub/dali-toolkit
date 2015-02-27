// Follow instructions on the internets on how to install yuidoc
// Then run this command

yuidoc --config yuidoc.json -e ".cpp,.js,.md"  -o generated .. ../../../docs/content/shared-javascript-and-cpp-documentation/

// -e says what extensions to pass, we have comments in js and cpp files
// -o specifies output directory ( in this case we output to a folder called generated)
// The list of directories after the generated, is the folders to scan for documentation

To view the output
firefox generated/index.html &
