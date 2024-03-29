/***********************************************************************
KARUTA server
Copyright (C) 2016-2017  Kazuichi Takashiro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

takashiro@qq.com
************************************************************************/

#include "Json.h"

#include <sstream>
#include <fstream>

KA_NAMESPACE_BEGIN

Json::Json()
	: m_type(Null)
{
}

Json::Json(bool value)
	: m_type(Boolean)
{
	m_value.boolean = value;
}

Json::Json(int32 value)
	: m_type(Integer)
{
	m_value.inum = value;
}

Json::Json(uint32 value)
	: m_type(UInteger)
{
	m_value.unum = value;
}

Json::Json(int64 value)
	: m_type(Integer)
{
	m_value.inum = value;
}

Json::Json(uint64 value)
	: m_type(UInteger)
{
	m_value.unum = value;
}

Json::Json(double value)
	: m_type(Double)
{
	m_value.dnum = value;
}

Json::Json(const char *value)
{
	m_type = String;
	m_value.str = new std::string(value);
}

Json::Json(const std::string &value)
	: m_type(String)
{
	m_value.str = new std::string(value);
}

Json::Json(std::string &&value)
	: m_type(String)
{
	m_value.str = new std::string(std::move(value));
}

Json::Json(const JsonArray &array)
	: m_type(Array)
{
	m_value.array = new JsonArray(array);
}

Json::Json(JsonArray &&array)
	: m_type(Array)
{
	m_value.array = new JsonArray(std::move(array));
}

Json::Json(const JsonObject &object)
	: m_type(Object)
{
	m_value.object = new JsonObject(object);
}

Json::Json(JsonObject &&object)
	: m_type(Object)
{
	m_value.object = new JsonObject(std::move(object));
}

Json::Json(const Json &source)
{
	copy(source);
}

Json::Json(Json &&source)
	: m_type(source.m_type)
	, m_value(source.m_value)
{
	source.m_type = Null;
}

Json::~Json()
{
	release();
}

bool Json::toBool() const
{
	switch (m_type) {
	case Boolean:
		return m_value.boolean;
	case Integer:
		return (int) m_value.dnum != 0;
	case Double:
		return m_value.inum != 0;
	case String:
		return !m_value.str->empty();
	case Array:
		return m_value.array->size() > 0;
	case Object:
		return m_value.object->size() > 0;
	default:
		return false;
	}
}

double Json::toDouble() const
{
	if (m_type == Double)
		return m_value.dnum;
	else if (m_type == Integer)
		return (double) m_value.inum;

	if (m_type == String) {
		std::stringstream s;
		s << *(m_value.str);
		double number;
		s >> number;
		return number;
	}

	return 0.0;
}

int32 Json::toInt() const
{
	if (m_type == Integer)
		return static_cast<int32>(m_value.inum);
	else if (m_type == UInteger)
		return static_cast<int32>(m_value.unum);
	else if (m_type == Double)
		return static_cast<int32>(m_value.dnum);

	if (m_type == String) {
		std::stringstream s;
		s << *(m_value.str);
		int32 number;
		s >> number;
		return number;
	}

	return 0;
}

uint32 Json::toUInt() const
{
	if (m_type == Integer)
		return static_cast<uint32>(m_value.inum);
	else if (m_type == UInteger)
		return static_cast<uint32>(m_value.unum);
	else if (m_type == Double)
		return static_cast<uint32>(m_value.dnum);

	if (m_type == String) {
		std::stringstream s;
		s << *(m_value.str);
		uint32 number;
		s >> number;
		return number;
	}

	return 0;
}

int64 Json::toInt64() const
{
	if (m_type == Integer)
		return m_value.inum;
	else if (m_type == UInteger)
		return static_cast<int64>(m_value.unum);
	else if (m_type == Double)
		return static_cast<int64>(m_value.dnum);

	if (m_type == String) {
		std::stringstream s;
		s << *(m_value.str);
		int64 number;
		s >> number;
		return number;
	}

	return 0;
}

