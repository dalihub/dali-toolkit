Deep internal documentation for Dali Toolkit.

Generate the documentation here as follows:

    # Go to dali-toolkit/build/tizen/docs-internal/
    cd ../../build/tizen/docs-internal/
    # Start doxygen:
    ./build.sh

Note, the doxfile assumes you have your Dali repositories checked-out
side by side with the following structure:

    .
    ├── dali
    ├── dali-adaptor
    └── dali-toolkit

If you have used different names or structure, you will need to adjust
the value of the `INPUT` variable in the doxyfile:

   build/tizen/docs-internal/dali_internal.doxy




