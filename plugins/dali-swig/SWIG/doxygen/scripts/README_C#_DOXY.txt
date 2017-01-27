README
=============================

To run the script and get Doxygen Documentation for a .cs file , do the following:

1.  Run GenerateIntermediate script and generate .i files.

      $user@user:~/../../dali-toolkit/plugins/dali-swig/SWIG/doxygen/scripts$ chmod 777 GenerateIntermediate.sh
      $user@user:~/../../dali-toolkit/plugins/dali-swig/SWIG/doxygen/scripts$ ./GenerateIntermediate.sh

2.  Intermediate .i files are copied to ~/../../dali-toolkit/plugins/dali-swig/SWIG/doxygen/

3.  Run make to generate the .cs file with documentation.

      $user@user:~/../../dali-toolkit/plugins/dali-swig/ ./autogen.sh && ./configure &&  make clean && make && make check

