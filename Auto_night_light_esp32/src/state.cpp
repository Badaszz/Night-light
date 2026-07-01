#include "state.h"
#include "config.h"

// This is the DEFINITION - the variable actually lives here in memory.
// Every other file just borrows a reference via the extern declarations.

bool autoMode       = true;
bool ledOn          = false;
int  brightness     = 0;
int  lightThreshold = DEFAULT_THRESHOLD;
int  lightLevel     = 0;
