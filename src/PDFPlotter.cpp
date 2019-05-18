// Copyright(C) AOKI Yasuharu 2019 

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
	"0 0 0 1 K 0 0 0 1 k\n",
	"1 0.5 0 0 K 1 0.5 0 0 k\n",
	"1 0 1 0 K 1 0 1 0 k\n",
	"0 1 1 0 K 0 1 1 0 k\n"
};

static const char Differences[] = "[17 /CrslDn /CrslUp /CrslR /CrslL /Home /Clear 33 /Excl /DQuot /Hash /Doller /Pct /Amp /SQuot /PrnthsL /PrnthsR /Astrsk " \
"/Plus /Comma /Minus /Dot /Slash /Zero /One /Two /Three /Four /Five /Six /Seven /Eight /Nine /Colon /Smcln /Smaller /Equal /Greater /Question /At "\
"/A /B /C /D /E /F /G /H /I /J /K /L /M /N /O /P /Q /R /S /T /U /V /W /X /Y /Z /OpBrkt /Bkslsh /ClBrkt /Up /Left "\
"112 /Sun /Mon /Tue /Wed /Thu /Fri /Sat /Birth /Year /Hour /Min /Sec /En /Yen /Pound /Play /Down /Maru /OpKagi /ClKagi /Ten /Pochi "\
"/KanaWo /SKanaA /SKanaI /SKanaU /SKanaE /SKanaO /SKanaYa /SKanaYu /SKanaYo /SKanaTsu /KanaBar /KanaA /KanaI /KanaU /KanaE /KanaO "\
"/KanaKa /KanaKi /KanaKu /KanaKe /KanaKo /KanaSa /KanaShi /KanaSu /KanaSe /KanaSo /KanaTa /KanaChi /KanaTsu /KanaTe /KanaTo "\
"/KanaNa /KanaNi /KanaNu /KanaNe /KanaNo /KanaHa /KanaHi /KanaFu /KanaHe /KanaHo /KanaMa /KanaMi /KanaMu /KanaMe /KanaMo /KanaYa /KanaYu /KanaYo "\
"/KanaRa /KanaRi /KanaRu /KanaRe /KanaRo /KanaWa /KanaN /KanaDaku /KanaHan /Right 207 /Under 215 /Over 255 /Pi]";
static const char Widths[] = "[6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 "\
"6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 "\
"6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 "\
"6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 "\
"6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 "\
"6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6 6]";

static const char GryphPrefix[] = "6 0 0 -0.375 6 8 d1 q [] 0 d 1 J 1 j ";
static const char GryphPostfix[] = " Q";

