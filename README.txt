# README #

### SUMMARY ###

CrossLink is a program for simulating crosslinking proteins between two rigid filaments, like microtubules.
One microtubule is considered to be stationary, while another is mobile. Then, crosslinkers can stretch, (un)bind,
and diffuse between the microtubules. We use this dynamics to study the movement of the mobile filament,
which can move under influence of passive diffusion or external forces, and to quantify the friction between
the filaments caused by the crosslinkers.

### HOW TO COMPILE ###

I have created the executables using CodeBlocks and g++. Make sure to run "addGitHash.sh" before every compile (include it in the build process).
When compiling on a system where git is not available, this should first be broken out of the Log class.

### HOW TO RUN ###

Set the parameter values in the file named parameters.txt (different name can be set by passing it to the constructor of Input).
If there is an unexpected or missing parameter, or the formatting of the parameters is wrong, the user will be asked to create a default input file.
Run the program by executing the CrossLink executable.

### CHOICES ###

I chose to keep all units to be SI units throughout the program, and not use non-dimensionalised units.
This is to reduce complexity of the data, even though it may come at a (slight) performance cost.
Also, it allows for versatile use of the code: e.g. by not dividing all positions by the lattice spacing,
we can allow for two different lattice spacings on the two filaments.

Further, I chose to keep concepts quite separated in the design of the program:
there is a system state, which stores the current state and defines how the state can change;
then there are propagators, which define the rules of the dynamics, and act on the system state.

### AUTHOR ###

Harmen Wierenga
h.wierenga@amolf.nl
2018
