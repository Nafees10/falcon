#include <iostream>
#include <fstream>


#include "pluto.h"
#include "parser.h"
#include "utils.h"

ZObject nil;

ZObject init(){
	nil.type = Z_NIL;
	Module* module = vm_allocModule();
	module->members.emplace("render", ZObjFromFunction("render", &render));

	return ZObjFromModule(module);
};

ZObject renderUnits(std::vector<Unit>&, Dictionary&);

ZObject render(ZObject* args, int n){
	// expect 2 args, filename, and dict
	if (n != 2)
		return Z_Err(ArgumentError, "Expected 2 arguments (filename, dict)");
	if (args[0].type != Z_STR)
		return Z_Err(TypeError, "expected first argumnt as string filename");
	if (args[1].type != Z_DICT)
		return Z_Err(TypeError, "expected second argumnt as dictionary");
	string filename = *(string*)args[0].ptr;
	Dictionary &map = *(Dictionary*)args[1].ptr;

	std::vector<Unit> units;
	{
		std::string stream;
		if (!readFile(filename, stream))
			return Z_Err(FileIOError, "Failed to open file");
		units = parse(stream);
	}
	// make sure no invalid in there
	for (auto unit : units){
		if (unit.type == Unit::Invalid)
			return Z_Err(ValueError, "Syntax error in pluto template");
	}
	return renderUnits(units, map);
}

/// result of last if block
bool lastIfRes = true;

ZObject renderStatic(Unit &unit){
	for (auto &val : unit.vals)
		std::cout << val;
	return nil;
}

ZObject getVal(Dictionary &map, std::string &a){
	ZObject obj;
	auto iterator = map.find(ZObjFromStrPtr(&a));
	if (iterator == map.end())
		return Z_Err(ValueError, "map does not contain " + a);
	obj = iterator->second;
	return obj;
}

ZObject getVal(Dictionary &map, std::string &a, std::string &b){
	ZObject obj = getVal(map, a);;
	if (obj.type == Z_ERROBJ) return obj;

	if (obj.type == Z_OBJ){
		auto &subMap = ((KlassObject*)(obj.ptr))->members;
		auto it = subMap.find(b);
		if (it == subMap.end())
			return Z_Err(ValueError, "map does not contain " + a + "." + b);
		return it->second;
	}
	if (obj.type == Z_DICT){
		Dictionary &subMap = *(Dictionary*)(obj.ptr);
		auto iterator = subMap.find(ZObjFromStrPtr(&b));
		if (iterator == subMap.end())
			return Z_Err(ValueError, "map does not contain " + a + "." + b);
		return iterator->second;
	}
	return Z_Err(ValueError, "Expected a map or class instance");
}

ZObject renderInterpolate(Unit &unit, Dictionary &map){
	ZObject obj;
	if (unit.vals.size() == 2)
		obj = getVal(map, unit.vals[0], unit.vals[1]);
	else
		obj = getVal(map, unit.vals[0]);
	if (obj.type == Z_ERROBJ)
		return obj;

	if (obj.type != Z_STR)
		return Z_Err(ValueError, "cannot interpolate non string data");
	std::cout << *(std::string*)(obj.ptr);
	return nil;
}

ZObject renderIf(Unit &unit, Dictionary &map){
	ZObject obj;
	if (unit.vals.size() == 2)
		obj = getVal(map, unit.vals[0], unit.vals[1]);
	else
		obj = getVal(map, unit.vals[0]);
	if (obj.type == Z_ERROBJ || (obj.type == Z_BOOL && obj.i == 0)){
		lastIfRes = false;
		return nil; // skip rendering
	}
	auto ret = renderUnits(unit.subs, map);
	lastIfRes = true;
	return ret;
}

ZObject renderElse(Unit &unit, Dictionary &map){
	if (lastIfRes)
		return nil;
	lastIfRes = true;
	return renderUnits(unit.subs, map);
}

ZObject renderElseIf(Unit &unit, Dictionary &map){
	if (lastIfRes)
		return nil;
	return renderIf(unit, map);
}

ZObject renderFor(Unit &unit, Dictionary &map){
	ZObject obj = getVal(map, unit.vals[1]);
	if (obj.type != Z_LIST)
		return Z_Err(ValueError, "can only iterate with for over lists");
	std::vector<ZObject> list = *(ZList*)(obj.ptr);
	for (auto &elem : list){
		Dictionary sub(map);
		sub.emplace(ZObjFromStr(unit.vals[0]), elem);
		auto ret =renderUnits(unit.subs, sub);
		if (ret.type == Z_ERROBJ)
			return ret;
	}
	return nil;
}

ZObject renderUnits(std::vector<Unit> &units, Dictionary &map){
	for (auto &unit : units){
		ZObject ret;
		//ret.type = Z_BOOL; // dummy
		switch (unit.type){
			case Unit::Invalid: break;
			case Unit::Static: ret = renderStatic(unit); break;
			case Unit::Interpolate: ret = renderInterpolate(unit, map); break;
			case Unit::If: ret = renderIf(unit, map); break;
			case Unit::Else: ret = renderElse(unit, map); break;
			case Unit::ElseIf: ret = renderElseIf(unit, map); break;
			case Unit::For: ret = renderFor(unit, map); break;
		}
		if (ret.type == Z_ERROBJ)
			return ret;
	}
	return nil;
}
