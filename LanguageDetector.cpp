// LanguageDetector.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <wininet.h>
#include <wchar.h>
#include <commctrl.h>
//#include "translator.h"
//#include "log.h"
//#include "project.h"
#include <time.h>
#include <commdlg.h>
#include <commctrl.h>
#include <SHELLAPI.H>
#include <COMMCTRL.H>
#pragma comment(lib,"wininet")

#include "statistic.h"
#include <iostream>

#define MAX_LOADSTRING 100
//#define Gtransl_WINDOW_WIDTH 660
//#define Gtransl_WINDOW_HIGHT  240
 #pragma comment (lib, "ws2_32.lib")
#include <winsock2.h>
#include <assert.h>

#include <CRTDBG.H>	

 

#define WINDOW_WIDTH 750
#define WINDOW_HIGHT 450

#define G_MAX_LEN_STRING 4500
#define MAX_TRANSLATE_BUFFER  100000
#define SERVER_COUNT 31
#define  TR_OK    272
#define G_TRANSLATE_BUTTON				344
#define SAVE  345


int ver=104;
char subver[8]=" ";

char ProgramPatch[MAX_PATH*2];
char UserName[28];
HANDLE hThread_Sender2;
DWORD dwId_;
RECT rt;
char SaveDirectory[MAX_PATH]="";

DWORD User_name_len=sizeof(UserName);
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];

HFONT hCurFont,hUIFont;								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

#define LANG_COUNT 40

#define MAX_LEN_STRING			1184 
HWND hGtranWndElWnd;
static HWND   hGlanguageWnd, hGProgressBarWnd,hG_Save,hG_Code,hGProgressBarWnd2,hGSourceSentence,hGTargetSentence, hG_Translate;

WCHAR Buffer[2000];
WCHAR Buffer2[2000];
char s[G_MAX_LEN_STRING];
int progress=0;


 char G_szSTR[G_MAX_LEN_STRING];
 WCHAR G_sw2[G_MAX_LEN_STRING];
 WCHAR G_sw[G_MAX_LEN_STRING];

WCHAR* G_translate(int SLanguage,int DLanguage, char* str);
WCHAR* DetectLanguage(char* str);
int DetectLanguage2(char* str);
WCHAR* DetectLanguage3(char* str);
int DetectLanguage4(char* str);

int Get_UTC_time(LPSYSTEMTIME lpSystemTime);

HANDLE  hThread_GoogleTranslatorLocal=0;
DWORD dwG_translatorLocal;
extern unsigned long __stdcall Sender2(void *Param);

//================= connect UNICODE to UTF8==============================================
 //char* __stdcall    	UnicodeToUtf8(sw,s);
void UnicodeToUtf8( const WCHAR* wstr , char* szOut )
 {// Convert unicode to utf8
   //	assert(len > 0);//check
//assert(_CrtIsValidPointer( wstr, wcslen( wstr) * sizeof( WCHAR), TRUE));


          const WCHAR* w;
          
   
          int len = 0;
          for ( w = wstr; *w; w++ ) 
		  {
              if ( *w < 0x0080 ) len++;
              else if ( *w < 0x0800 ) len += 2;
              else len += 3;
          }

          //unsigned char* szOut = ( unsigned char* )malloc( len+1 );
          //if ( szOut == NULL )
              //return NULL;
  
          int i = 0;
          for ( w = wstr; *w; w++ ) 
		  {
              if ( *w < 0x0080 )
                  szOut[i++] = ( unsigned char ) *w;
  
              else if ( *w < 0x0800 ) 
			  {
                  szOut[i++] = 0xc0 | (( *w ) >> 6 );
                  szOut[i++] = 0x80 | (( *w ) & 0x3f );
              }
              else 
			  {
                  szOut[i++] = 0xe0 | (( *w ) >> 12 );
                  szOut[i++] = 0x80 | (( ( *w ) >> 6 ) & 0x3f );
                  szOut[i++] = 0x80 | (( *w ) & 0x3f );
              } 
		  }
  
          szOut[ i ] = '\0';
          //return ( char* )szOut;
      }
//================= connect UNICODE to UTF8==============================================

unsigned long __stdcall G_translator(void *Param)
{
	ShowWindow(hGProgressBarWnd, SW_SHOW );

	memset(G_sw,0,G_MAX_LEN_STRING);
	GetWindowTextW(hGSourceSentence, G_sw,G_MAX_LEN_STRING ); //SendStatistic(100);

	if(G_sw[0]==0)
			{MessageBoxW(0 ,L"No text.",L"Error", MB_ICONERROR); goto exit;}

	memset(G_szSTR,0,G_MAX_LEN_STRING);
	UnicodeToUtf8(G_sw,G_szSTR);

 
	 SetWindowTextW(hGTargetSentence,DetectLanguage3(G_szSTR));

exit:
	SendMessage(hGtranWndElWnd, WM_COMMAND,TR_OK,0); // 	
	ShowWindow(hGProgressBarWnd, SW_HIDE );
	ExitThread(0);
		return 0;
}


inline void  Translate()
{
	//if(hThread_GoogleTranslatorLocal)
	// TerminateThread(hThread_GoogleTranslatorLocal,0);

	hThread_GoogleTranslatorLocal = CreateThread( NULL, 0, G_translator, NULL, 0, &dwG_translatorLocal );
}



char  szData[MAX_TRANSLATE_BUFFER];

//int Language=0;
char GLanguageM[LANG_COUNT][8]={
"en",
"fr",
"de",
"ja",
"ko",
"ru",
"es",
"it",
"tr",
"zh-CN",
"hu",
"bg",
"cs",
"sk",
"uk",
"ro",
"th",
"he",
"id",
"vi",
"fi","no","sv","da","nl","pl","pt","sr","ar",
"km",
"zh-TW",
"pt","eu","et","el","hi","ms","en","en",
"auto"};

char google_str[SERVER_COUNT][32]={
"translate.google.as",
"translate.google.ae",
"translate.google.am",
"translate.google.az",
"translate.google.bg",
"translate.google.ca",
"translate.google.cn",
"translate.google.co.in",
"translate.google.co.jp",
"translate.google.co.kr",
"translate.google.co.th",
"translate.google.com",
"translate.google.com.tw",
"translate.google.com.hk",
"translate.google.com.ph",
"translate.google.com.ua",
"translate.google.com.vn",
"translate.google.cz",
"translate.google.de",
"translate.google.es",
"translate.google.fi",
"translate.google.fr",
"translate.google.ga",
"translate.google.nl",
"translate.google.no",
"translate.google.mn",
"translate.google.pl",
"translate.google.pt",
"translate.google.ro",
"translate.google.ru",
"translate.google.sl"
};


#define DETECT_LANG_COUNT 161


char DetectLangList[DETECT_LANG_COUNT][3][32]=
{
{"ab","ABKHAZIAN","XX"},
{"aa","AFAR","XX"},
{"af","AFRIKAANS","XX"},
{"ak","AKAN","XX"},
{"sq","ALBANIAN","XX"},
{"am","AMHARIC","XX"},
{"ar","ARABIC","ARB" },
{"hy","ARMENIAN","XX"},
{"as","ASSAMESE","XX"},
{"ay","AYMARA","XX"},
{"az","AZERBAIJANI","XX"},
{"ba","BASHKIR","XX"},
{"eu","BASQUE","XX"},
{"be","BELARUSIAN","XX"},
{"bn","BENGALI","XX"},
{"bh","BIHARI","XX"},
{"bi","BISLAMA","XX"},
{"bs","BOSNIAN","BS"},
{"br","BRETON","XX"},
{"bg","BULGARIAN","BG" },
{"my","BURMESE","BM"},
{"ca","CATALAN","XX"},
{"ceb","CEBUANO","XX"},
{"chr","CHEROKEE","XX"},
{"co","CORSICAN","XX"},
{"hr","CROATIAN","CR"},
{"cs","CZECH","CZ"},
{"zh","CHINESE","C"},
{"zh-Hant","CHINESET","T"},
{"da","DANISH","DA"},
{"dv","DHIVEHI","XX"},
{"nl","DUTCH","DU"},
{"dz","DZONGKHA","XX"},
{"en","ENGLISH","E"},
{"eo","ESPERANTO","XX"},
{"et","ESTONIAN","EE"},
{"fo","FAROESE","XX"},
{"fj","FIJIAN","XX"},
{"fi","FINNISH","FI"},
{"fr","FRENCH","F"},
{"fy","FRISIAN","XX"},
{"gl","GALICIAN","XX"},
{"lg","GANDA","XX"},
{"ka","GEORGIAN","XX"},
{"de","GERMAN","G"},
{"el","GREEK","GK"},
{"kl","GREENLANDIC","XX"},
{"gn","GUARANI","XX"},
{"gu","GUJARATI","XX"},
{"ht","HAITIAN_CREOLE","XX"},
{"ha","HAUSA","XX"},
{"haw","HAWAIIAN","XX"},
{"iw","HEBREW","HB"},
{"hi","HINDI","HI"},
{"hmn","HMONG","XX"},
{"hu","HUNGARIAN","HUG"},
{"is","ICELANDIC","XX"},
{"ig","IGBO","XX"},
{"id","INDONESIAN","ID"},
{"ia","INTERLINGUA","XX"},
{"ie","INTERLINGUE","XX"},
{"iu","INUKTITUT","XX"},
{"ik","INUPIAK","XX"},
{"ga","IRISH","XX"},
{"it","ITALIAN","I"},
{"jw","JAVANESE","XX"},
{"ja","JAPANESE","J"},
{"kn","KANNADA","XX"},
{"ks","KASHMIRI","XX"},
{"kk","KAZAKH","KZ"},
{"kha","KHASI","XX"},
{"km","KHMER","XX"},
{"rw","KINYARWANDA","XX"},
{"ku","KURDISH","XX"},
{"ky","KYRGYZ","XX"},
{"ko","KOREAN","K"},
{"lo","LAOTHIAN","XX"},
{"la","LATIN","XX"},
{"lv","LATVIAN","LV"},
{"lif","LIMBU","XX"},
{"ln","LINGALA","XX"},
{"lt","LITHUANIAN","LT"},
{"lb","LUXEMBOURGISH","XX"},
{"mk","MACEDONIAN","MK"},
{"mg","MALAGASY","XX"},
{"ms","MALAY","MY"},
{"ml","MALAYALAM","XX"},
{"mt","MALTESE","XX"},
{"gv","MANX","XX"},
{"mi","MAORI","XX"},
{"mr","MARATHI","XX"},
{"mfe","MAURITIAN_CREOLE","XX"},
{"mn","MONGOLIAN","XX"},
{"na","NAURU","XX"},
{"nr","NDEBELE","XX"},
{"ne","NEPALI","XX"},
{"no","NORWEGIAN","NW"},
{"ny","NYANJA","XX"},
{"oc","OCCITAN","XX"},
{"or","ORIYA","XX"},
{"om","OROMO","XX"},
{"ps","PASHTO","XX"},
{"nso","PEDI","XX"},
{"fa","PERSIAN","FA"},
{"pl","POLISH","PL"},
{"pt","PORTUGUESE","PG"},
{"pa","PUNJABI","XX"},
{"qu","QUECHUA","XX"},
{"rm","RHAETO_ROMANCE","XX"},
{"ro","ROMANIAN","RO"},
{"rn","RUNDI","XX"},
{"ru","RUSSIAN","R"},
{"sm","SAMOAN","XX"},
{"sg","SANGO","XX"},
{"sa","SANSKRIT","XX"},
{"sco","SCOTS","XX"},
{"gd","SCOTS_GAELIC","XX"},
{"sr","SERBIAN","SB"},
{"crs","SESELWA","XX"},
{"st","SESOTHO","XX"},
{"sn","SHONA","XX"},
{"sd","SINDHI","XX"},
{"si","SINHALESE","XX"},
{"ss","SISWANT","XX"},
{"sk","SLOVAK","SK"},
{"sl","SLOVENIAN","SL"},
{"so","SOMALI","XX"},
{"es","SPANISH","S"},
{"su","SUNDANESE","XX"},
{"sw","SWAHILI","XX"},
{"sv","SWEDISH","SW"},
{"syr","SYRIAC","XX"},
{"tl","TAGALOG","XX"},
{"tg","TAJIK","XX"},
{"ta","TAMIL","XX"},
{"tt","TATAR","XX"},
{"te","TELUGU","XX"},
{"th","THAI","TH"},
{"bo","TIBETAN","XX"},
{"ti","TIGRINYA","XX"},
{"to","TONGA","XX"},
{"ts","TSONGA","XX"},
{"tn","TSWANA","XX"},
{"tr","TURKISH","TR"},
{"tk","TURKMEN","XX"},
{"ug","UIGHUR","XX"},
{"uk","UKRAINIAN","UA"},
{"ur","URDU","XX"},
{"uz","UZBEK","XX"},
{"ve","VENDA","XX"},
{"vi","VIETNAMESE","VN"},
{"vo","VOLAPUK","XX"},
{"war","WARAY_PHILIPPINES","XX"},
{"cy","WELSH","XX"},
{"wo","WOLOF","XX"},
{"xh","XHOSA","XX"},
{"yi","YIDDISH","XX"},
{"yo","YORUBA","XX"},
{"za","ZHUANG","XX"},
{"zu","ZULU","XX"}//,{"zh-Hant","ZULU","XX"}
};



