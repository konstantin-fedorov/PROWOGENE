mkdir package
cd package

mkdir src
echo d | xcopy /s  ..\console          src\console
echo d | xcopy /s  ..\core             src\core
echo d | xcopy /s  ..\tools_blender    src\tools_blender
echo d | xcopy /s  ..\tutorial         src\tutorial
echo f | xcopy     ..\CMakeLists.txt   src\CMakeLists.txt
echo f | xcopy     ..\doxygen.ini      src\doxygen.ini

cd src
doxygen doxygen.ini
rem out of src
cd ..
mkdir doc
echo d | xcopy /s  src\doc doc
rmdir /s /q src\doc

mkdir win_x64
cd win_x64

mkdir build
cd build
cmake -A x64 ../../src
cmake --build . --config Release
rem out of build
cd ..

echo f | xcopy /s build\console\Release\prowogene_console.exe prowogene_console.exe
echo f | xcopy /s build\core\Release\prowogene_core.lib prowogene_core.lib
rmdir /s /q build

rem out of win_x64
cd ..

echo d | xcopy /s ..\config config

rem out of package
cd ..
