========================================================================
    STATIC LIBRARY : wblib Project Overview
========================================================================

AppWizard has created this wblib library project for you. 

No source files were created as part of your project.


wblib.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

This library has been created in Visual Studio 2005. It has been created by
File > New > Project > Win32 > Win32 Project > Static Library without Precompiled 
headers. A static libraries code is fully linked into the executable of a program.
If several programs are started, the code of a static library is loaded into the
memory for each single instance or program which has been compiled using the
static library. This is redundant memory usage. A dynamic libraries cdoe is not 
compiled into the program. In addition to the .lib file, a .dll file is created,
which contains the real code and is stored in a systemwide folder. If a program
needs the dynamic libraries code, it is loaded and reused by any subsequent program.
This saves space. This library is static though.

Usage:

Compile the library. wbLib.lib is created. The header files are already existing,
as they are contained in the wbLib code and are needed to build wbLib.lib.

To use the library, you need to link the wbLib.lib file to your projects executable
and you need to use the header files of wbLib inside your project. Otherwise the
compile knows nothing about the classes that wbLib provides.

Copy the header files to the some folder in the project you
want to use wbLib in. You could create a folder lib and a folder wbLib inside
lib and put all headers there. In your project in visual studio, 
add the header files of wbLib to your project. (For better organisation, you 
can create new filters lib and inside the lib filter, 
you can create a filter wbLib. Add the headers to those filters). 

The compiler looks for wbLib.lib in the projects source folder and in all folders specified
via Tools > Options > Projects and Solutions > VC++ Directories. This means you can either
put the lib in the src directory which clutters up your folder structure, or you can 
include your projects lib/wbLib folder to the entire Visual Studio setUp which makes no sense,
as the project folder might be deleted sometime but your Visual Studio installation might be
needed even after your project has been deleted.

There is a better way to link wbLib.lib. By using the #pragma comment (lib, "name"), you can
link the library from any folder. Write the pragma into main.cpp and use it as follows:
#pragma comment (lib, "lib/wbLib/wbLib.lib"). (Remove projects context menu > 
properties > Configuration Properties > linker > input > additional dependencies > wbLib.lib 
if you want to use the pragma)

If you want to use the src folder to contain wbLib.lib or if you want to put
wbLib.lib in a lib folder specified in visual studio, open your projects context menu > 
properties > Configuration Properties > linker > input > additional dependencies >
add wbLib.lib. The linker will search all lib folders for wbLib.lib and it will use the
first one it finds. The position of the folders in the list give the sequence in which 
the folders are searched by the linker. The topmost folder is searched first.

Use the #include "lib/wbLib/HeightMapLoader.h" statement and create a variable
from a class in wbLib. Do not forget to either use using namespace wbLib or
use the wbLib:: namespace qualifier.

compile your project.

Code StyleGuide:
Every header #ifndef _LOADER starts with an underscore to prevent name clashes

Header and source files are camel-cased and start with a uppercase letter

Classes are camel-cased and start with a uppercase letter

Methods start with a lowercase letter

One tab equals two space characters

Methods are never declared in header files but always in source files

Namespace names are camel-cased and start with a lowercase letter

Comments are written in the cpp file and not in the header file

Parameters of methods and functions start with an underscore to distinguish
them from local variables. After the leading underscore, they are camel-cased
and begin with a lower-case letter

The are no spaces between meathod or function names and the opening bracket of the
parameter list. There are no spaces between the opening bracket of the parameter
list and the first parameter. There is one space after each comma in the parameterlist.
There is no space between the closing bracket of the parameter list. Nested function
calls are not separete by spaces from trailing brackets

Operators are surrounded by spaces

Everything is written in english language. English as well as american englisch is allowed.