WCHAR* ChangeToUnicode(char* str)
{
static	WCHAR UNstr1[MAX_LEN_STRING];
static	WCHAR UNstr2[MAX_LEN_STRING];
static	WCHAR UNstr3[MAX_LEN_STRING];
static number=1;
	switch(number)
	{
	case 1:
		memset(UNstr1,0,MAX_LEN_STRING * sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP,0, str, strlen(str), UNstr1,MAX_LEN_STRING );number++;
		return (WCHAR*)&UNstr1;
		break;
	case 2:
		memset(UNstr2,0,MAX_LEN_STRING * sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP,0, str, strlen(str), UNstr2,MAX_LEN_STRING );number++;
		return (WCHAR*)&UNstr2;
		break;
	case 3:
		memset(UNstr3,0,MAX_LEN_STRING * sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP,0, str, strlen(str), UNstr3,MAX_LEN_STRING );number=1;
		return (WCHAR*)&UNstr3;
		break;
	default:
		wcscpy(UNstr1,L"NULL");
		return (WCHAR*)&UNstr1;
		break;
	}
}

WCHAR* ChangeToUnicodeUTF8(char* str)
{
static	WCHAR UNstr1[MAX_LEN_STRING];
static	WCHAR UNstr2[MAX_LEN_STRING];
static	WCHAR UNstr3[MAX_LEN_STRING];
static number=1;
	switch(number)
	{
	case 1:
		memset(UNstr1,0,MAX_LEN_STRING * sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8,0, str, strlen(str), UNstr1,MAX_LEN_STRING );number++;
		return (WCHAR*)&UNstr1;
		break;
	case 2:
		memset(UNstr2,0,MAX_LEN_STRING * sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8,0, str, strlen(str), UNstr2,MAX_LEN_STRING );number++;
		return (WCHAR*)&UNstr2;
		break;
	case 3:
		memset(UNstr3,0,MAX_LEN_STRING * sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8,0, str, strlen(str), UNstr3,MAX_LEN_STRING );number=1;
		return (WCHAR*)&UNstr3;
		break;
	default:
		wcscpy(UNstr1,L"NULL");
		return (WCHAR*)&UNstr1;
		break;
	}
}


char* ChangeToANSI(  WCHAR* str)
{
 static	char UNstr1[MAX_LEN_STRING];
 static	char UNstr2[MAX_LEN_STRING];
 static	char UNstr3[MAX_LEN_STRING];
 static	char UNstr4[MAX_LEN_STRING];
static number=1;
	switch(number)
	{
	case 1:
		number++;memset(UNstr1,0,MAX_LEN_STRING * sizeof(char ));
		WideCharToMultiByte(CP_ACP,0, str, wcslen(str),UNstr1,MAX_LEN_STRING, NULL, NULL); 
		//MultiByteToWideChar(CP_ACP,0, str, strlen(str), UNstr1,MAX_LEN_STRING );number++;
		Sleep(1);return (char*)UNstr1;
		break;
	case 2:
		number++;memset(UNstr2,0,MAX_LEN_STRING * sizeof(char ));
		WideCharToMultiByte(CP_ACP,0, str, wcslen(str),UNstr2,MAX_LEN_STRING, NULL, NULL); 
		Sleep(1);return (char*)UNstr2;
		break;
	case 3:
		number++;memset(UNstr3,0,MAX_LEN_STRING * sizeof(char ));
		WideCharToMultiByte(CP_ACP,0, str, wcslen(str),UNstr3,MAX_LEN_STRING, NULL, NULL); 
		Sleep(1);return (char*)&UNstr3;
		break;
	case 4:
		number=1;memset(UNstr4,0,MAX_LEN_STRING * sizeof(char ));
		WideCharToMultiByte(CP_ACP,0, str, wcslen(str),UNstr4,MAX_LEN_STRING, NULL, NULL); 
		Sleep(1);return (char*)&UNstr4;
		break;
	default:
		strcpy(UNstr1,"NULL");
		return (char*)UNstr1;
		break;
	}
}



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LANGUAGEDETECTOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LANGUAGEDETECTOR);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LANGUAGEDETECTOR);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+0);
	wcex.lpszMenuName	= (LPCSTR)IDC_LANGUAGEDETECTOR;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND hGTranslateWnd;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   

   hInst = hInstance; // Store instance handle in our global variable

   hGTranslateWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      (GetSystemMetrics(SM_CXSCREEN)- WINDOW_WIDTH)/2,
													 (GetSystemMetrics(SM_CYSCREEN)- WINDOW_HIGHT)/2+80+2,
													WINDOW_WIDTH ,WINDOW_HIGHT, NULL, NULL, hInstance, NULL);

   if (!hGTranslateWnd)
   {
      return FALSE;
   }

   ShowWindow(hGTranslateWnd, nCmdShow);
   UpdateWindow(hGTranslateWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
	case WM_CREATE:
		hUIFont=CreateFontW(16,0,0,0,FW_MEDIUM,0,0,0,
		// hUIFont=CreateFontW(16,0,0,0,FW_SEMIBOLD,0,0,0,
					0,OUT_CHARACTER_PRECIS,CLIP_DEFAULT_PRECIS,
					PROOF_QUALITY,FIXED_PITCH|FF_ROMAN,L"Arial Unicode MS");
		hGTargetSentence=0;hGSourceSentence=0; hG_Code=0;hGlanguageWnd=0;

			 memset(UserName,0,sizeof(UserName));
			GetUserName((LPTSTR)UserName,&User_name_len);
			UserName[27]=0;

			SYSTEMTIME sm;
			memset(&sm,0,sizeof(SYSTEMTIME));
			GetLocalTime(&sm);
			if(sm.wYear>2014&&sm.wMonth>9 &&sm.wDay>3)
				{	
					MessageBoxW(hWnd, L"The debug version has expired. Please update the tool.", L"Error", MB_ICONSTOP);
					break;
				}
		  hThread_Sender2 = CreateThread( NULL, 0, Sender2, NULL, 0, &dwId_ );
		  
		 GetCurrentDirectory(sizeof(SaveDirectory),SaveDirectory);

		 InitStatistic();
		  
		 //CloseStatistic();
		break;
	
	case WM_SIZE:
			GetClientRect(hGTranslateWnd, &rt);

			 DestroyWindow(hGProgressBarWnd);
			 DestroyWindow(hGProgressBarWnd2);
			DestroyWindow(hG_Translate);
			DestroyWindow(hG_Save);
			 
				if(hGlanguageWnd)
					SetWindowPos(hGlanguageWnd,NULL, rt.right/2-1-60, 0, 122,rt.bottom-40,SWP_NOZORDER);
				else
				{
					hGlanguageWnd=CreateWindowExW( WS_EX_TOPMOST      ,
							L"EDIT", L"",
							WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE |WS_TABSTOP|ES_AUTOVSCROLL,//|WS_CAPTION  ,
                                        rt.right/2-1-60, 0, 122,rt.bottom-40,
                                         hGTranslateWnd, (HMENU) 0, hInst, NULL );
					SendMessage(hGlanguageWnd, WM_SETFONT, (WPARAM)hUIFont, FALSE);
					//SetWindowTextW(hGSourceSentence,G_sw);
				}



				if(hGSourceSentence)
					SetWindowPos(hGSourceSentence,NULL, rt.left+0, 0, rt.right/2-1-60,rt.bottom-40,SWP_NOZORDER);
				else
				{
					hGSourceSentence=CreateWindowExW( WS_EX_TOPMOST      ,
							L"EDIT", L"",
							WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE |WS_TABSTOP|ES_AUTOVSCROLL,//|WS_CAPTION  ,
                                        rt.left+0, 0, rt.right/2-1-60,rt.bottom-40,
                                         hGTranslateWnd, (HMENU) 0, hInst, NULL );
					SendMessage(hGSourceSentence, WM_SETFONT, (WPARAM)hUIFont, FALSE);
					 //SetWindowTextW(hGSourceSentence,L"Please copy your text here, then press <Detect> button.");
				}

				if(hGTargetSentence)
					SetWindowPos(hGTargetSentence,NULL, rt.right/2+1+60, 0, rt.right/2-1,rt.bottom-40,SWP_NOZORDER);
				else
				{
					hGTargetSentence= CreateWindowExW( WS_EX_TOPMOST      ,
							L"EDIT", L"",
							WS_VISIBLE | WS_CHILD | WS_BORDER |  ES_MULTILINE |WS_TABSTOP|ES_AUTOVSCROLL,//|WS_CAPTION  ,
                                        rt.right/2+1+60, 0, rt.right/2-1,rt.bottom-40,
                                         hGTranslateWnd, (HMENU) 0, hInst, NULL );
					SendMessage(hGTargetSentence, WM_SETFONT, (WPARAM)hUIFont, FALSE);
					//SetWindowTextW(hGTargetSentence,G_sw2);
				}

				if(hG_Code )
					SetWindowPos(hG_Code,NULL, 90, rt.bottom-30,rt.right-180,22,SWP_NOZORDER);
				else
				{
					hG_Code= CreateWindowExW( WS_EX_TOPMOST      ,
							L"EDIT", L"",
							WS_VISIBLE | WS_CHILD | WS_BORDER  |ES_RIGHT|ES_AUTOHSCROLL|WS_TABSTOP,//|WS_CAPTION  ,
                                        90, rt.bottom-30,rt.right-180,22,
                                         hGTranslateWnd, (HMENU) 0, hInst, NULL );
					SendMessage(hG_Code, WM_SETFONT, (WPARAM)hUIFont, FALSE);
				}

				hG_Translate=CreateWindowExW(WS_EX_CONTROLPARENT        ,
					L"BUTTON",      L"Detect",       // button text
							WS_VISIBLE | WS_CHILD | WS_TABSTOP,// |ES_AUTOVSCROLL,  // styles
							 10, rt.bottom-30,
							 70, 22,      hGTranslateWnd,       // parent window
							(HMENU)G_TRANSLATE_BUTTON, (HINSTANCE) GetWindowLong(hGTranslateWnd, GWL_HINSTANCE), NULL); 
							SendMessage(hG_Translate, WM_SETFONT, (WPARAM)hUIFont, FALSE);

				hG_Save=CreateWindowExW(WS_EX_CONTROLPARENT        ,
					L"BUTTON",      L"Save",       // button text
							WS_VISIBLE | WS_CHILD | WS_TABSTOP,//|ES_AUTOVSCROLL ,  // styles
							 rt.right-80 , rt.bottom-30,
							 70, 22,      hGTranslateWnd,       // parent window
							(HMENU) SAVE, (HINSTANCE) GetWindowLong(hGTranslateWnd, GWL_HINSTANCE), NULL);
							SendMessage(hG_Save, WM_SETFONT, (WPARAM)hUIFont, FALSE);


			/*	hGProgressBarWnd=CreateWindow( PROGRESS_CLASS        ,
							  "progress bar",  WS_VISIBLE | WS_CHILD  , 
							 // rt.right-90, rt.bottom-30+1+22, 80, 6,   
							    0, rt.bottom-6, rt.right, 6,  
							   
							 hGTranslateWnd,   NULL,  (HINSTANCE) GetWindowLong(hGTranslateWnd,0), NULL); 
				SendMessage( hGProgressBarWnd, PBM_SETRANGE, 0, MAKELPARAM(0,100));
				SendMessage( hGProgressBarWnd, PBM_SETSTEP, 1, 0);
				SendMessage( hGProgressBarWnd, PBM_SETPOS, 0, 0);
				ShowWindow(hGProgressBarWnd, SW_HIDE);
				SendMessage(hG_Translate, WM_SETFONT, (WPARAM)hUIFont, FALSE);*/


				hGProgressBarWnd2=CreateWindow( PROGRESS_CLASS        ,
							  "progress bar",  WS_VISIBLE | WS_CHILD  , 
							  // rt.right-90, rt.bottom-30+1-6,  80, 6,   
							   //0, rt.bottom-40, rt.right, 6,  
							   0, rt.bottom-6, rt.right, 6,  
							 hGTranslateWnd,   NULL,  (HINSTANCE) GetWindowLong(hGTranslateWnd,0), NULL); 
				SendMessage( hGProgressBarWnd, PBM_SETRANGE, 0, MAKELPARAM(0,100));
				SendMessage( hGProgressBarWnd, PBM_SETSTEP, 1, 0);
				SendMessage( hGProgressBarWnd, PBM_SETPOS, 0, 0);
				ShowWindow(hGProgressBarWnd2, SW_HIDE);
				

			break;

	
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
			case SAVE:
				{
					 //SendStatistic(200); 
					 int error=0;
					char file[MAX_PATH]="";
					GetWindowText(hG_Code, file, MAX_PATH );

					if(file[0]==0)
					{MessageBoxW(hWnd ,L"No code.",L"Error", MB_ICONERROR); break;}

					for(unsigned int i=0;i<strlen(file);i++)
						if(file[i]=='?') error=1; 

					if(error){MessageBoxW(hWnd ,L"There is a question mark in the code.",L"Error", MB_ICONERROR); break; }

					strcat(file,".tdd");
					
					OPENFILENAME ofn;
					 ZeroMemory(&ofn, sizeof(OPENFILENAME));
					 ofn.lStructSize = sizeof(OPENFILENAME); 
										
										ofn.hwndOwner = hWnd; 
										ofn.lpstrFile = file; 
										ofn.nMaxFile = MAX_PATH; 
										ofn.lpstrFilter =  &"Language code Files (*.tdd)\0*.TDD\0"  \
															"All files (*.*)\0*.*\0"; 
										ofn.nFilterIndex = 0; 
										ofn.lpstrTitle=&"Save Language code File"; 
										ofn.lpstrInitialDir=SaveDirectory; 
										ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_ENABLESIZING |OFN_HIDEREADONLY;
										
										if(!GetSaveFileName(&ofn)) 
											break; 

									HANDLE hFile=CreateFile(file, GENERIC_WRITE,
									FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
							if (hFile==INVALID_HANDLE_VALUE)
								MessageBoxW(hWnd ,L"Cannot create file.",L"Error", MB_ICONERROR);
							else
								CloseHandle(hFile);	
							 
						break;
				}

			case G_TRANSLATE_BUTTON:
					
					//SendStatistic(100); 
					ShowWindow(hGProgressBarWnd, SW_SHOWNORMAL);
					SetWindowTextW(hGTargetSentence,L"");
					progress=0;
					Translate();

					break;
					
				case TR_OK:
					hThread_GoogleTranslatorLocal=0;
					ShowWindow(hGProgressBarWnd, SW_HIDE);
					ShowWindow(hGProgressBarWnd2, SW_HIDE);

					 memset(G_sw2,0,G_MAX_LEN_STRING);
					GetWindowTextW(hGTargetSentence, G_sw2,G_MAX_LEN_STRING );
				 
					break;
					
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			//RECT rt;
			//GetClientRect(hWnd, &rt);
			//DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER);
			EndPaint(hWnd, &ps);
			break;
			case WM_CLOSE:
				CloseStatistic(); 	
				DestroyWindow(hWnd);
				break;
			case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	char szSTR[100];
	int y;
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
			case WM_PAINT:
			                      
			hdc = BeginPaint(hDlg, &ps);

			hCurFont=CreateFontW(16,0,0,0,FW_NORMAL,0,0,0,
				RUSSIAN_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				PROOF_QUALITY,VARIABLE_PITCH|FF_ROMAN,L"Arial Unicode MS");
				
			SelectObject(hdc,hCurFont);

			SetBkMode(hdc, 0);
			y=0;
			
			if((ver%100)<10)
				wsprintf(szSTR,"LanguageDetector ver. %d.0%d%s beta",ver/100,ver%100,subver);
			else
				wsprintf(szSTR,"LanguageDetector ver. %d.%d%s beta",ver/100,ver%100,subver);
 

			TextOut(hdc, 90,20+y*20,szSTR,strlen(szSTR));
			y++;
			wsprintf(szSTR,"Copyright(c)2015 Sasha_p");												
			TextOut(hdc, 90,20+y*20,szSTR,strlen(szSTR));
	 
			y++;

			wsprintf(szSTR,"Thanks Grace for idea");												
			TextOut(hdc, 90,20+y*20,szSTR,strlen(szSTR));

			y++;

			wsprintf(szSTR,"Thanks Melina for test");												
			TextOut(hdc, 90,20+y*20,szSTR,strlen(szSTR));
		

			y++;
			
			wsprintf(szSTR,"Thanks Llu liu for help");												
			TextOut(hdc, 90,20+y*20,szSTR,strlen(szSTR));

			y++;
	 
			/*wsprintf(szSTR,"Special Thanks to:");												
			TextOut(hdc, 90,20+y*20,szSTR,strlen(szSTR));
			y++;*/
		 
		
 

			DeleteObject(hCurFont);

			EndPaint(hDlg, &ps);

			break;
	}
    return FALSE;
}







