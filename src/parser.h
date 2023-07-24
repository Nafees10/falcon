#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

struct Unit{
	enum Type{
		Invalid,
		Static,
		Interpolate,
		If,
		Else,
		ElseIf,
		For
	};
	Type type;
	std::vector<std::string> vals;
	std::vector<Unit> subs;
	Unit() : type(Invalid), vals() {}
	Unit (std::string text){
		type = Static;
		vals.push_back(text);
	}
	Unit(Type t, std::string a, const std::vector<Unit> &s) : type(t) {
		vals.push_back(a);
		this->subs = s;
	}
	Unit(Type t, const std::vector<Unit> &s) : type(t) {
		this->subs = s;
	}
	Unit(Type t, std::string a, std::string b, const std::vector<Unit> &s) :
		type(t) {
		vals.push_back(a);
		vals.push_back(b);
		this->subs = s;
	}
	Unit(Type t, std::vector<std::string> v, std::vector<Unit> s){
		this->type = t;
		this->vals = v;
		this->subs = s;
	}
};

std::vector<Unit> parse(const std::string &stream);

#endif
