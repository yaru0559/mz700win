#include "PDFPlotter.h"
#include "Dict.h"
#include "Stream.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

#define NOMINMAX
#include <Windows.h>

using namespace TPDFL;

static const char* PENCOLOR[] = {
	"0 0 0 1 K\n",
	"1 1 0 0 K\n",
	"1 0 1 0 K\n",
	"0 1 1 0 K\n"
};

PDFPlotter::PDFPlotter()
	: m_mgr(NULL), m_pages(NULL), m_pagesArray(NULL), m_currentPage(NULL), m_currentContents(NULL), m_numPages(0)
	, m_penDown(false), m_penColor(0), m_lineStyle(0), m_penx(0), m_peny(0), m_ymin(0), m_ymax(0)
{}

PDFPlotter::~PDFPlotter()
{
	if (m_currentPage) {
		EndPage();
	}
	if (m_mgr) {
		CloseFile();
	}
}

void PDFPlotter::NewFile()
{
	time_t t = time(NULL);
	const tm* lt = localtime(&t);
	char buf[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char folder[_MAX_DIR];
	char file[_MAX_FNAME];
	char extent[_MAX_EXT];
	::GetModuleFileName(NULL, buf, _MAX_PATH);
	if (0 == _splitpath_s(buf, drive, folder, file, extent)) {
		sprintf_s(buf, "%s%s%02d%02d%02d%02d%02d%02d.pdf", drive, folder, 
			lt->tm_year % 100, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	}
	else {
		sprintf_s(buf, "./%02d%02d%02d%02d%02d%02d.pdf", 
			lt->tm_year % 100, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	}

	FILE* fp(NULL);
	fopen_s(&fp, buf, "wb");
	if (NULL == fp) return;
	m_mgr = new ObjMgr(fp);
	Dict* catalog = new Dict(m_mgr);
	catalog->Add("Type", "/Catalog");
	m_mgr->SetRootObj(catalog);
	Dict* info = new Dict(m_mgr);
	m_mgr->SetInfoObj(info);
	m_pages = new Dict(m_mgr);
	m_pages->Add("Type", "/Pages");
	catalog->Add("Pages", m_pages);
	m_pagesArray = new Array(m_mgr);
	m_pages->Add("Kids", m_pagesArray);
}

void PDFPlotter::NewPage()
{
	m_currentPage = new Dict(m_mgr);
	m_currentPage->Add("Type", "/Page");
	m_currentPage->Add("Parent", m_pages);
	m_pagesArray->Append(m_currentPage);
	m_currentContents = new Stream(m_mgr);
	m_currentPage->Add("Contents", m_currentContents);
	m_numPages++;
	m_penx = m_peny = m_ymin = m_ymax = 0;
	m_penDown = false;
}

void PDFPlotter::SetLineGState()
{
	std::ostringstream oss;
	oss << "0.75 w 1 J 1 j" << std::endl;
	if (0 < m_lineStyle) {
		oss << "[" << (double)m_lineStyle * 1.5 << "] 0 d ";
	}
	m_currentContents->AddStream(oss.str().c_str());
	m_currentContents->AddStream(PENCOLOR[m_penColor]);
}

void PDFPlotter::CloseFile()
{
	EndPage();
	if (m_mgr) {
		std::ostringstream oss;
		oss << m_numPages;
		m_pages->Add("Count", oss.str().c_str());
		m_mgr->Finalize();
		delete m_mgr; m_mgr = NULL;
		m_pages = NULL;
		m_pagesArray = NULL;
		m_currentPage = NULL;
		m_currentContents = NULL;
	}
}

void PDFPlotter::EndPage()
{
	if (m_currentPage) {
		std::ostringstream oss;
		if (m_currentPage && m_penDown) {
			m_currentContents->AddStream("S\n");
		}
		oss << "[ -20 " << m_ymin - 20 << " 500 " << m_ymax + 20 << " ]";
		m_currentPage->Add("MediaBox", oss.str().c_str());
		m_currentPage = NULL;
		m_currentContents = NULL;
	}
}

void PDFPlotter::SetPenColor(int i)
{
	m_penColor = i % 4;
	if (m_penDown) {
		Move(m_penx, m_peny);
	}
}

void PDFPlotter::SetLineStyle(int i)
{
	m_lineStyle = (0 > i || 15 < i) ? 0 : i;
	if (m_penDown) {
		Move(m_penx, m_peny);
	}
}

void PDFPlotter::Draw(int x, int y)
{
	if (!m_mgr) {
		NewFile();
	}
	if (!m_currentPage) {
		NewPage();
		m_currentContents->AddStream("0 0 m\n");
	}
	if (!m_penDown) {
		SetLineGState();
		m_penDown = true;
	}
	std::ostringstream oss;
	oss << x << " " << y << " l" << std::endl;
	m_currentContents->AddStream(oss.str().c_str());
	m_penx = x;
	m_peny = y;
	m_ymin = std::min(m_ymin, y);
	m_ymax = std::max(m_ymax, y);
}

void PDFPlotter::Move(int x, int y)
{
	if (!m_mgr) {
		NewFile();
	}
	if (!m_currentPage) {
		NewPage();
	}
	std::ostringstream oss;
	if (m_penDown) {
		oss << "S" << std::endl;
		m_penDown = false;
	}
	oss << x << " " << y << " m" << std::endl;
	m_currentContents->AddStream(oss.str().c_str());
	m_penx = x;
	m_peny = y;
	m_ymin = std::min(m_ymin, y);
	m_ymax = std::max(m_ymax, y);
}

void PDFPlotter::Print(const char* str, int size, int rotate)
{
	if (!m_mgr) {
		NewFile();
	}
	if (!m_currentPage) {
		NewPage();
	}
	std::ostringstream oss;
	if (m_penDown) {
		oss << "S" << std::endl;
		m_penDown = false;
	}

	// Not yet implemented
	m_penDown = false;
}