########################################################################
#                                                                      #
# This script takes all the class*.xml, struct*.xml and namespace*.xml #
# generated & use them to generate .i files which can be added to      #
# dali.i for generation of doxygen comments for corresponding C# files #
#                                                                      #
########################################################################

#PATH="../../"
# DoxyComments for class*.xml files in the folder

cd ..
PATH_TO_I_FILES=$(pwd)
cd -

cd ../..
PATH_TO_DALII=$(pwd)
cd -

PATH_TO_SCRIPT=$(pwd)
PATH_TO_CORE_PUBLIC_APIS="/dali-core/dali/public-api/"
PATH_TO_TOOLKIT_PUBLIC_APIS="/dali-toolkit/dali-toolkit/public-api/"
PATH_TO_ADAPTOR_PUBLIC_APIS="/dali-adaptor/adaptors/public-api/adaptor-framework/"

cd ../../../../../../
ROOT_PATH=$(pwd)
cd -

CORE_PATH=$ROOT_PATH$PATH_TO_CORE_PUBLIC_APIS
TOOLKIT_PATH=$ROOT_PATH$PATH_TO_TOOLKIT_PUBLIC_APIS
ADAPTOR_PATH=$ROOT_PATH$PATH_TO_ADAPTOR_PUBLIC_APIS

echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo "Generating Doxygen for DALi-Core module"
echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"

for directory in `ls $CORE_PATH`;
do
  cp $PATH_TO_SCRIPT"/Doxyfile" $CORE_PATH$directory                               #copy doxy file to ../public/../ folder
  cd $CORE_PATH$directory
  doxygen                                                                          #run doxygen command to generate .i files
  cp $PATH_TO_SCRIPT"/doxy2swig_class.py" $CORE_PATH$directory"/xml"               #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/doxy2swig_namespace.py" $CORE_PATH$directory"/xml"           #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/doxy2swig_struct.py" $CORE_PATH$directory"/xml"              #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/genDoxy.sh" $CORE_PATH$directory"/xml"                       #copy doxy file to ../public/../ folder
  cd $CORE_PATH$directory"/xml"
  chmod 777 genDoxy.sh
  ./genDoxy.sh
  cp *.i $PATH_TO_I_FILES"/"
  rm $CORE_PATH$directory"/Doxyfile"
  rm -rf $CORE_PATH$directory"/xml"
  rm -rf $CORE_PATH$directory"/html"
  rm -rf $CORE_PATH$directory"/latex"
done

echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo "Generating Doxygen for DALi-Adaptor module"
echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"

  cd $ADAPTOR_PATH
  cp $PATH_TO_SCRIPT"/Doxyfile" $ADAPTOR_PATH                                                 #copy doxy file to ../public/../ folder
  doxygen
  cp $PATH_TO_SCRIPT"/doxy2swig_class.py" $ADAPTOR_PATH"/xml"                                 #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/doxy2swig_namespace.py" $ADAPTOR_PATH"/xml"                             #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/doxy2swig_struct.py" $ADAPTOR_PATH"/xml"                                #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/genDoxy.sh" $ADAPTOR_PATH"/xml"                                         #copy doxy file to ../public/../ folder
  cd $ADAPTOR_PATH"/xml"
  chmod 777 genDoxy.sh
  ./genDoxy.sh
  cp *.i $PATH_TO_I_FILES"/"

  rm $ADAPTOR_PATH"/Doxyfile"
  rm -rf $ADAPTOR_PATH"/xml"
  rm -rf $ADAPTOR_PATH"/html"
  rm -rf $ADAPTOR_PATH"/latex"

echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
echo "Generating Doxygen for DALi-Toolkit module"
echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"

