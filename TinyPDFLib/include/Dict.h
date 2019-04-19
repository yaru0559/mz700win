#pragma once

#include "Object.h"
#include "Value.h"
#include <string>
#include <map>

namespace TPDFL {
	class Dict : public Object
	{
	public:
		Dict(ObjMgr* mgr);
		virtual ~Dict();

		virtual ErrorCode Write();
		void Add(const std::string& key, const std::string& value);
		void Add(const std::string& key, const Object* ref);
		void Add(const std::string& key, Array* array);

	protected:
		ErrorCode WriteDictPart();
		ErrorCode CloseDictPart();

	private:
		Dict();

	protected:
		std::map<std::string, Value*> m_dict;
	};
}