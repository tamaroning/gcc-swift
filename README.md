# gcc-swift

experimental project

## Prerequisites

- C++11 compiler

## Build

```
git clone <this repo>
cd gcc-swift
git checkout swift
./contrib/download_prerequisites
cd ..
mkdir gcc-build
cd gcc-build
../gcc-swift/configure --prefix=$(pwd)/../gcc-install --disable-bootstrap --disable-multilib --enable-languages=swift
make -j$(getconf _NPROCESSORS_ONLN)
```

<!--

## References
- A tiny GCC front end: https://thinkingeek.com/2016/01/05/tiny-gcc-front-part-1/
- GCC COBOL project: https://cobolforgcc.sourceforge.net/cobol_14.html
- GCC Frontend HOWTO: https://tldp.org/HOWTO/GCC-Frontend-HOWTO.html

-->
