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

		// オブジェクト番号の発行
		unsigned long IssueId(Object* obj);

		// カタログ辞書を指定する
		void SetRootObj(Object* catalog);

		// Info辞書を指定する
		void SetInfoObj(Object* info);

		// すべての未書き込みオブジェクトをファイルに書き出す
		ErrorCode Finalize();

	private:
		// 指定サイズ書き込み
		ErrorCode WriteBinary(const void* buffer, unsigned long bytes);

		// 文字列書き込み
		ErrorCode WriteString(const char* buffer);

	private:
		ObjMgr();

		// xrefとtrailerを書き込む
		ErrorCode	Trailer();

		// ファイル中の特定位置を書き換える(C文字列専用)
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