static const char ProcCrslDn[] = "1 1 4 6 re 3 6 m 3 2 l 2 3 m 3 2 l 4 3 l S";
static const char ProcCrslUp[] = "1 1 4 6 re 3 6 m 3 2 l 2 5 m 3 6 l 4 5 l S";
static const char ProcCrslR[] = "1 1 4 6 re 2 4 m 4 4 l 3 5 m 4 4 l 3 3 l S";
static const char ProcCrslL[] = "1 1 4 6 re 2 4 m 4 4 l 3 5 m 2 4 l 3 3 l S";
static const char ProcHome[] = "1 1 4 6 re 2 2 m 2 6 l 4 6 m 4 2 l 2 4 m 4 4 l S";
static const char ProcClear[] = "1 1 4 6 re 4 5 m 3 6 l 2 5 l 2 3 l 3 2 l 4 3 l S";
static const char ProcExcl[] = "2 1 m 4 1 l 3 2 l 2 1 l 3 3 m 2 4 l 2 7 l 4 7 l 4 4 l 3 3 l S";
static const char ProcDQuot[] = "2 7 m 2 6 l 4 7 m 4 6 l S";
static const char ProcHash[] = "1 5 m 5 5 l 5 3 m 1 3 l 2 1 m 2 7 l 4 7 m 4 1 l S";
static const char ProcDoller[] = "5 5 m 4 6 l 2 6 l 1 5 l 2 4 l 4 4 l 5 3 l 4 2 l 2 2 l 1 3 l 3 1 m 3 7 l S";
static const char ProcPct[] = "2 5 1 1 re 3 2 1 1 re 5 6 m 1 2 l S";
static const char ProcAmp[] = "5 1 m 1 5 l 1 6 l 2 7 l 3 6 l 3 5 l 1 3 l 1 2 l 2 1 l 3 1 l 5 3 l S";
static const char ProcSQuot[] = "4 7 m 2 5 l S";
static const char ProcPrnthsL[] = "4 7 m 3 7 l 2 6 l 2 2 l 3 1 l 4 1 l S";
static const char ProcPrnthsR[] = "2 7 m 3 7 l 4 6 l 4 2 l 3 1 l 2 1 l S";
static const char ProcAstrsk[] = "3 7 m 3 1 l 1 2 m 5 6 l 1 6 m 5 2 l S";
static const char ProcPlus[] = "1 4 m 5 4 l 3 6 m 3 2 l S";
static const char ProcComma[] = "2 0 m 3 1 l 3 3 l 2 3 l 2 2 l 3 2 l S";
static const char ProcMinus[] = "1 4 m 5 4 l S";
static const char ProcDot[] = "2 1 1 1 re S";
static const char ProcSlash[] = "5 6 m 1 2 l S";
static const char ProcZero[] = "4 1 m 2 1 l 1 2 l 1 6 l 2 7 l 4 7 l 5 6 l 5 2 l 4 1 l 1 2 m 5 6 l S";
static const char ProcOne[] = "2 6 m 3 7 l 3 1 l S";
static const char ProcTwo[] = "5 1 m 1 1 l 5 5 l 5 6 l 4 7 l 2 7 l 1 6 l S";
static const char ProcThree[] = "1 6 m 2 7 l 4 7 l 5 6 l 5 5 l 4 4 l 3 4 l 4 4 m 5 3 l 5 2 l 4 1 l 2 1 l 1 2 l S";
static const char ProcFour[] = "4 1 m 4 7 l 1 4 l 1 3 l 5 3 l S";
static const char ProcFive[] = "5 7 m 1 7 l 1 5 l 4 5 l 5 4 l 5 2 l 4 1 l 2 1 l 1 2 l S";
static const char ProcSix[] = "5 6 m 4 7 l 2 7 l 1 6 l 1 2 l 2 1 l 4 1 l 5 2 l 5 4 l 4 5 l 2 5 l 1 4 l S";
static const char ProcSeven[] = "1 7 m 5 7 l 5 5 l 1 1 l S";
static const char ProcEight[] = "2 4 m 4 4 l 5 5 l 5 6 l 4 7 l 2 7 l 1 6 l 1 5 l 2 4 l 1 3 l 1 2 l 2 1 l 4 1 l 5 2 l 5 3 l 4 4 l S";
static const char ProcNine[] = "1 2 m 2 1 l 4 1 l 5 2 l 5 6 l 4 7 l 2 7 l 1 6 l 1 5 l 2 4 l 4 4 l 5 5 l S";
static const char ProcColon[] = "2 5 1 1 re 2 2 1 1 re S";
static const char ProcSmcln[] = "2 5 1 1 re 2 2 1 1 re 3 2 m 3 1 l 2 0 l S";
static const char ProcSmaller[] = "5 7 m 2 4 l 5 1 l S";
static const char ProcEqual[] = "1 5 m 5 5 l 5 3 m 1 3 l S";
static const char ProcGreater[] = "1 7 m 4 4 l 1 1 l S";
static const char ProcQuestion[] = "1 5 m 1 6 l 2 7 l 4 7 l 5 6 l 5 5 l 4 4 l 3 4 l 3 3 l 3 2 m 3 1 l S";
static const char ProcAt[] = "5 1 m 2 1 l 1 2 l 1 6 l 2 7 l 4 7 l 5 6 l 5 3 l 4 3 l 4 5 l 3 5 l 2 4 l 2 3 l 3 2 l 4 3 l S";
static const char ProcA[] = "1 1 m 1 5 l 3 7 l 5 5 l 5 1 l 1 4 m 5 4 l S";
static const char ProcB[] = "1 1 m 1 7 l 4 7 l 5 6 l 5 5 l 4 4 l 5 3 l 5 2 l 4 1 l 1 1 l 1 4 m 4 4 l S";
static const char ProcC[] = "5 6 m 4 7 l 2 7 l 1 6 l 1 2 l 2 1 l 4 1 l 5 2 l S";
static const char ProcD[] = "1 1 m 1 7 l 4 7 l 5 6 l 5 2 l 4 1 l 1 1 l S";
static const char ProcE[] = "5 7 m 1 7 l 1 1 l 5 1 l 1 4 m 4 4 l S";
static const char ProcF[] = "5 7 m 1 7 l 1 1 l 1 4 m 4 4 l S";
static const char ProcG[] = "5 6 m 4 7 l 2 7 l 1 6 l 1 2 l 2 1 l 4 1 l 5 2 l 3 4 m 5 4 l 5 1 l S";
static const char ProcH[] = "1 1 m 1 7 l 1 4 m 5 4 l 5 7 m 5 1 l S";
static const char ProcI[] = "2 7 m 4 7 l 3 7 m 3 1 l 2 1 m 4 1 l S";
static const char ProcJ[] = "4 7 m 4 2 l 3 1 l 2 1 l 1 2 l 1 3 l S";
static const char ProcK[] = "5 7 m 1 3 l 1 7 m 1 1 l 5 1 m 2 4 l S";
static const char ProcL[] = "1 7 m 1 1 l 5 1 l S";
static const char ProcM[] = "1 1 m 1 7 l 3 5 l 5 7 l 5 1 l S";
static const char ProcN[] = "1 1 m 1 7 l 5 7 m 5 1 l 5 2 m 1 6 l S";
static const char ProcO[] = "4 1 m 2 1 l 1 2 l 1 6 l 2 7 l 4 7 l 5 6 l 5 2 l 4 1 l S";
static const char ProcP[] = "1 1 m 1 7 l 4 7 l 5 6 l 5 5 l 4 4 l 1 4 l S";
static const char ProcQ[] = "4 1 m 2 1 l 1 2 l 1 6 l 2 7 l 4 7 l 5 6 l 5 2 l 4 1 l 3 3 m 5 1 l S";
static const char ProcR[] = "1 1 m 1 7 l 4 7 l 5 6 l 5 5 l 4 4 l 1 4 l 2 4 m 5 1 l S";
static const char ProcS[] = "1 2 m 2 1 l 4 1 l 5 2 l 5 3 l 4 4 l 2 4 l 1 5 l 1 6 l 2 7 l 4 7 l 5 6 l S";
static const char ProcT[] = "1 7 m 5 7 l 3 7 m 3 1 l S";
static const char ProcU[] = "1 7 m 1 1 l 5 1 l 5 7 l S";
static const char ProcV[] = "1 7 m 1 3 l 3 1 l 5 3 l 5 7 l S";
static const char ProcW[] = "1 7 m 1 1 l 3 3 l 3 4 l 3 3 m 5 1 l 5 7 l S";
static const char ProcX[] = "1 1 m 1 2 l 5 6 l 5 7 l 1 7 m 1 6 l 5 2 l 5 1 l S";
static const char ProcY[] = "1 7 m 3 5 l 5 7 l 3 5 m 3 1 l S";
static const char ProcZ[] = "1 7 m 4 7 l 5 6 l 1 2 l 2 1 l 5 1 l 2 5 m 4 3 l S";
static const char ProcOpBrkt[] = "4 7 m 2 7 l 2 1 l 4 1 l S";
static const char ProcBkslsh[] = "1 6 m 5 2 l S";
static const char ProcClBrkt[] = "2 7 m 4 7 l 4 1 l 2 1 l S";
static const char ProcUp[] = "3 1 m 3 7 l 1 5 l 3 7 m 5 5 l S";
static const char ProcLeft[] = "3 6 m 1 4 l 5 4 l 1 4 m 3 2 l S";
static const char ProcSun[] = "2 1 3 6 re 2 4 m 5 4 l S";
static const char ProcMon[] = "1 1 m 2 2 l 2 7 l 5 7 l 5 1 l 5 5 m 2 5 l 2 3 m 5 3 l S";
static const char ProcTue[] = "1 1 m 3 3 l 3 7 l 1 5 m 2 4 l 4 4 m 5 5 l 3 3 m 5 1 l S";
static const char ProcWed[] = "1 2 m 3 4 l 2 5 l 1 5 l 5 6 m 3 4 l 5 2 l 3 7 m 3 1 l 2 2 l S";
static const char ProcThu[] = "1 5 m 5 5 l 3 7 m 3 1 l 1 2 m 3 4 l 5 2 l S";
static const char ProcFri[] = "1 5 m 3 7 l 5 5 l 4 5 m 2 5 l 1 4 m 5 4 l 5 3 m 4 2 l 2 2 m 1 3 l 3 5 m 3 1 l 1 1 m 5 1 l S";
static const char ProcSat[] = "1 4 m 5 4 l 3 7 m 3 1 l 1 1 m 5 1 l S";
static const char ProcBirth[] = "1 4 m 2 5 l 2 6 l 2 5 m 5 5 l 5 3 m 1 3 l 1 1 m 5 1 l 3 7 m 3 1 l S";
static const char ProcYear[] = "1 5 m 2 6 l 2 7 l 2 6 m 5 6 l 4 4 m 2 4 l 2 2 l 3 6 m 3 1 l 1 2 m 5 2 l S";
static const char ProcHour[] = "1 2 1 4 re 1 4 m 2 4 l 2 6 m 4 6 l 3 7 m 3 5 l 2 5 m 5 5 l 5 4 m 2 4 l 4 5 m 4 1 l 3 2 l S";
static const char ProcMin[] = "1 1 m 3 3 l 3 5 l 1 5 m 3 7 l 4 6 l 5 6 l 1 5 m 5 5 l 5 1 l 4 1 l S";
static const char ProcSec[] = "1 6 m 2 7 l 2 1 l 1 2 m 1 4 l 2 5 l 3 4 l 4 5 l 5 5 l 5 3 l 3 1 l 1 5 m 3 5 l 4 7 m 4 3 l S";
static const char ProcEn[] = "1 1 m 1 7 l 5 7 l 5 1 l 4 1 l 3 7 m 3 4 l 1 4 m 5 4 l S";
static const char ProcYen[] = "1 7 m 3 5 l 5 7 l 3 5 m 3 1 l 5 5 m 1 5 l 1 3 m 5 3 l S";
static const char ProcPound[] = "1 5 m 4 5 l 5 6 m 4 7 l 2 7 l 2 1 l 1 1 l 1 3 l 2 3 l 4 1 l 5 1 l S";
static const char ProcPlay[] = "3 7 m 3 1 l 5 3 m 3 1 l 1 3 l 1 1 m 5 1 l S";
static const char ProcDown[] = "3 7 m 3 1 l 1 3 m 3 1 l 5 3 l S";
static const char ProcMaru[] = "2 1 m 1 2 l 2 3 l 3 2 l 2 1 l S";
static const char ProcOpKagi[] = "2 4 m 2 7 l 4 7 l S";
static const char ProcClKagi[] = "2 1 m 4 1 l 4 4 l S";
static const char ProcTen[] = "1 3 m 3 1 l S";
static const char ProcPochi[] = "2 1 m 3 2 l S";
static const char ProcKanaWo[] = "2 1 m 5 4 l 5 7 l 1 7 l 1 5 m 5 5 l S";
static const char ProcSKanaA[] = "1 5 m 4 5 l 2 3 l 2 4 m 2 2 l 1 1 l S";
static const char ProcSKanaI[] = "1 3 m 2 3 l 4 5 l 3 4 m 3 1 l S";
static const char ProcSKanaU[] = "1 3 m 1 4 l 4 4 l 4 3 l 2 1 l 2 5 m 3 4 l S";
static const char ProcSKanaE[] = "1 4 m 5 4 l 3 4 m 3 1 l 1 1 m 5 1 l S";
static const char ProcSKanaO[] = "1 1 m 3 3 l 1 4 m 4 4 l 3 5 m 3 1 l S";
static const char ProcSKanaYa[] = "1 4 m 4 4 l 3 3 l 2 5 m 2 1 l S";
static const char ProcSKanaYu[] = "1 4 m 3 4 l 3 1 l 1 1 m 4 1 l S";
static const char ProcSKanaYo[] = "1 1 m 4 1 l 4 5 l 1 5 l 1 3 m 4 3 l S";
static const char ProcSKanaTsu[] = "1 5 m 1 3 l 2 3 m 2 5 l 4 5 m 4 3 l 2 1 l 1 1 l S";
static const char ProcKanaBar[] = "2 4 m 4 4 l S";
static const char ProcKanaA[] = "1 7 m 5 7 l 5 6 l 3 4 l 3 5 m 3 2 l 2 1 l S";
static const char ProcKanaI[] = "4 7 m 4 6 l 1 3 l 3 5 m 3 1 l S";
static const char ProcKanaU[] = "1 4 m 1 6 l 5 6 l 5 4 l 2 1 l 3 7 m 3 6 l S";
static const char ProcKanaE[] = "1 6 m 5 6 l 3 6 m 3 1 l 1 1 m 5 1 l S";
static const char ProcKanaO[] = "1 6 m 5 6 l 1 2 m 4 5 l 4 7 m 4 1 l S";
static const char ProcKanaKa[] = "1 6 m 5 6 l 5 2 l 4 1 l 2 7 m 2 2 l 1 1 l S";
static const char ProcKanaKi[] = "1 6 m 5 6 l 5 4 m 1 4 l 3 7 m 3 1 l S";
static const char ProcKanaKu[] = "1 5 m 2 6 l 2 7 l 5 7 l 5 5 l 1 1 l S";
static const char ProcKanaKe[] = "1 7 m 1 5 l 1 6 m 5 6 l 4 6 m 4 2 l 3 1 l S";
static const char ProcKanaKo[] = "1 7 m 4 7 l 4 1 l 1 1 l S";
static const char ProcKanaSa[] = "1 6 m 5 6 l 2 7 m 2 4 l 4 7 m 4 3 l 2 1 l S";
static const char ProcKanaShi[] = "1 7 m 3 7 l 3 5 m 1 5 l 1 1 m 3 1 l 5 3 l 5 7 l S";
static const char ProcKanaSu[] = "1 7 m 5 7 l 5 6 l 1 2 l 1 1 l 3 4 m 5 2 l 5 1 l S";
static const char ProcKanaSe[] = "1 6 m 5 6 l 5 5 l 4 4 l 2 7 m 2 2 l 3 1 l 5 1 l S";
static const char ProcKanaSo[] = "1 6 m 2 5 l 2 4 l 5 7 m 5 4 l 2 1 l 1 1 l S";
static const char ProcKanaTa[] = "1 4 m 2 5 l 2 7 l 5 7 l 5 4 l 2 1 l 2 5 m 5 5 l S";
static const char ProcKanaChi[] = "4 7 m 3 6 l 1 6 l 1 4 m 5 4 l 3 6 m 3 2 l 2 1 l S ";
static const char ProcKanaTsu[] = "1 7 m 1 5 l 3 5 m 3 7 l 5 7 m 5 4 l 2 1 l 1 1 l S";
static const char ProcKanaTe[] = "2 7 m 4 7 l 5 5 m 1 5 l 3 5 m 3 2 l 2 1 l S";
static const char ProcKanaTo[] = "2 7 m 2 1 l 2 5 m 4 3 l S";
static const char ProcKanaNa[] = "1 5 m 5 5 l 3 7 m 3 3 l 1 1 l S";
static const char ProcKanaNi[] = "2 6 m 4 6 l 1 1 m 5 1 l S";
static const char ProcKanaNu[] = "1 7 m 5 7 l 5 5 l 1 1 l 2 4 m 5 1 l S";
static const char ProcKanaNe[] = "3 7 m 3 6 l 1 6 m 5 6 l 1 2 l 1 1 l 3 4 m 3 1 l 3 4 m 5 2 l 5 1 l S";
static const char ProcKanaNo[] = "5 7 m 5 5 l 1 1 l S";
static const char ProcKanaHa[] = "1 4 m 1 1 l 3 7 m 5 5 l 5 1 l S";
static const char ProcKanaHi[] = "5 5 m 1 5 l 1 7 m 1 2 l 2 1 l 5 1 l S";
static const char ProcKanaFu[] = "1 7 m 5 7 l 5 4 l 2 1 l 1 1 l S";
static const char ProcKanaHe[] = "1 6 m 2 7 l 5 4 l 5 1 l S";
static const char ProcKanaHo[] = "1 6 m 5 6 l 3 7 m 3 1 l 1 1 m 1 3 l 2 4 l 4 4 m 5 3 l 5 1 l S";
static const char ProcKanaMa[] = "1 7 m 5 7 l 5 5 l 2 2 l 1 3 m 3 1 l S";
static const char ProcKanaMi[] = "2 6 m 4 6 l 2 4 m 4 4 l 1 2 m 4 2 l 5 1 l S";
static const char ProcKanaMu[] = "5 7 m 1 3 l 1 2 l 5 2 l 5 3 m 5 1 l S";
static const char ProcKanaMe[] = "5 7 m 5 5 l 1 1 l 2 4 m 4 2 l S";
static const char ProcKanaMo[] = "1 7 m 5 7 l 1 4 m 5 4 l 3 7 m 3 1 l 5 1 l S";
static const char ProcKanaYa[] = "1 6 m 5 6 l 3 4 l 2 7 m 2 1 l S";
static const char ProcKanaYu[] = "2 7 m 4 7 l 4 1 l 1 1 m 5 1 l S";
static const char ProcKanaYo[] = "1 7 m 5 7 l 5 1 l 1 1 l 1 4  m 5 4 l S";
static const char ProcKanaRa[] = "1 7 m 5 7 l 1 5 m 5 5 l 5 4 l 2 1 l 1 1 l S";
static const char ProcKanaRi[] = "1 7 m 1 4 l 4 7 m 4 3 l 2 1 l S";
static const char ProcKanaRu[] = "1 7 m 1 1 l 3 7 m 3 1 l 5 3 l S";
static const char ProcKanaRe[] = "2 7 m 2 1 l 5 4 l S";
static const char ProcKanaRo[] = "2 1 3 6 re S";
static const char ProcKanaWa[] = "1 5 m 1 7 l 5 7 l 5 4 l 2 1 l S";
static const char ProcKanaN[] = "1 5 m 3 5 l 1 1 m 3 1 l 5 3 l 5 7 l S";
static const char ProcKanaDaku[] = "3 4 m 3 5 l 2 6 l 4 7 m 5 6 l 5 5 l S";
static const char ProcKanaHan[] = "3 7 m 2 6 l 3 5 l 4 6 l 3 7 l S";
static const char ProcRight[] = "3 6 m 5 4 l 3 2 l 1 4 m 5 4 l S ";
static const char ProcUnder[] = "1 1 4 1 re S";
static const char ProcOver[] = "1 6 4 1 re S";
static const char ProcPi[] = "1 7 m 5 7 l 2 7 m 2 2 l 1 1 l 3 7 m 3 1 l 4 1 l 5 2 l S";