int DetectLanguage2(char* str)
{  //key=c898524750c007fd6196e26db05234eb 
 int Detected_lang=0;
progress=0;
	#define MAX_DET_LEN 1500
	static WCHAR result[G_MAX_LEN_STRING];	
//--------------------------
 
memset(result,0,G_MAX_LEN_STRING*2);
MultiByteToWideChar(CP_UTF8,0, str, strlen(str), result,MAX_LEN_STRING*2);
int char_count=wcslen(result);
SendStatistic(char_count,13);
memset(result,0,G_MAX_LEN_STRING*2);
//--------------------


struct LangAray
{
int lang;
double confidence;
int relaible;
};
struct LangAray *pL;
 #define  MAX_LANG 5

	pL=0;
	pL=(struct LangAray*)new struct LangAray[MAX_LANG ];
	if(pL==0)
	{
		MessageBoxW(0, L"Cannot allocate memory for pL. Please close a unused programs and try again.", L"Warning", MB_ICONWARNING);			
		return(0);	
	}
	memset(pL,0x00, MAX_LANG *sizeof(struct LangAray));	
	int	lang_count=0;



memset(result,0,G_MAX_LEN_STRING*2);
char reguest[G_MAX_LEN_STRING];
 char B_szSTR2[G_MAX_LEN_STRING];
char s[G_MAX_LEN_STRING];
char end[9]= "\r\n\r\n";

	int i;
    bool ok = false;

	char str_temp[MAX_LEN_STRING*4];
	memset (str_temp,0,MAX_LEN_STRING*4);
	strcpy(str_temp,str);


if(strlen(str_temp)>MAX_DET_LEN)
	wsprintf(&str_temp[MAX_DET_LEN-30]," (*Translation not completed*)\0");

//char caption[100];
//strcat(caption,window_name);
	memset(result,0,G_MAX_LEN_STRING);

	 HINTERNET hInternet =
        ::InternetOpen(
           //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)"),
		   TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Win32)"),
		   //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; MRA 5.4 (build 02625); .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)"),
           // INTERNET_OPEN_TYPE_DIRECT,//
			INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,NULL,
            0);

   if (hInternet != NULL) 
   {
        // открываем HTTP сессию
        HINTERNET hConnect = 
            ::InternetConnect(
                hInternet,
		//	"translate.google.com",
		//++"fanyi.youdao.com",
			//++	"openapi.baidu.com" , 
			//	 TEXT("www.api.microsofttranslator.com"),
			 	//TEXT(google_str[CurrentServer]),
				"ws.detectlanguage.com",
                //INTERNET_DEFAULT_HTTPS_PORT ,
				INTERNET_DEFAULT_HTTP_PORT,
                NULL,NULL,
                INTERNET_SERVICE_HTTP,
                0,
                1u);

		SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//		SetWindowText(hGtranWndElWnd,"Connect to server");
		 if (hConnect != NULL) 
		 {
			//Code_bold_symbols(str_temp);

			 int str_len=(int)strlen(str_temp);
			 
 
			   
					int len=0;
					memset(B_szSTR2,0,G_MAX_LEN_STRING);
					for ( i=0;i<str_len;i++)
					 {
						B_szSTR2[len]='%';
						B_szSTR2[len+1]=(unsigned char)str_temp[i]/16; 
						if(B_szSTR2[len+1]>9) B_szSTR2[len+1]+=55; 
						else B_szSTR2[len+1]+=0x30;

						B_szSTR2[len+2]=(unsigned char)str_temp[i]%16; 
						if(B_szSTR2[len+2]>9)  B_szSTR2[len+2]+=55; 
						else B_szSTR2[len+2]+=0x30;

						len+=3;
					 }

					if(strlen((const char*)B_szSTR2)>MAX_DET_LEN)
					{
						memcpy(str_temp,(const char*)B_szSTR2,MAX_DET_LEN-30);
						wsprintf(&str_temp[MAX_DET_LEN-30]," (*Translation not completed*)\0");
					}
					else
					 strcpy(str_temp,(const char*)B_szSTR2);// strcpy(str,(const char*)B_szSTR2);
			  
if(strlen(str_temp)>2000-120)
{
	for(int h=2000-120-40;h>0;h--)
		if(str_temp[h]=='%'&&str_temp[h+1]=='2'&&str_temp[h+2]=='0')
		{
			memset(&str_temp[h],0,MAX_LEN_STRING*4-h);
		//wsprintf(&str_temp[h]," (*Translation not completed*)\0");
		break;
		}
}

			memset (reguest,0,G_MAX_LEN_STRING);// rabbit  D0B7D0b0d0b5d186  %d0%b7%d0%b0%d0%b5%d1%86
			//http://api.bing.net/xml.aspx?AppId=61C738345474CF52FD6F313331A7B00837031F14&Query=%D0%B7%D0%b0%d0%b5%d1%86&Sources=Spell&Version=2.0&Market=ru-RU&Options=EnableHighlighting
 
			strcat(reguest,"/0.2/detect?q=");//// /0.2/detect?q[]=buenos+dias+senor&q[]=hello+world&key=demo 
			strcat(reguest,str_temp);

			//strcat(reguest,"&key=demo");
			strcat(reguest,"&key=c898524750c007fd6196e26db05234eb");

				
			strcat(reguest,end);
			////	memset (reguest,0,G_MAX_LEN_STRING);	strcpy(reguest,"?hl=ru&ie=UTF-8&sa=N#en|ru|Chick-pea");  strcat(reguest,end);
			
			SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//			SetWindowText(hGtranWndElWnd,"Prepare Request");

            // открываем запрос
            HINTERNET hRequest = 
					::HttpOpenRequest(hConnect,"GET", reguest,
					NULL,
					 NULL,
                    0,
                    INTERNET_FLAG_KEEP_CONNECTION,
                    1);

			SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//			SetWindowText(hGtranWndElWnd,"Request opened");

			memset(szData,0,MAX_TRANSLATE_BUFFER);
			DWORD dwCurrentPos=0;
			DWORD dwBytesRead=0;

		//SendMessage( hGProgressBarWnd, PBM_SETPOS, progress+=5, 0);

            if (hRequest != NULL) 
			{
                // посылаем запрос
                BOOL bSend = ::HttpSendRequest(hRequest, NULL,0, NULL,0);

                if (bSend) 
				{
                    // создаём выходной файл
//                    ofstream fnews("news.html",ios::out|ios::binary);
                  //  if (fnews.is_open()) 
					for (;;) 
					{
                        // читаем данные
                       // DWORD dwBytesRead=0;
                        BOOL bRead = 
                            ::InternetReadFile(
                                hRequest,
                                &szData[dwCurrentPos],sizeof(szData)-1,
                                &dwBytesRead);

                        // выход из цикла при ошибке или завершении
                        if (bRead == FALSE  ||  dwBytesRead == 0)
                            break;
						dwCurrentPos+=dwBytesRead;	
						SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
//						SetWindowText(hGtranWndElWnd,"Retrieving data");
					}

					if(dwCurrentPos>MAX_TRANSLATE_BUFFER-1)
					{
						MessageBox(hGtranWndElWnd,"Google buffer is overflow","error",MB_OK); 
						return 0;
					}

				 	szData[dwCurrentPos] = 0;
			
						memset(s,0,G_MAX_LEN_STRING);

					 //SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
							//////////////////////////////////////////////////////////////////////////////////////////
			 	   /*HANDLE hFile=CreateFile("language2.xml", GENERIC_WRITE,
									FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
							if (hFile==INVALID_HANDLE_VALUE)
								MessageBox(0 ,"cannot write file","error",MB_OK);				
							else
							{
								SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
												
								 if(WriteFile(hFile, (LPVOID)szData, strlen(szData) , &nBytesSave5, NULL))
								 {
									;//wsprintf( szOut, "Saved in %s (ANSI)",translate_file);
									 //MessageBox(hNewWnd ,szOut," File saved succesfully",MB_OK);
								 }

								CloseHandle(hFile);	
							}    */
						/////////////////////////////////////////////////////////////////////////////////////////////
						SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
//						SetWindowText(hGtranWndElWnd,"Retrieve string");



						char conn_res[]="<TITLE>Connection reset by peer</TITLE>";
						char unknown_host[]="<TITLE>Unknown Host</TITLE>";
						//char dictionary[]="<div id=gt-res-dict";
	
						//char dictionary[]="\"trans\":";	
						char dictionary[]="{\"detections\":[{";	
						int dictionary_len =strlen(dictionary);
						char req_end[]="server_time";
						int req_end_len=strlen(req_end);
						char err400[]="Error ";
						int err400_len=strlen(err400);

						int t_len=0;
						for(int i=0;i<(int)dwCurrentPos-4;i++)
						{
							if(szData[i  ]==req_end[0]&&szData[i +1 ]==req_end[1])
								if(!memcmp(req_end,&szData[i  ],req_end_len))
									break;

								//Error 400 (Bad Request)
								if(szData[i  ]==err400[0]&&szData[i +1 ]==err400[1])
								if(!memcmp(err400,&szData[i  ],err400_len))
								{	
									char ss[100]; memset(ss,0,100);
									for(int d=0;d<30;d++)
											if(szData[i+d]==(char)'{'	||
												szData[i+d]==(char)'*'||
												szData[i+d]== (char)'<')
												break;
											else
												ss[d]=szData[i+d];
									SetWindowText(hGtranWndElWnd,ss);
									//Sleep(1000);
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
								break;
								}

								

						//	<TITLE>Connection reset by peer</TITLE>/
							if(szData[i  ]==conn_res[0]&&szData[i +1 ]==conn_res[1])
								if(!memcmp(conn_res,&szData[i  ],strlen(conn_res)))
								{
									SetWindowText(hGtranWndElWnd,"Connection reset by peer");
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
								 
								}

							//	"<TITLE>Unknown Host</TITLE>";
							if(szData[i  ]==unknown_host[0]&&szData[i +1 ]==conn_res[1])
								if(!memcmp(unknown_host,&szData[i  ],strlen(unknown_host)))
								{
									SetWindowText(hGtranWndElWnd,"Unknown Host");
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
									 
								}
		
					 		if(szData[i  ]==dictionary[0])// ["
								 if(!memcmp(dictionary,&szData[i  ],dictionary_len))
								{
									i+=dictionary_len; //i++;
									//#define DETECT_LANG_COUNT 160
									//char DetectLangList[DETECT_LANG_COUNT][2][32]=

										char dland[]="\"language\":\"";
										char conf[]="\"confidence\":";
										char relaibleT[]="\"isReliable\":true";
										char relaibleF[]="\"isReliable\":false";
										char str[100];
										int relaible=0;

										for(int x=0;x<120;x++)
										{
											if(szData[i+x]=='}'&&szData[i+x +1]=='}') 
												break;

											if(szData[i+x]==dland[0]) 
											if(!memcmp( &szData[i+x],dland, strlen(dland)))
											{
												char lang_code[8]="";memset(lang_code,0,8);
												x+=strlen(dland);
												for(int a=0;a<8;a++)
												if(szData[i+x+a]==(char)'"'  )
													break;
												else
												 lang_code[a]=szData[i+x+a];

												for(a=0;a<DETECT_LANG_COUNT;a++)
													if(lang_code[0]==DetectLangList[a][0][0])
													 if(!strcmp(lang_code,DetectLangList[a][0]))
													 {
														wsprintf(str,"%s\r\n\r\n",DetectLangList[a][1]);strcat(s,str);

														pL[lang_count].lang=a;

														break;
													 }

											}
											
											if(szData[i+x]==relaibleT[0]) 
											if(!memcmp( &szData[i+x],relaibleT, strlen(relaibleT)))
											{  
												relaible=1; //
												wsprintf(str,"It is reliable. ");strcat(s,str);

												pL[lang_count].relaible=relaible;
												 
											}

											if(szData[i+x]==relaibleF[0]) 
											if(!memcmp( &szData[i+x],relaibleF, strlen(relaibleF)))
											{  
												relaible=0; //
												wsprintf(str,"It is not reliable. "); strcat(s,str);
												pL[lang_count].relaible=relaible;
												 
											}

											if(szData[i+x]==conf[0]) 
											if(!memcmp( &szData[i+x],conf, strlen(conf)))
											{
												char confidence[8]="";memset(confidence,0,8);
												double confidence_=0;
												x+=strlen(conf);
												for(int a=0;a<4;a++)
												if(szData[i+x+a]==(char)'}'  )
													break;
												else
												 confidence[a]=szData[i+x+a];

												confidence_=atof(confidence);

												wsprintf(str,"Confidence=%s\r\n\r\n",confidence);
												strcat(s,str);

												pL[lang_count].confidence=confidence_;
												lang_count++;
	 
											}

										}

											
									/*	if(relaible)
											wsprintf(str,"It is reliable\r\n"); 
										else
										wsprintf(str,"It is not reliable\r\n"); 
										strcat(s,str);*/

										
									/*	for(t_len;t_len<G_MAX_LEN_STRING;t_len++)
											if(szData[i]==(char)'}'&&szData[i+1]!=(char)'}' )
												break;
											else
											{s[t_len]=szData[i];i++;}*/
								 
									// break;
								}

							if(szData[i  ]=='}'&&szData[i +1 ]=='}')///[,,
								break;
						}
                        
 					 SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
//					 	SetWindowText(hGtranWndElWnd,"Decode string");


				}
			//	else
			//		MessageBoxW(0, L"Perhaps, the sentense is too long.", L"Error", MB_OK);
					
			
                // закрываем запрос
                ::InternetCloseHandle(hRequest);
            }
            // закрываем сессию
            ::InternetCloseHandle(hConnect);
        }
        // закрываем WinInet
        ::InternetCloseHandle(hInternet);
    }

   SendMessage( hGProgressBarWnd, PBM_SETPOS, 100, 0);
   SetWindowTextW(hGtranWndElWnd,  L"Language Detector" );

	if(lang_count)
		for (int a=0;a<lang_count;a++)
			//if(pL[a].relaible)
				if(pL[a].confidence>1)
					Detected_lang= pL[a].lang;

   delete(pL);