uint64 Json::toUInt64() const
{
	if (m_type == Integer)
		return m_value.inum;
	else if (m_type == UInteger)
		return m_value.unum;
	else if (m_type == Double)
		return static_cast<int64>(m_value.dnum);

	if (m_type == String) {
		std::stringstream s;
		s << *(m_value.str);
		uint64 number;
		s >> number;
		return number;
	}

	return 0;
}

std::string Json::toString() const
{
	if (m_type == String)
		return *(m_value.str);

	if (m_type == Double) {
		std::stringstream s;
		s << m_value.dnum;
		std::string str;
		s >> str;
		return str;
	} else if (m_type == Integer) {
		std::stringstream s;
		s << m_value.inum;
		std::string str;
		s >> str;
		return str;
	} else if (m_type == UInteger) {
		std::stringstream s;
		s << m_value.unum;
		std::string str;
		s >> str;
		return str;
	}

	return std::string();
}

JsonArray &Json::array()
{
	return *(m_value.array);
}

const JsonArray &Json::toArray() const
{
	return *(m_value.array);
}

JsonObject &Json::object()
{
	return *(m_value.object);
}

const JsonObject &Json::toObject() const
{
	return *(m_value.object);
}

Json &Json::operator =(const Json &source)
{
	release();
	copy(source);
	return *this;
}

Json &Json::operator = (Json &&source)
{
	release();
	m_type = source.m_type;
	m_value = source.m_value;
	source.m_type = Null;
	return *this;
}

const Json &Json::value(const std::string &key, const Json &defaultValue) const
{
	if (contains(key))
		return value(key);
	return defaultValue;
}

size_t Json::size() const
{
	if (isArray())
		return m_value.array->size();
	else if (isObject())
		return m_value.object->size();
	return 0;
}

void Json::clear()
{
	if (m_type == Array) {
		m_value.array->clear();
	} else if (m_type == Object) {
		m_value.object->clear();
	} else if (m_type == String) {
		m_value.str->clear();
	} else if (m_type == Integer) {
		m_value.inum = 0;
	} else if (m_type == Double) {
		m_value.dnum = 0.0;
	}
}

void Json::release()
{
	if (m_type == Array) {
		delete m_value.array;
	} else if (m_type == Object) {
		delete m_value.object;
	} else if (m_type == String) {
		delete m_value.str;
	}
}

namespace
{

bool json_try_read(std::istream &in, char ch)
{
	char first = '\0';
	do {
		in.get(first);
	} while (first > '\0' && isspace(first));

	if (first == ch)
		return true;

	in.unget();
	return false;
}

bool json_try_read(std::istream &in, const char *str)
{
	char first = '\0';
	do {
		if (in.eof()) {
			return false;
		}
		in.get(first);
	} while (first > '\0' && isspace(first));

	const char *cur = str;
	if (*cur != first) {
		in.unget();
		return false;
	}

	cur++;
	while (*cur) {
		if (json_try_read(in, *cur)) {
			cur++;
		} else {
			while (cur != str) {
				cur--;
				in.putback(*cur);
			}
			return false;
		}
	}
	return true;
}

}

