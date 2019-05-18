// Copyright(C) AOKI Yasuharu 2019 

#include "Object.h"
#include "ObjMgr.h"
#include "Value.h"

using namespace TPDFL;

namespace {
	// PDF記述テンプレート文字列
	const char REFERENCE[] = "%d 0 R ";
}

Value::Value(ObjMgr* mgr)
	: m_mgr(mgr)
{}

Value::~Value()
{}


DirectValue::DirectValue(ObjMgr* mgr, const std::string& str)
	: Value(mgr), m_value(str)
{}

DirectValue::~DirectValue()
{}

ErrorCode DirectValue::Write()
{
	ErrorCode ret = m_mgr->WriteString(m_value.c_str());
	if (NoError != ret) return ret;
	return m_mgr->WriteString(" ");
}


Reference::Reference(ObjMgr* mgr, const Object* obj)
	: Value(mgr)
{
	m_refId = obj->GetId();
}

Reference::~Reference()
{}

ErrorCode Reference::Write()
{
	// X 0 R を書く。Xは最大で10桁のため最大15文字
	char buffer[17];
	sprintf_s(buffer, 15, REFERENCE, m_refId);
	return m_mgr->WriteString(buffer);
}


Array::Array(ObjMgr* mgr)
	: Value(mgr)
{}

Array::~Array()
{
	for (std::vector<Value*>::iterator it = m_array.begin(); it != m_array.end(); ++it)
	{
		delete (*it);
	}

}

ErrorCode Array::Write()
{
	CHECK_RESULT;
	OKorRETURN(m_mgr->WriteString("[ "));
	for (std::vector<Value*>::iterator it = m_array.begin(); it != m_array.end(); ++it)
	{
		(*it)->Write();
	}
	OKorRETURN(m_mgr->WriteString("]"));
	return NoError;
}

void Array::Append(const std::string& str)
{
	DirectValue* v = new DirectValue(m_mgr, str);
	m_array.push_back(v);
}

void Array::Append(const Object* obj)
{
	Reference* r = new Reference(m_mgr, obj);
	m_array.push_back(r);
}