return Detected_lang;  
}






WCHAR* DetectLanguage3(char* str)
{
	ShowWindow(hGProgressBarWnd2, SW_SHOW);	
	SendMessage( hGProgressBarWnd2, PBM_SETPOS,  1,0);
//WCHAR Buffer[1000];
memset(Buffer,0,2000*2);
memset(Buffer2,0,2000*2);
WCHAR code[200]=L"";
SetWindowTextW(hG_Code, code);
char query[300]="";
int start=0,t;
int langs_count=0;
int langs[50];
memset(langs,0,50*sizeof(int));

int progress2=0;


int len=strlen(str);
for (int i=0;i<len;i++)
if(str[i]==0x0D||i==len-1)
	{
	if(str[i]==0x0D)str[i]=0;
	
	strcpy(query,&str[start]);

	if(query[0]&&query[1]!=0)
	{

 		 int lang=DetectLanguage2(query); progress2+=1;SendMessage( hGProgressBarWnd2, PBM_SETPOS,  progress2,0);

		 if(lang==0||lang==33)
		 {lang=DetectLanguage4(query); progress2+=1;SendMessage( hGProgressBarWnd2, PBM_SETPOS,  progress2,0);}

			if(lang)
			{
				int point=wcslen(Buffer);
				//wcscat(Buffer, ChangeToUnicode( DetectLangList[lang][1]));  
				wcscat(Buffer2, ChangeToUnicode( DetectLangList[lang][1]));  
				//wcscat(Buffer,L"		");//wcscat(Buffer,L"                             ");

				for(int u=0;u<LANG_COUNT; u++)
				 if(!strcmp(DetectLangList[lang][0],GLanguageM[u]))
				 {
					//	wcscpy( &Buffer[point+32] , G_translate( u ,0,query)); //
						wcscat( Buffer , G_translate( u ,0,query));
					break;
				 }

				if(u==LANG_COUNT)
				//	 wcscpy( &Buffer[point+32] , G_translate( LANG_COUNT-1 ,0,query));//
				wcscat( Buffer , G_translate( LANG_COUNT-1 ,0,query));

				progress2+=1;SendMessage( hGProgressBarWnd2, PBM_SETPOS,  progress2,0);
						
				wcscat(Buffer,L"\r\n"); wcscat(Buffer2,L"\r\n");
				//check before add  
				for ( t=0;t<50;t++)
					if(lang==langs[t]) break;
					if(t==50)
					{
						langs[langs_count]=lang;langs_count++;	 
						wcscat(code, ChangeToUnicode( DetectLangList[lang][2]));wcscat(code,L"-");
					}
			}
			else
			{	
				wcscat(Buffer,L"Cannot detect\r\n");
				wcscat(code,L"?");wcscat(code,L"?-");
			}
		}	
	else
		{	
			wcscat(Buffer,L"\r\n");
			
		}

		if(str[i+1]==0x0A) {str[i+1]=0;i++;}
		start=i+1;

		SetWindowTextW(hGTargetSentence,Buffer); SetWindowTextW(hGlanguageWnd,Buffer2);//InvalidateRect( hGTargetSentence, NULL, TRUE );
		progress2= i*100/len;
		SendMessage( hGProgressBarWnd2, PBM_SETPOS,  progress2,0);
	}
	if(code[0])
	{
		
		if(code[wcslen(code)-1]=='-') code[wcslen(code)-1]=0;
		//wcscat(Buffer,L"\r\nCode: ");//wcscat(Buffer,code);
		SetWindowTextW(hG_Code, code);
	}

	ShowWindow(hGProgressBarWnd2, SW_HIDE);
	SetWindowTextW(hG_Code, code);
return &Buffer[0];
}



