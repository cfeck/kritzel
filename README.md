# Kritzel

### About

Sample Qt5 application that uses the cellwriter engine to
recognize handwritten characters.

### Training

The cellwriter application must still be used to train.
Alternatively, copy file "profile" into "~/.cellwriter"
before characters can be recognized, but this will only
work for my handwriting style.

### Building

Requirements:

* Kitware's CMake 3.0+ https://cmake.org/
* Qt Company's Qt 5.6+ https://wiki.qt.io/
* extra-cmake-modules https://github.com/KDE/extra-cmake-modules

Optional:

* PhatWare®'s WritePad Handwriting Recognition Engine https://github.com/phatware/WritePad-Handwriting-Recognition-Engine
