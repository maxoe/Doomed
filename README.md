# Earthbender
A game about weird geometry and exotic physics.

# How to Build

1. install `vcpkg`
2. install with `vcpkg install @vcpkg_reponse_file.txt`
3. `cmake -Bbuild`

If it did not work, it may be because the default is to try the x86 packge versions. Try to set the system environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows` or install the x64 versions manually using `:x64-windows` suffixes (if you're using Windows of course) 

4. `cmake --build build`