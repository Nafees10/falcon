#include <iostream>
#include <fstream>

#include "PltObject.h"
#include "pluto.h"

#define BUF_LEN 1024

PltObject init(){
	Module* module = vm_allocModule();
	module->members.emplace("render", PObjFromFunction("render", &render));

	return PObjFromModule(module);
};

void render(std::ifstream&, Dictionary&);

PltObject render(PltObject* args, int n){
	// expect 2 args, filename, and dict
	if (n != 2)
		return Plt_Err(ArgumentError, "Expected 2 arguments (filename, dict)");
	string filename = *(string*)args[0].ptr;
	Dictionary map = *(Dictionary*)args[1].ptr;

	std::ifstream in(filename);
	if (!in)
		return Plt_Err(FileIOError, "Failed to open file");
	render(in, map);
	in.close();

	return PltObject();
}


void render(std::ifstream &in, Dictionary &map){
	static char buffer[BUF_LEN] = {0};
	static unsigned int bufSeek;
	char ch;
	while (in >> noskipws >> ch){

	}
}
