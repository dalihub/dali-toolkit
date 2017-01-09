########################################################################
#                                                                      #
# This script takes all the class*.xml, struct*.xml and namespace*.xml #
# generated & use them to generate .i files which can be added to      #
# dali.i for generation of doxygen comments for corresponding C# files #
#                                                                      #
########################################################################

#PATH="../../"
# DoxyComments for class*.xml files in the folder
for file in class*.xml; do
  suffix=".xml"
  python doxy2swig_class.py "$file" "${file%$suffix}.i" > temp_version;
done


# DoxyComments for struct*.xml files in the folder
for file in struct*.xml; do
  suffix=".xml"
  python doxy2swig_struct.py "$file" "${file%$suffix}.i" > temp_version;
done


# DoxyComments for namespace*.xml files in the folder
for file in namespace*.xml; do
  suffix=".xml"
  python doxy2swig_namespace.py "$file" "${file%$suffix}.i" > temp_version;
done
