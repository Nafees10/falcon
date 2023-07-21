#include <iostream>
#include <fstream>

#include "PltObject.h"
#include "pluto.h"
#include "utils.h"

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

void renderFor(const std::string&, size_t&, Dictionary&);
void renderIf(const std::string&, size_t&, Dictionary&);
void renderElse(const std::string&, size_t&, Dictionary&);
void renderElseIf(const std::string&, size_t&, Dictionary&);

void render(const std::string &stream, size_t &index, Dictionary &map){
	char ch;
	while (index < stream.length()){
		char c = stream[index];
		if (c == '<'){
			if (++index >= stream.length()){
				std::cout << c;
				continue;
			}
			bool closing = stream[index] == '/';
			std::string tagName = tagNameAttr(stream, index);
			if (!isRelevantTag(tagName)){
				std::cout << tagName;
				continue;
			}
			if (closing){
				// skip until closing `>`
				index += count(stream, index, [](char c) {return c != '>';});
				return;
			}
			if (tagName == "for"){
				renderFor(stream, index, map);
			}
		}
	}
}

void renderFor(const std::string &stream, size_t &index, Dictionary &map){

}
