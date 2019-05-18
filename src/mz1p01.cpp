/// MZ-1P01 simulator implementation
// Copyright(C) AOKI Yasuharu 2019 

#include <windows.h>
#include <fstream>
#include <string>
#include <vector>

#include "PDFPlotter.h"

#define DUMP 1

// The plotter emulator is implemented as a class.

class MZ1P01
{
public:
	MZ1P01();
	~MZ1P01();
	UINT8	GetFlag();
	void	SendCtrl(int value);
	void	SendData(int value);
	void	Eject();
	void	Reset();

private:
	void ResetInterpreter();
	void ResetStatus();

	void TextInterpret(int value);
	void GraphInterpret(int value);

	// Printer Commands
	void TextCode();
	void GraphicCode();
	void LineUp();
	void PenTest();
	void ReductionScale();
	void ReductionCancel();
	void LineCounterSet(int value);
	void LineFeed();
	void MagnifyScale();
	void MagnifyCancel();
	void CarriageReturn();
	void BackSpace();
	void FormFeed();
	void NextColor();
	void TextPrint(int value);

	void LineType(int value);
	void AllInitialize();
	void Home();
	void HSet();
	void Draw(int x, int y);
	void Move(int x, int y);
	void RelativeDraw(int x, int y);
	void RelativeMove(int x, int y);
	void ColorChange(int value);
	void ScaleSet(int value);
	void AlphaRotate(int value);

	void Print(const std::string& str);
	void Axis(int p, int q, int r);

	// Sent letter handling
	void Handle09();
	void Handle0B();
	void Handle0D();
	void HandleMinus();
	void HandleNumber(int value);
	void HandleComma();
	void HandleMisc(int value);

	void ChooseGraphCommand(int value);

private:
#if DUMP
	std::ofstream	m_ofs;
	std::ofstream	m_ofs2;
#endif
	// IO handshake
	UINT8	m_flag;

	// Plotter instance
	PDFPlotter	m_plotter;

	// Printer status
	int m_mode;				// 0:Text mode  1:Graphic mode		
	int m_lineCounter;
	int m_linesPerPage;
	int m_scale;
	int m_textRotate;		// 0:0 deg.  1: 90 deg. clockwize  2: 180 deg.  3:270 deg. clockwize
	int m_relx;				// relative position of the pen from home
	int m_rely;				// relative position of the pen from home
	int m_homex;			// absolute position of the home
	int m_homey;			// absolute position of the home

	// Command interpretation status
	enum {
		IStatReady,
		IStat09_1,		// Received $09 once
		IStat09_2,		// Received $09 twice
		IStatGettingNumber,
		IStatGettingText
	} m_interpretStatus;

	enum {
		CmdEmpty,
		CmdLineCounterSet,
		CmdLineType,
		CmdColorChange,
		CmdScaleSet,
		CmdAlphaRotate,
		CmdDraw,
		CmdRelativeDraw,
		CmdMove,
		CmdRelativeMove,
		CmdPrint,
		CmdAxis
	} m_currentCommand;
	std::string m_bufferingStr;
	std::vector<int> m_bufferingNums;
};

// The entity of emulator class
static MZ1P01 printer;
	
/* Global interfaces for C caller */
extern "C" {
	UINT8 MZPGetFlag()
	{
		return printer.GetFlag();
	}

	void MZPSendCtrl(int value)
	{
		printer.SendCtrl(value);
	}

	void MZPSendData(int value)
	{
		printer.SendData(value);
	}

	void MZPEjectPaper()
	{
		printer.Eject();
	}

	void MZPReset()
	{
		printer.Reset();
	}
}

/// Implementation

MZ1P01::MZ1P01()
	: m_flag(0), m_mode(0)
{
#if DUMP
	m_ofs = std::ofstream("./DUMP.dat", std::ios::binary);
	m_ofs2 = std::ofstream("./DUMP_Command.dat", std::ios::binary);
#endif
	ResetStatus();

/*	PDFPlotter p;
	p.Move(0, 0);
	p.Draw(0, 100);
	p.Draw(100, 100);
	p.SetPenColor(2);
	p.Draw(100, 0);
	p.Draw(0, 0);
	p.EndPage();
	p.SetPenColor(3);
	p.SetLineStyle(1);
	p.Move(100, 100);
	p.Draw(200, 100);
	p.Draw(200, 200);
	p.Draw(100, 200);
	p.Draw(100, 100); */
}