// 0:Not Printable, 1:Printable, 2:Printable but needs to escape in PDF
static const char CharPropertyTable[257] =
"0000000000000000"\
"0111111000000000"\
"1111111122111111"\
"1111111111111111"\
"1111111111111111"\
"1111111111112111"\
"0000000000000000"\
"1111111111111111"\
"1111111111111111"\
"1111111111111111"\
"1111111111111111"\
"1111111111111111"\
"1000000000000001"\
"0000000100000000"\
"0000000000000000"\
"0000000000000001";

PDFPlotter::PDFPlotter()
	: m_mgr(NULL), m_pages(NULL), m_pagesArray(NULL), m_currentPage(NULL), m_currentContents(NULL)
	, m_fontPool(NULL), m_fontEncoding(NULL), m_numPages(0)
	, m_penDown(false), m_penColor(0), m_lineStyle(0), m_penx(0), m_peny(0), m_ymin(0), m_ymax(0)
{
	memset(m_fontDicts, 0, sizeof(m_fontDicts));
}

PDFPlotter::~PDFPlotter()
{
	if (m_currentPage) {
		EndPage();
	}
	if (m_mgr) {
		CloseFile();
	}
}

int PDFPlotter::CharProperty(const char c)
{
	int index = static_cast<int>(static_cast<unsigned char>(c));
	return (static_cast<int>(CharPropertyTable[index] - '0'));
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
	m_fontPool = new Dict(m_mgr);
	m_fontEncoding = new Dict(m_mgr);
	m_fontEncoding->Add("Type", "/Encoding");
	m_fontEncoding->Add("Differences", Differences);
}