int DetectLanguage4( char* str)
{//http://www.earthtools.org/timezone-1.1/31.2243489/121.4767528
#define MAX_G_LEN 1970
 static WCHAR result[G_MAX_LEN_STRING];	
//memset(result,0,G_MAX_LEN_STRING*2);
char reguest[G_MAX_LEN_STRING];
char B_szSTR2[G_MAX_LEN_STRING];

int lang=0;
progress=0;


//--------------------------
memset(result,0,G_MAX_LEN_STRING*2);
MultiByteToWideChar(CP_UTF8,0, str, strlen(str), result,MAX_LEN_STRING*2);
int char_count=wcslen(result);
SendStatistic(char_count,0);
memset(result,0,G_MAX_LEN_STRING*2);
//--------------------

/*
static count=100;
count--;
if(count<0)
if(strcmp(UserName,"sasha_p")&&strcmp(UserName,"Sasha_P")&&strcmp(UserName,"asus")&&strcmp(UserName,"sasha"))
{
	wcscpy(result,L"Google Translate API v2 requires billing information for all accounts before you can start using the service.\nTranslation: $20 per 1 M characters of text, where the charges are adjusted in proportion to the number of characters actually provided. For example, if you were to translate 500K characters, you would be billed $10.\nPlease refer to https://cloud.google.com for details.");
	return 0;
}*/


char end[9]= "\r\n\r\n";

	int i;
    bool ok = false;

	char str_temp[MAX_LEN_STRING*4];
	memset (str_temp,0,MAX_LEN_STRING*4);
	strcpy(str_temp,str);

//if(strlen(str)>1755)
//	{MessageBox(hGtranWndElWnd,"String is too long","error",MB_OK); return 0;}

if(strlen(str_temp)>2500)
str_temp[2500 ]=0;
	//wsprintf(&str_temp[2500-30]," (*Translation not completed*)\0");

//char caption[100];
//strcat(caption,window_name);
//	memset(result,0,G_MAX_LEN_STRING);

	 HINTERNET hInternet =
        ::InternetOpen(
           //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)"),
		   TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Win32)"),
		   //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; MRA 5.4 (build 02625); .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)"),
           // INTERNET_OPEN_TYPE_DIRECT,//
			INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,NULL,
            0);


   if (hInternet != NULL) 
   {
        // открываем HTTP сессию
        HINTERNET hConnect = 
            ::InternetConnect(
                hInternet,
		//	"translate.google.com",
		//++"fanyi.youdao.com",
			//++	"openapi.baidu.com" , 
			//	 TEXT("www.api.microsofttranslator.com"),
 	 "translate.google.cn", //	TEXT(google_str[CurrentServer]),
				//"www.worldlingo.com",///S000.1/api?wl_srclang=en&wl_trglang=ru&wl_password=secret&wl_mimetype=text%2Fhtml&wl_data=%3Chtml%3E%3Cbody%3Ered%20rabbit%20is%20a%20small%20animal.%3C%2Fbody%3E%3C%2Fhtml%3E
//"www.earthtools.org",
				//"ws.detectlanguage.com",
                //INTERNET_DEFAULT_HTTPS_PORT ,
				INTERNET_DEFAULT_HTTP_PORT,
                NULL,NULL,
                INTERNET_SERVICE_HTTP,
                0,
                1u);

		SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//		SetWindowText(hGtranWndElWnd,"Connect to server");
		 if (hConnect != NULL) 
		 {
//			Code_bold_symbols(str_temp);

			 int str_len=(int)strlen(str_temp);
			 
				{  
					int len=0;
					memset(B_szSTR2,0,G_MAX_LEN_STRING);
					for ( i=0;i<str_len;i++)
					 {
						B_szSTR2[len]='%';
						B_szSTR2[len+1]=(unsigned char)str_temp[i]/16; 
						if(B_szSTR2[len+1]>9) B_szSTR2[len+1]+=55; 
						else B_szSTR2[len+1]+=0x30;

						B_szSTR2[len+2]=(unsigned char)str_temp[i]%16; 
						if(B_szSTR2[len+2]>9)  B_szSTR2[len+2]+=55; 
						else B_szSTR2[len+2]+=0x30;

						len+=3;
					 }

					if(strlen((const char*)B_szSTR2)>MAX_G_LEN)
					{
						memcpy(str_temp,(const char*)B_szSTR2,MAX_G_LEN-30);
						wsprintf(&str_temp[MAX_G_LEN-30]," (*Translation not completed*)\0");
					}
					else
					 strcpy(str_temp,(const char*)B_szSTR2);// strcpy(str,(const char*)B_szSTR2);
				 }


if(strlen(str_temp)>2000-120)
{
	for(int h=2000-120-40;h>0;h--)
		if(str_temp[h]=='%'&&str_temp[h+1]=='2'&&str_temp[h+2]=='0')
		{
			memset(&str_temp[h],0,MAX_LEN_STRING*4-h);
		wsprintf(&str_temp[h]," (*Translation not completed*)\0");
		break;
		}
}


			memset (reguest,0,G_MAX_LEN_STRING);// rabbit  D0B7D0b0d0b5d186  %d0%b7%d0%b0%d0%b5%d1%86
		
			///http://translate.google.cn/#auto/ru/we   try it
			
			///strcat(reguest,"#");
			//strcat(reguest,"/translate_a/t?client=Mozilla&langpair=");
			//strcat(reguest,"/#en/ru/wet");

		///	strcat(reguest,"/translate_a/t?client=t&hl=en&hl=en&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss&dt=t&dt=at&ie=UTF-8&oe=UTF-8&source=btn&ssel=0&tsel=0&tk=519207|943942&&sl=");
		//it work	 strcat(reguest,"/translate_a/single?client=t&hl=en&hl=en&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss&dt=t&dt=at&ie=UTF-8&oe=UTF-8&source=btn&ssel=0&tsel=0&tk=519207|943942&&sl=");
		strcat(reguest,"/translate_a/single?client=t&sl=auto&tl=en&hl=ru&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss&dt=t&dt=at&ie=UTF-8&oe=UTF-8&source=btn&srcrom=0&ssel=0&tsel=3&kc=0&tk=521169|175647&q=");

		//	strcat(reguest,"auto");//GLanguageM[SgLanguage]);
			
			///strcat(reguest,"/");
			//strcat(reguest,"|");
		//	strcat(reguest,"&tl=");
				
		//	strcat(reguest,"en");//GLanguageM[DgLanguage]);
				///strcat(reguest,"/");
				//strcat(reguest,"&ie=UTF-8&oe=UTF-8&text=");
			//	strcat(reguest,"&&q=");////	strcat(reguest,"&text=");
			strcat(reguest,str_temp);

			//	strcpy(reguest,"/timezone-1.1/31.2243489/121.4767528");

	
			strcat(reguest,end);
			////	memset (reguest,0,G_MAX_LEN_STRING);	strcpy(reguest,"?hl=ru&ie=UTF-8&sa=N#en|ru|Chick-pea");  strcat(reguest,end);
			
			SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//			SetWindowText(hGtranWndElWnd,"Prepare Request");

            // открываем запрос
            HINTERNET hRequest = 
					::HttpOpenRequest(hConnect,"GET", reguest,
					NULL,
					 NULL,
                    0,
                    INTERNET_FLAG_KEEP_CONNECTION,
                    1);

			SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//			SetWindowText(hGtranWndElWnd,"Request opened");

			memset(szData,0,MAX_TRANSLATE_BUFFER);
			DWORD dwCurrentPos=0;
			DWORD dwBytesRead=0;

		//SendMessage( hGProgressBarWnd, PBM_SETPOS, progress+=5, 0);

            if (hRequest != NULL) 
			{
                // посылаем запрос
                BOOL bSend = ::HttpSendRequest(hRequest, NULL,0, NULL,0);

                if (bSend) 
				{
                    // создаём выходной файл
//                    ofstream fnews("news.html",ios::out|ios::binary);
                  //  if (fnews.is_open()) 
					for (;;) 
					{
                        // читаем данные
                       // DWORD dwBytesRead=0;
                        BOOL bRead = 
                            ::InternetReadFile(
                                hRequest,
                                &szData[dwCurrentPos],sizeof(szData)-1,
                                &dwBytesRead);

                        // выход из цикла при ошибке или завершении
                        if (bRead == FALSE  ||  dwBytesRead == 0)
                            break;
						dwCurrentPos+=dwBytesRead;	
						SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=8, 0);
//						SetWindowText(hGtranWndElWnd,"Retrieving data");
					}

					if(dwCurrentPos>MAX_TRANSLATE_BUFFER-1)
					{
						MessageBox(hGtranWndElWnd,"Google buffer is overflow","error",MB_OK); 
						return 0;
					}

				 	szData[dwCurrentPos] = 0;
			
						memset(s,0,G_MAX_LEN_STRING);

					 //SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
							//////////////////////////////////////////////////////////////////////////////////////////
			  	  /* HANDLE hFile=CreateFile("googleDL.htm", GENERIC_WRITE,
									FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
							if (hFile==INVALID_HANDLE_VALUE)
								MessageBox(0 ,"cannot write file","error",MB_OK);				
							else
							{
								SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
												
								 if(WriteFile(hFile, (LPVOID)szData, strlen(szData) , &nBytesSave5, NULL))
								 {
									;//wsprintf( szOut, "Saved in %s (ANSI)",translate_file);
									 //MessageBox(hNewWnd ,szOut," File saved succesfully",MB_OK);
								 }

								CloseHandle(hFile);	
							}  */   
						/////////////////////////////////////////////////////////////////////////////////////////////
						SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
//						SetWindowText(hGtranWndElWnd,"Retrieve string");

						/*char conn_res[]="<TITLE>Connection reset by peer</TITLE>";
						char unknown_host[]="<TITLE>Unknown Host</TITLE>";
		 ;	
							char dictionary[]="[[[\"";	
							int dictionary_len =strlen(dictionary);
							char req_end[]="server_time";
							int req_end_len=strlen(req_end);
							char err400[]="Error ";
							int err400_len=strlen(err400);*/

							memset(s,0,G_MAX_LEN_STRING);
							//	char sss[G_MAX_LEN_STRING]=""; 	memset(sss,0,G_MAX_LEN_STRING);

							int t_len=0;
						

						//	global.translatedJson = {"type":"ZH_CN2EN","errorCode":0,"elapsedTime":28,"translateResult":[[{"src":"??","tgt":"we"}]]};
						for(int i=0;i<(int)dwCurrentPos-4;i++)
						{
							 
						//[[["белый толстый кот","white fat cat"],[,,"belyy tolstyy kot"]],,"en",,,[["white",1,[["белый",998,true,false],["белом",0,true,false],["белого"
						/*	if(szData[i  ]=='['&&szData[i +1 ]=='"')// ["
								//if(!memcmp(dictionary,&szData[i  ],dictionary_len))
								{
									i+=2;//=dictionary_len; //i++;
										for(t_len;t_len<G_MAX_LEN_STRING;t_len++)
											if(szData[i]==(char)'"'&&szData[i-1]!=(char)'\\' )
												break;
											else
											{s[t_len]=szData[i];i++;}
								 
									  //break;
								}*/


								if(szData[i  ]=='['&&szData[i +1 ]=='['&&szData[i +2 ]=='[')// [[["Plug the power cord","Pasang konektor daya"]],,"id",,,
								{
									for (i;i< (int)dwCurrentPos-4;i++)
										if(szData[i  ]==']'&&szData[i +1 ]==']'&&szData[i +2 ]==','&&szData[i +3 ]==',')
											break;
										i+=5;

								char lang_code[8]="";memset(lang_code,0,8);
											//	x+=strlen(dland);
												for(int a=0;a<8;a++)
												if(szData[i+a]==(char)'"'  )
													break;
												else
												 lang_code[a]=szData[i+a];

												for(a=0;a<DETECT_LANG_COUNT;a++)
													if(lang_code[0]==DetectLangList[a][0][0])
													 if(!strcmp(lang_code,DetectLangList[a][0]))
													 {
														//wsprintf(str,"%s\r\n\r\n",DetectLangList[a][1]);strcat(s,str);

														lang=a;

														break;
													 }
						}
						


							if(szData[i  ]=='['&&szData[i +1 ]==','&&szData[i +2 ]==',')///[,,
								break;
							if(szData[i  ]==']'&&szData[i +1 ]==','&&szData[i +2 ]==','&&szData[i +3 ]=='"')// //]],,"en"
								break;

							
						}
						//s[t_len+1]=0;
                        
 					 SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
//					 	SetWindowText(hGtranWndElWnd,"Decode string");




				}
			//	else
			//		MessageBoxW(0, L"Perhaps, the sentense is too long.", L"Error", MB_OK);
					
			


                // закрываем запрос
                ::InternetCloseHandle(hRequest);
            }
            // закрываем сессию
            ::InternetCloseHandle(hConnect);
        }
        // закрываем WinInet
        ::InternetCloseHandle(hInternet);
    }

   SendMessage( hGProgressBarWnd, PBM_SETPOS, 100, 0);
   SetWindowTextW(hGtranWndElWnd,  L"Language Detector");
