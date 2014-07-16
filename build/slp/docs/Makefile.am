all-local:
	@rm -f doxygen-errors.txt
	@-doxygen dali.doxy &> doxygen-errors.txt || rm doxygen-errors.txt
	@touch doxygen-errors.txt
	@cat doxygen-errors.txt
	@if [ -s doxygen-errors.txt ]; then exit 1 ; fi
	@rm doxygen-errors.txt