void PDFPlotter::NewPage()
{
	m_currentPage = new Dict(m_mgr);
	m_currentPage->Add("Type", "/Page");
	m_currentPage->Add("Parent", m_pages);
	m_pagesArray->Append(m_currentPage);
	m_currentContents = new Stream(m_mgr);
	m_currentPage->Add("Contents", m_currentContents);
	Dict* resources = new Dict(m_mgr);
	resources->Add("Font", m_fontPool);
	m_currentPage->Add("Resources", resources);
	m_numPages++;
	m_penx = m_peny = m_ymin = m_ymax = 0;
	m_penDown = false;

	// Test
	ConstructFontDict(1);
}

void PDFPlotter::SetLineGState()
{
	std::ostringstream oss;
	oss << "0.75 w 1 J 1 j [";
	if (0 < m_lineStyle) {
		oss << (double)m_lineStyle * 1.5;
	}
	oss << "] 0 d" << std::endl;;
	m_currentContents->AddStream(oss.str().c_str());
	m_currentContents->AddStream(PENCOLOR[m_penColor]);
}

void PDFPlotter::ConstructFontDict(int size)
{
	char fkey[4];
	sprintf_s(fkey, "F%d", size);
	Dict* fn = new Dict(m_mgr);
	m_fontDicts[size] = fn;
	m_fontPool->Add(fkey, fn);
	fn->Add("Type", "/Font");
	fn->Add("Subtype", "/Type3");
	fn->Add("FirstChar", "17");
	fn->Add("LastChar", "255");
	fn->Add("Encoding", m_fontEncoding);
	fn->Add("FontBBox", "[0 0 6 8]");
	fn->Add("FontMatrix", "[0.125 0 0 0.125 0 0]");
	fn->Add("Widths", Widths);
	Dict* charProcs = new Dict(m_mgr);
	fn->Add("CharProcs", charProcs);
	AddGryph(charProcs, "CrslDn", ProcCrslDn, size);
	AddGryph(charProcs, "CrslUp", ProcCrslUp, size);
	AddGryph(charProcs, "CrslR", ProcCrslR, size);
	AddGryph(charProcs, "CrslL", ProcCrslL, size);
	AddGryph(charProcs, "Home", ProcHome, size);
	AddGryph(charProcs, "Clear", ProcClear, size);
	AddGryph(charProcs, "Excl", ProcExcl, size);
	AddGryph(charProcs, "DQuot", ProcDQuot, size);
	AddGryph(charProcs, "Hash", ProcHash, size);
	AddGryph(charProcs, "Doller", ProcDoller, size);
	AddGryph(charProcs, "Pct", ProcPct, size);
	AddGryph(charProcs, "Amp", ProcAmp, size);
	AddGryph(charProcs, "SQuot", ProcSQuot, size);
	AddGryph(charProcs, "PrnthsL", ProcPrnthsL, size);
	AddGryph(charProcs, "PrnthsR", ProcPrnthsR, size);
	AddGryph(charProcs, "Astrsk", ProcAstrsk, size);
	AddGryph(charProcs, "Plus", ProcPlus, size);
	AddGryph(charProcs, "Comma", ProcComma, size);
	AddGryph(charProcs, "Minus", ProcMinus, size);
	AddGryph(charProcs, "Dot", ProcDot, size);
	AddGryph(charProcs, "Slash", ProcSlash, size);
	AddGryph(charProcs, "Zero", ProcZero, size);
	AddGryph(charProcs, "One", ProcOne, size);
	AddGryph(charProcs, "Two", ProcTwo, size);
	AddGryph(charProcs, "Three", ProcThree, size);
	AddGryph(charProcs, "Four", ProcFour, size);
	AddGryph(charProcs, "Five", ProcFive, size);
	AddGryph(charProcs, "Six", ProcSix, size);
	AddGryph(charProcs, "Seven", ProcSeven, size);
	AddGryph(charProcs, "Eight", ProcEight, size);
	AddGryph(charProcs, "Nine", ProcNine, size);
	AddGryph(charProcs, "Colon", ProcColon, size);
	AddGryph(charProcs, "Smcln", ProcSmcln, size);
	AddGryph(charProcs, "Smaller", ProcSmaller, size);
	AddGryph(charProcs, "Equal", ProcEqual, size);
	AddGryph(charProcs, "Greater", ProcGreater, size);
	AddGryph(charProcs, "Question", ProcQuestion, size);
	AddGryph(charProcs, "At", ProcAt, size);
	AddGryph(charProcs, "A", ProcA, size);
	AddGryph(charProcs, "B", ProcB, size);
	AddGryph(charProcs, "C", ProcC, size);
	AddGryph(charProcs, "D", ProcD, size);
	AddGryph(charProcs, "E", ProcE, size);
	AddGryph(charProcs, "F", ProcF, size);
	AddGryph(charProcs, "G", ProcG, size);
	AddGryph(charProcs, "H", ProcH, size);
	AddGryph(charProcs, "I", ProcI, size);
	AddGryph(charProcs, "J", ProcJ, size);
	AddGryph(charProcs, "K", ProcK, size);
	AddGryph(charProcs, "L", ProcL, size);
	AddGryph(charProcs, "M", ProcM, size);
	AddGryph(charProcs, "N", ProcN, size);
	AddGryph(charProcs, "O", ProcO, size);
	AddGryph(charProcs, "P", ProcP, size);
	AddGryph(charProcs, "Q", ProcQ, size);
	AddGryph(charProcs, "R", ProcR, size);
	AddGryph(charProcs, "S", ProcS, size);
	AddGryph(charProcs, "T", ProcT, size);
	AddGryph(charProcs, "U", ProcU, size);
	AddGryph(charProcs, "V", ProcV, size);
	AddGryph(charProcs, "W", ProcW, size);
	AddGryph(charProcs, "X", ProcX, size);
	AddGryph(charProcs, "Y", ProcY, size);
	AddGryph(charProcs, "Z", ProcZ, size);
	AddGryph(charProcs, "OpBrkt", ProcOpBrkt, size);
	AddGryph(charProcs, "Bkslsh", ProcBkslsh, size);
	AddGryph(charProcs, "ClBrkt", ProcClBrkt, size);
	AddGryph(charProcs, "Up", ProcUp, size);
	AddGryph(charProcs, "Left", ProcLeft, size);
	AddGryph(charProcs, "Sun", ProcSun, size);
	AddGryph(charProcs, "Mon", ProcMon, size);
	AddGryph(charProcs, "Tue", ProcTue, size);
	AddGryph(charProcs, "Wed", ProcWed, size);
	AddGryph(charProcs, "Thu", ProcThu, size);
	AddGryph(charProcs, "Fri", ProcFri, size);
	AddGryph(charProcs, "Sat", ProcSat, size);
	AddGryph(charProcs, "Birth", ProcBirth, size);
	AddGryph(charProcs, "Year", ProcYear, size);
	AddGryph(charProcs, "Hour", ProcHour, size);
	AddGryph(charProcs, "Min", ProcMin, size);
	AddGryph(charProcs, "Sec", ProcSec, size);
	AddGryph(charProcs, "En", ProcEn, size);
	AddGryph(charProcs, "Yen", ProcYen, size);
	AddGryph(charProcs, "Pound", ProcPound, size);
	AddGryph(charProcs, "Play", ProcPlay, size);
	AddGryph(charProcs, "Down", ProcDown, size);
	AddGryph(charProcs, "Maru", ProcMaru, size);
	AddGryph(charProcs, "OpKagi", ProcOpKagi, size);
	AddGryph(charProcs, "ClKagi", ProcClKagi, size);
	AddGryph(charProcs, "Ten", ProcTen, size);
	AddGryph(charProcs, "Pochi", ProcPochi, size);
	AddGryph(charProcs, "KanaWo", ProcKanaWo, size);
	AddGryph(charProcs, "SKanaA", ProcSKanaA, size);
	AddGryph(charProcs, "SKanaI", ProcSKanaI, size);
	AddGryph(charProcs, "SKanaU", ProcSKanaU, size);
	AddGryph(charProcs, "SKanaE", ProcSKanaE, size);
	AddGryph(charProcs, "SKanaO", ProcSKanaO, size);
	AddGryph(charProcs, "SKanaYa", ProcSKanaYa, size);
	AddGryph(charProcs, "SKanaYu", ProcSKanaYu, size);
	AddGryph(charProcs, "SKanaYo", ProcSKanaYo, size);
	AddGryph(charProcs, "SKanaTsu", ProcSKanaTsu, size);
	AddGryph(charProcs, "KanaBar", ProcKanaBar, size);
	AddGryph(charProcs, "KanaA", ProcKanaA, size);
	AddGryph(charProcs, "KanaI", ProcKanaI, size);
	AddGryph(charProcs, "KanaU", ProcKanaU, size);
	AddGryph(charProcs, "KanaE", ProcKanaE, size);
	AddGryph(charProcs, "KanaO", ProcKanaO, size);
	AddGryph(charProcs, "KanaKa", ProcKanaKa, size);
	AddGryph(charProcs, "KanaKi", ProcKanaKi, size);
	AddGryph(charProcs, "KanaKu", ProcKanaKu, size);
	AddGryph(charProcs, "KanaKe", ProcKanaKe, size);
	AddGryph(charProcs, "KanaKo", ProcKanaKo, size);
	AddGryph(charProcs, "KanaSa", ProcKanaSa, size);
	AddGryph(charProcs, "KanaShi", ProcKanaShi, size);
	AddGryph(charProcs, "KanaSu", ProcKanaSu, size);
	AddGryph(charProcs, "KanaSe", ProcKanaSe, size);
	AddGryph(charProcs, "KanaSo", ProcKanaSo, size);
	AddGryph(charProcs, "KanaTa", ProcKanaTa, size);
	AddGryph(charProcs, "KanaChi", ProcKanaChi, size);
	AddGryph(charProcs, "KanaTsu", ProcKanaTsu, size);
	AddGryph(charProcs, "KanaTe", ProcKanaTe, size);
	AddGryph(charProcs, "KanaTo", ProcKanaTo, size);
	AddGryph(charProcs, "KanaNa", ProcKanaNa, size);
	AddGryph(charProcs, "KanaNi", ProcKanaNi, size);
	AddGryph(charProcs, "KanaNu", ProcKanaNu, size);
	AddGryph(charProcs, "KanaNe", ProcKanaNe, size);
	AddGryph(charProcs, "KanaNo", ProcKanaNo, size);
	AddGryph(charProcs, "KanaHa", ProcKanaHa, size);
	AddGryph(charProcs, "KanaHi", ProcKanaHi, size);
	AddGryph(charProcs, "KanaFu", ProcKanaFu, size);
	AddGryph(charProcs, "KanaHe", ProcKanaHe, size);
	AddGryph(charProcs, "KanaHo", ProcKanaHo, size);
	AddGryph(charProcs, "KanaMa", ProcKanaMa, size);
	AddGryph(charProcs, "KanaMi", ProcKanaMi, size);
	AddGryph(charProcs, "KanaMu", ProcKanaMu, size);
	AddGryph(charProcs, "KanaMe", ProcKanaMe, size);
	AddGryph(charProcs, "KanaMo", ProcKanaMo, size);
	AddGryph(charProcs, "KanaYa", ProcKanaYa, size);
	AddGryph(charProcs, "KanaYu", ProcKanaYu, size);
	AddGryph(charProcs, "KanaYo", ProcKanaYo, size);
	AddGryph(charProcs, "KanaRa", ProcKanaRa, size);
	AddGryph(charProcs, "KanaRi", ProcKanaRi, size);
	AddGryph(charProcs, "KanaRu", ProcKanaRu, size);
	AddGryph(charProcs, "KanaRe", ProcKanaRe, size);
	AddGryph(charProcs, "KanaRo", ProcKanaRo, size);
	AddGryph(charProcs, "KanaWa", ProcKanaWa, size);
	AddGryph(charProcs, "KanaN", ProcKanaN, size);
	AddGryph(charProcs, "KanaDaku", ProcKanaDaku, size);
	AddGryph(charProcs, "KanaHan", ProcKanaHan, size);
	AddGryph(charProcs, "Right", ProcRight, size);
	AddGryph(charProcs, "Under", ProcUnder, size);
	AddGryph(charProcs, "Over", ProcOver, size);
	AddGryph(charProcs, "Pi", ProcPi, size);
}

