// Copyright(C) AOKI Yasuharu 2019 

#pragma once

#include "ObjMgr.h"
#include <vector>

class PDFPlotter
{
public:
	PDFPlotter();
	~PDFPlotter();

	static int CharProperty(const char c);	// 0: Not Printable  1: Printable  2: Printable but need to escape
	void SetPenColor(int i);
	void SetLineStyle(int i);
	void Move(int x, int y);
	void Draw(int x, int y);
	void Print(const char* str, int size, int rotate);
	void Axis(bool vertical, int pitch, int count);
	void CloseFile();
	void EndPage();
	void SetPenX(int x);
	void SetPenY(int y);
	int PenColor() { return m_penColor; }
	int PenX() { return m_penx; }
	int PenY() { return m_peny; }

private:
	void NewFile();
	void NewPage();
	void SetLineGState();
	void ConstructFontDict(int size);
	void AddGryph(TPDFL::Dict* charProcs, const char* key, const char* gryphBody, int size);
	void PrintLine(const char* str, int size, int rotate);

private:
	TPDFL::ObjMgr* m_mgr;
	TPDFL::Dict* m_pages;
	TPDFL::Array* m_pagesArray;
	TPDFL::Dict* m_currentPage;
	TPDFL::Stream* m_currentContents;
	TPDFL::Dict* m_fontPool;
	TPDFL::Dict* m_fontDicts[64];
	TPDFL::Dict* m_fontEncoding;

	int m_numPages;

	bool m_penDown;
	int m_penColor;
	int m_lineStyle;
	int m_penx;
	int m_peny;
	int m_ymin;
	int m_ymax;

};