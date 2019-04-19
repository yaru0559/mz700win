#include "Dict.h"
#include "ErrorCode.h"
#include "ObjMgr.h"

using namespace TPDFL;

Dict::Dict(ObjMgr* mgr)
	: Object(mgr)
{}

Dict::~Dict()
{}

ErrorCode Dict::Write()
{
	CHECK_RESULT;
	OKorRETURN(OpenObj());
	OKorRETURN(WriteDictPart());
	OKorRETURN(CloseDictPart());
	OKorRETURN(CloseObj());
	return NoError;
}

ErrorCode Dict::WriteDictPart()
{
	CHECK_RESULT;
	OKorRETURN(m_mgr->WriteString("<<\r"));
	for (std::map<std::string, Value*>::iterator it = m_dict.begin(); it != m_dict.end(); ++it)
	{
		OKorRETURN(m_mgr->WriteString("/"));
		OKorRETURN(m_mgr->WriteString((*it).first.c_str()));
		OKorRETURN(m_mgr->WriteString(" "));
		OKorRETURN((*it).second->Write());
		OKorRETURN(m_mgr->WriteString("\r"));
	}
	return NoError;
}

ErrorCode Dict::CloseDictPart()
{
	CHECK_RESULT;
	OKorRETURN(m_mgr->WriteString(">>\r"));
	return NoError;
}

void Dict::Add(const std::string& key, const std::string& value)
{
	DirectValue* v = NewValue(value);
	m_dict[key] = v;
}

void Dict::Add(const std::string& key, const Object* ref)
{
	Reference* r = NewReference(ref);
	m_dict[key] = r;
}

void Dict::Add(const std::string& key, Array* array)
{
	m_dict[key] = array;
}