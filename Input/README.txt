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
