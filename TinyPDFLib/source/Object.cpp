#include "Object.h"
#include "ObjMgr.h"
#include "Value.h"

using namespace TPDFL;

namespace {
	const char STARTOBJ[] = "%d 0 obj\r";
	const char ENDOBJ[] = "endobj\r";
}


Object::Object(ObjMgr* mgr)
	: m_mgr(mgr)
{
	m_id = m_mgr->IssueId(this);
}

Object::~Object()
{
	for (std::vector<Value*>::iterator it = m_needToDelete.begin(); it != m_needToDelete.end(); ++it)
	{
		delete (*it);
	}
}

unsigned long Object::GetId() const
{
	return m_id;
}

DirectValue* Object::NewValue(const std::string& str)
{
	DirectValue* v = new DirectValue(m_mgr, str);
	m_needToDelete.push_back(v);
	return v;
}

Reference* Object::NewReference(const Object* obj)
{
	Reference* r = new Reference(m_mgr, obj);
	m_needToDelete.push_back(r);
	return r;
}


ErrorCode Object::OpenObj()
{
	m_mgr->m_offsets[m_id] = m_mgr->m_currentBytes;
	// X 0 obj‚ð‘‚­BX‚ÍÅ‘å‚Å10Œ…‚Ì‚½‚ßÅ‘å16•¶Žš
	char buffer[17];
	sprintf_s(buffer, 17, STARTOBJ, m_id);
	return m_mgr->WriteString(buffer);
}

ErrorCode Object::CloseObj()
{
	// endobj‚ð‘‚­
	return m_mgr->WriteString(ENDOBJ);
}
