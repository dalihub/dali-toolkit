#!/bin/bash

d=./dali-toolkit/dali-toolkit/devel-api/graphics/shaders/
outdir=./dali-toolkit/dali-toolkit/devel-api/graphics/generated

if [ ! -e $d ] ; then
    echo "Error! "$d" not found! This script must be launched from top-level DALi project directory!"
    exit 0
fi

compiler=
if [ -e ./dali-env/vulkan/latest/x86_64/bin/glslangValidator ] ; then
    compiler=./dali-env/vulkan/latest/x86_64/bin/glslangValidator
else
    compiler=$(which glslangValidator)
fi

if [[ $compiler = "" ]] ; then
    echo "Erorr! Cannot locate glslangValidator! Make sure Vulkan SDK binaries are added to the PATH environment variable!"
    exit 0
fi

echo "GLSL Compiler: $compiler"

cd $d
all_shaders=$(ls -1 *.{vert,frag})
cd $OLDPWD

for name in $all_shaders ; do
  echo "Compiling $name..."
  varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')  
  if ( ! $compiler -V1.0 $d/$name --vn $varname -o /tmp/spv.h ) ; then
    echo "Error compiling $name!"
    exit 0
  fi

  newname=$(echo ${name} | sed -e 's/\./-/;')".h"
  echo Writing $newname

  cat /tmp/spv.h | sed -e 's/const uint32_t /std::vector<uint32_t> /g;s/\[\]//g;' | tail -n +3 > $outdir/$newname
  echo >> $outdir/$newname
done

echo "Generating cpp file..."
echo "#include <cstdint>" > $outdir/builtin-shader-gen.cpp
echo "#include <vector>" >> $outdir/builtin-shader-gen.cpp
echo "#include <string>" >> $outdir/builtin-shader-gen.cpp
echo -e "#include <map>\n" >> $outdir/builtin-shader-gen.cpp

varnames=
for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    varnames="${varnames} $varname"
    echo "#include \"$newname\"" >> $outdir/builtin-shader-gen.cpp
done

echo "static std::map<std::string, std::vector<uint32_t>> gGraphicsBuiltinShader = {" >> $outdir/builtin-shader-gen.cpp
for var in ${varnames} ; do
    echo "  { \"$var\", $var }," >> $outdir/builtin-shader-gen.cpp
done
echo -e "};\n" >> $outdir/builtin-shader-gen.cpp

# generate graphics shader getter function
cat >> $outdir/builtin-shader-gen.cpp << EOF
extern "C" {

#define IMPORT_API __attribute__ ((visibility ("default")))

IMPORT_API std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag );

std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag )
{
    auto iter = gGraphicsBuiltinShader.find( tag );
    if( iter != gGraphicsBuiltinShader.end() )
    {
        return iter->second;
    }
    return {};
}

}
EOF

echo "Generating extern header file ( for external use )..."
echo "#ifndef GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" > $outdir/../builtin-shader-extern-gen.h
echo "#define GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" >> $outdir/../builtin-shader-extern-gen.h
echo "#include <cstdint>" >> $outdir/../builtin-shader-extern-gen.h
echo "#include <vector>" >> $outdir/../builtin-shader-extern-gen.h
echo "#include <string>" >> $outdir/../builtin-shader-extern-gen.h
echo "" >> $outdir/../builtin-shader-extern-gen.h

for name in $all_shaders ; do
    varname=$(echo "SHADER_$name" | tr [a-z] [A-Z] | sed -e 's/-/_/g;s/\./_/g;')
    newname=$(echo ${name} | sed -e 's/\./-/;')".h"
    echo "extern std::vector<uint32_t> $varname;" >> $outdir/../builtin-shader-extern-gen.h
done
cat >> $outdir/../builtin-shader-extern-gen.h << EOF
extern "C" std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag );
#define GraphicsGetBuiltinShaderId( x ) GraphicsGetBuiltinShader( #x )
EOF
echo "#endif // GRAPHICS_BUILTIN_SHADER_EXTERN_GEN_H" >> $outdir/../builtin-shader-extern-gen.h
