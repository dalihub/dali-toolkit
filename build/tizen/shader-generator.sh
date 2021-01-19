#!/bin/bash

indir=$1
outdir=$2

mkdir -p $outdir

if [ ! -e $indir ] ; then
    echo "Error! "$indir" not found!"
    exit 0
fi

cd $indir
all_shaders=$(ls -1 *.{vert,frag,def})
cd $OLDPWD

# Generate one header file per shader which is defined as a const std::string_view
for name in $all_shaders ; do
  echo "Generating header files for $name..."
  varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')

  newname=$(echo ${name} | sed -e 's/\./-/;')".h"
  echo Writing $newname

  shader_fullpath=$(echo ${indir})$name

  header_name="${varname}_GEN_H"
  echo "const std::string_view" "$varname""{" > $outdir/$newname
  cat $shader_fullpath | sed -e 's/^..*$/"&\\n"/' >> $outdir/$newname
  echo "};" >> $outdir/$newname
done

# Generate one cpp file that includes all the previously generated string_views for shaders
echo "Generating cpp file..."
echo -e "#include \"../builtin-shader-extern-gen.h\"\n" > $outdir/builtin-shader-gen.cpp

varnames=
for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    varnames="${varnames} $varname"
    echo "#include \"$newname\"" >> $outdir/builtin-shader-gen.cpp
done

# Generate one header file that defines all the shader string_views as extern variables
echo "Generating extern header file ( for external use )..."
echo "#ifndef GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" > $outdir/../builtin-shader-extern-gen.h
echo -e "#define GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H\n" >> $outdir/../builtin-shader-extern-gen.h

echo "#include <string_view>" >> $outdir/../builtin-shader-extern-gen.h
echo "" >> $outdir/../builtin-shader-extern-gen.h

for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    echo "extern const std::string_view $varname;" >> $outdir/../builtin-shader-extern-gen.h
done
cat >> $outdir/../builtin-shader-extern-gen.h << EOF

#endif // GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H
EOF

