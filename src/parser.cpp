#include "parser.h"
#include "utils.h"

std::vector<Unit> parse(const std::string &stream, size_t &i);

std::vector<Unit> parse(const std::string &stream){
	size_t i = 0;
	return parse(stream, i);
}

bool skipComment(const std::string&, size_t&);
Unit parseTag(const std::string&, std::string, size_t&);
Unit parseForTag(const std::string&, size_t&);
Unit parseIfTag(const std::string&, size_t&);
Unit parseElseTag(const std::string&, size_t&);
Unit parseElseIfTag(const std::string&, size_t&);
Unit parseInterpolation(const std::string&, size_t&);

std::vector<Unit> parse(const std::string &str, size_t &i){
	std::vector<Unit> ret;
	size_t start = i;

	for (; i < str.length(); i ++){
		auto ch = str[i];
		if (ch == '<'){
			if (skipComment(str, i))
				continue;
			bool ending = false;
			const auto ind = i;
			if (i + 1 < str.length() && str[i + 1] == '/'){
				ending = true;
				i ++;
			}
			++ i;
			auto tagName = tagNameAttr(str, i);
			lowercase(tagName);
			if (isRelevantTag(tagName)){
				// end this unit here
				if (start < ind)
					ret.push_back(Unit(str.substr(start, ind - start)));
				if (ending){
					// skip to >
					while (i < str.length() && str[i] != '>') i ++;
					i ++;
					return ret;
				}
				ret.push_back(parseTag(str, tagName, i));
				start = i;
				i --; continue;
			}
		}else if (ch == '\''){
			// just skip to end
			for (; i < str.length() && str[i] != '\''; i ++){
				if (str[i] == '\\'){
					i ++; continue;
				}
			}
		}else if (ch == '{'){
			const auto ind = i;
			Unit interp = parseInterpolation(str, i);
			if (interp.type == Unit::Invalid){
				i --; continue;
			}
			// end this unit
			if (start < ind)
				ret.push_back(Unit(str.substr(start, ind - start)));
			ret.push_back(interp);
			start = i;
			i --; continue;
		}
	}
	if (start + 1 < str.length())
		ret.push_back(Unit(str.substr(start, str.length() - start)));
	return ret;
}

bool skipComment(const std::string &str, size_t &i){
	if (i + 3 <= str.length() || str.substr(i, 4) != "<!--")
		return false;
	i += 4; // skip opening
	while (i + 3 < str.length() && str.substr(i, 3) != "-->")
		i ++;
	i += 3;
	return true;
}

Unit parseTag(const std::string &str, std::string tagName, size_t &i){
	if (tagName == "for"){
		return parseForTag(str, i);
	}else if (tagName == "if"){
		return parseIfTag(str, i);
	}else if (tagName == "else"){
		return parseElseTag(str, i);
	}else if (tagName == "elseif"){
		return parseElseIfTag(str, i);
	}
	return Unit();
}

Unit parseForTag(const std::string &str, size_t &i){
	std::vector<std::string> args;
	std::string s = tagNameAttr(str, i);
	if (i >= str.length())
		return Unit();
	args.push_back(s);
	if (str[i] == '.'){
		s = tagNameAttr(str, i);
		args.push_back(s);
	}
	if (i >= str.length())
		return Unit();
	args.push_back(tagNameAttr(str, i));
	i += count(str, i, [](char c) { return c != '>'; });

	return Unit(Unit::For, args, parse(str, i));
}

Unit parseIfTag(const std::string &str, size_t &i){
	std::string a = nullptr;
	std::string b = nullptr;
	a = tagNameAttr(str, i);
	if (i >= str.length())
		return Unit();
	if (str[i] == '.'){
		b = tagNameAttr(str, i);
		if (i >= str.length())
			return Unit();
	}
	i += count(str, i, [](char c) { return c != '>'; });
	if (b.length() > 0)
		return Unit(Unit::If, a, b, parse(str, i));
	return Unit(Unit::If, a, parse(str, i));
}

Unit parseElseTag(const std::string &str, size_t &i){
	// skip until >
	i += count(str, i, [](char c) { return c != '>'; });
	return Unit(Unit::Else, parse(str, i));
}

Unit parseElseIfTag(const std::string &str, size_t &i){
	// same as if
	auto ret = parseIfTag(str, i);
	ret.type = Unit::ElseIf;
	return ret;
}

Unit parseInterpolation(const std::string &str, size_t &i){

}
