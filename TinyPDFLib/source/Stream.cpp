#include "ObjMgr.h"
#include "Stream.h"

using namespace TPDFL;

namespace {
	const char LENGTHKEY[] = "/Length ";
	const char LENGTHPLACEHOLDER[] = "0         \r"; // Max 10 digisW
	const char BEGINSTREAM[] = ">>\rstream\r";
	const char ENDSTREAM[] = "\rendstream\r";
}


static const unsigned long BLOCKSIZE = 2;
static const int BUFSIZE = 16;

Stream::Stream(ObjMgr* mgr)
	: Dict(mgr), m_writing(false), m_streamSize(0), m_freeInBlock(0)
{}

Stream::~Stream()
{
	for (std::vector<char*>::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		delete[](*it);
	}
}

ErrorCode Stream::Write()
{
	char buf[BUFSIZE];
	CHECK_RESULT;
	OKorRETURN(OpenObj());
	OKorRETURN(WriteDictPart());
	OKorRETURN(m_mgr->WriteString(LENGTHKEY));
	sprintf_s(buf, "%d\r", m_streamSize);
	OKorRETURN(m_mgr->WriteString(buf));
	OKorRETURN(m_mgr->WriteString(BEGINSTREAM));
	unsigned long bytesLeft = m_streamSize;
	for (std::vector<char*>::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it)
	{
		if (bytesLeft >= BLOCKSIZE) {
			OKorRETURN(m_mgr->WriteBinary((*it), BLOCKSIZE));
			bytesLeft -= BLOCKSIZE;
			continue;
		}
		OKorRETURN(m_mgr->WriteBinary((*it), bytesLeft));
	}
	OKorRETURN(m_mgr->WriteString(ENDSTREAM));
	OKorRETURN(CloseObj());
	return NoError;
}

void Stream::AddStream(const void* buffer, unsigned long bytes) 
{
	char* startPos = (m_blocks.empty()) ? NULL
		: (*(m_blocks.rbegin())) + BLOCKSIZE - m_freeInBlock;
	if (bytes > m_freeInBlock) {
		if (startPos) {
			memcpy(startPos, buffer, m_freeInBlock);
			m_streamSize += m_freeInBlock;
		}
		unsigned long bytesLeft = bytes - m_freeInBlock;
		char* bufferLeft = (char*)buffer + m_freeInBlock;
		m_blocks.push_back(new char[BLOCKSIZE]);
		m_freeInBlock = BLOCKSIZE;
		AddStream(bufferLeft, bytesLeft);
		return;
	}
	memcpy(startPos, buffer, bytes);
	m_streamSize += bytes;
	m_freeInBlock -= bytes;
	return;
}

void Stream::AddStream(const char* buffer)
{
	AddStream(buffer, strlen(buffer));
}