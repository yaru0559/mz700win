#pragma once

#include <stdio.h>
#include <vector>
#include "ErrorCode.h"

namespace TPDFL {

	class Object;

	class ObjMgr
	{
		friend class Object;
		friend class DirectValue;
		friend class Reference;
		friend class Array;
		friend class Dict;
		friend class Stream;

	public:
		ObjMgr(FILE* fp);
		~ObjMgr();

		// �I�u�W�F�N�g�ԍ��̔��s
		unsigned long IssueId(Object* obj);

		// �J�^���O�������w�肷��
		void SetRootObj(Object* catalog);

		// Info�������w�肷��
		void SetInfoObj(Object* info);

		// ���ׂĂ̖��������݃I�u�W�F�N�g���t�@�C���ɏ����o��
		ErrorCode Finalize();

	private:
		// �w��T�C�Y��������
		ErrorCode WriteBinary(const void* buffer, unsigned long bytes);

		// �����񏑂�����
		ErrorCode WriteString(const char* buffer);

	private:
		ObjMgr();

		// xref��trailer����������
		ErrorCode	Trailer();

		// �t�@�C�����̓���ʒu������������(C�������p)
		ErrorCode OverwriteString(long pos, const char* buffer);

	private:
		FILE* m_fp;
		unsigned long m_currentBytes;
		unsigned long m_nextId;
		unsigned long m_rootObjId;
		unsigned long m_infoObjId;
		std::vector<unsigned long>	m_offsets;
		std::vector<Object*> m_objects;

	};
}