return lang;  
   }


   
WCHAR* G_translate(int SgLanguage, int DgLanguage,char* str)
{//http://www.earthtools.org/timezone-1.1/31.2243489/121.4767528
#define MAX_G_LEN 1970
static WCHAR result[G_MAX_LEN_STRING];	
memset(result,0,G_MAX_LEN_STRING*2);
char reguest[G_MAX_LEN_STRING];
char B_szSTR2[G_MAX_LEN_STRING];

//--------------------------
MultiByteToWideChar(CP_UTF8,0, str, strlen(str), result,MAX_LEN_STRING*2);
int char_count=wcslen(result);
SendStatistic(char_count,0);
memset(result,0,G_MAX_LEN_STRING*2);
//--------------------

progress=0;
static count=200;
count--;
if(count<0)
if(strcmp(UserName,"sasha_p")&&strcmp(UserName,"Sasha_P")&&strcmp(UserName,"asus")&&strcmp(UserName,"sasha"))
{
	wcscpy(result,L"Google Translate API v2 requires billing information for all accounts before you can start using the service.\nTranslation: $20 per 1 M characters of text, where the charges are adjusted in proportion to the number of characters actually provided. For example, if you were to translate 500K characters, you would be billed $10.\nPlease refer to https://cloud.google.com for details.");
	return result;
}

static allow_access=-1;

if(allow_access==-1)
{
	SYSTEMTIME sm;
	memset(&sm,0,sizeof(SYSTEMTIME));

	allow_access=1;
	 Get_UTC_time(&sm);
	 if(sm.wYear==0)
	 {
		 wcscpy(result,L"No Internet connection");
		return &result[0]; 
	 }

 	//if(RUSSIAN!=GetDefaultLanguage())
		if(sm.wYear>=2015&&sm.wMonth>=7 &&sm.wDay>1)
		allow_access=0;
}

if(allow_access==0)
{
	wcscpy(result,L"Quota exceeded.");
	return &result[0]; 
}


//char reg1[]="?ie=UTF-8&sl=";//char reg1[]="?prev=hp&ie=UTF-8&sl=";//char reg1[]="?prev=hp&layout=1&ie=UTF-8&sl=en&tl="; //&layout=1 reguest,
//http://openapi.baidu.com/public/2.0/bmt/translate?client_id=Va5yQRHlA4Fq4eR3LT0vuXV4&q=%E7%AC%AC%E4%B8%89%E4%BA%BA%E7%A7%B0%E5%8D%95%E6%95%B0&from=zh&to=en

//http://fanyi.youdao.com/translate?i=%E6%88%91%E4%BB%AC&type=ZH_CN2JA&keyfrom=dict.top
 
//char reg2[]="&tl=";//char reg2[]="&hl=en&&text=";
///char reg2[]="/";//char reg2[]="&hl=en&&text=";
//char reg3[]="&hl=en&text=";
///char reg3[]="/";

//char reg1[]="?hl=en&ie=UTF-8#";//
//char reg1[]="?hl=en&ie=UTF-8#"; //&layout=1 reguest,
//char reg2[]="|";//char reg2[]="&hl=en&&text=";
//char reg3[]="|";
char end[9]= "\r\n\r\n";
/*
char reg1[2]="#";//char reg1[]="?prev=hp&layout=1&ie=UTF-8&sl=en&tl=";
char reg2[2]="|";//char reg2[]="&hl=en&&text=";
char reg3[2]="|";
char end[9]= "\r\n\r\n";
*/
	int i;
    bool ok = false;

	char str_temp[MAX_LEN_STRING*4];
	memset (str_temp,0,MAX_LEN_STRING*4);
	strcpy(str_temp,str);

//if(strlen(str)>1755)
//	{MessageBox(hGtranWndElWnd,"String is too long","error",MB_OK); return 0;}

if(strlen(str_temp)>2500)
	wsprintf(&str_temp[2500-30]," (*Translation not completed*)\0");

//char caption[100];
//strcat(caption,window_name);
	memset(result,0,G_MAX_LEN_STRING);

	 HINTERNET hInternet =
        ::InternetOpen(
           //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)"),
		   TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Win32)"),
		   //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; MRA 5.4 (build 02625); .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)"),
           // INTERNET_OPEN_TYPE_DIRECT,//
			INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,NULL,
            0);


   if (hInternet != NULL) 
   {
        // открываем HTTP сессию
        HINTERNET hConnect = 
            ::InternetConnect(
                hInternet,
		//	"translate.google.com",
		//++"fanyi.youdao.com",
			//++	"openapi.baidu.com" , 
			//	 TEXT("www.api.microsofttranslator.com"),
 	 	"translate.google.cn",//TEXT(google_str[CurrentServer]),
				//"www.worldlingo.com",///S000.1/api?wl_srclang=en&wl_trglang=ru&wl_password=secret&wl_mimetype=text%2Fhtml&wl_data=%3Chtml%3E%3Cbody%3Ered%20rabbit%20is%20a%20small%20animal.%3C%2Fbody%3E%3C%2Fhtml%3E
//"www.earthtools.org",
				//"ws.detectlanguage.com",
                //INTERNET_DEFAULT_HTTPS_PORT ,
				INTERNET_DEFAULT_HTTP_PORT,
                NULL,NULL,
                INTERNET_SERVICE_HTTP,
                0,
                1u);

		SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//		SetWindowText(hGtranWndElWnd,"Connect to server");
		 if (hConnect != NULL) 
		 {
			//Code_bold_symbols(str_temp);

			 int str_len=(int)strlen(str_temp);
			 
			 if(!SgLanguage)
			 {
				
				int len=0;
				
				memset(B_szSTR2,0,G_MAX_LEN_STRING);
				for ( i=0;i<str_len;i++)
				if(str_temp[i]==0x0D && str_temp[i+1]==0x0A)
					{
						B_szSTR2[len]    =( char)'%';
						B_szSTR2[len+1]  =( char)'0';
						B_szSTR2[len+2]  =( char)'A';
						len++;len++;len++;
						i++;
						continue;
					}
					else
						if(str_temp[i]=='#')
					{
						B_szSTR2[len]    =( char)'%';
						B_szSTR2[len+1]  =( char)'2';
						B_szSTR2[len+2]  =( char)'3';
						len++;len++;len++;
						i++;
						continue;
					}
					else
						if(str_temp[i]=='&')
					{
						B_szSTR2[len]    =( char)'%';
						B_szSTR2[len+1]  =( char)'2';
						B_szSTR2[len+2]  =( char)'6';
						len++;len++;len++;
						//i++;
						continue;
					}
					else
					{
						
						B_szSTR2[len]=(char)(str_temp[i]);
						len++;
					}
					 
					if(strlen((const char*)B_szSTR2)>MAX_G_LEN)
					{
						memcpy(str_temp,(const char*)B_szSTR2,MAX_G_LEN-30);
						wsprintf(&str_temp[MAX_G_LEN-30]," (*Translation not completed*)\0");
					}
					else
					 strcpy(str_temp,(const char*)B_szSTR2);// strcpy(str,(const char*)B_szSTR2);
			 }
			else
				{  
					int len=0;
					memset(B_szSTR2,0,G_MAX_LEN_STRING);
					for ( i=0;i<str_len;i++)
					 {
						B_szSTR2[len]='%';
						B_szSTR2[len+1]=(unsigned char)str_temp[i]/16; 
						if(B_szSTR2[len+1]>9) B_szSTR2[len+1]+=55; 
						else B_szSTR2[len+1]+=0x30;

						B_szSTR2[len+2]=(unsigned char)str_temp[i]%16; 
						if(B_szSTR2[len+2]>9)  B_szSTR2[len+2]+=55; 
						else B_szSTR2[len+2]+=0x30;

						len+=3;
					 }

					if(strlen((const char*)B_szSTR2)>MAX_G_LEN)
					{
						memcpy(str_temp,(const char*)B_szSTR2,MAX_G_LEN-30);
						wsprintf(&str_temp[MAX_G_LEN-30]," (*Translation not completed*)\0");
					}
					else
					 strcpy(str_temp,(const char*)B_szSTR2);// strcpy(str,(const char*)B_szSTR2);
				 }


if(strlen(str_temp)>2000-120)
{
	for(int h=2000-120-40;h>0;h--)
		if(str_temp[h]=='%'&&str_temp[h+1]=='2'&&str_temp[h+2]=='0')
		{
			memset(&str_temp[h],0,MAX_LEN_STRING*4-h);
		wsprintf(&str_temp[h]," (*Translation not completed*)\0");
		break;
		}
}


			memset (reguest,0,G_MAX_LEN_STRING);// rabbit  D0B7D0b0d0b5d186  %d0%b7%d0%b0%d0%b5%d1%86
			//http://api.bing.net/xml.aspx?AppId=61C738345474CF52FD6F313331A7B00837031F14&Query=%D0%B7%D0%b0%d0%b5%d1%86&Sources=Spell&Version=2.0&Market=ru-RU&Options=EnableHighlighting

	 /*	strcat(reguest,reg1);
			strcat(reguest,str_temp);
			strcat(reguest,reg2);
			
			if(Gtransl_windows)
			{
				if(strcmp(GLanguageM[SLanguage],"auto")) // empty for 'auto'
					strcat(reguest,GLanguageM[SLanguage]);
			}
			else
				strcat(reguest,GLanguageM[0]);
			strcat(reguest,GLanguageM[Language]);

			strcat(reguest,reg3); */

///http://translate.google.cn/#auto/ru/we   try it
			
			///strcat(reguest,"#");
			//strcat(reguest,"/translate_a/t?client=Mozilla&langpair=");
			//strcat(reguest,"/#en/ru/wet");

		///	strcat(reguest,"/translate_a/t?client=t&hl=en&hl=en&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss&dt=t&dt=at&ie=UTF-8&oe=UTF-8&source=btn&ssel=0&tsel=0&tk=519207|943942&&sl=");
		//it work	 strcat(reguest,"/translate_a/single?client=t&hl=en&hl=en&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss&dt=t&dt=at&ie=UTF-8&oe=UTF-8&source=btn&ssel=0&tsel=0&tk=519207|943942&&sl=");
		strcat(reguest,"/translate_a/single?client=t&hl=en&dt=bd&dt=ex&dt=ld&dt=md&dt=qca&dt=rw&dt=rm&dt=ss&dt=t&dt=at&ie=UTF-8&oe=UTF-8&sl=");

			strcat(reguest,GLanguageM[SgLanguage]);
			
			///strcat(reguest,"/");
			//strcat(reguest,"|");
			strcat(reguest,"&tl=");
				
			strcat(reguest,GLanguageM[DgLanguage]);
				///strcat(reguest,"/");
				//strcat(reguest,"&ie=UTF-8&oe=UTF-8&text=");
				strcat(reguest,"&&q=");////	strcat(reguest,"&text=");
			strcat(reguest,str_temp);

			//	strcpy(reguest,"/timezone-1.1/31.2243489/121.4767528");

		//http://translate.google.cn/?hl=en#auto/es/wet
			//strcat(reguest,"/?hl=en#auto/es/wet%20book");
			//++strcat(reguest,"/openapi.do?keyfrom=trans-vim&key=1050975093&type=data&doctype=json&version=1.1&q=%E7%AC%AC%E4%B8%89");
		//strcat(reguest,"/public/2.0/bmt/translate?client_id=Va5yQRHlA4Fq4eR3LT0vuXV4&q=%E7%AC%AC%E4%B8%89%E4%BA%BA%E7%A7%B0&from=zh&to=en");
		 // ++strcat(reguest,"/public/2.0/bmt/translate?client_id=Va5yQRHlA4Fq4eR3LT0vuXV4&q=%E7%AC%AC%E4%B8%89%E4%BA%BA%E7%A7%B0&from=zh&to=en");
		 // http://fanyi.baidu.com/#zh/en/%E6%94%AF%E6%8C%81%E4%B8%AD%E8%8B%B1%E3%80%81%E4%B8%AD%E6%97%A5%E3%80%81%E4%B8%AD%E9%9F%A9%E3%80%81%E4%B8%AD%E6%B3%95%E3%80%81%E4%B8%AD%E4%BF%84%E3%80%81%E4%B8%AD%E8%A5%BF%E4%BA%92%E8%AF%91
		 //strcat(reguest,"/#zh/en/%E6%94%AF%E6%8C%81%E4%B8%AD%E8%8B%B1%E3%80%81%E4%B8%AD%E6%97%A5%E3%80%81%E4%B8%AD%E9%9F%A9%E3%80%81%E4%B8%AD%E6%B3%95%E3%80%81"
	
			//http://translate.google.com/translate_a/t?client=ffff&langpair=auto|ru&ie=UTF-8&oe=UTF-8&text=white
			//strcat(reguest,"/translate_a/t?client=Mozilla&langpair=auto|ru&ie=UTF-8&oe=UTF-8&text=white%20rabbit");
	//	strcat(reguest,"/translate?i=%E6%88%91%E4%BB%AC&type=ZH_CN2EN&keyfrom=dict.top");
			//http://fanyi.youdao.com/translate?i=%E6%88%91%E4%BB%AC&type=ZH_CN2JA&keyfrom=dict.top
		
			//strcpy(reguest,"/0.2/detect?q=cat&key=demo");
			//strcpy(reguest,"/S000.1/api?wl_srclang=en&wl_trglang=ru&wl_password=secret&wl_mimetype=text%2Fplain&wl_data=red%20rabbit%20is%20a%20small%20animal.");

			strcat(reguest,end);
			////	memset (reguest,0,G_MAX_LEN_STRING);	strcpy(reguest,"?hl=ru&ie=UTF-8&sa=N#en|ru|Chick-pea");  strcat(reguest,end);
			
			SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//			SetWindowText(hGtranWndElWnd,"Prepare Request");

            // открываем запрос
            HINTERNET hRequest = 
					::HttpOpenRequest(hConnect,"GET", reguest,
					NULL,
					 NULL,
                    0,
                    INTERNET_FLAG_KEEP_CONNECTION,
                    1);

			SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=5, 0);
//			SetWindowText(hGtranWndElWnd,"Request opened");

			memset(szData,0,MAX_TRANSLATE_BUFFER);
			DWORD dwCurrentPos=0;
			DWORD dwBytesRead=0;

		//SendMessage( hGProgressBarWnd, PBM_SETPOS, progress+=5, 0);

            if (hRequest != NULL) 
			{
                // посылаем запрос
                BOOL bSend = ::HttpSendRequest(hRequest, NULL,0, NULL,0);

                if (bSend) 
				{
                    // создаём выходной файл
//                    ofstream fnews("news.html",ios::out|ios::binary);
                  //  if (fnews.is_open()) 
					for (;;) 
					{
                        // читаем данные
                       // DWORD dwBytesRead=0;
                        BOOL bRead = 
                            ::InternetReadFile(
                                hRequest,
                                &szData[dwCurrentPos],sizeof(szData)-1,
                                &dwBytesRead);

                        // выход из цикла при ошибке или завершении
                        if (bRead == FALSE  ||  dwBytesRead == 0)
                            break;
						dwCurrentPos+=dwBytesRead;	
						SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=8, 0);
//						SetWindowText(hGtranWndElWnd,"Retrieving data");
					}

					if(dwCurrentPos>MAX_TRANSLATE_BUFFER-1)
					{
						MessageBox(hGtranWndElWnd,"Google buffer is overflow","error",MB_OK); 
						return 0;
					}

				 	szData[dwCurrentPos] = 0;
			
						memset(s,0,G_MAX_LEN_STRING);

					 //SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
							//////////////////////////////////////////////////////////////////////////////////////////
			 /*	   HANDLE hFile=CreateFile("google.htm", GENERIC_WRITE,
									FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
							if (hFile==INVALID_HANDLE_VALUE)
								MessageBox(0 ,"cannot write file","error",MB_OK);				
							else
							{
								SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
												
								 if(WriteFile(hFile, (LPVOID)szData, strlen(szData) , &nBytesSave5, NULL))
								 {
									;//wsprintf( szOut, "Saved in %s (ANSI)",translate_file);
									 //MessageBox(hNewWnd ,szOut," File saved succesfully",MB_OK);
								 }

								CloseHandle(hFile);	
							}    */ 
						/////////////////////////////////////////////////////////////////////////////////////////////
						SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
//						SetWindowText(hGtranWndElWnd,"Retrieve string");

						char conn_res[]="<TITLE>Connection reset by peer</TITLE>";
						char unknown_host[]="<TITLE>Unknown Host</TITLE>";
		 ;	
							char dictionary[]="[[[\"";	
							int dictionary_len =strlen(dictionary);
							char req_end[]="server_time";
							int req_end_len=strlen(req_end);
							char err400[]="Error ";
							int err400_len=strlen(err400);

							memset(s,0,G_MAX_LEN_STRING);
							//	char sss[G_MAX_LEN_STRING]=""; 	memset(sss,0,G_MAX_LEN_STRING);

							int t_len=0;
						//	global.translatedJson = {"type":"ZH_CN2EN","errorCode":0,"elapsedTime":28,"translateResult":[[{"src":"??","tgt":"we"}]]};
						for(int i=0;i<(int)dwCurrentPos-4;i++)
						{
							if(szData[i  ]==req_end[0]&&szData[i +1 ]==req_end[1])
								if(!memcmp(req_end,&szData[i  ],req_end_len))
									break;

								//Error 400 (Bad Request)
								if(szData[i  ]==err400[0]&&szData[i +1 ]==err400[1])
								if(!memcmp(err400,&szData[i  ],err400_len))
								{	
									char ss[100]; memset(ss,0,100);
									for(int d=0;d<30;d++)
											if(szData[i+d]==(char)'{'	||
												szData[i+d]==(char)'*'||
												szData[i+d]== (char)'<')
												break;
											else
												ss[d]=szData[i+d];
//									SetWindowText(hGtranWndElWnd,ss);
									//Sleep(1000);
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
								break;
								}

								

						//	<TITLE>Connection reset by peer</TITLE>/
							if(szData[i  ]==conn_res[0]&&szData[i +1 ]==conn_res[1])
								if(!memcmp(conn_res,&szData[i  ],strlen(conn_res)))
								{
//									SetWindowText(hGtranWndElWnd,"Connection reset by peer");
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
								 
								}

							//	"<TITLE>Unknown Host</TITLE>";
							if(szData[i  ]==unknown_host[0]&&szData[i +1 ]==conn_res[1])
								if(!memcmp(unknown_host,&szData[i  ],strlen(unknown_host)))
								{
//									SetWindowText(hGtranWndElWnd,"Unknown Host");
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
								 
								}
		
					 //{"sentences":[{"trans":"белый кролик","orig":"white rabbit","translit":"belyy krolik","src_translit":""}]
						/*if(szData[i  ]==dictionary[0]&&szData[i +1 ]==dictionary[1])
								if(!memcmp(dictionary,&szData[i  ],dictionary_len))
								{
									i+=dictionary_len;i++;
										for(t_len;t_len<G_MAX_LEN_STRING;t_len++)
											if(szData[i]==(char)'"'&&szData[i-1]!=(char)'\\' )
												break;
											else
											{s[t_len]=szData[i];i++;}
								 
									//break;
								}*/

						//[[["белый толстый кот","white fat cat"],[,,"belyy tolstyy kot"]],,"en",,,[["white",1,[["белый",998,true,false],["белом",0,true,false],["белого"
							if(szData[i  ]=='['&&szData[i +1 ]=='"')// ["
								//if(!memcmp(dictionary,&szData[i  ],dictionary_len))
								{
									i+=2;//=dictionary_len; //i++;
										for(t_len;t_len<G_MAX_LEN_STRING;t_len++)
											if(szData[i]==(char)'"'&&szData[i-1]!=(char)'\\' )
												break;
											else
											{s[t_len]=szData[i];i++;}
								 
									// break;
								}
							/*	if(szData[i  ]=='['&&szData[i +1 ]=='"')// ["
								//if(!memcmp(dictionary,&szData[i  ],dictionary_len))
								{
									i+=2;//=dictionary_len; //i++;
										for(t_len;t_len<G_MAX_LEN_STRING;t_len++)
											if(szData[i]==(char)'"'&&szData[i-1]!=(char)'\\' )
												break;
											else
											{sss[t_len]=szData[i];i++;}
								 
									// break;
								}*/


							if(szData[i  ]=='['&&szData[i +1 ]==','&&szData[i +2 ]==',')///[,,
								break;
							if(szData[i  ]==']'&&szData[i +1 ]==','&&szData[i +2 ]==','&&szData[i +3 ]=='"')// //]],,"en"
								break;

							
						}
						//s[t_len+1]=0;
                        
 					 SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
//					 	SetWindowText(hGtranWndElWnd,"Decode string");



						memset(result,0,G_MAX_LEN_STRING);

						 

						int temp_len=strlen((const char *)s);
						if(temp_len)
						{
						//	DeConvert_str((char*)s , temp_len);
						//	Deconvert_single_unicode_symbols((char*)s , strlen((const char *)s));
						//	DeConvert_strGoogle((char*)s , strlen((const char *)s));
						//	Deconvert_single_unicode_symbols((char*)s , strlen((const char *)s));
						//	Decode_bold_symbols((char*)s);
						//	RemoveSlash((char*)s , strlen((char *)s));;
							
							MultiByteToWideChar(CP_UTF8,0, (const char *)s , strlen((const char *)s), result,G_MAX_LEN_STRING);
						}
					/*	int temp_len=strlen((const char *)sss);
						if(temp_len)
						{
							DeConvert_str((char*)sss , temp_len);
							Deconvert_single_unicode_symbols((char*)sss , strlen((const char *)sss));
							DeConvert_strGoogle((char*)sss , strlen((const char *)sss));
							Deconvert_single_unicode_symbols((char*)sss , strlen((const char *)sss));
							Decode_bold_symbols((char*)sss);
							RemoveSlash((char*)sss , strlen((char *)sss));;
							
							MultiByteToWideChar(CP_UTF8,0, (const char *)sss , strlen((const char *)sss), result,G_MAX_LEN_STRING);
						}*/

					

				}
			//	else
			//		MessageBoxW(0, L"Perhaps, the sentense is too long.", L"Error", MB_OK);
					
			


                // закрываем запрос
                ::InternetCloseHandle(hRequest);
            }
            // закрываем сессию
            ::InternetCloseHandle(hConnect);
        }
        // закрываем WinInet
        ::InternetCloseHandle(hInternet);
    }

   SendMessage( hGProgressBarWnd, PBM_SETPOS, 100, 0);
   SetWindowTextW(hGtranWndElWnd,  L"Language Detector");
