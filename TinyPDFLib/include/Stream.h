// Copyright(C) AOKI Yasuharu 2019 

#pragma once

#include "Dict.h"

namespace TPDFL {
	class Stream : public Dict
	{
	public:
		Stream(ObjMgr* mgr);
		virtual ~Stream();

		virtual ErrorCode Write();
		void AddStream(const void* buffer, unsigned long bytes);
		void AddStream(const char* buffer);

	private:
		Stream();

	protected:
		bool m_writing;
		unsigned long m_streamSize;
		unsigned long m_freeInBlock;
		std::vector<char*> m_blocks;
	};
}