MZ1P01::~MZ1P01()
{
#if DUMP
	m_ofs.close();
	m_ofs2.close();
#endif
}

UINT8 MZ1P01::GetFlag()
{
	return m_flag;
}

void MZ1P01::SendCtrl(int value)
{
	m_flag = (value & 0x80) ? 1 : 0;
}

void MZ1P01::SendData(int value)
{
#if DUMP
	char word = static_cast<char>(0xff & value);
	m_ofs << word;
#endif

	if (0 == m_mode) {
		TextInterpret(value);
	}
	else {
		GraphInterpret(value);
	}
}

void MZ1P01::Eject()
{
	m_plotter.CloseFile();
	m_homex = m_homey = 0;
}

void MZ1P01::Reset()
{
	m_plotter.CloseFile();
	m_plotter.SetPenColor(0);
	ResetStatus();
}

void MZ1P01::TextInterpret(int value)
{
	switch (value)
	{
	case 0x01:
		TextCode();
		ResetInterpreter();
		break;
	case 0x02:
		GraphicCode();
		ResetInterpreter();
		break;
	case 0x03:
		LineUp();
		ResetInterpreter();
		break;
	case 0x04:
		PenTest();
		ResetInterpreter();
		break;
	case 0x09:
		Handle09();
		break;
	case 0x0a:
		LineFeed();
		ResetInterpreter();
		break;
	case 0x0b:
		Handle0B();
		break;
	case 0x0c:
		MagnifyCancel();
		ResetInterpreter();
		break;
	case 0x0d:
		Handle0D();
		break;
	case 0x0e:
		BackSpace();
		ResetInterpreter();
		break;
	case 0x0f:
		FormFeed();
		ResetInterpreter();
		break;
	case 0x1d:
		NextColor();
		ResetInterpreter();
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		HandleNumber(value);
		break;
	default:
		TextPrint(value);
	}
}

void MZ1P01::GraphInterpret(int value)
{
	switch (value)
	{
	case 0x01:
		TextCode();
		ResetInterpreter();
		break;
	case 0x02:
		GraphicCode();
		ResetInterpreter();
		break;
	case 0x03:
		LineUp();
		ResetInterpreter();
		break;
	case 0x04:
		PenTest();
		ResetInterpreter();
		break;
	case 0x0d:
		Handle0D();
		break;
	case ',':
		HandleComma();
		break;
	case '-':
		HandleMinus();
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		HandleNumber(value);
		break;
	default:
		HandleMisc(value);
	}
}

void MZ1P01::ResetInterpreter()
{
	m_interpretStatus = IStatReady;
	m_currentCommand = CmdEmpty;
	m_bufferingStr.clear();
	m_bufferingNums.clear();
}

void MZ1P01::ResetStatus()
{
	m_mode = 0;
	m_lineCounter = 0;
	m_linesPerPage = 66;
	m_scale = 1;
	m_textRotate = 0;
	m_relx = 0;
	m_rely = 0;
	m_homex = 0;
	m_homey = 0;
}

void MZ1P01::Handle09()
{
	switch (m_interpretStatus)
	{
	case IStatReady:
		m_interpretStatus = IStat09_1;
		break;
	case IStat09_1:
		m_interpretStatus = IStat09_2;
		break;
	case IStat09_2:
		ReductionScale();
		ResetInterpreter();
		break;
	default:
		ResetInterpreter();
		m_interpretStatus = IStat09_1;
		break;
	}
}

void MZ1P01::Handle0B()
{
	if (IStat09_2 == m_interpretStatus) {
		ReductionCancel();
		ResetInterpreter();
	} else {
		MagnifyScale();
		ResetInterpreter();
	}
}

