#ifndef PLUTO_H
#define PLUTO_H

#include "PltObject.h"

extern "C"{
	PltObject init();
	PltObject render(PltObject* args, int n);
}

#endif