void PDFPlotter::AddGryph(TPDFL::Dict* charProcs, const char* key, const char* gryphBody, int size)
{
	std::ostringstream oss;
	oss << GryphPrefix;
	oss << 0.75 / (size + 1);
	oss << " w ";
	oss << gryphBody;
	oss << GryphPostfix;
	Stream* proc = new Stream(m_mgr);
	proc->AddStream(oss.str().c_str());
	charProcs->Add(key, proc);
}

void PDFPlotter::PrintLine(const char* str, int size, int rotate)
{
	std::ostringstream oss;
	oss << "BT" << std::endl;
	oss << "/F" << size << " " << 8 * (size + 1) << " Tf ";

	int strWidth = strlen(str) * 6 * (size + 1);
	int strHeight = 8 * (size + 1);
	int x = m_penx;
	int y = m_peny;
	switch (rotate % 4)
	{
	case 0:
		m_penx += strWidth;
		m_ymax = std::max(m_ymax, y + strHeight);
		oss << "1 0 0 1 ";
		break;
	case 1:
		m_peny -= strWidth;
		m_ymin = std::min(m_ymin, m_peny);
		oss << "0 -1 1 0 ";
		break;
	case 2:
		m_penx -= strWidth;
		m_ymin = std::min(m_ymin, y - strHeight);
		oss << "-1 0 0 -1 ";
		break;
	case 3:
		m_peny += strWidth;
		oss << "0 1 -1 0 ";
		m_ymax = std::max(m_ymax, m_peny);
		break;
	}
	oss << x << " " << y << " Tm ";

	oss << "(" << str << ") Tj" << std::endl;
	m_currentContents->AddStream(oss.str().c_str());
	m_currentContents->AddStream("ET\n");
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
	const char NumToChar[] = "0123456789ABCDEF";
	if (!m_mgr) {
		NewFile();
	}
	if (!m_currentPage) {
		NewPage();
	}
	if (m_penDown) {
		Move(m_penx, m_peny);
	}

	if (size < 0 || size > 63) {
		return;
	}
	if (NULL == m_fontDicts[size]) {
		ConstructFontDict(size);
	}
	int length = strlen(str);
	if (0 >= length) return;
	m_currentContents->AddStream(PENCOLOR[m_penColor]);
	std::string strToPrint;
	bool printable = (0 != CharProperty(str[0]));
	bool needColorReset = false;
	for (int i = 0; i < length; ++i)
	{
		if (0x0f >= static_cast<unsigned char>(str[i])) continue;	// コントロールコードは印字しない
		int charProperty = CharProperty(str[i]);
		bool charPrintable = (0 != charProperty);
		if ( charPrintable != printable) {
			if (printable) {
				m_currentContents->AddStream(PENCOLOR[m_penColor]);
			}
			else {
				m_currentContents->AddStream(PENCOLOR[(m_penColor + 1) % 4]);
				needColorReset = true;
			}
			PrintLine(strToPrint.c_str(), size, rotate);
			strToPrint.clear();
			printable = charPrintable;
		}
		if (2 == charProperty) {
			strToPrint.append("\\");
			strToPrint.append(1, str[i]);
		}
		else if (1 == charProperty) {
			strToPrint.append(1, str[i]);
		}
		else {
			char upper = NumToChar[(str[i] & 0xF0) >> 4];
			char lower = NumToChar[str[i] & 0x0F];
			strToPrint.append(1, upper);
			strToPrint.append(1, lower);
		}
	}
	if (printable && needColorReset) {
		m_currentContents->AddStream(PENCOLOR[m_penColor]);
	} else if (!printable) {
		m_currentContents->AddStream(PENCOLOR[(m_penColor + 1) % 4]);
	}
	PrintLine(strToPrint.c_str(), size, rotate);
	if (!printable) {
		m_currentContents->AddStream(PENCOLOR[m_penColor]);
	}
}

