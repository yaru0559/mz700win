#pragma once

#include "ObjMgr.h"
#include <vector>

class PDFPlotter
{
public:
	PDFPlotter();
	~PDFPlotter();

	void SetPenColor(int i);
	void SetLineStyle(int i);
	void Move(int x, int y);
	void Draw(int x, int y);
	void Print(const char* str, int size, int rotate);
	void CloseFile();
	void EndPage();
	int PenX() { return m_penx; }
	int PenY() { return m_peny; }

private:
	void NewFile();
	void NewPage();
	void SetLineGState();


private:
	TPDFL::ObjMgr* m_mgr;
	TPDFL::Dict* m_pages;
	TPDFL::Array* m_pagesArray;
	TPDFL::Dict* m_currentPage;
	TPDFL::Stream* m_currentContents;
	int m_numPages;

	bool m_penDown;
	int m_penColor;
	int m_lineStyle;
	int m_penx;
	int m_peny;
	int m_ymin;
	int m_ymax;

};