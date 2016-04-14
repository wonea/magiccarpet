# Introduction #
This wiki helps you to find your way around in this Magic Carpet Fan Project. You can learn about checking out the source code from SVN, downloading the installer, watch screenshots, how to contribute, read the documentation and you can follow important links.

# Download #
There are no downloads yet

# Screenshots #
There are no screenshots yet

# Contribute #
  * We need Milkshape 3D models for monsters from the game. The game has been tested to load models created with Milkshape 3D 1.8.2 but other models made with other Versions of Milkshape 3D are welcome also.

# Credits #
  * Song Ho Ahn http://www.songho.ca/ for its very good and very nicely working OpenGL Win32 Api Gui article and source code
  * Brett Porter http://web.archive.org/web/20010408222059/http://rsn.gamedev.net/tutorials/ms3danim.asp for its good explanation of milkshape skeletal animation
  * Bryan Turner http://www.gamasutra.com/features/20000403/turner_01.htm for his article and source code about his semi implementation (only does splits and no merges) of the roam algorithm

# Documentation #

# SVN #
## What is SVN ##
Source code and content of computer programs are stored in a so called repository, which automatically detects changes and performs versioning on different versions of the source files and the content files.

The repository is used to store sources and means to compile the project. It does not store binary data created from the sources! For example, the svn stores the header files, source files and the makefiles or project and solution files of the C++ project. It does not store the compiled executable.

The repository is used for work on the sources. If you want to play, you can download the installer and you do not need to work with the repository.

The SVN software is used to maintain the repository and is running on one of the google servers hosting this project. You need a SVN client to access the SVN server. In a Windows environment, Tortoise SVN is used most often. All further descriptions use Tortoise SVN under Windows.

## HOWTO: Check out the repository ##
Download and install Tortoise SVN. Create a folder on your harddrive. The folder name does not play any role so lets call it magiccarpet. Open the contex menu on the magiccarpet folder and select SVN Checkout.

Use https://magiccarpet.googlecode.com/svn/trunk/ as the server to check out from and click the OK Button. A Dialog pops up requesting your credentials. Leave them blank for checking out anonymously or enter your credentials if you are contributing changes to the source code.

Your SVN client will now transfer the complete project from the SVN server to the magiccarpet folder. You can then compile the project.

# Compiling #
Download log4cplus from http://sourceforge.net/projects/log4cplus/. Extract the archive and use Visual Studio to load the log4cplus.sln Solution stored in log4cplus-1.0.3-rc1\log4cplus-1.0.3-rc1\msvc8. Compile the static project called log4cplus\_static. In the Debug\_Unicode/Static folder of the project, you will find log4cplusSD.lib after succesfull compilation. Copy this file to C:\Program Files\Microsoft Visual Studio 9.0\VC\lib (Adjust the path to meet your environment). Copy the folder log4cplus-1.0.3-rc1\log4cplus-1.0.3-rc1\include\log4cplus to C:\Program Files\Microsoft Visual Studio 9.0\VC\include (Adjust the path to meet your environment). For compiling and using libjpeg see <a href='http://w3studi.informatik.uni-stuttgart.de/~bischowg/languages/C++/CPPlibjpeg.html'><a href='http://w3studi.informatik.uni-stuttgart.de/~bischowg/languages/C++/CPPlibjpeg.html'>http://w3studi.informatik.uni-stuttgart.de/~bischowg/languages/C++/CPPlibjpeg.html</a></a>

# Compilation Errors #
## Cannot find manifest file ##
Stupid Solution: Copy glWin.exe.intermediate.manifest from the Debug folder one folder up into the glWin folder and rename it to glWin.exe.manifest.

Better Solution: Mark the project glWin > Open contextmenu > properties > configuration properties > Linker > Manifest File > Manifest File > `$(IntDir)\..\$(TargetFileName).manifest`