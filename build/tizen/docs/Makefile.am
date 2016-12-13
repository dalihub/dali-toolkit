all-local:
	@rm -f doxygen-errors.txt
	@-doxygen -u dali.doxy
	@-doxygen dali.doxy &> doxygen-errors.txt || rm doxygen-errors.txt
	@touch doxygen-errors.txt
	@cat doxygen-errors.txt
	@if [ -s doxygen-errors.txt ]; then exit 1 ; fi
	@rm doxygen-errors.txt
# Build JavaScript API documentation using yuidoc ( Yahoo JavaScript Documentation Tool )
# We first test yuidoc exists, then change in to the dali-script-v8 docs directory and run yuidoc
	@if [ ! `which yuidoc` = "" ]; then cd ../../../plugins/dali-script-v8/docs && yuidoc --config yuidoc.json -e ".cpp,.js,.md"  -o generated .. ../../../docs/content/shared-javascript-and-cpp-documentation/ ; fi
