#pragma once

#include "ErrorCode.h"
#include <string>

namespace TPDFL {
	class ObjMgr;
	class Object;

	class Value
	{
	public:
		Value(ObjMgr* mgr);
		virtual ~Value();
		virtual ErrorCode Write() = 0;
	private:
		Value();

	protected:
		ObjMgr* m_mgr;
	};

	class DirectValue : public Value
	{
	public:
		DirectValue(ObjMgr* mgr, const std::string& str);
		~DirectValue();

		ErrorCode Write();

	protected:
		std::string m_value;
	};

	class Reference : public Value
	{
	public:
		Reference(ObjMgr* mgr, const Object* obj);
		~Reference();

		ErrorCode Write();

	protected:
		unsigned long m_refId;
	};

	class Array : public Value
	{
	public:
		Array(ObjMgr* mgr);
		virtual ~Array();
		void Append(const std::string& str);
		void Append(const Object* ref);

	protected:
		virtual ErrorCode Write();

	private:
		Array();

	protected:
		std::vector<Value*> m_array;
	};

}