void MZ1P01::Handle0D()
{
	if (CmdPrint == m_currentCommand) {
		Print(m_bufferingStr);
	}
	else if (IStatGettingNumber == m_interpretStatus) {
		int i;
		sscanf_s(m_bufferingStr.c_str(), "%d", &i); 
		i %= 1000;
		m_bufferingNums.push_back(i);
		std::vector<int>::iterator it = m_bufferingNums.begin();
		switch (m_currentCommand)
		{
		case CmdLineCounterSet:
			if (0 < *it && 256 > *it) { LineCounterSet(*it); }
			break;
		case CmdLineType:
			LineType(*it);
			break;
		case CmdColorChange:
			ColorChange(*it);
			break;
		case CmdScaleSet:
			ScaleSet(*it);
			break;
		case CmdAlphaRotate:
			AlphaRotate(*it);
			break;
		case CmdDraw:
			if (2 <= m_bufferingNums.size()) {
				int x = *it++;
				int y = *it;
				Draw(x, y);
			}
			break;
		case CmdMove:
			if (2 <= m_bufferingNums.size()) {
				int x = *it++;
				int y = *it;
				Move(x, y);
			}
			break;
		case CmdRelativeDraw:
			if (2 <= m_bufferingNums.size()) {
				int x = *it++;
				int y = *it;
				RelativeDraw(x, y);
			}
			break;
		case CmdRelativeMove:
			if (2 <= m_bufferingNums.size()) {
				int x = *it++;
				int y = *it;
				RelativeMove(x, y);
			}
			break;
		case CmdAxis:
			if (3 <= m_bufferingNums.size()) {
				int p = *it++;
				int q = *it++;
				int r = *it;
				Axis(p, q, r);
			}
			break;
		default:
			break;
		}
	}
	else if (0 == m_mode) {
		CarriageReturn();
	}
	ResetInterpreter();
}

void MZ1P01::HandleMinus()
{
	if ((IStatGettingText == m_interpretStatus)
		|| (IStatGettingNumber == m_interpretStatus && m_bufferingStr.empty())) {
		m_bufferingStr.append("-");
	}
}

void MZ1P01::HandleComma()
{
	if (IStatGettingNumber == m_interpretStatus) {
		int i;
		sscanf_s(m_bufferingStr.c_str(), "%d", &i);
		i %= 1000;
		m_bufferingNums.push_back(i);
		std::vector<int>::iterator it = m_bufferingNums.begin();
		switch (m_currentCommand)
		{
		case CmdLineCounterSet:
			if (0 < *it && 256 > *it) { LineCounterSet(*it); }
			ResetInterpreter();
			break;
		case CmdLineType:
			LineType(*it);
			ResetInterpreter();
			break;
		case CmdColorChange:
			ColorChange(*it);
			ResetInterpreter();
			break;
		case CmdScaleSet:
			ScaleSet(*it);
			ResetInterpreter();
			break;
		case CmdAlphaRotate:
			AlphaRotate(*it);
			ResetInterpreter();
			break;
		default:
			m_bufferingStr.clear();
			break;
		}
	}
	else if (IStatGettingText == m_interpretStatus) {
		m_bufferingStr.push_back(',');
	}
}

void MZ1P01::HandleNumber(int value)
{
	char word = static_cast<char>(0xff & value);
	if (IStat09_2 == m_interpretStatus) {
		m_currentCommand = CmdLineCounterSet;
		m_interpretStatus = IStatGettingNumber;
		m_bufferingStr.push_back(word);
		return;
	}
	else if ((IStatGettingText == m_interpretStatus)
		|| (IStatGettingNumber == m_interpretStatus)) {
		m_bufferingStr.push_back(word);
		return;
	}
	else if (0 == m_mode) {
		TextPrint(value);
	}
}

void MZ1P01::HandleMisc(int value)
{
	if ((1 == m_mode) && (IStatReady == m_interpretStatus)) {
		return ChooseGraphCommand(value);
	}
	else if (IStatGettingText == m_interpretStatus) {
		char word = static_cast<char>(0xff & value);
		m_bufferingStr.push_back(word);
	}
}

