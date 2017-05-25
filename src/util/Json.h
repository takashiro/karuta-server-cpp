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

#pragma once

#include "global.h"

#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <map>

KA_NAMESPACE_BEGIN

class Json;
using JsonArray = std::vector<Json>;
using JsonObject = std::map<std::string, Json>;

class KA_DLL_EXPORT Json
{
public:
	enum Type
	{
		Null,
		Boolean,
		Integer,
		UInteger,
		Double,
		String,
		Array,
		Object
	};

	//Null
	Json();

	//Boolean
	Json(bool value);

	//Number
	Json(int32 value);
	Json(uint32 value);
	Json(int64 value);
	Json(uint64 value);
	Json(double value);

	//String
	Json(const char *value);
	Json(const std::string &value);
	Json(std::string &&value);

	//Array
	Json(const JsonArray &array);
	Json(JsonArray &&array);

	//Object
	Json(const JsonObject &object);
	Json(JsonObject &&object);

	Json(const Json &source);
	Json(Json &&source);
	~Json();

	Type type() const { return m_type; }
	bool isNull() const { return m_type == Null; }
	bool isNumber() const { return m_type == Integer || m_type == UInteger || m_type == Double; }
	bool isInteger() const { return m_type == Integer; }
	bool isUInteger() const { return m_type == UInteger; }
	bool isDouble() const { return m_type == Double; }
	bool isString() const { return m_type == String; }
	bool isArray() const { return m_type == Array; }
	bool isObject() const { return m_type == Object; }

	bool toBool() const;

	double toDouble() const;
	int32 toInt() const;
	uint32 toUInt() const;
	int64 toInt64() const;
	uint64 toUInt64() const;

	std::string toString() const;

	JsonArray &array();
	const JsonArray &toArray() const;

	JsonObject &object();
	const JsonObject &toObject() const;

	//Assignment functions
	Json &operator =(const Json &source);
	Json &operator =(Json &&source);

	//Array Functions
	Json &operator[](uint i) { return (*m_value.array)[i]; }

	const Json &operator[](uint i) const { return m_value.array->at(i); }
	const Json &at(uint i) const { return m_value.array->at(i); }

	//Object Functions
	Json &operator[](const std::string &key) { return (*(m_value.object))[key]; }

	const Json &operator[](const std::string &key) const { return m_value.object->at(key); }
	const Json &value(const std::string &key) const { return m_value.object->at(key); }
	const Json &value(const std::string &key, const Json &defaultValue) const;

	bool contains(const std::string &key) const { return m_value.object->find(key) != m_value.object->end(); }

	//Array/Object functions
	size_t size() const;
	void clear();

	friend KA_DLL_EXPORT std::istream &operator>>(std::istream &in, Json &value);
	friend KA_DLL_EXPORT std::ostream &operator<<(std::ostream &out, const Json &value);

private:
	void copy(const Json &source);
	void release();

	Json::Type m_type;

	union Value
	{
		bool boolean;
		int64 inum;
		uint64 unum;
		double dnum;
		std::string *str;
		JsonArray *array;
		JsonObject *object;
	};
	Value m_value;
};

KA_NAMESPACE_END
