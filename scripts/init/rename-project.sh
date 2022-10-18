#!/bin/bash

echo $PWD

name="$PWD/scripts/init/project-name"

# Take the search string
search=$(cat $name)

# Take the replace string
read -p "Enter new project($search) name: " replace

if [[ $replace != "" ]]; then
find $PWD -type f -iname \*\.cpp -o -iname \*\.hpp -o -iname \*\.c -o -iname \*\.h \
-o -iname CMakeLists.txt -o -iname doxygen.conf -o -iname project.4coder | \
xargs grep -l "$search" | \
xargs sed -i "s/$search/$replace/g" &&
echo "$replace" > $name
fi