void MZ1P01::ChooseGraphCommand(int value)
{
	switch (value)
	{
	case 'L':
		m_currentCommand = CmdLineType;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'A':
		m_mode = 0;
		ResetInterpreter();
		break;
	case 'H':
		Home();
		ResetInterpreter();
		break;
	case 'I':
		HSet();
		ResetInterpreter();
		break;
	case 'D':
		m_currentCommand = CmdDraw;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'J':
		m_currentCommand = CmdRelativeDraw;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'M':
		m_currentCommand = CmdMove;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'R':
		m_currentCommand = CmdRelativeMove;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'C':
		m_currentCommand = CmdColorChange;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'S':
		m_currentCommand = CmdScaleSet;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'Q':
		m_currentCommand = CmdAlphaRotate;
		m_interpretStatus = IStatGettingNumber;
		break;
	case 'P':
		m_currentCommand = CmdPrint;
		m_interpretStatus = IStatGettingText;
		break;
	case 'X':
		m_currentCommand = CmdAxis;
		m_interpretStatus = IStatGettingNumber;
		break;
	default:
		break;
	}
}

void MZ1P01::TextCode()
{
#if DUMP
	m_ofs2 << "Text Code" << std::endl;
#endif
	m_mode = 0;
	m_lineCounter = 0;
	m_scale = 1;
	m_textRotate = 0;
	m_homex = 0;
	m_relx = m_plotter.PenX();
}

void MZ1P01::GraphicCode()
{
#if DUMP
	m_ofs2 << "Graphic Code" << std::endl;
#endif
	m_mode = 1;
	m_homex = 0;
	m_homey = m_plotter.PenY();
	m_relx = m_plotter.PenX();
	m_rely = 0;
}

void MZ1P01::LineUp()
{
#if DUMP
	m_ofs2 << "Line Up" << std::endl;
#endif
	m_plotter.SetPenY(m_plotter.PenY() + 12 * (m_scale + 1));
	if (0 < m_lineCounter) m_lineCounter--;
}

void MZ1P01::PenTest()
{
#if DUMP
	m_ofs2 << "Pen Test" << std::endl;
#endif
	// not implemented
}

void MZ1P01::ReductionScale()
{
#if DUMP
	m_ofs2 << "Reduction Scale" << std::endl;
#endif
	m_scale = 0;
}

void MZ1P01::ReductionCancel()
{
#if DUMP
	m_ofs2 << "Reduction Cancel" << std::endl;
#endif
	m_scale = 1;
}

void MZ1P01::LineCounterSet(int value)
{
#if DUMP
	m_ofs2 << "Line Counter Set (" << value << ")" << std::endl;
#endif
	m_linesPerPage = value;

}

void MZ1P01::LineFeed()
{
#if DUMP
	m_ofs2 << "\n";
#endif
	if (++m_lineCounter < m_linesPerPage) {
		m_plotter.SetPenY(m_plotter.PenY() - 12 * (m_scale + 1));
	}
	else {
		m_plotter.EndPage();
		m_lineCounter = 0;
	}
}

void MZ1P01::MagnifyScale()
{
#if DUMP
	m_ofs2 << "Magnify Scale" << std::endl;
#endif
	m_scale = 2;
}

void MZ1P01::MagnifyCancel()
{
#if DUMP
	m_ofs2 << "Magnify Cancel" << std::endl;
#endif
	m_scale = 1;
}

void MZ1P01::CarriageReturn()
{
#if DUMP
	m_ofs2 << "\r";
#endif
	if (!m_bufferingStr.empty()) {
		m_plotter.Print(m_bufferingStr.c_str(), m_scale, 0);
		m_bufferingStr.clear();
	}
	m_relx = 0;
	m_plotter.SetPenX(0);
	LineFeed();
}

void MZ1P01::BackSpace()
{
#if DUMP
	m_ofs2 << "Back Space" << std::endl;
#endif
	if (!m_bufferingStr.empty()) {
		m_plotter.Print(m_bufferingStr.c_str(), m_scale, 0);
		m_bufferingStr.clear();
	}
	int penX = m_plotter.PenX() - 6 * (m_scale + 1);
	m_plotter.SetPenX(0 < penX ? penX : 0);
}

