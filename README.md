# Simulating the sliding of crosslinked filaments

## Summary

CrossLink is a program for simulating crosslinking proteins between two rigid filaments, like microtubules.
One microtubule is considered to be stationary, while another is mobile. Then, crosslinkers can stretch, (un)bind,
and diffuse between the microtubules. We use this dynamics to study the movement of the mobile filament,
which can move under influence of passive diffusion or external forces, and to quantify the friction between
the filaments caused by the crosslinkers.

## Compilation

I have created the executables using CodeBlocks and g++. Make sure to run "addGitHash.sh" before every compile (include it in the build process).
When compiling on a system where git is not available, this should first be broken out of the Log class.
In Code::Blocks, do this automatically before each compilation by adding "./addGitHash.sh" to the Pre-build steps in Project->Build Options->Pre/post build steps.
Then, if the git hash has changed before compilation, GitHashWhenCompiled.hpp will change. Since Code::Blocks automatically makes
Log.cpp depend on GitHashWhenCompiled.hpp, a recompile of Log will happen and the new hash is added.

To create a debug version of the code, define MYDEBUG as a variable. This is done in g++ by adding the "-DDEBUG" flag.
In Code::Blocks, go to Project->Build Options->Debug->Compiler Settings-> #defines, and add MYDEBUG as a variable.

The project uses the GNU Scientific Library (GSL) for some special functions. For this, make sure that the GSL is installed,
and that the compiler and linker know where to find it.
If the libraries are installed in the default location, finding them should be automatic.
The linker needs the options -lgsl -lgslcblas -lm, which can be added in Code::Blocks by adding gsl, gslcblas and m to:
Project->Build Options->CrossLink->linker settings-> add

For creating graphics, the program uses the Simple and Fast Multimedia Library (SFML), which uses OpenGL.
The linker needs the options -lsfml-graphics -lsfml-window -lsfml-system, which can be added in
Code::Blocks by adding sfml-graphics sfml-window sfml-system to:
Project->Build Options->CrossLink->linker settings-> add

To create a makefile out of the Code::Blocks project, use the tool cbp2make. Download here: https://sourceforge.net/projects/cbp2make/
To run it, first compile cbp2make, then ./cbp2make -in <input project.cbp> (--all-os) (-out <makefile name>)
The --all-os option creates makefiles for windows, mac-os and unix-like systems (linux).

## How to run

Set the parameter values in the file named parameters.txt (different name can be set by passing it to the constructor of Input).
If there is an unexpected or missing parameter, or the formatting of the parameters is wrong, the user will be asked to create a default input file.
Run the program by executing the CrossLink executable.

## Details on Input class

Input is a class that reads the input file, and checks if it conforms to the standard.
If not, the user (std::cout) is asked whether a default file should be produced.
If so, the input file is overridden.

A parameter called "runName" should always be one of the defined parameters, and from its value, the name of the run is taken. 
If the name was used before, then a number is appended.

The name of the input file can be passed to the constructor, and is "parameters.txt" by default.
If a different input file name is used, then a new set of run names is assumed,
and all existing results will be overridden if the same run name is used in two runs with different input file names.
Hence, use one input file name for all runs in the same directory.

For reading a parameter from the file, use the member function copyParameter(name, parameter),
which copies the value from the file with parameter name "name" to "parameter". 
The type of parameter can be int, double or std::string, and is deduced at compile time.

To define the parameters, change the file "DefaultParameterMap.cpp". 
Here, the constructor for the ParameterMap class is defined.
Add a parameter by calling the function defineParameter(name, value, (unit)).
If "unit" is omitted, "unitless" is added.

## Graphics controls

Close window: Escape, Q
Toggle run/pause: P, Pause
Propagate one step when paused: Space/T
Zoom in/out: mouse wheel, arrows up/down, W/S
Move view left/right: arrows left/right, A/D
Change distance between microtubules: +/-
Change distance between nodes: PageUp/PageDown

## Output

The program will always create a log file which shows the run time, the git hash of the program that created the data, and error messages or other std::out or
std::err messages that the program send.

Then, the microtubule_position file reports the position of the mobile microtubule every probePeriod time steps.

The times_barrier_crossings file lists all times and intervals between the times that a free energy barrier has been crossed.

Statistical_analysis can report statistics on those barrier crossing times, if there are enough samples (>1),
and can also report on positional remainders within a lattice spacing, if the parameter samplePositionalDistribution is set to TRUE.

In the latter case, another file called positional_histogram is created, binning the numbers of times that a certain position was found within a bin.

Finally, the input file is copied and stored for future reference and reproducability.

## Choices

I chose to keep all units to be SI units throughout the program, and not use non-dimensionalised units.
This is to reduce complexity of the data, even though it may come at a (slight) performance cost.
Also, it allows for versatile use of the code: e.g. by not dividing all positions by the lattice spacing,
we can allow for two different lattice spacings on the two filaments.

Further, I chose to keep concepts quite separated in the design of the program:
there is a system state, which stores the current state and defines how the state can change;
then there are propagators, which define the rules of the dynamics, and act on the system state.

## Author

Harmen Wierenga
h.wierenga@amolf.nl
2018