void PDFPlotter::Axis(bool vertical, int pitch, int count)
{
	if (255 < count) {
		count = 255;
	}

	std::ostringstream oss;
	Move(m_penx, m_peny);
	oss << "0.75 w 1 J 1 j " << PENCOLOR[m_penColor];
	oss << " q [] 0 d ";
	int x = m_penx;
	int y = m_peny;
	if (vertical) {
		y += pitch * count;
		m_ymin = std::min(m_ymin, y);
		m_ymax = std::max(m_ymax, y);
	}
	else {
		x += pitch * count;
	}
	oss << x << " " << y << " l ";

	x = m_penx;
	y = m_peny;
	for (int i = 0; i < count; ++i)
	{
		if (vertical) {
			y += pitch;
			oss << x - 2 << " " << y << " m " << x + 2 << " " << y << " l ";
		}
		else {
			x += pitch;
			oss << x << " " << y - 2 << " m " << x << " " << y + 2 << " l ";
		}
	}
	oss << "S Q" << std::endl;
	m_currentContents->AddStream(oss.str().c_str());

	m_penx = x;
	m_peny = y;
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
		m_fontPool = NULL;
		memset(m_fontDicts, 0, sizeof(m_fontDicts));
		m_fontEncoding = NULL;
		m_numPages = 0;
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

void PDFPlotter::SetPenX(int x)
{
	m_penx = x;
}

void PDFPlotter::SetPenY(int y)
{
	m_peny = y;
	m_ymin = std::min(m_ymin, y);
	m_ymax = std::max(m_ymax, y);
}