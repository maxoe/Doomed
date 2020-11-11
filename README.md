# Doomed
A game about a maze that does not follow the rules of your mind.

# How to Build

1. install `vcpkg`
2. install with `vcpkg install @vcpkg_reponse_file.txt`
3. Use `vcpkg integrate install` to setup vcpkg for use in e.g. Visual Studio
4. `cmake -Bbuild`

If it did not work, you might have to set the environment variable `VCPKG_INSTALLATION_ROOT` manually to the vcpkg directory. If this does not help and you are on Windows, try to set the system environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows` or install the x64 versions manually using `:x64-windows` suffixes. On windows, vcpkg defaults the x86 package versions.
If it still didn't work, try to execute `cmake -Bbuild ..` from an empty build directory. This seems to fix some problems and afterwards you can execute it normally.

5. `cmake --build build` or on Windows open the `.sln` file within Visual Studio, set the project as startup project and build