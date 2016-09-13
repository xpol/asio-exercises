# asio-exercises

Personal exercises for asio library.

## Environments

For macOS:

```bash
brew install asio
cmake -H. -Bbuild -GXcode
```

The Xcode project files will located in build directory.

For Windows:

Install [boost binaries](https://sourceforge.net/projects/boost/files/boost-binaries/) (use 64 bit version).
Assume boost installed in C:/local/boost_1_61_0

```batch
cmake -H. -Bbuild -G"Visual Studio 14 2015 Win64" -DBOOST_ROOT=C:/local/boost_1_61_0 -DBOOST_ROOT=C:/local/boost_1_61_0/lib64-msvc-14.0
```

The Visual Studio project files will located in build directory.
