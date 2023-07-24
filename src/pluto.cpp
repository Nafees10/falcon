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

bool render(std::ifstream&, size_t&, Dictionary&);
bool renderFor(const std::string&, size_t&, Dictionary&);
bool renderIf(const std::string&, size_t&, Dictionary&);
bool renderElse(const std::string&, size_t&, Dictionary&);
bool renderElseIf(const std::string&, size_t&, Dictionary&);

PltObject render(PltObject* args, int n){
	// expect 2 args, filename, and dict
	if (n != 2)
		return Plt_Err(ArgumentError, "Expected 2 arguments (filename, dict)");
	string filename = *(string*)args[0].ptr;
	Dictionary map = *(Dictionary*)args[1].ptr;

	std::ifstream in(filename);
	if (!in)
		return Plt_Err(FileIOError, "Failed to open file");
	size_t index;
	bool ret = render(in, index, map);
	in.close();

	return PObjFromBool(ret);
}

/// result of last if block
bool lastIfRes = true;

bool render(const std::string &stream, size_t &index, Dictionary &map){
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
			lowercase(tagName);
			if (!isRelevantTag(tagName)){
				std::cout << tagName;
				continue;
			}
			if (closing){
				// skip until closing `>`
				index += count(stream, index, [](char c) {return c != '>';});
				return true;
			}
			bool (*func)(const std::string&, size_t&, Dictionary&) = nullptr;
			if (tagName == "for"){
				func = renderFor;
			}else if (tagName == "if"){
				func = renderIf;
			}else if (tagName == "else"){
				func = renderElse;
			}else if (tagName == "elseif"){
				func = renderElseIf;
			}
			if (!func(stream, index, map))
				return false;
		}
	}
	return true;
}

bool renderFor(const std::string &stream, size_t &index, Dictionary &map){
	std::string itName = tagNameAttr(stream, index);
	std::string ctName = tagNameAttr(stream, index);
	// skip until `>`
	index += count(stream, index, [](char c) {return c != '>';});
	auto iterator = map.find(PObjFromStr(ctName));
	if (iterator == map.end()){
		std::cerr << "PLUTO ERROR: map does not include " << ctName << "\n";
		return false;
	}
	PltObject container = iterator->second;
	if (container.type != PLT_LIST){
		std::cerr << "PLUTO ERROR: cannot iterate over non-list " << ctName << "\n";
		return false;
	}
	vector<PltObject> list = *(PltList*)(container.ptr);
	size_t endIndex;
	for (auto &obj : list){
		size_t i = index;
		Dictionary sub(map);
		sub.emplace(PObjFromStr(itName), obj);
		if (!render(stream, i, sub))
			return false;
		endIndex = i;
	}
	index = endIndex;
	return true;
}

bool renderIf(const std::string &stream, size_t &index, Dictionary &map){
	std::string ctName = tagNameAttr(stream, index);
	// skip until `>`
	index += count(stream, index, [](char c) {return c != '>';});

}