void MZ1P01::FormFeed()
{
#if DUMP
	m_ofs2 << "Form Feed" << std::endl;
#endif
	int linesToFeed = m_linesPerPage - m_lineCounter - 1;
	int y = m_plotter.PenY() - 12 * (m_scale + 1) * linesToFeed;
	m_plotter.Move(0, y);
	m_plotter.EndPage();
}

void MZ1P01::NextColor()
{
#if DUMP
	m_ofs2 << "Next Color" << std::endl;
#endif
	int nextColor = (m_plotter.PenColor() + 1) % 4;
	m_plotter.SetPenColor(nextColor);
}

void MZ1P01::TextPrint(int value)
{
	char word = static_cast<char>(0xff & value);
#if DUMP
		m_ofs2 << word;
#endif
	int textWidth = 6 * (m_scale + 1);
	if (0 == m_plotter.CharProperty(word)) {
		textWidth *= 2;
	}
	if (m_relx + textWidth > 480) {
		CarriageReturn();
	}
	m_bufferingStr.append(1, word);
	m_relx += textWidth;
}

void MZ1P01::LineType(int value)
{
#if DUMP
	m_ofs2 << "LINE TYPE " << value << std::endl;
#endif
	m_plotter.SetLineStyle(value);
}

void MZ1P01::AllInitialize()
{
#if DUMP
	m_ofs2 << "ALL INITIALIZE" << std::endl;
#endif
	m_mode = 0;
	m_lineCounter = 0;
	m_scale = 1;
	m_textRotate = 0;
	m_homex = 0;
}

void MZ1P01::Home()
{
#if DUMP
	m_ofs2 << "HOME" << std::endl;
#endif
	m_plotter.Move(m_homex, m_homey);
}

void MZ1P01::HSet()
{
#if DUMP
	m_ofs2 << "HSET" << std::endl;
#endif
	m_homex = m_plotter.PenX();
	m_homey = m_plotter.PenY();
	m_relx = 0;
	m_rely = 0;
}

void MZ1P01::Draw(int x, int y)
{
#if DUMP
	m_ofs2 << "DRAW " << x << ", " << y << std::endl;
#endif
	m_relx = x;
	m_rely = y;
	m_plotter.Draw(m_homex + m_relx, m_homey + m_rely);
}

void MZ1P01::Move(int x, int y)
{
#if DUMP
	m_ofs2 << "MOVE " << x << ", " << y << std::endl;
#endif
	m_relx = x;
	m_rely = y;
	m_plotter.Move(m_homex + m_relx, m_homey + m_rely);
}

void MZ1P01::RelativeDraw(int x, int y)
{
#if DUMP
	m_ofs2 << "RELATIVE DRAW " << x << ", " << y << std::endl;
#endif
	m_relx += x;
	m_rely += y;
	m_plotter.Draw(m_homex + m_relx, m_homey + m_rely);
}

void MZ1P01::RelativeMove(int x, int y)
{
#if DUMP
	m_ofs2 << "RELATIVE MOVE " << x << ", " << y << std::endl;
#endif
	m_relx += x;
	m_rely += y;
	m_plotter.Move(m_homex + m_relx, m_homey + m_rely);
}

void MZ1P01::ColorChange(int value)
{
#if DUMP
	m_ofs2 << "COLOR CHANGE " << value << std::endl;
#endif
	m_plotter.SetPenColor(value);
}

void MZ1P01::ScaleSet(int value)
{
#if DUMP
	m_ofs2 << "SCALE SET " << value << std::endl;
#endif
	m_scale = value;
}

void MZ1P01::AlphaRotate(int value)
{
#if DUMP
	m_ofs2 << "ALPHA ROTATE " << value << std::endl;
#endif
	m_textRotate = value;
}

void MZ1P01::Print(const std::string& str)
{
#if DUMP
	m_ofs2 << "PRINT " << str << std::endl;
#endif
	m_plotter.Print(str.c_str(), m_scale, m_textRotate);
}

void MZ1P01::Axis(int p, int q, int r)
{
#if DUMP
	m_ofs2 << "AXIS " << p << ", " << q << ", " << r << std::endl;
#endif
	m_plotter.Axis((0 == p), q, r);
}

