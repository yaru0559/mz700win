// Copyright(C) AOKI Yasuharu 2019 

#include "ObjMgr.h"
#include "Object.h"

using namespace TPDFL;

namespace {
	// PDF記述テンプレート文字列
	const char HEADER[] = "%PDF-1.3\r%\342\343\317\323\r";
	const char XREFSTART[] = "xref\r0 %d\r0000000000 65535 f \r";
	const char XREFENTRY[] = "%010d 00000 n \r";
	const char TRAILERSTART[] = "trailer\r<<\r";
	const char TRAILERBODY[] = "/Size %d\r/Root %d 0 R\r/Info %d 0 R\r";
	const char TRAILEREND[] = ">>\rstartxref\r%d\r%%%%EOF";
}

ObjMgr::ObjMgr(FILE* fp) 
	: m_fp(fp), m_currentBytes(0), m_nextId(1), m_rootObjId(0), m_infoObjId(0)
{
	// PDFオブジェクト番号は1からなので、混乱を避けるためダミーのm_offsets[0], m_objects[0]を作っておく
	m_offsets.push_back(0);
	m_objects.push_back(NULL);
	WriteString(HEADER);
}

ObjMgr::~ObjMgr()
{
	Finalize();
}

unsigned long ObjMgr::IssueId(Object* obj)
{
	m_objects.push_back(obj);
	m_offsets.push_back(0);
	return m_nextId++;
}

void ObjMgr::SetRootObj(Object* obj)
{
	m_rootObjId = obj->GetId();
}

void ObjMgr::SetInfoObj(Object* obj)
{
	m_infoObjId = obj->GetId();
}

ErrorCode ObjMgr::Finalize()
{
	if (NULL == m_fp) return NoError;
	ErrorCode ret(NoError);
	for (std::vector<Object*>::iterator it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		if ((NoError == ret) && (*it)) {
			ret = (*it)->Write();
		}
		delete (*it);
		(*it) = NULL;
	}
	if (NoError == ret) {
		ret = Trailer();
	}
	fclose(m_fp);
	m_fp = NULL;
	return ret;
}

ErrorCode ObjMgr::WriteBinary(const void* buffer, unsigned long bytes)
{
	m_currentBytes += static_cast<unsigned long>(fwrite(buffer, 1, bytes, m_fp));
	return NoError;
}

ErrorCode ObjMgr::WriteString(const char* buffer)
{
	m_currentBytes += static_cast<unsigned long long>(fwrite(buffer, 1, strlen(buffer), m_fp));
	return NoError;
}

ErrorCode ObjMgr::Trailer()
{
	unsigned long long startxref = m_currentBytes;
	fprintf(m_fp, XREFSTART, m_nextId);
	for (unsigned long i = 1; i < m_nextId; ++i) {
		fprintf(m_fp, XREFENTRY, m_offsets[i]);
	}
	fprintf(m_fp, TRAILERSTART);
	fprintf(m_fp, TRAILERBODY, m_nextId, m_rootObjId, m_infoObjId);
	fprintf(m_fp, TRAILEREND, startxref);
	return NoError;
}

ErrorCode ObjMgr::OverwriteString(long pos, const char* buffer)
{
	fseek(m_fp, pos, SEEK_SET);
	fwrite(buffer, 1, strlen(buffer), m_fp);
	fseek(m_fp, 0, SEEK_END);
	return NoError;
}