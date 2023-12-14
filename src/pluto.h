#ifndef PLUTO_H
#define PLUTO_H

#ifdef _WIN32
  #include "C:\\zuko\\ZObject.h"
  #define EXPORT __declspec(dllexport)
#else
  #include "/opt/zuko/ZObject.h"
  #define EXPORT
#endif

extern "C"{
	EXPORT ZObject init();
	EXPORT ZObject render(ZObject* args, int32_t n);
}

#endif
