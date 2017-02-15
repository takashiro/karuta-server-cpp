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
typedef std::vector<Json> JsonArray;
typedef std::map<std::string, Json> JsonObject;

class Json
{
public:
	enum Type
	{
		Null,
		Boolean,
		Integer,
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
	Json(int value);
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
	bool isNumber() const { return m_type == Integer || m_type == Double; }
	bool isInteger() const { return m_type == Integer; }
	bool isDouble() const { return m_type == Double; }
	bool isString() const { return m_type == String; }
	bool isArray() const { return m_type == Array; }
	bool isObject() const { return m_type == Object; }

	bool toBool() const;

	double toDouble() const;
	int toInt() const;

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

	friend std::istream &operator>>(std::istream &in, Json &value);
	friend std::ostream &operator<<(std::ostream &out, const Json &value);

private:
	void copy(const Json &source);
	void release();

	Json::Type m_type;

	union Value
	{
		bool boolean;
		int inum;
		double dnum;
		std::string *str;
		JsonArray *array;
		JsonObject *object;
	};
	Value m_value;
};

KA_NAMESPACE_END