std::istream &operator>>(std::istream &in, Json &json)
{
	if (json.type() != Json::Null) {
		json.release();
	}

	if (json_try_read(in, "null")) {
		json.m_type = Json::Null;
	} else if (json_try_read(in, "false")) {
		json.m_type = Json::Boolean;
		json.m_value.boolean = false;
	} else if (json_try_read(in, "true")) {
		json.m_type = Json::Boolean;
		json.m_value.boolean = true;
	} else if (json_try_read(in, '"')) {
		json.m_type = Json::String;
		std::string str;
		char ch;
		while (!in.eof()) {
			in.get(ch);
			if (ch == '\\') {
				in.get(ch);
				if (ch == '"') {
					str += ch;
					continue;
				}
			} else if (ch == '"') {
				break;
			}
			str += ch;
		}
		json.m_value.str = new std::string(std::move(str));
	} else {
		char ch;
		in.get(ch);
		if (ch == '-' || (ch >= '0' && ch <= '9') || ch == '.') {
			in.unget();
			std::streamsize offset = in.tellg();
			bool negative = ch == '-';
			if (negative) {
				in >> json.m_value.inum;
			} else {
				in >> json.m_value.unum;
			}
			in.get(ch);
			if (ch == '.' || ch == 'e') {
				in.seekg(offset, std::ios::beg);
				json.m_type = Json::Double;
				in >> json.m_value.dnum;
			} else {
				json.m_type = negative ? Json::Integer : Json::UInteger;
				in.unget();
			}
		} else if (ch == '[') {
			json.m_type = Json::Array;
			json.m_value.array = new JsonArray();
			if (json_try_read(in, ']')) {
				return in;
			}

			JsonArray &array = (*json.m_value.array);
			for (;;) {
				Json element;
				in >> element;
				array.push_back(std::move(element));

				if (!json_try_read(in, ',')) {
					if (!(json_try_read(in, ']'))) {
						//vError("Expect ]");
					}
					break;
				}
			}
		} else if (ch == '{') {
			json.m_type = Json::Object;
			json.m_value.object = new JsonObject();
			if (json_try_read(in, '}')) {
				return in;
			}

			JsonObject &object = *(json.m_value.object);
			for (;;) {
				Json key;
				in >> key;
				if (key.type() != Json::String) {
					//vError("Expect JSON key string.");
					break;
				}
				if (!json_try_read(in, ':')) {
					//vError("Expect :");
					break;
				}

				Json value;
				in >> value;
				object[key.toString()] = std::move(value);

				if (!json_try_read(in, ',')) {
					if (!(json_try_read(in, '}'))) {
						//vError("Expect }");
					}
					break;
				}
			}
		} else {
			//vError("Unexpected character: " << ch);
		}
	}
	return in;
}

std::ostream &operator<<(std::ostream &out, const Json &value)
{
	switch (value.type()) {
	case Json::Null:
		out << "null";
		break;
	case Json::Boolean:
		if (value.toBool())
			out << "true";
		else
			out << "false";
		break;
	case Json::Integer:
		out << value.toInt64();
		break;
	case Json::UInteger:
		out << value.toInt64();
		break;
	case Json::Double:
		out << value.toDouble();
		break;
	case Json::String:
		out << '"' << value.toString() << '"';
		break;
	case Json::Array:{
		out << '[';
		const JsonArray &elements = value.toArray();
		size_t size = elements.size();
		if (size > 0) {
			out << elements.at(0);
			for(size_t i = 1; i < size; i++)
				out << ", " << elements.at(i);
		}
		out << ']';
		break;
	}
	case Json::Object:{
		out << '{';
		const JsonObject &jsonObject = value.toObject();
		if (!jsonObject.empty()) {
			JsonObject::const_iterator iter = jsonObject.begin();
			out << '"' << iter->first << "\" : " << iter->second;
			for (iter++; iter != jsonObject.end(); iter++) {
				out << ", \"" << iter->first << "\" : " << iter->second;
			}
		}
		out << '}';
		break;
	}
	default:
		//vError("invalid JSON value can't be printed");
		break;
	}
	return out;
}

void Json::copy(const Json &source)
{
	m_type = source.type();
	switch (m_type) {
	case Json::String:
		m_value.str = new std::string(*source.m_value.str);
		break;
	case Json::Array:
		m_value.array = new JsonArray(*source.m_value.array);
		break;
	case Json::Object:
		m_value.object = new JsonObject(*source.m_value.object);
		break;
	default:
		m_value = source.m_value;
		break;
	}
}

KA_NAMESPACE_END
