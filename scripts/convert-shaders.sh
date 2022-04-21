#!/bin/sh

BASEDIR=$(dirname $(readlink -f -- "$0"))/../
SRCFILE=$BASEDIR/src/shaders.hpp

cat <<EOF > $SRCFILE
#include <string>

namespace resources {
EOF

echo -e -n 'const std::string SHADER_VS = R"(\n' >> $SRCFILE
cat $BASEDIR/resources/shader.vs >> $SRCFILE
echo -e -n ')";\n\n' >> $SRCFILE

echo -e -n 'const std::string SHADER_FS = R"(\n' >> $SRCFILE
cat $BASEDIR/resources/shader.fs >> $SRCFILE
echo -e -n ')";\n\n' >> $SRCFILE

echo -e -n '}\n' >> $SRCFILE
