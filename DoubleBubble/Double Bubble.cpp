#include <iostream>
#include <time.h>
#include "Board.h"
#include "MouseMovement.h"
#include <Windows.h>
#include "wtypes.h"
#include <vector>
#include <string>
#include <fstream>
#include <cassert>

using namespace std;

int SCREENWIDTH = -1;
int SCREENHEIGHT = -1;
// Pure wrapper class so I can write a good operator==
class PBOARD
{
public:
	Board * myBoard;
};

enum BOARDSTATUS { ACTIVE, SCOREBOARD, NOTVALID };

class BoardCollection
{
public:
	BoardCollection(Board A, int x, int y, BOARDSTATUS mystatus)
	{
		mBoard = A;
		anchorX = x;
		anchorY = y;
		status = mystatus;
	}
	Board mBoard;
	int anchorX;
	int anchorY;
	BOARDSTATUS status;
	int myScore;
};
void colorPickerLoop()
{
	POINT * curserpos = new POINT();
	while (true)
	{
		HDC myHDC = GetDC(NULL);
		GetCursorPos(curserpos);
		COLORREF TURTLE = GetPixel(myHDC, curserpos->x, curserpos->y);
		cout << hex << TURTLE << endl;
	}
}
//BOOL SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
//{
//  HDC hDC;
//
//  int iBits;
//
//  WORD wBitCount;
//
//  DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;
//
//  BITMAP Bitmap;
//
//  BITMAPFILEHEADER bmfHdr;
//
//  BITMAPINFOHEADER bi;
//
//  LPBITMAPINFOHEADER lpbi;
//
//  HANDLE fh, hDib, hPal,hOldPal=NULL;
// 
//  hDC = CreateDC("DISPLAY" L, NULL, NULL, NULL);
//  iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
//  DeleteDC(hDC);
//  if (iBits <= 1)
//    wBitCount = 1;
//  else if (iBits <= 4)
//    wBitCount = 4;
//  else if (iBits <= 8)
//    wBitCount = 8;
//  else
//    wBitCount = 24;
//  GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
//  bi.biSize = sizeof(BITMAPINFOHEADER);
//  bi.biWidth = Bitmap.bmWidth;
//  bi.biHeight = Bitmap.bmHeight;
//  bi.biPlanes = 1;
//  bi.biBitCount = wBitCount;
//  bi.biCompression = BI_RGB;
//  bi.biSizeImage = 0;
//  bi.biXPelsPerMeter = 0;
//  bi.biYPelsPerMeter = 0;
//  bi.biClrImportant = 0;
//  bi.biClrUsed = 0;
//  dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
//
//  hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
//  lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
//  *lpbi = bi;
//
//  hPal = GetStockObject(DEFAULT_PALETTE);
//  if (hPal)
//  {
//    hDC = GetDC(NULL);
//    hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
//    RealizePalette(hDC);
//  }
// 
//
//  GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
//    +dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
//
//  if (hOldPal)
//  {
//    SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
//    RealizePalette(hDC);
//    ReleaseDC(NULL, hDC);
//  }
//
//  fh = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,
//    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
// 
//  if (fh == INVALID_HANDLE_VALUE)
//    return FALSE;
//
//  bmfHdr.bfType = 0x4D42; // "BM"
//  dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
//  bmfHdr.bfSize = dwDIBSize;
//  bmfHdr.bfReserved1 = 0;
//  bmfHdr.bfReserved2 = 0;
//  bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
//
//  WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
//
//  WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
//  GlobalUnlock(hDib);
//  GlobalFree(hDib);
//  CloseHandle(fh);
//  return TRUE;
//}
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
struct vec4
{
	vec4(int RR, int GG, int BB, int AA)
	{
		R = RR;
		G = GG;
		B = BB;
		A = AA;
	}
	vec4()
	{}
	int R;
	int G;
	int B;
	int A;
};
bool operator== (vec4 a, vec4 b)
{
	return a.A == b.A && a.B == b.B && a.G == b.G && a.R == b.R;
}
bool operator== (PBOARD A, PBOARD B)
{
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS; c++)
		{
			if (A.myBoard->mboard[r][c] != B.myBoard->mboard[r][c])
			{
				return false;
			}
		}
	}
	return true;
}
bool operator== (Board A, Board B)
{
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS; c++)
		{
			if (A.mboard[r][c] != B.mboard[r][c])
			{
				return false;
			}
		}
	}
	return true;
}
vector<BoardCollection>  lookforbilge(BYTE* bmpBuffer, int size)
{
	vector<BoardCollection> ret;
	const vec4 ad(211, 211, 211, 212);
	const vec4 aw(181, 181, 181, 182);
	const vec4 bd(251, 251, 252, 251);
	const vec4 bw(197, 197, 198, 197);
	const vec4 cd(173, 170, 170, 170);
	const vec4 cw(166, 165, 165, 165);
	const vec4 dd(245, 245, 245, 245);
	const vec4 dw(195, 195, 195, 195);
	const vec4 ed(213, 213, 197, 200);
	const vec4 ew(182, 182, 176, 177);
	const vec4 fd(204, 204, 204, 204);
	const vec4 fw(179, 179, 179, 179);
	const vec4 gd(244, 236, 234, 235);
	const vec4 gw(195, 191, 191, 191);
	//specials
	//puffer
	const vec4 pd(150, 245, 172, 212);
	const vec4 pw(157, 195, 166, 182);
	//krab
	const vec4 kd(0, 0, 0, 0);
	const vec4 kw(161, 198, 199, 199);
	//jelly
	const vec4 jd(142, 218, 211, 10);
	const vec4 jw(154, 184, 181, 101);
	int consecutive = 0;
	int k = 0;
	int numUnknown = 0;
	bool look_for_names = false;
	for (; k < size - 4; k += 4)
	{
		bool foundActiveBoard = false;
		int active_counter = 0;
		int scoreboard_counter = 0;
		for (int j = 0; j < 12; j += 4)
		{
			if (bmpBuffer[k + j] == 0x99 && bmpBuffer[k + j + 1] == 0xd8 && bmpBuffer[k + j + 2] == 0xe4)
			{
				active_counter++;
				if (active_counter == 3)
				{
					active_counter = 0;
					ret.push_back(BoardCollection(Board(), (k / 4) - ((k / 4) / SCREENWIDTH)*SCREENWIDTH + 2, (k / 4) / SCREENWIDTH, ACTIVE));
					//cout<<"found active board"<<endl;
					break;
				}
			}
			else if (bmpBuffer[k + j] == 0x73 && bmpBuffer[k + j + 1] == 0xa2 && bmpBuffer[k + j + 2] == 0xab)
			{
				scoreboard_counter++;
				if (scoreboard_counter == 3)
				{
					ret.push_back(BoardCollection(Board(), (k / 4) - ((k / 4) / SCREENWIDTH)*SCREENWIDTH, (k / 4) / SCREENWIDTH, SCOREBOARD));
					look_for_names = true;
					//cout<<"found scoreboard"<<endl;
					break;
				}
			}
			else
			{
				break;
			}
		}
		//if(look_for_names)
		//{ //we have found at least one scoreboard, so keep our eyes out for a tan bandana
		//	bool found_it=true; //assume the best
		//	int p=1;
		//	for(; p<=7; p++)
		//	{
		//		switch(p)
		//		{
		//		case 1:
		//				{
		//					if(bmpBuffer[4*p+k] == 0x3c && bmpBuffer[4*p+k+1]==0x76 && bmpBuffer[4*p+k+2]==0xad)
		//					{
		//						break;
		//					}
		//					else
		//					{
		//						found_it=false;
		//						break;
		//					}
		//				}
		//		case 2:
		//				{
		//					if(bmpBuffer[4*p+k] == 0x42 && bmpBuffer[4*p+k+1]==0x76 && bmpBuffer[4*p+k+2]==0xa2)
		//					{
		//						break;
		//					}
		//					else
		//					{
		//						found_it=false;
		//						break;
		//					}
		//				}
		//		case 3:
		//				{
		//					if(bmpBuffer[4*p+k] == 0x41 && bmpBuffer[4*p+k+1]==0x6f && bmpBuffer[4*p+k+2]==0x94)
		//					{
		//						break;
		//					}
		//					else
		//					{
		//						found_it=false;
		//						break;
		//					}
		//				}
		//		case 4:
		//				{
		//					if(bmpBuffer[4*p+k] == 0x42 && bmpBuffer[4*p+k+1]==0x6d && bmpBuffer[4*p+k+2]==0x8d)
		//					{
		//						break;
		//					}
		//					else
		//					{
		//						found_it=false;
		//						break;
		//					}
		//				}
		//		case 5:
		//				{
		//					if(bmpBuffer[4*p+k] == 0x2b && bmpBuffer[4*p+k+1]==0x4e && bmpBuffer[4*p+k+2]==0x6d)
		//					{
		//						break;
		//					}
		//					else
		//					{
		//						found_it=false;
		//						break;
		//					}
		//				}
		//		}
		//	}
		//	if(found_it)
		//	{
		//		//determine whether we have teh frankerz or haswell
		//		if(bmpBuffer[4*(p+SCREENWIDTH-2)+k] == 0xa5 && bmpBuffer[4*(p+SCREENWIDTH-2)+k+1]==0xcd && bmpBuffer[4*(p+SCREENWIDTH-2)+k+2]==0xfa
		//			&& bmpBuffer[4*(p-1+SCREENWIDTH)+k] == 0xa8 && bmpBuffer[4*(p-1+SCREENWIDTH)+k+1]==0xd3 && bmpBuffer[4*(p-1+SCREENWIDTH)+k+2]==0xfa)
		//		{
		//			//assign Frankerz position to the closest board anchor
		//			int frankx=(k/4)-((k/4)/SCREENWIDTH)*SCREENWIDTH;
		//			int franky=(k/4)/SCREENWIDTH;
		//			int closeBoardIndex=0;
		//			double closeBoardDist=1000000;
		//			for(int k=0; k< ret.size(); k++)
		//			{
		//				double dist= abs(ret[k].anchorX-frankx);
		//				if(dist<closeBoardDist)
		//				{
		//					closeBoardDist=dist;
		//					closeBoardIndex=k;
		//				}
		//			}
		//			if(ret.size()>0)
		//			{
		//				ret[closeBoardIndex].pirate_locs.frankerX=(k/4)-((k/4)/SCREENWIDTH)*SCREENWIDTH+2;
		//				ret[closeBoardIndex].pirate_locs.frankerY=(k/4)/SCREENWIDTH;
		//			}
		//			cout<<"found Frankerz on the scoreboard at index: "<<closeBoardIndex<<" x: "<< ret[closeBoardIndex].pirate_locs.frankerX<<
		//				" y: "<<ret[closeBoardIndex].pirate_locs.frankerY<<endl;
		//		}
		//		else if(bmpBuffer[4*(p-2+SCREENWIDTH)+k] == 0x2a && bmpBuffer[4*(p-2+SCREENWIDTH)+k+1]==0x66 && bmpBuffer[4*(p-2+SCREENWIDTH)+k+2]==0xbd
		//			&& bmpBuffer[4*(p-1+SCREENWIDTH)+k] == 0x2c && bmpBuffer[4*(p-1+SCREENWIDTH)+k+1]==0x70 && bmpBuffer[4*(p-1+SCREENWIDTH)+k+2]==0xbd)
		//		{
		//			//assign Haswell position to the closest board anchor
		//			int hasx=(k/4)-((k/4)/SCREENWIDTH)*SCREENWIDTH;
		//			int hasy=(k/4)/SCREENWIDTH;
		//			int closeBoardIndex=0;
		//			double closeBoardDist=1000000;
		//			for(int k=0; k< ret.size(); k++)
		//			{
		//				double dist= abs(ret[k].anchorX-hasx);
		//				if(dist<closeBoardDist)
		//				{
		//					closeBoardDist=dist;
		//					closeBoardIndex=k;
		//				}
		//			}
		//			if(ret.size()>0)
		//			{
		//				ret[closeBoardIndex].pirate_locs.haswellX=(k/4)-((k/4)/SCREENWIDTH)*SCREENWIDTH+2;
		//				ret[closeBoardIndex].pirate_locs.haswellY=(k/4)/SCREENWIDTH;
		//			}
		//			cout<<"found Haswell on the scoreboard at index: "<<closeBoardIndex<<" x: "<< ret[closeBoardIndex].pirate_locs.haswellX<<
		//				" y: "<<ret[closeBoardIndex].pirate_locs.haswellY<<endl;
		//		}
		//	}
		//}
	}

	for (int m = 0; m < ret.size(); m++)
	{
		//inner pixel vals
		if (ret[m].status == SCOREBOARD)
		{
			continue;
		}
		int inX = 12;
		int inY = 25;
		int startX = ret[m].anchorX - 103 + inX; //anchor point +- offset + pixel picker
		int startY = ret[m].anchorY + 40 + inY;
		//anch x=111, anch y = 31
		for (int r = 0; r < 12; r++)
		{
			for (int c = 0; c < 6; c++)
			{
				vec4 curPoint;
				curPoint.R = bmpBuffer[((r * 45 + startY)*SCREENWIDTH + c * 45 + startX) * 4];
				curPoint.G = bmpBuffer[((r * 45 + startY)*SCREENWIDTH + c * 45 + startX + 1) * 4];
				curPoint.B = bmpBuffer[((r * 45 + startY)*SCREENWIDTH + c * 45 + startX + 2) * 4];
				curPoint.A = bmpBuffer[((r * 45 + startY)*SCREENWIDTH + c * 45 + startX + 3) * 4];

				char entry = '@';
				if (curPoint == ad || curPoint == aw)
				{
					entry = 'a';
				}
				else if (curPoint == bd || curPoint == bw)
				{
					entry = 'b';
				}
				else if (curPoint == cd || curPoint == cw)
				{
					entry = 'c';
				}
				else if (curPoint == dd || curPoint == dw)
				{
					entry = 'd';
				}
				else if (curPoint == ed || curPoint == ew)
				{
					entry = 'e';
				}
				else if (curPoint == fd || curPoint == fw)
				{
					entry = 'f';
				}
				else if (curPoint == pd || curPoint == pw)
				{
					entry = 'p';
				}
				else if (curPoint == kd || curPoint == kw)
				{
					entry = 'k';
				}
				else if (curPoint == jd || curPoint == jw)
				{
					entry = 'j';
				}
				else if (curPoint == gd || curPoint == gw)
				{
					entry = 'g';
				}
				else
				{
					entry = '@';
					numUnknown++;
				}
				ret[m].mBoard.mboard[r][c] = entry;
			}
		}
	}
	//cout<<"Unknown Pieces:"<<numUnknown;
	return ret;
}
vector<BoardCollection>  createBoard()
{
	//get client DC
	HDC desktophandle = GetDC(NULL);
	//create memory DC
	HDC memHDC = CreateCompatibleDC(desktophandle);
	//create new HBITMAP compatible with client dc
	HBITMAP Teemo = CreateCompatibleBitmap(desktophandle, SCREENWIDTH, SCREENHEIGHT);
	//select bitmap into memory DC
	SelectObject(memHDC, Teemo);
	//copy from client DC to memory DC
	BitBlt(memHDC, 0, 0, SCREENWIDTH, SCREENHEIGHT, desktophandle, 0, 0, SRCCOPY);
	// GetBitmapBits
	long numBits = SCREENWIDTH*SCREENHEIGHT * 4;
	LPVOID lpvBITBUF = new LPVOID;
	BYTE* bmpBuffer = (BYTE*)GlobalAlloc(GPTR, SCREENWIDTH*SCREENHEIGHT * 4);
	GetBitmapBits(Teemo, numBits, bmpBuffer);
	//SaveToFile(Teemo, L"Teemo.bmp");
	vector<BoardCollection> ret = lookforbilge(bmpBuffer, SCREENWIDTH*SCREENHEIGHT * 4);
	ReleaseDC(NULL, desktophandle);
	DeleteDC(memHDC);
	DeleteObject(Teemo);
	delete lpvBITBUF;
	GlobalFree(bmpBuffer);
	return ret;
}
void BOT()
{
	// Record/Load an MM
	cout << "Press Enter to record" << endl;
	string k;
	getline(cin, k);
	MouseMovement MM;
	cout << "Recording Mouse Movement." << endl;
	MM.Record(1, 1000);
	cout << "Done Recording. Looking for Board." << endl;

	ofstream scoreTracker("Score Track.csv");
	scoreTracker << "Move Counter,Score,Average Score" << endl;
	double totalScore = 0;
	double moveCounter = 0;
	int myScore = 0;
	int index = 0;

	while (true) //run the bot ad nausium
	{

		vector <BoardCollection> curBoards;
		vector <BoardCollection> prevBoard;
		vector <BoardCollection> newestBoard;
		bool set_otherBoards = false;
		while (true) //keep trying to create a usable board until one is found
		{
			curBoards = createBoard();
			//now curboards should be constructed (if they exist)
			bool activeBoardExists = false;
			if (curBoards.size() <= 0)
			{
				index = 0;
				Sleep(100);
				continue;
			}
			else
			{
				for (int k = 0; k < curBoards.size(); k++)
				{
					if (curBoards[(k + index + 1) % curBoards.size()].status == ACTIVE)
					{
						index = (k + index + 1) % curBoards.size();
						activeBoardExists = true;
						break;
					}
				}
				if (!activeBoardExists)
				{
					continue;
				}
				// reenable that later TODO
				Sleep(200);
				prevBoard = createBoard();
				Sleep(200);
				newestBoard = createBoard();
			}
			bool eqvBoards = true;
			if (curBoards.size() > 0 && curBoards.size() == prevBoard.size() && curBoards.size() == newestBoard.size())
			{
				for (int k = 0; k < curBoards.size(); k++)
				{
					PBOARD A, B, C;
					A.myBoard = &curBoards[k].mBoard;
					B.myBoard = &prevBoard[k].mBoard;
					C.myBoard = &newestBoard[k].mBoard;
					bool eqvBoards = (A == B  && A == C); //reads two boards to make sure they are the same
					bool breakflag = true;
					if (!eqvBoards)
					{
						curBoards[k].status = NOTVALID;
						//cout<<"Boards changed"<<endl;
					}
					else
					{
						for (int r = 0; r < 12; r++)
						{
							for (int c = 0; c < 6; c++)
							{
								if (curBoards[k].mBoard.mboard[r][c] == '@') // if we get the error char, try again
								{
									//cout<<"invalid char at r:"<<r<<" c:"<<c<<endl;
									curBoards[k].status = NOTVALID;
									breakflag = false;
								}
							}
						}
					}
				}
			}
			//now check to make sure there is still at least one active board
			bool haveActiveBoard = false;

			for (int k = 0; k < curBoards.size(); k++)
			{
				if (curBoards[(k + index + 1) % curBoards.size()].status == ACTIVE)
				{
					index = (k + index + 1) % curBoards.size();
					activeBoardExists = true;
					break;
				}
			}
			bool scoreOrInvalid = false; //TODO: remove this for loop when your score experiment is over
			/*	for(int k=0; k<curBoards.size(); k++)
				{
				if( curBoards[(k+index+1)%curBoards.size()].status!=ACTIVE)
				{
				index=(k+index+1)%curBoards.size();
				scoreOrInvalid=true;
				break;
				}
				}*/
			if (!activeBoardExists /*|| scoreOrInvalid*/)
			{
				//we need to output and clean the histograms if all boards are scoreboards
				//bool allScores=true;
				//for(int m=0; m<curBoards.size(); m++)
				//{
				//	if(curBoards[m].status!=SCOREBOARD)
				//	{
				//		allScores=false;
				//	}
				//}
				//if(allScores)
				//{
				//	//now check if all hists are cleanable
				//	bool allCleanable=true;
				//	for(int n=0; n<Runners.size(); n++)
				//	{
				//		if(!Runners[n].cleanable)
				//		{
				//			allCleanable=false;
				//		}
				//	}
				//	if(allCleanable)
				//	{
				//		output_hists(Runners, curBoards[index]);
				//		for(int Q=0; Q<Runners.size(); Q++)
				//		{
				//			Runners[Q].cleanHist();
				//		}
				//	}
				//}
				continue;
			}
			else
			{
				break; // run the bot
			}
		}
		//index=0; //TODO: remove this later
		int prevIndex = index;
		bool firstone = true;
		do
		{ //loop through active boards taking moves if there are some

			if (curBoards[index].status != ACTIVE) // NOTVALID and SCOREBOARD should fall through here
			{
				index = (index + 1) % curBoards.size();
				if (index == prevIndex)
				{
					break;
				}
				continue;
			}
			else
			{
				
				int row = -1;
				int col = -1;
				//calculate


				clock_t tStart = clock();

				//curBoards[index].mBoard.findBestMove(myScore, 1, row, col);
				curBoards[index].mBoard.findBestMoveOMP(myScore, 1, row, col);
				double timeTaken = (double)(clock() - tStart) / CLOCKS_PER_SEC;

				vector <BoardCollection> testBoards = createBoard();
				if (curBoards.size() == testBoards.size() && firstone)
				{
					firstone = false;
					PBOARD A, B;
					A.myBoard = &curBoards[index].mBoard;
					B.myBoard = &testBoards[index].mBoard;
					bool eqvBoards = (A == B); //reads two boards to make sure they are the same
					if (!eqvBoards)
					{
						cout << "Skipping this move because the boards don't match" << endl;
						index = (index + 1) % curBoards.size();
						if (index == prevIndex)
						{
							break;
						}
						continue;
					}
				}
				if (myScore > 10000)
				{
					cout << "you expect me to believe this?" << endl;
				}
				// Now playback the movement;
				POINT * curserposinit = new POINT();
				GetCursorPos(curserposinit);
				Point start(curserposinit->x, curserposinit->y);
				delete curserposinit;
				//                     anchor +- mod + centerer + piecemod
				Point destCenter = Point(curBoards[index].anchorX - 104 + 40 + 45 * col, curBoards[index].anchorY + 40 + 22 + 45 * row);
				Point Dest = generateDest(destCenter, 20);
				moveCounter++;
				totalScore += myScore;
				scoreTracker << moveCounter << "," << myScore << "," << (totalScore / (double)moveCounter) << endl;
				cout << "Index:" << index << " R: " << row << " C: " << col << " score:" << myScore << " Average:" << (totalScore / (double)moveCounter) << " Time:" << timeTaken << " Seconds" << endl;
				MM.PlayBetweenPoints(start, Dest, BellRand(0.8, 0.8));
				Sleep(BellRand(200, 150));
				MM.Click();
				

				//increment the index
				index = (index + 1) % curBoards.size();

			}
		} while (prevIndex != index);
		Sleep(100);
	}
}
bool fileExists(const std::string& name) {
	ifstream f(name.c_str());
	if (f.good()) {
		f.close();
		return true;
	}
	else {
		f.close();
		return false;
	}
}
void setGlobalsFromSettings()//double scoreMat[][4][2][2])
{
	for (int one = 0; one < 4; one++)
	{
		for (int two = 0; two < 4; two++)
		{
			for (int three = 0; three < 2; three++)
			{
				for (int four = 0; four < 2; four++)
				{
					scoreMat[one][two][three][four] = 0;
				}
			}
		}
	}
	vector<int> Scores;
	if (fileExists("_settings"))
	{
		ifstream settingsFile("_settings");
		string curLine;
		for (auto &i : Scores){ i = 0; }
		while (getline(settingsFile, curLine))
		{
			if (curLine.size() > 0)
			{
				if (curLine[0] != '!')
				{
					Scores.push_back(stoi(curLine));
				}
			}
		}
	}
	int scoreInd = 0;
	int finalval = 1;
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 3;
	}
	MAXDEPTH = finalval;
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 1;
	}
	SINGLESSCORE = finalval;
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 1;
	}
	UNCERTAINTY_SCORE = finalval;
	// Defaults to TZZ style scoring
	scoreMat[0][0][0][0];
	// single 3 -> 3pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 30;
	}
	scoreMat[0][0][0][1] = scoreMat[0][0][1][0] = scoreMat[0][1][0][0] = scoreMat[1][0][0][0] = finalval;
	// single 4 -> 4pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 40;
	}
	scoreMat[2][0][0][0] = scoreMat[0][2][0][0] = finalval;
	// single 5 -> 5pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 50;
	}
	scoreMat[3][0][0][0] = scoreMat[0][3][0][0] = finalval;
	//      3x3 -> 12pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 120;
	}
	scoreMat[1][1][0][0] = scoreMat[1][0][1][0] = scoreMat[1][0][0][1] = scoreMat[0][1][1][0] = scoreMat[0][1][0][1] = scoreMat[0][0][1][1] = finalval;
	//      3x4 -> 14pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 140;
	}
	scoreMat[2][1][0][0] = scoreMat[2][0][1][0] = scoreMat[2][0][0][1] = scoreMat[1][2][0][0] = scoreMat[0][2][1][0] = scoreMat[0][2][0][1] = finalval;
	//      3x5 -> 16pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 160;
	}
	scoreMat[3][1][0][0] = scoreMat[3][0][1][0] = scoreMat[3][0][0][1] = scoreMat[1][3][0][0] = scoreMat[0][3][1][0] = scoreMat[0][3][0][1] = finalval;
	//		4x4 -> 16
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 160;
	}
	scoreMat[2][2][0][0] = finalval;
	//		4x5 -> 18
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 180;
	}
	scoreMat[2][3][0][0] = scoreMat[3][2][0][0] = finalval;
	//		5x5 -> 20
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 200;
	}
	scoreMat[3][3][0][0] = finalval;
	//333 Bingo -> 24pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 240;
	}
	scoreMat[1][1][1][0] = scoreMat[1][1][0][1] = scoreMat[1][0][1][1] = scoreMat[0][1][1][1] = finalval;
	//334 Bingo -> 27pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 270;
	}
	scoreMat[2][1][1][0] = scoreMat[2][1][0][1] = scoreMat[2][0][1][1] = scoreMat[1][2][1][0] = scoreMat[1][2][0][1] = scoreMat[0][2][1][1] = finalval;
	//344 Bingo -> 30pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 300;
	}
	scoreMat[2][2][1][0] = scoreMat[2][2][0][1] = finalval;
	//335 Bingo -> 30pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 300;
	}
	scoreMat[3][1][1][0] = scoreMat[3][1][0][1] = scoreMat[3][0][1][1] = scoreMat[1][3][1][0] = scoreMat[1][3][0][1] = scoreMat[0][3][1][1] = finalval;
	//345 Bingo -> 33pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 330;
	}
	scoreMat[3][2][1][0] = scoreMat[3][2][0][1] = scoreMat[2][3][1][0] = scoreMat[2][3][0][1] = finalval;
	//355 Bingo -> 36pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 360;
	}
	scoreMat[3][3][1][0] = scoreMat[3][3][0][1] = finalval;
	//3333donkey-> 80pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 800;
	}
	scoreMat[1][1][1][1] = finalval;
	//3334donkey-> 88pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 880;
	}
	scoreMat[2][1][1][1] = scoreMat[1][2][1][1] = finalval;
	//3344donkey-> 96pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 960;
	}
	scoreMat[2][2][1][1] = finalval;
	//3335vegas-> 144pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 1440;
	}
	scoreMat[3][1][1][1] = scoreMat[1][3][1][1] = finalval;
	//3345vegas-> 156pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 1560;
	}
	scoreMat[3][2][1][1] = scoreMat[2][3][1][1] = finalval;
	//3355vegas-> 168pts
	if (scoreInd < Scores.size() && Scores[scoreInd])
	{
		finalval = Scores[scoreInd];
		scoreInd++;
	}
	else
	{
		finalval = 1680;
	}
	scoreMat[3][3][1][1] = finalval;
}
int main()
{
	//concurrency::parallel_for(0, 60, [&](size_t i){
	//	cout << i << endl;
	//	
	//});
	//for (int i = 0; i < NUMROWS*NUMCOLS; i++)
	//{

	//	int r = i / NUMCOLS;
	//	int c = i - (r * NUMCOLS);
	//	cout << r << "," << c << endl;
	//}
	//string k;
	//getline(cin, k);
	//// Now playback the movement;
	//POINT * curserposinit = new POINT();
	//GetCursorPos(curserposinit);
	//Point start(curserposinit->x, curserposinit->y);
	//delete curserposinit;
	//Point Dest = generateDest(Point(500,500), 20);
	//MouseMovement MM;
	//MM.Record(1.0, 100);
	//MM.Play();
	//MM.PlayBetweenPoints(Point(0,0), Point(500,500));
	//return 0;
	//testScoreMat(9,9);
	//return 0;
	//colorPickerLoop();
	//setup
	srand(time(NULL));
	setGlobalsFromSettings();
	//initScore();
	GetDesktopResolution(SCREENWIDTH, SCREENHEIGHT);
	//the bot part


	BOT();
	//end bot part

	Board turtle(true);
	turtle.displayBoard();
	cout << endl;
	for (int r = 0; r < 12; r++)
	{
		for (int c = 0; c < 5; c++)
		{
			cout << turtle.makeMove(r, c) << ",";
		}
		cout << endl;
	}
	int rowrow = 0;
	int colcol = 0;
	int score = 0;
	clock_t tStart = clock();
	//turtle.findBestMove(score,1,rowrow,colcol);
	turtle.findBestMoveOMP(score, 1, rowrow, colcol);
	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC);
	cout << rowrow << endl;
	cout << colcol << endl;


	cout << "hiu mworld" << endl;
}