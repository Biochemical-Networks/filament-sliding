#! /bin/bash

echo -e "#ifndef GITHASHWHENCOMPILED_HPP \n#include <string> \n\nconst std::string gitHash=\"$(git rev-parse HEAD)\"; \n\n#endif // GITHASHWHENCOMPILED_HPP" > GitHashWhenCompiled.hpp

