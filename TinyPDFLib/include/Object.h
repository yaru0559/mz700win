#pragma once

#include "ErrorCode.h"
#include <vector>

#define CHECK_RESULT ErrorCode ret(UndefinedError);
#define OKorRETURN(_func_) ret = _func_ ; if (NoError != ret) return ret;

namespace TPDFL {
	class ObjMgr;
	class Value;
	class DirectValue;
	class Reference;

	class Object
	{
	public:
		Object(ObjMgr* mgr);
		virtual ~Object();
		virtual ErrorCode Write() = 0;
		unsigned long GetId() const;

	protected:
		DirectValue* NewValue(const std::string& str);
		Reference* NewReference(const Object* obj);

		// PDF�I�u�W�F�N�g�������݊J�n�錾
		ErrorCode	OpenObj();

		// PDF�I�u�W�F�N�g�������ݏI���錾
		ErrorCode CloseObj();

	private:
		Object();

	protected:
		ObjMgr* m_mgr;
		unsigned long m_id;
		std::vector<Value*> m_needToDelete;
	};
}