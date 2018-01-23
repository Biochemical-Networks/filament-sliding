# README #

### SUMMARY ###

CrossLink is a program for simulating crosslinking proteins between two rigid filaments, like microtubules.
One microtubule is considered to be stationary, while another is mobile. Then, crosslinkers can stretch, (un)bind,
and diffuse between the microtubules. We use this dynamics to study the movement of the mobile filament,
which can move under influence of passive diffusion or external forces, and to quantify the friction between
the filaments caused by the crosslinkers.

### HOW TO RUN ###

Set the parameter values in the file named parameters.txt (different name can be set by passing it to the constructor of Input).
If there is an unexpected or missing parameter, or the formatting of the parameters is wrong, the user will be asked to create a default input file.
Run the program by executing the CrossLink executable.

### AUTHOR ###

Harmen Wierenga
h.wierenga@amolf.nl
2018
