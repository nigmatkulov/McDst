# McDst

Data format (DST) for a Monte Carlo event generator output using ROOT package.
The McDst set of classes (that can be compiled to the library) allows to store and work with *filename.mcDst.root* files or with a list (*filename.lis* or *filename.list*) of such files.

Brief description of how to compile and run the analysis over mcDst on your machine.

## Installation

a) System has to have ROOT preinstalled (should work with versions 5 and 6).

b) There is a Makefile stored in the **_mcdst_** directory. In order to compile the codes one needs to run:

```
make
```

c) Since it is the most commonly used, the **g++** compiler is used for the compilation. However, one can also switch it to **clang++** (has been successfully tested)

d) After the compilation is finished the shared library *libMcDst.so* will be created.

e) If you have some errors during the compilation please contact the McDst maintainer.

## Processing mcDst

There are three ROOT macroses with example of how to perform a simple analysis using McDst. They are stored in the **mcdst/macros** directory and called: *analyseMcDst.C*, *runProcessing.C*, and *analyseWithBranches.C*.

Lets assume that one has and input file(s) *InputFile* with a *name.mcDst.root* or a list of mcDst files, called *name.lis(t)*, and McDst is compiled (i.e. the *libMcDst.so* library exists).

There are 2 possible processing scenarios of using McDst library and classes depending on the ROOT version:

### ROOT 5:

One should run ROOT from the terminal:

```
[myterm]> root runProcessing.C\(\"InputFile\",\"oFileName\"\)
```

Aforementioned will load *libMcDst.so*, compile and run *runProcessing.C*. After the processing will be finished, it will remove dictionary and library created by ACLiC.

### ROOT 6:

Since ROOT 6 does not have CINT there is some extra flexibility on how to analyze the data. The first one is listed above. The second option is to run the *analyseMcDst.C* macro directly.

Either from the terminal:

```
root root runProcessing.C\(\"InputFile\",\"oFileName\"\)
```

Or from the root session:

```
[myterm]> root
root [0].x root runProcessing.C("InputFile","oFileName")
```

### Simple Processing

The other possibility is not to use **McDst** classes, but read *filename.mcDst.root* files as regular ROOT TTree. The macros *analyseWithBranches.C* shows an example of doing it.

## Conversion to McDst

There are various MC generators used in the field. To convert their output one can convert it to the McDst format via special tiny C++ programs (e.g., urqmd2mc.cpp). To do it, one needs to compile an executable file:

```
make converters
```

To run the converter, the MCDST environment variable should be added to PATH and LD_LIBRARY_PATH. One can do it either by calling a shell script (mcdst_environment.bash) which will define the variable for the current session:

```
source mcdst_environment.sh
```

or by hands to the environment.

After the environment variable is added one can run the converter.

## Troubleshooting

For any questions or with any suggestions please contact the package maintainer.
