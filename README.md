# Doomed
A game about a maze that does not follow the rules of your mind.

# How to Build

1. Install [vcpkg](https://github.com/microsoft/vcpkg) using `git clone https://github.com/microsoft/vcpkg` and `.\vcpkg\bootstrap-vcpkg.bat` or `./vcpkg/bootstrap-vcpkg.sh`
2. Install packages with `vcpkg install @vcpkg_reponse_file.txt`
3. `vcpkg integrate install`
4. Set the `VCPKG_INSTALLATION_ROOT` environment variable to the vcpkg directory.
5. `cmake -Bbuild`

If it failed and you are on Windows, try to set the system environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows`. On Windows, vcpkg defaults to the x86 package versions. You can also install the x64 versions manually using `:x64-windows` suffixes.
If it still didn't work, try to execute `cmake ..` from an empty build directory. This seems to fix some problems and afterwards you can execute it normally.

6. `cmake --build build` or on Windows open the `.sln` file within Visual Studio, set the project as startup project and build