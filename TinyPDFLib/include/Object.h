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

		// PDFオブジェクト書き込み開始宣言
		ErrorCode	OpenObj();

		// PDFオブジェクト書き込み終了宣言
		ErrorCode CloseObj();

	private:
		Object();

	protected:
		ObjMgr* m_mgr;
		unsigned long m_id;
		std::vector<Value*> m_needToDelete;
	};
}