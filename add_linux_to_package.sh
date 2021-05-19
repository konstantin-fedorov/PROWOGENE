cd package
mkdir linux_x64
cd linux_x64

mkdir build
cd build
cmake ../../src
cmake --build . --config Release
# out of build
cd ..

cp build/console/prowogene_console prowogene_console
cp build/core/libprowogene_core.a libprowogene_core.a
rm -r build

# out of linux_x64
cd ..

# out of package
cd ..
