
## How to install MoonGLMATH

### On Ubuntu

Install Lua as described in the installation instructions for 
[MoonFLTK](https://github.com/stetre/moonfltk/).

Download MoonGLMATH's [latest release](https://github.com/stetre/moonglmath/releases)
(e.g. moonglmath-0.1.tar.gz).

```sh
$ tar -zxpvf moonglmath-0.1.tar.gz
$ cd moonglmath-0.1
moonglmath-0.1$ make
moonglmath-0.1$ sudo make install
```

### On Windows with MSYS2/MinGW

Set up the MSYS2/MinGW environment and install Lua as described in the installation 
instructions for [MoonFLTK](https://github.com/stetre/moonfltk/).

Download MoonGLMATH's [latest release](https://github.com/stetre/moonglmath/releases)
(e.g. moonglmath-0.1.tar.gz).

```sh
$ tar -zxpvf moonglmath-0.1.tar.gz
$ cd moonglmath-0.1
moonglmath-0.1$ make platform=mingw
moonglmath-0.1$ make install platform=mingw
```

