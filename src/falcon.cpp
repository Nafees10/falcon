#include <iostream>
#include <fstream>
#include <string>

#include "falcon.h"
#include "parser.h"
#include "utils.h"

ZObject nil;

ZObject init(){
	nil.type = Z_NIL;
	Module* module = vm_allocModule();
	Module_addNativeFun(module,"render",&render);

	return ZObjFromModule(module);
}
ZObject quickErr(Klass* k,std::string s){
	return Z_Err(k,s.c_str());
}
ZObject renderUnits(std::vector<Unit>&,ZDict*);

ZObject render(ZObject* args, int n){
	// expect 2 args, filename, and dict
	if (n != 2)
		return Z_Err(ArgumentError, "Expected 2 arguments (filename, dict)");
	if (args[0].type != Z_STR)
		return Z_Err(TypeError, "expected first argumnt as string filename");
	if (args[1].type != Z_DICT)
		return Z_Err(TypeError, "expected second argumnt as dictionary");
	const char* filename = AS_STR(args[0])->val;
	ZDict* map = AS_DICT(args[1]);

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
			return Z_Err(ValueError, "Syntax error in falcon template");
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

ZObject getVal(ZDict* map, std::string &a){
	ZObject obj;
	ZStr tmp;
	tmp.len = a.length();
	tmp.val = (char*)a.c_str();
	if (!ZDict_get(map,ZObjFromStrPtr(&tmp),&obj))
		return quickErr(ValueError, "map does not contain " + a);
	return obj;
}

ZObject getVal(ZDict* &map, std::string &a, std::string &b){
	ZObject obj = getVal(map, a);;
	if (obj.type == Z_ERROBJ) return obj;

	if (obj.type == Z_OBJ){
		StrMap* subMap = &(((KlassObject*)(obj.ptr))->members);
		ZObject val;
		if (!StrMap_get(subMap,b.c_str(),&val))
			return quickErr(ValueError, "map does not contain " + a + "." + b);
		return val;
	}
	if (obj.type == Z_DICT){
		ZDict* subMap = (ZDict*)(obj.ptr);
		ZStr tmp;
		tmp.len = b.length();
		tmp.val = (char*)b.c_str();
		ZObject val;
		if (!ZDict_get(subMap,ZObjFromStrPtr(&tmp),&val))
			return quickErr(ValueError, "map does not contain " + a + "." + b);
		return val;
	}
	return Z_Err(ValueError, "Expected a map or class instance");
}

ZObject renderInterpolate(Unit &unit, ZDict* &map){
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

ZObject renderIf(Unit &unit, ZDict* &map){
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

ZObject renderElse(Unit &unit, ZDict* &map){
	if (lastIfRes)
		return nil;
	lastIfRes = true;
	return renderUnits(unit.subs, map);
}

ZObject renderElseIf(Unit &unit, ZDict* &map){
	if (lastIfRes)
		return nil;
	return renderIf(unit, map);
}

ZObject renderFor(Unit &unit, ZDict* map){
	ZObject obj = getVal(map, unit.vals[1]);
	if (obj.type != Z_LIST)
		return Z_Err(ValueError, "can only iterate with for over lists");
	ZList* list = AS_LIST(obj);
	for (size_t idx = 0 ; idx < list->size; idx++){
		ZObject elem = list->arr[idx];
		ZDict* sub = vm_allocDict();
		ZDict_assign(sub,map); //make deep copy
		ZDict_emplace(sub,ZObjFromStr(unit.vals[0].c_str()), elem);
		auto ret =renderUnits(unit.subs, sub);
		if (ret.type == Z_ERROBJ)
			return ret;
	}
	return nil;
}

ZObject renderUnits(std::vector<Unit> &units, ZDict* map){
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
