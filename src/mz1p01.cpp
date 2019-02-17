/// MZ-1P01 simulator implementation
#include <windows.h>
#include <fstream>

// The plotter emulator is implemented as a class.

class MZ1P01
{
public:
	MZ1P01() { m_ofs = std::ofstream("./DUMP.dat"); };
	~MZ1P01() { m_ofs.close(); };
	UINT8	GetStatus() { return m_status; }
	void	SendCtrl(int value);
	void	SendData(int value);

private:
	UINT8			m_status;
	std::ofstream	m_ofs;
};

// The entity of emulator class
static MZ1P01 printer;
	
/* Global interfaces for C caller */
extern "C" {
	UINT8 MZPGetStatus()
	{
		return printer.GetStatus();
	}

	void MZPSendCtrl(int value)
	{
		printer.SendCtrl(value);
	}

	void MZPSendData(int value)
	{
		printer.SendData(value);
	}
}

/// Implementation
void MZ1P01::SendCtrl(int value)
{
	m_status = (value & 0x80) ? 1 : 0;
}

void MZ1P01::SendData(int value)
{
	char word = static_cast<char>(0xff & value);
	m_ofs << word;
}



