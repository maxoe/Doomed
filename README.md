# Doomed
A game about a maze that does not follow the rules of your mind.

# How to Build

1. Install [vcpkg](https://github.com/microsoft/vcpkg) using `git clone https://github.com/microsoft/vcpkg` and `.\vcpkg\bootstrap-vcpkg.bat` or `./vcpkg/bootstrap-vcpkg.sh`
2. Install packages with `vcpkg install @vcpkg_reponse_file.txt`
3. `vcpkg integrate install`
4. Set the `VCPKG_INSTALLATION_ROOT` environment variable to the vcpkg directory.
5. `cmake -Bbuild`

If it failed and you are on Windows, try to set the system environment variable `VCPKG_DEFAULT_TRIPLET=x64-windows`. On Windows, vcpkg defaults to the x86 package versions. You can also install the x64 versions manually using `:x64-windows` suffixes.

6. `cmake --build build` or on Windows open the `.sln` file within Visual Studio, set the project as startup project and build

# A Note to VisualStudio

- You might have to setup the include and source directories properly by adding them manually in `Project --> Properties --> VC++ Directories`

- If you receive an error like `The command 'setlocal' failed` then try to delete the `ìnclude` and `resource` hardlinks in the build directory and try to _rebuild_ the project in Visual Studio.