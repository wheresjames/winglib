
cd ..

mkdir %1
md %1

cp cmake/CMakeLists.txt %1/CMakeLists.txt
copy cmake/CMakeLists.txt %1/CMakeLists.txt

cd %1
cmake -G %2
