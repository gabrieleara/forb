# FORB

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Documentation](https://codedocs.xyz/gabrieleara/forb.svg)](https://codedocs.xyz/gabrieleara/forb/)
<!-- [![HitCount](http://hits.dwyl.io/gabrieleara/forb.svg)](http://hits.dwyl.io/gabrieleara/forb) -->

*A Fast Object Request Broker*

This is a project for the Component-Based Software Design exam
of the [Master Degree in Embedded Computing Systems](http://mecs.sssup.it)
at [Scuola Superiore Sant'Anna, Pisa](https://www.santannapisa.it/en).

The project consists in the implementation of an Object Request Broker (ORB) and Remote Procedure Call (RPC) mechanism.

A distributed program using the FORB library to perform RPC calls could gain performance improvements in certain
scenarios, without any penalization if said conditions are met.
The optimal scenario is the one in which multiple communicating components are deployed on different containers
(or virtualized environments) on the same host. This often happens in cloud environments and the common solution so far
is to use sockets to exchange data, even if the two containers are on the same host.

The approach of this library is to use a shared memory area instead if the optimal scenario is detected, which
eliminates the need of bytes marshalling/unmarshalling when serialization/deserialization of exchanged data is
performed. If said condition is not met, the library uses once again socket communication, so communication between
various hosts is not affected by the library itself.

While right now this acceleration mechanism works only if the two programs that need to communicate share an IPC
namespace, it can be further expanded in the future to adapt it to the more general scenario illustrated above.

## Project Structure
The project is build under CMake and thus it resembles common CMake projects file tree structure.

Source files are within sub-folders of the root folder, each associated with a sub-project which can be
either built separately or altogether the main project.

The folders containing sub-projects are:
- [compiler](compiler) &mdash; the compiler for the FORB IDL language, which is an *Interface Definition Language*
that resembles CORBA IDL language and that can be used to define classes that can be accessed remotely using
the FORB library RPC mechanism.
- [library](library) &mdash; the FORB library itself, it contains all source code and headers needed to build and
install the shared library under a Linux host. Public headers (which will be installed on the host include path)
are within [library/include](library/include) subfolder.
- [tests](tests) contains a few tests that can be used to understand and checkout the compiler/library combined
functionality.

Other folders are:
- [examples](examples) &mdash; contains a few examples of more structured FORB IDL files than the ones within
[tests](tests) folder.
- [docs](docs) &mdash; contains input Doxygen files, used either during CMake build or during automatic documentation
generation (see [Documentation](#documentation)).

## Building FORB Compiler and Library

Building the project requires [CMake](https://cmake.org/). It is pretty simple to build in `Release` mode as it requires only the creation
of a `build` folder within project root (to perform out-of-source build) and the execution of the following lines
within said folder:

```bash
cmake ..
make all
```

That's basically it, the build command will then generate a folder within `build` directory for each source folder
(basically [compiler](compiler), [library](library) and [tests](tests)), each containing the targets associated with
each folder.

This will build also Doxygen documentation, see [Documentation](#documentation).

## Installation

Once the project has been built using previous procedure it can be easily installed under the Linux host by
executing `make install` as superuser within `build` directory.

This command will install:
- Compiler executable `forbcc` under `/usr/bin/`
- Shared library `libforb.so` under `/usr/lib/`
- Include files under `/usr/include/forb` 
- A few shared `.cmake` files in `/usr/share/forbcc/cmake` and `usr/share/libforb/cmake` which can be used to
build other projects with CMake that use FORB-generated sources.
**A more detailed explanation will be added soon.**

Notice that is totally not necessary to install the library to build targets within [tests](tests) folder, because
it will automatically files within `build` directory to compile FORB IDL files and link each target with
the shared library, re-building needed files on-demand if necessary

## Documentation

This project uses [Doxygen](http://www.doxygen.nl/) to document code for both the compiler and the shared library.

Documentation for the project can be found online on [CodeDocs](https://codedocs.xyz/) website
[here](https://codedocs.xyz/gabrieleara/forb/). 

> For now there is no project main documentation file, but each class in compiler and library targets has been
> properly documented. Check them out by clicking on the `Classes` button or simply use
[this link](https://codedocs.xyz/gabrieleara/forb/annotated.html).

The [docs](docs) folder contains two *Doxyfile*, which will be used depending the type of build performed:
- when building using CMake in `Release` mode (in `Debug` mode Doxygen generation is disabled), the
[docs/CMakeDoxyfile.in](docs/CMakeDoxyfile.in) will be used;
- the other file, [docs/Doxyfile.in](docs/Doxyfile.in) will be used only to generate documentation using
[CodeDocs](https://codedocs.xyz/) whenever a new commit is pushed on the main branch on the GitHub repo.

