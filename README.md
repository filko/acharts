# acharts

Astronomical Charts renders the sky into a map.  It produces a vector
image in a SVG format.  It supports drawing planets in the solar
system, grids, horizon, constellations, and objects loaded from
standard catalogues.

## Dependencies

- GNU autotools (autoconf, automake, make).
- Modern C++ toolchain with good C++11 support.
- libnova.
- Boost header only libraries (spirit, algorithm; acharts does not link to any boost library).
- libz.

###### Optional dependencies

- asciidoc and xmlto (for manpage generation).

## Building

acharts is a classic autotools project, so after installing dependencies:

```
./autogen.bash && mkdir build && cd build && ../configure && make
```

should produce an acharts binary.  `make install` is not necessary as
acharts can be run from build directory.

## Getting started

Example configuration uses Yale Bright Star Catalogue which is not
shipped with acharts and can be obtained at
[Vizier](http://cdsarc.u-strasbg.fr/viz-bin/Cat?V/50).  File to
download is named "catalog.gz".  (Vizier happens to be a very good
repository of catalogues that can be used by acharts.)

After obtaining the catalogue, an example configuration can be tested
by using `./build/src/acharts examples/genesis.txt`.  Further
documentation is available as a manpage at `./build/doc/acharts.1`.

acharts only generates geometry of the map, annotated with relevant
class and id attributes.  Those attributes can be referenced in
Cascading Style Sheet that is embedded in the output, giving
flexibility of usage.
