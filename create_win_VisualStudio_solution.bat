mkdir buildVS2022_64_v141
cd buildVS2022_64_v141
cmake.exe -G "Visual Studio 17 2022"  -A x64 -D USE_OSG=True ..
cd..