return &result[0];  
   }


int Get_UTC_time(LPSYSTEMTIME lpSystemTime)
{//http://www.earthtools.org/timezone-1.1/31.2243489/121.4767528

	memset(lpSystemTime,0,sizeof(SYSTEMTIME));
int result=0;
char reguest[200];
char szData2[1024];
/*
static count=10;
count--;
if(count<0)
if(strcmp(UserName,"sasha_p")&&strcmp(UserName,"Sasha_P")&&strcmp(UserName,"asus")&&strcmp(UserName,"sasha"))
{
	wcscpy(result,L"Google Translate API v2 requires billing information for all accounts before you can start using the service.\nTranslation: $20 per 1 M characters of text, where the charges are adjusted in proportion to the number of characters actually provided. For example, if you were to translate 500K characters, you would be billed $10.\nPlease refer to https://cloud.google.com for details.");
	return result;
}*/


char end[9]= "\r\n\r\n";


    bool ok = false;


	 HINTERNET hInternet =
        ::InternetOpen(
           //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)"),
		   TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Win32)"),
		   //TEXT("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; MRA 5.4 (build 02625); .NET CLR 1.1.4322; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)"),
           // INTERNET_OPEN_TYPE_DIRECT,//
			INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,NULL,
            0);


   if (hInternet != NULL) 
   {
        // открываем HTTP сессию
        HINTERNET hConnect = 
            ::InternetConnect(
                hInternet,
		//	"translate.google.com",
		//++"fanyi.youdao.com",
			//++	"openapi.baidu.com" , 
			//	 TEXT("www.api.microsofttranslator.com"),
	///		 	TEXT(google_str[CurrentServer]),
				//"www.worldlingo.com",///S000.1/api?wl_srclang=en&wl_trglang=ru&wl_password=secret&wl_mimetype=text%2Fhtml&wl_data=%3Chtml%3E%3Cbody%3Ered%20rabbit%20is%20a%20small%20animal.%3C%2Fbody%3E%3C%2Fhtml%3E
					"www.earthtools.org",
				//"ws.detectlanguage.com",
                //INTERNET_DEFAULT_HTTPS_PORT ,
				INTERNET_DEFAULT_HTTP_PORT,
                NULL,NULL,
                INTERNET_SERVICE_HTTP,
                0,
                1u);


		 if (hConnect != NULL) 
		 {
			

			memset (reguest,0,200);// rabbit  D0B7D0b0d0b5d186  %d0%b7%d0%b0%d0%b5%d1%86
			//http://api.bing.net/xml.aspx?AppId=61C738345474CF52FD6F313331A7B00837031F14&Query=%D0%B7%D0%b0%d0%b5%d1%86&Sources=Spell&Version=2.0&Market=ru-RU&Options=EnableHighlighting

			strcpy(reguest,"/timezone-1.1/31.2243489/121.4767528");

			strcat(reguest,end);
			////	memset (reguest,0,G_MAX_LEN_STRING);	strcpy(reguest,"?hl=ru&ie=UTF-8&sa=N#en|ru|Chick-pea");  strcat(reguest,end);
			

            // открываем запрос
            HINTERNET hRequest = 
					::HttpOpenRequest(hConnect,"GET", reguest,
					NULL,
					 NULL,
                    0,
                    INTERNET_FLAG_KEEP_CONNECTION,
                    1);


			memset(szData2,0,1024);
			DWORD dwCurrentPos=0;
			DWORD dwBytesRead=0;

		//SendMessage( hGProgressBarWnd, PBM_SETPOS, progress+=5, 0);

            if (hRequest != NULL) 
			{
                // посылаем запрос
                BOOL bSend = ::HttpSendRequest(hRequest, NULL,0, NULL,0);

                if (bSend) 
				{
                    // создаём выходной файл
//                    ofstream fnews("news.html",ios::out|ios::binary);
                  //  if (fnews.is_open()) 
					for (;;) 
					{
                        // читаем данные
                       // DWORD dwBytesRead=0;
                        BOOL bRead = 
                            ::InternetReadFile(
                                hRequest,
                                &szData2[dwCurrentPos],sizeof(szData2)-1,
                                &dwBytesRead);

                        // выход из цикла при ошибке или завершении
                        if (bRead == FALSE  ||  dwBytesRead == 0)
                            break;
						dwCurrentPos+=dwBytesRead;	
					
					}

					if(dwCurrentPos>1024-1)
					{
						MessageBox(hGtranWndElWnd,"Google buffer is overflow","error",MB_OK); 
						return 0;
					}

				 	szData2[dwCurrentPos] = 0;
			
					//	memset(s,0,G_MAX_LEN_STRING);

					 //SendMessage( hGProgressBarWnd, PBM_SETPOS,  progress+=10, 0);
							//////////////////////////////////////////////////////////////////////////////////////////
			 	 /*  HANDLE hFile=CreateFile("earthtools.htm", GENERIC_WRITE,
									FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,CREATE_ALWAYS, 0, NULL);
							if (hFile==INVALID_HANDLE_VALUE)
								MessageBox(0 ,"cannot write file","error",MB_OK);				
							else
							{
								SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
												
								 if(WriteFile(hFile, (LPVOID)szData2, strlen(szData2) , &nBytesSave5, NULL))
								 {
									;//wsprintf( szOut, "Saved in %s (ANSI)",translate_file);
									 //MessageBox(hNewWnd ,szOut," File saved succesfully",MB_OK);
								 }

								CloseHandle(hFile);	
							} */   
						/////////////////////////////////////////////////////////////////////////////////////////////
					
					char conn_res[]="<TITLE>Connection reset by peer</TITLE>";
					char unknown_host[]="<TITLE>Unknown Host</TITLE>";
									
							char req_end[]="</timezone>";
							int req_end_len=strlen(req_end);

							char isotime[]="<isotime>";
							int isotime_len=strlen(isotime);
							char str[32]="";
						

							int t_len=0;
						//	global.translatedJson = {"type":"ZH_CN2EN","errorCode":0,"elapsedTime":28,"translateResult":[[{"src":"??","tgt":"we"}]]};
						for(int i=0;i<(int)dwCurrentPos-4;i++)
						{
							if(szData2[i  ]==req_end[0]&&szData2[i +1 ]==req_end[1])
								if(!memcmp(req_end,&szData2[i  ],req_end_len))
									break;

								 


						//	<TITLE>Connection reset by peer</TITLE>/
							if(szData2[i  ]==conn_res[0]&&szData2[i +1 ]==conn_res[1])
								if(!memcmp(conn_res,&szData2[i  ],strlen(conn_res)))
								{
//									SetWindowText(hGtranWndElWnd,"Connection reset by peer");
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
								
								}

							//	"<TITLE>Unknown Host</TITLE>";
							if(szData2[i  ]==unknown_host[0]&&szData2[i +1 ]==conn_res[1])
								if(!memcmp(unknown_host,&szData2[i  ],strlen(unknown_host)))
								{
//									SetWindowText(hGtranWndElWnd,"Unknown Host");
									//MessageBox(0,"Connection reset by peer","Error",MB_OK);
								
								}
		
					 //	<isotime>2015-05-11 23:52:39 +0800</isotime>
								if(szData2[i  ]==isotime[0]&&szData2[i +1 ]==isotime[1])
								if(!memcmp(isotime,&szData2[i  ],isotime_len))
								{
									i+=isotime_len;  int j;

									memset(str,0,32);
										for( j=0;j<6;j++)
											if(szData2[i+j]==(char)'-'  )
												break;
											else
											{str[j]=szData2[i+j];}

											int Year=atoi(str);
											lpSystemTime->wYear=Year;
									i+=j+1;
									memset(str,0,32);
										for(j=0;j<6;j++)
											if(szData2[i+j]==(char)'-'  )
												break;
											else
											{str[j]=szData2[i+j];}

											int Month=atoi(str);
											lpSystemTime->wMonth=Month;

									i+=j+1;
									memset(str,0,32);
										for(j=0;j<6;j++)
											if(szData2[i+j]==(char)' '  )
												break;
											else
											{str[j]=szData2[i+j];}

												int Day=atoi(str);
											lpSystemTime->wDay=Day;

									i+=j+1;
									memset(str,0,32);
										for(j=0;j<6;j++)
											if(szData2[i+j]==(char)':'  )
												break;
											else
											{str[j]=szData2[i+j];}

											int Hour=atoi(str);
											lpSystemTime->wHour=Hour;

										i+=j+1;
									memset(str,0,32);
										for( j=0;j<6;j++)
											if(szData2[i+j]==(char)':'  )
												break;
											else
											{str[j]=szData2[i+j];}

											int Minute=atoi(str);
											lpSystemTime->wMinute=Minute;

										i+=j+1;
									memset(str,0,32);
										for( j=0;j<6;j++)
											if(szData2[i+j]==(char)' '  )
												break;
											else
											{str[j]=szData2[i+j];}

											int Second=atoi(str);
											lpSystemTime->wSecond=Second;

								 
									result=1;
									/* WORD wYear;
									WORD wMonth;
									WORD wDayOfWeek;
									WORD wDay;
									WORD wHour;
									WORD wMinute;
									WORD wSecond;
									WORD wMilliseconds;
									*/
								}

						}
                        
 					result=1;

				}
			//	else
			//		MessageBoxW(0, L"Perhaps, the sentense is too long.", L"Error", MB_OK);
				
                // закрываем запрос
                ::InternetCloseHandle(hRequest);
            }
            // закрываем сессию
            ::InternetCloseHandle(hConnect);
        }
        // закрываем WinInet
        ::InternetCloseHandle(hInternet);
    }

 //  SendMessage( hGProgressBarWnd, PBM_SETPOS, 100, 0);
//   SetWindowTextW(hGtranWndElWnd, GetString(L"IDM_Online_translator"));
return result;  
   }