for directory in `ls $TOOLKIT_PATH`;
do
  cd $TOOLKIT_PATH$directory
  cp $PATH_TO_SCRIPT"/Doxyfile" $TOOLKIT_PATH$directory"/"                         #copy doxy file to ../public/../ folder
  doxygen
  cp $PATH_TO_SCRIPT"/doxy2swig_class.py" $TOOLKIT_PATH$directory"/xml"            #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/doxy2swig_namespace.py" $TOOLKIT_PATH$directory"/xml"        #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/doxy2swig_struct.py" $TOOLKIT_PATH$directory"/xml"           #copy doxy file to ../public/../ folder
  cp $PATH_TO_SCRIPT"/genDoxy.sh" $TOOLKIT_PATH$directory"/xml"                    #copy doxy file to ../public/../ folder
  cd $TOOLKIT_PATH$directory"/xml"
  chmod 777 genDoxy.sh
  ./genDoxy.sh
  cp *.i $PATH_TO_I_FILES"/"

  rm $TOOLKIT_PATH$directory"/Doxyfile"
  rm -rf $TOOLKIT_PATH$directory"/xml"
  rm -rf $TOOLKIT_PATH$directory"/html"
  rm -rf $TOOLKIT_PATH$directory"/latex"
  for subdirectory in `ls $TOOLKIT_PATH$directory`;
  do
    cp $PATH_TO_SCRIPT"/Doxyfile" $TOOLKIT_PATH$directory"/"$subdirectory"/"                     #copy doxy file to ../public/../ folder
    cd $TOOLKIT_PATH$directory"/"$subdirectory                                                   #copy doxy file to ../public/../ folder
    doxygen                                                                                      #run doxygen command to generate .i files
    cp $PATH_TO_SCRIPT"/doxy2swig_class.py" $TOOLKIT_PATH$directory"/"$subdirectory"/xml"        #copy doxy file to ../public/../ folder
    cp $PATH_TO_SCRIPT"/doxy2swig_namespace.py" $TOOLKIT_PATH$directory"/"$subdirectory"/xml"    #copy doxy file to ../public/../ folder
    cp $PATH_TO_SCRIPT"/doxy2swig_struct.py" $TOOLKIT_PATH$directory"/"$subdirectory"/xml"       #copy doxy file to ../public/../ folder
    cp $PATH_TO_SCRIPT"/genDoxy.sh" $TOOLKIT_PATH$directory"/"$subdirectory"/xml"                #copy doxy file to ../public/../ folder
    cd $TOOLKIT_PATH$directory"/"$subdirectory"/xml"
    chmod 777 genDoxy.sh
    ./genDoxy.sh
    cp *.i $PATH_TO_I_FILES"/"

    rm $TOOLKIT_PATH$directory"/"$subdirectory"/Doxyfile"
    rm -rf $TOOLKIT_PATH$directory"/"$subdirectory"/xml"
    rm -rf $TOOLKIT_PATH$directory"/"$subdirectory"/html"
    rm -rf $TOOLKIT_PATH$directory"/"$subdirectory"/latex"
    for sd in `ls $TOOLKIT_PATH$directory"/"$subdirectory`;
    do
      cp $PATH_TO_SCRIPT"/Doxyfile" $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/"                     #copy doxy file to ../public/../ folder
      cd $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd                                                   #copy doxy file to ../public/../ folder
      doxygen                                                                                            #run doxygen command to generate .i files
      cp $PATH_TO_SCRIPT"/doxy2swig_class.py" $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/xml"        #copy doxy file to ../public/../ folder
      cp $PATH_TO_SCRIPT"/doxy2swig_namespace.py" $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/xml"    #copy doxy file to ../public/../ folder
      cp $PATH_TO_SCRIPT"/doxy2swig_struct.py" $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/xml"       #copy doxy file to ../public/../ folder
      cp $PATH_TO_SCRIPT"/genDoxy.sh" $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/xml"                #copy doxy file to ../public/../ folder
      cd $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/xml"
      chmod 777 genDoxy.sh
      ./genDoxy.sh
      cp *.i $PATH_TO_I_FILES"/"

      rm $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/Doxyfile"
      rm -rf $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/xml"
      rm -rf $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/html"
      rm -rf $TOOLKIT_PATH$directory"/"$subdirectory"/"$sd"/latex"
    done #sd
  done #subdirectory
done   #directory

cd $PATH_TO_I_FILES
ls *.i >> File_List.txt
awk '{print "%include doxygen/" $0}' File_List.txt  2>&1 | tee > $PATH_TO_DALII"/"doxygen.i
rm File_List.txt
cd -
