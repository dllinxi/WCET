echo "hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gel/trunk gel"
hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gel/trunk gel
echo "cd gel"
cd gel
echo "cmake . -DCMAKE_INSTALL_PREFIX=/usr/local"
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local
echo "make install"
make install
echo "cd .."
cd ..
echo "hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gliss2/trunk gliss2"
hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/gliss2/trunk gliss2
echo "cd gliss2"
cd gliss2
echo "make"
make
echo "cd .."
cd ..
echo "hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/ppc2/trunk ppc2"
hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/ppc2/trunk ppc2
echo "cd ppc2"
cd ppc2
echo "make WITH_DYNLIB=1"
make WITH_DYNLIB=1
echo "cd .."
cd ..
echo "cd lp_solve5/lpsolve55"
cd lp_solve5/lpsolve55
echo "sh ccc.osx"
sh ccc.osx
echo "cd ../.."
cd ../..
echo "hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/elm/trunk elm"
hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/elm/trunk elm
echo "cd elm"
cd elm
echo "cmake . -DCMAKE_INSTALL_PREFIX=/usr/local"
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local
echo "make install"
make install
echo "cd .."
cd ..
echo "git clone https://github.com/hcasse/Thot.git thot"
git clone https://github.com/hcasse/Thot.git thot
echo "cd thot"
cd thot
echo "make all"
make all
echo "cd .."
cd ..
echo "hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa/trunk otawa"
hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/otawa/trunk otawa
echo "cd otawa"
cd otawa
echo "cmake . -DCMAKE_INSTALL_PREFIX=/usr/local"
cmake . -DCMAKE_INSTALL_PREFIX=/usr/local
echo "make install"
make install
echo "cd .."
cd ..
echo "hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/frontc/trunk frontc"
hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/frontc/trunk frontc
echo "cd frontc"
cd frontc
echo "make NATIVE=1"
make NATIVE=1
echo "cd .."
cd ..
echo "hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/orange/trunk orange"
hg clone https://anon:ok@wwwsecu.irit.fr/hg/TRACES/orange/trunk orange
echo "cd orange"
cd orange
echo "make install PREFIX=/usr/local/ NATIVE=1"
make install PREFIX=/usr/local NATIVE=1
echo "cd .."
cd ..

