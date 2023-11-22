# gcc-swift

experimental project

## Prerequisites

- C++11 compiler

## Build

At first clone this repository:
```
git clone <this repo>
cd gcc-swift
git checkout swift
./contrib/download_prerequisites
cd ..
```

Then, build gcc-swift
```
mkdir gcc-build
cd gcc-build
../gcc-swift/configure --prefix=$(pwd)/../gcc-install --disable-bootstrap --disable-multilib --enable-languages=swift
make -j$(getconf _NPROCESSORS_ONLN)
```

Install gcc-swift
```
make install
```

## Try gcc-swift


```
$ echo "if false {}" > a.swift
$ ./gcc-install/bin/gccswift -c a.swift
<id=IF, text=, a.swift, line=1, col=1>
<id=IDENTIFIER, text=false, a.swift, line=1, col=4>
<id=LEFT_CURLY, text=, a.swift, line=1, col=10>
<id=RIGHT_CURLY, text=, a.swift, line=3, col=1>
<id=END_OF_FILE, text=, a.swift, line=4, col=1>
```

<!--

## References
- A tiny GCC front end: https://thinkingeek.com/2016/01/05/tiny-gcc-front-part-1/
- GCC COBOL project: https://cobolforgcc.sourceforge.net/cobol_14.html
- GCC Frontend HOWTO: https://tldp.org/HOWTO/GCC-Frontend-HOWTO.html

-->
