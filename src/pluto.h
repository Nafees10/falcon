#ifndef PLUTO_H
#define PLUTO_H

#ifdef _WIN32
  #include "C:\\plutonium\\PltObject.h"
  #define EXPORT __declspec(dllexport)
#else
  #include "/opt/plutonium/PltObject.h"
  #define EXPORT
#endif

extern "C"{
	EXPORT PltObject init();
	EXPORT PltObject render(PltObject* args, int n);
}

#endif
