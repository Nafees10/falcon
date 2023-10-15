#include <iostream>
#include <fstream>

#include "PltObject.h"
#include "pluto.h"
#include "parser.h"
#include "utils.h"

PltObject nil;

PltObject init(){
	nil.type = PLT_NIL;
	Module* module = vm_allocModule();
	module->members.emplace("render", PObjFromFunction("render", &render));

	return PObjFromModule(module);
};

PltObject renderUnits(std::vector<Unit>&, Dictionary&);

PltObject render(PltObject* args, int n){
	// expect 2 args, filename, and dict
	if (n != 2)
		return Plt_Err(ArgumentError, "Expected 2 arguments (filename, dict)");
	if (args[0].type != PLT_STR)
		return Plt_Err(TypeError, "expected first argumnt as string filename");
	if (args[1].type != PLT_DICT)
		return Plt_Err(TypeError, "expected second argumnt as dictionary");
	string filename = *(string*)args[0].ptr;
	Dictionary &map = *(Dictionary*)args[1].ptr;

	std::vector<Unit> units;
	{
		std::string stream;
		if (!readFile(filename, stream))
			return Plt_Err(FileIOError, "Failed to open file");
		units = parse(stream);
	}
	// make sure no invalid in there
	for (auto unit : units){
		if (unit.type == Unit::Invalid)
			return Plt_Err(ValueError, "Syntax error in pluto template");
	}
	return renderUnits(units, map);
}

/// result of last if block
bool lastIfRes = true;

PltObject renderStatic(Unit &unit){
	for (auto &val : unit.vals)
		std::cout << val;
	return nil;
}

PltObject getVal(Dictionary &map, std::string &a){
	PltObject obj;
	auto iterator = map.find(PObjFromStrPtr(&a));
	if (iterator == map.end())
		return Plt_Err(ValueError, "map does not contain " + a);
	obj = iterator->second;
	return obj;
}

PltObject getVal(Dictionary &map, std::string &a, std::string &b){
	PltObject obj = getVal(map, a);;
	if (obj.type == PLT_ERROBJ) return obj;

	if (obj.type == PLT_OBJ){
		auto &subMap = ((KlassObject*)(obj.ptr))->members;
		auto it = subMap.find(b);
		if (it == subMap.end())
			return Plt_Err(ValueError, "map does not contain " + a + "." + b);
		return it->second;
	}
	if (obj.type == PLT_DICT){
		Dictionary &subMap = *(Dictionary*)(obj.ptr);
		auto iterator = subMap.find(PObjFromStrPtr(&b));
		if (iterator == subMap.end())
			return Plt_Err(ValueError, "map does not contain " + a + "." + b);
		return iterator->second;
	}
	return Plt_Err(ValueError, "Expected a map or class instance");
}

PltObject renderInterpolate(Unit &unit, Dictionary &map){
	PltObject obj;
	if (unit.vals.size() == 2)
		obj = getVal(map, unit.vals[0], unit.vals[1]);
	else
		obj = getVal(map, unit.vals[0]);
	if (obj.type == PLT_ERROBJ)
		return obj;

	if (obj.type != PLT_STR)
		return Plt_Err(ValueError, "cannot interpolate non string data");
	std::cout << *(std::string*)(obj.ptr);
	return nil;
}

PltObject renderIf(Unit &unit, Dictionary &map){
	PltObject obj;
	if (unit.vals.size() == 2)
		obj = getVal(map, unit.vals[0], unit.vals[1]);
	else
		obj = getVal(map, unit.vals[0]);
	if (obj.type == PLT_ERROBJ || (obj.type == PLT_BOOL && obj.i == 0)){
		lastIfRes = false;
		return nil; // skip rendering
	}
	auto ret = renderUnits(unit.subs, map);
	lastIfRes = true;
	return ret;
}

PltObject renderElse(Unit &unit, Dictionary &map){
	if (lastIfRes)
		return nil;
	lastIfRes = true;
	return renderUnits(unit.subs, map);
}

PltObject renderElseIf(Unit &unit, Dictionary &map){
	if (lastIfRes)
		return nil;
	return renderIf(unit, map);
}

PltObject renderFor(Unit &unit, Dictionary &map){
	PltObject obj = getVal(map, unit.vals[1]);
	if (obj.type != PLT_LIST)
		return Plt_Err(ValueError, "can only iterate with for over lists");
	std::vector<PltObject> list = *(PltList*)(obj.ptr);
	for (auto &elem : list){
		Dictionary sub(map);
		sub.emplace(PObjFromStr(unit.vals[0]), elem);
		auto ret =renderUnits(unit.subs, sub);
		if (ret.type == PLT_ERROBJ)
			return ret;
	}
	return nil;
}

PltObject renderUnits(std::vector<Unit> &units, Dictionary &map){
	for (auto &unit : units){
		PltObject ret;
		//ret.type = PLT_BOOL; // dummy
		switch (unit.type){
			case Unit::Invalid: break;
			case Unit::Static: ret = renderStatic(unit); break;
			case Unit::Interpolate: ret = renderInterpolate(unit, map); break;
			case Unit::If: ret = renderIf(unit, map); break;
			case Unit::Else: ret = renderElse(unit, map); break;
			case Unit::ElseIf: ret = renderElseIf(unit, map); break;
			case Unit::For: ret = renderFor(unit, map); break;
		}
		if (ret.type == PLT_ERROBJ)
			return ret;
	}
	return nil;
}
