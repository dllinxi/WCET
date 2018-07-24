echo "cd gel"
cd gel
make clean
./clean
echo "cmake . -DCMAKE_INSTALL_PREFIX=/usr/local"
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Debug
echo "make install"
sudo make install
echo "cd .."
cd ..

echo "cd gliss2"
cd gliss2
make clean
echo "make"
make
echo "cd .."
cd ..

echo "cd ppc2"
cd ppc2
make clean
echo "make WITH_DYNLIB=1"
make WITH_DYNLIB=1
echo "cd .."
cd ..

echo "cd lp_solve5/lpsolve55"
cd lp_solve5/lpsolve55
echo "sh ccc.linux"
sh ccc.linux
echo "cd ../.."
cd ../..

echo "cd elm"
cd elm
make clean
./clean
echo "cmake . -DCMAKE_INSTALL_PREFIX=/usr/local"
sudo cmake . -DCMAKE_INSTALL_PREFIX=/usr/local
echo "make install"
sudo make install
echo "cd .."
cd ..

echo "cd thot"
cd thot
make clean
echo "make all"
make all
echo "cd .."
cd ..

echo "cd otawa"
cd otawa
sudo make clean
./clean
echo "cmake . -DCMAKE_INSTALL_PREFIX=/usr/local"
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local 
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Debug   (another choice)
echo "make install"
sudo make install
echo "cd .."
cd ..

echo "cd frontc"
cd frontc
make clean
echo "make NATIVE=1"
make NATIVE=1
echo "cd .."
cd ..

echo "cd orange"
cd orange
make clean
echo "make install PREFIX=/usr/local/ NATIVE=1"
sudo make install PREFIX=/usr/local NATIVE=1
echo "cd .."
cd ..

