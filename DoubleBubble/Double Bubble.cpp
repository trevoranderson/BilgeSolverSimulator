#include <iostream>
#include <time.h>
#include "Board.h"
#include "MouseMovement.h"
#include <Windows.h>
#include "wtypes.h"
#include <vector>
#include <fstream>
#include <cassert>

using namespace std;

int SCREENWIDTH=-1;
int SCREENHEIGHT=-1;
// Pure wrapper class so I can write a good operator==
class PBOARD
{
public:
	Board * myBoard;
};
class BilgeHist
{
	//Histogram for bilge scoring purposes.
public:
	BilgeHist()
	{
		for(int a=0; a<4; a++)
		{
			for(int b=0; b<4; b++)
			{
				for(int c=0; c<2; c++)
				{
					for(int d=0; d<2; d++)
					{
						frequencies[a][b][c][d]=0;
					}
				}
			}
		}
		numChains=0;
		crabScore=0;
		cleanable=false;
		won=false;
	}
	void cleanHist()
	{
		for(int a=0; a<4; a++)
		{
			for(int b=0; b<4; b++)
			{
				for(int c=0; c<2; c++)
				{
					for(int d=0; d<2; d++)
					{
						frequencies[a][b][c][d]=0;
					}
				}
			}
		}
		numChains=0;
		crabScore=0;
		cleanable=false;
		won=false;
	}

	double frequencies[4][4][2][2];
	double numChains;
	double crabScore;
	bool won;
	bool cleanable;
};
void re_cleanBoard(Board A, BilgeHist &B,int row=0, int col=0, int highLeft=0, int highRight=0, int left=0, int right=0)
{
	
	//TODO: this is inefficient: make this faster
	// restrict domain of searching
	int totNormCleaned=0;
	int totCrabsCleaned=0;
	int numCleaned=1;
	while(numCleaned>0)
	{
		numCleaned=0;
		int normCleaned=0;
		int crabsCleared=0;
		bool cleanedSomething=false;
		Board subBoard=A;
		//loop through rows
		for(int r=0; r< NUMROWS; r++)
		{
			for(int c=0; c<NUMCOLS -3; c++)
			{
				if( A.mboard[r][c]==A.mboard[r][c+1] && A.mboard[r][c]==A.mboard[r][c+2] 
					&& A.mboard[r][c]<'j')//!='x' && A.mboard[r][c]!='k' && A.mboard[r][c]!='p' && A.mboard[r][c]!='j')
			//	if( !((A.mboard[r][c] ^ A.mboard[r][c+1]) + (A.mboard[r][c] ^ A.mboard[r][c+2])) 
			//		&& A.mboard[r][c]<'j')
				{
					char temp=A.mboard[r][c];
					cleanedSomething=true;
					//We know we have at least 3 in a row: look for more
					for(; A.mboard[r][c]==temp && c<NUMCOLS; c++)
					{
						subBoard.mboard[r][c]='o';
						normCleaned++;
					}
				}
				else if(A.mboard[r][c]=='k' && r<WATERLEVEL)
				{
					subBoard.mboard[r][c]='o';
					crabsCleared++;
					cleanedSomething=true;
				}
			}
		}
		//loop the other way
		for(int c=0; c<NUMCOLS; c++)
		{
			for(int r=0; r< NUMROWS -3; r++)
			{
				if( A.mboard[r][c]==A.mboard[r+1][c] && A.mboard[r][c]==A.mboard[r+2][c] 
					&& A.mboard[r][c]<'j')//!='x' && A.mboard[r][c]!='k' && A.mboard[r][c]!='p' && A.mboard[r][c]!='j' && A.mboard[r][c]!='o')
				{
					char temp=A.mboard[r][c];
					cleanedSomething=true;
					//We know we have at least 3 in a col: look for more
					for(; A.mboard[r][c]==temp && r<NUMROWS; r++)
					{
						subBoard.mboard[r][c]='o';
						normCleaned++;
					}
				}
			}
		}
		if(cleanedSomething)
		{
			// asess cleaning with subboard, then copy over and start again
			for(int r=0; r<NUMROWS; r++)
			{
				for(int c=0 ; c<NUMCOLS; c++)
				{
					if(subBoard.mboard[r][c]=='o')
					{
						numCleaned++;
						int rr=r;
						for(; rr<NUMROWS-1 && subBoard.mboard[rr][c]!='x'; rr++)
						{
							subBoard.mboard[rr][c]=subBoard.mboard[rr+1][c];
						}
						subBoard.mboard[rr][c]='x';
						c--;
					}
				}
			}
			//now copy subBoard to board
			memcpy(A.mboard,subBoard.mboard,6*12);
			totNormCleaned+= normCleaned;
			totCrabsCleaned+= crabsCleared * crabsCleared;
		}
	}
	B.crabScore+=totCrabsCleaned;
	B.numChains+=totNormCleaned;
}
void re_editBoard(Board A, BilgeHist &B, int row, int col, int leftVert, int rightVert, int vert_left_up, int vert_right_up, int left, int right)
{
	//Here, we want to take the given move and remove pieces and shift to get the next valid board (with some uncertainty in pieces).
	if(leftVert && rightVert)
	{
		//both sides have verticle evaluation

		//calc and shift the left side
		int topRowLeft= row - vert_left_up;
		for(int k=topRowLeft; k< NUMROWS-(leftVert+2); k++)
		{
			A.mboard[k][col]=A.mboard[k+(leftVert+2)][col];
		}
		for(int k=1; k<=(leftVert+2); k++)
		{
			A.mboard[NUMROWS-k][col]='x';
		}

		//calc and shift right side
		int topRowRight= row - vert_right_up;
		int nCol=col+1;
		for(int k=topRowRight; k< NUMROWS-(rightVert+2); k++)
		{
			A.mboard[k][nCol]=A.mboard[k+(rightVert+2)][nCol];
		}
		for(int k=1; k<=(rightVert+2); k++)
		{
			A.mboard[NUMROWS-k][nCol]='x';
		}

		//calc and shift left and right row matches
		if(left)
		{
			//double single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col-1]=A.mboard[k+1][col-1];
				A.mboard[k][col-2]=A.mboard[k+1][col-2];
			}
			A.mboard[NUMROWS-1][col-1]='x';
			A.mboard[NUMROWS-1][col-2]='x';
		}
		if(right)
		{
			//double single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col+2]=A.mboard[k+1][col+2];
				A.mboard[k][col+3]=A.mboard[k+1][col+3];
			}
			A.mboard[NUMROWS-1][col+2]='x';
			A.mboard[NUMROWS-1][col+3]='x';
		}
	}
	else if(leftVert && !rightVert)
	{
		//calc and shift the left side
		int topRowLeft= row - vert_left_up;
		for(int k=topRowLeft; k< NUMROWS-(leftVert+2); k++)
		{
			A.mboard[k][col]=A.mboard[k+(leftVert+2)][col];
		}
		for(int k=1; k<=(leftVert+2); k++)
		{
			A.mboard[NUMROWS-k][col]='x';
		}
		if(left)
		{
			//double single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col-1]=A.mboard[k+1][col-1];
				A.mboard[k][col-2]=A.mboard[k+1][col-2];
			}
			A.mboard[NUMROWS-1][col-1]='x';
			A.mboard[NUMROWS-1][col-2]='x';
		}
		if(right)
		{
			//triple single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col+1]=A.mboard[k+1][col+1];
				A.mboard[k][col+2]=A.mboard[k+1][col+2];
				A.mboard[k][col+3]=A.mboard[k+1][col+3];
			}
			A.mboard[NUMROWS-1][col+1]='x';
			A.mboard[NUMROWS-1][col+2]='x';
			A.mboard[NUMROWS-1][col+3]='x';
		}
	}
	else if(rightVert && !leftVert)
	{
		//calc and shift right side
		int topRowRight= row - vert_right_up;
		for(int k=topRowRight; k< NUMROWS-(rightVert+2); k++)
		{
			A.mboard[k][col+1]=A.mboard[k+(rightVert+2)][col+1];
		}
		//set X's for right col
		for(int k=1; k<=(rightVert+2); k++)
		{
			A.mboard[NUMROWS-k][col+1]='x';
		}

		if(left)
		{
			//triple single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col]=A.mboard[k+1][col];
				A.mboard[k][col-1]=A.mboard[k+1][col-1];
				A.mboard[k][col-2]=A.mboard[k+1][col-2];
			}
			A.mboard[NUMROWS-1][col]='x';
			A.mboard[NUMROWS-1][col-1]='x';
			A.mboard[NUMROWS-1][col-2]='x';
			
		}
		if(right)
		{
			//double single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col+2]=A.mboard[k+1][col+2];
				A.mboard[k][col+3]=A.mboard[k+1][col+3];
			}
			A.mboard[NUMROWS-1][col+2]='x';
			A.mboard[NUMROWS-1][col+3]='x';
		}
	}
	else
	{
		if(left)
		{
			//triple single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col]=A.mboard[k+1][col];
				A.mboard[k][col-1]=A.mboard[k+1][col-1];
				A.mboard[k][col-2]=A.mboard[k+1][col-2];
			}
			A.mboard[NUMROWS-1][col]='x';
			A.mboard[NUMROWS-1][col-1]='x';
			A.mboard[NUMROWS-1][col-2]='x';
		}
		if(right)
		{
			//double single column swaps
			for(int k=row; k< NUMROWS-1; k++)
			{
				A.mboard[k][col+1]=A.mboard[k+1][col+1];
				A.mboard[k][col+2]=A.mboard[k+1][col+2];
				A.mboard[k][col+3]=A.mboard[k+1][col+3];
			}
			A.mboard[NUMROWS-1][col+1]='x';
			A.mboard[NUMROWS-1][col+2]='x';
			A.mboard[NUMROWS-1][col+3]='x';
		}
	}
	if(left || right || leftVert || rightVert)
	{
		re_cleanBoard(A,B,row, col, vert_left_up, vert_right_up, left, right);
	}
	return;
}
void re_evalMove(Board A, int row, int col, BilgeHist &B)
{ //row and column mark the piece on the left side of the move
	//this function evualates the move its given
	//col should be >=0 and less than 6;
	//do da swap
	B.cleanable=true;
	char temp= A.mboard[row][col];
	A.mboard[row][col]=A.mboard[row][col+1];
	A.mboard[row][col+1]=temp;

	int vert_left_down=0;
	int k=1;
	for(;(row+k)<NUMROWS && A.mboard[row+k][col]== A.mboard[row][col]; k++)
	{}
	vert_left_down+= k-1;

	int vert_left_up=0;
	for(k=1; (row-k)>=0 && A.mboard[row-k][col]== A.mboard[row][col]; k++)
	{}
	vert_left_up+= k-1;


	int vert_right_down=0;
	int rCol=col+1;
	for(k=1; (row+k)<NUMROWS && A.mboard[row+k][rCol]== A.mboard[row][rCol] ; k++)
	{}
	vert_right_down+= k-1;

	int vert_right_up=0;
	for(k=1; (row-k)>=0 && A.mboard[row-k][rCol]== A.mboard[row][rCol]; k++)
	{}
	vert_right_up+= k-1;

	int left=0;
	int right=0;
	if(col>=2)
	{//fill left
		if(A.mboard[row][col-1]==A.mboard[row][col] && A.mboard[row][col-2]==A.mboard[row][col])
		{
			left=1;
		}
	}
	if(col<3)
	{ //fill right
		if(A.mboard[row][col+2]==A.mboard[row][col+1] && A.mboard[row][col+3]==A.mboard[row][col+1])
		{
			right= 1;
		}
	}

	int leftVert=vert_left_up+vert_left_down+1-2;
	if(leftVert<0)
	{
		leftVert=0;
	}
	int rightVert=vert_right_down+vert_right_up+1-2;
	if(rightVert<0)
	{
		rightVert=0;
	}
	B.frequencies[leftVert][rightVert][left][right]++;
	re_editBoard(A,B,row, col, leftVert, rightVert, vert_left_up, vert_right_up, left, right);
	return;
}
enum BOARDSTATUS {ACTIVE, SCOREBOARD,NOTVALID};
struct Scoreboard
{
	Scoreboard()
	{
		frankerX=-1;
		frankerY=-1;
		haswellX=-1;
		haswellY=-1;
		curScoreParsed=false;
	}
	int frankerX;
	int frankerY;
	int haswellX;
	int haswellY;
	bool curScoreParsed;
};
class BoardCollection
{
public:
	BoardCollection(Board A, int x, int y, BOARDSTATUS mystatus)
	{
		mBoard=A;
		anchorX=x;
		anchorY=y;
		status=mystatus;
	}
	Board mBoard;
	int anchorX;
	int anchorY;
	BOARDSTATUS status;
	int myScore;
	Scoreboard pirate_locs;
};
void output_hists(vector<BilgeHist> output, BoardCollection A)
{
	ofstream histout;//("");
	histout.open ("BilgeHists.txt", fstream::out | fstream::app);
	histout<<"Number of Hists"<<endl;
	histout<<output.size()<<endl;
	for(int k=0; k<output.size(); k++)
	{
		histout<<"Hist ID"<<endl;
		histout<<k<<endl;
		histout<<"Did we win this league"<<endl;
		histout<< (A.pirate_locs.frankerY < A.pirate_locs.haswellY) << endl;
		histout<<"Chain Scores"<<endl;
		histout<<output[k].numChains<<endl;
		histout<<"Crab Scores"<<endl;
		histout<<output[k].crabScore<<endl;
		for(int a=0; a<4; a++)
		{
			for(int b=0; b<4; b++)
			{
				for(int c=0; c<2; c++)
				{
					for(int d=0; d<2; d++)
					{
						histout<<a<<b<<c<<d<<endl;
						histout<<output[k].frequencies[a][b][c][d]<<endl;
					}
				}
			}
		}
	}
}
void colorPickerLoop()
{
	POINT * curserpos= new POINT();
	while(true)
	{
		HDC myHDC= GetDC(NULL);
		GetCursorPos(curserpos);
		COLORREF TURTLE = GetPixel( myHDC, curserpos->x,curserpos->y);
		cout<<hex<< TURTLE<<endl;
	}
}
void getpixelLots()
{
	cout<<"starting"<<endl;
	clock_t tStart = clock();
	for(int k=0; k<700; k++)
	{
		HDC myHDC= GetDC(NULL);
		COLORREF TURTLE = GetPixel( myHDC, 0,0);
	}
	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	cout<<"ending"<<endl;
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
		R=RR;
		G=GG;
		B=BB;
		A=AA;
	}
	vec4()
	{}
	int R;
	int G;
	int B;
	int A;
};
bool operator== ( vec4 a, vec4 b)
{
	return a.A==b.A && a.B==b.B && a.G==b.G && a.R==b.R;
}
bool operator== (PBOARD A, PBOARD B)
{
	for(int r=0; r<NUMROWS; r++)
	{
		for(int c=0; c<NUMCOLS; c++)
		{
			if( A.myBoard->mboard[r][c]!=B.myBoard->mboard[r][c])
			{
				return false;
			}
		}
	}
	return true;
}
bool operator== (Board A, Board B)
{
	for(int r=0; r<NUMROWS; r++)
	{
		for(int c=0; c<NUMCOLS; c++)
		{
			if(A.mboard[r][c]!=B.mboard[r][c])
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
	const vec4 ad(211,211,211,212);
	const vec4 aw(181,181,181,182);
	const vec4 bd(251,251,252,251);
	const vec4 bw(197,197,198,197);
	const vec4 cd(173,170,170,170);
	const vec4 cw(166,165,165,165);
	const vec4 dd(245,245,245,245);
	const vec4 dw(195,195,195,195);
	const vec4 ed(213,213,197,200);
    const vec4 ew(182,182,176,177);
	const vec4 fd(204,204,204,204);
    const vec4 fw(179,179,179,179);
	const vec4 gd(244,236,234,235);
    const vec4 gw(195,191,191,191);
	//specials
	//puffer
	const vec4 pd(150,245,172,212);
	const vec4 pw(157,195,166,182);
	//krab
	const vec4 kd(0,0,0,0);
	const vec4 kw(161,198,199,199);
	//jelly
	const vec4 jd(142,218,211,10);
	const vec4 jw(154,184,181,101);
	int consecutive=0;
	int k=0;
	int numUnknown=0;
	bool look_for_names=false;
	for(; k< size-4; k+=4)
	{
		bool foundActiveBoard=false;
		int active_counter=0;
		int scoreboard_counter=0;
		for(int j=0; j<12; j+=4)
		{
			if(bmpBuffer[k+j]==0x99 && bmpBuffer[k+j+1]==0xd8 && bmpBuffer[k+j+2]==0xe4)
			{
				active_counter++;
				if(active_counter==3)
				{
					active_counter=0;
					ret.push_back(BoardCollection(Board(),(k/4)-((k/4)/SCREENWIDTH)*SCREENWIDTH+2,(k/4)/SCREENWIDTH,ACTIVE));
						//cout<<"found active board"<<endl;
					break;
				}
			}
			else if(bmpBuffer[k+j]==0x73 && bmpBuffer[k+j+1]==0xa2 && bmpBuffer[k+j+2]==0xab)
			{
				scoreboard_counter++;
				if(scoreboard_counter==3)
				{
					ret.push_back(BoardCollection(Board(),(k/4)-((k/4)/SCREENWIDTH)*SCREENWIDTH,(k/4)/SCREENWIDTH,SCOREBOARD));
					look_for_names=true;
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
	
	for(int m=0; m<ret.size(); m++)
	{
		//inner pixel vals
		if(ret[m].status==SCOREBOARD)
		{
			continue;
		}
		int inX=12;
		int inY=25;
		int startX= ret[m].anchorX-103+inX; //anchor point +- offset + pixel picker
		int startY= ret[m].anchorY+40+inY;
		//anch x=111, anch y = 31
		for(int r=0; r<12; r++)
		{
			for(int c=0; c<6; c++)
			{
				vec4 curPoint;
				curPoint.R= bmpBuffer[((r*45+startY)*SCREENWIDTH + c*45 + startX)*4];
				curPoint.G= bmpBuffer[((r*45+startY)*SCREENWIDTH + c*45+ startX+1)*4];
				curPoint.B= bmpBuffer[((r*45+startY)*SCREENWIDTH + c*45+ startX+2)*4];
				curPoint.A= bmpBuffer[((r*45+startY)*SCREENWIDTH + c*45+ startX+3)*4];

				char entry='@';
				if(curPoint== ad || curPoint==aw)
				{
					entry='a';
				}
				else if(curPoint== bd || curPoint==bw)
				{
					entry='b';
				}
				else if(curPoint== cd || curPoint==cw)
				{
					entry='c';
				}
				else if(curPoint== dd || curPoint==dw)
				{
					entry='d';
				}
				else if(curPoint== ed || curPoint==ew)
				{
					entry='e';
				}
				else if(curPoint== fd || curPoint==fw)
				{
					entry='f';
				}
				else if(curPoint== pd || curPoint==pw)
				{
					entry='p';
				}
				else if(curPoint== kd || curPoint==kw)
				{
					entry='k';
				}
				else if(curPoint== jd || curPoint==jw)
				{
					entry='j';
				}
				else if(curPoint== gd || curPoint==gw)
				{
					entry='g';
				}
				else
				{
					entry='@';
					numUnknown++;
				}
				ret[m].mBoard.mboard[r][c]=entry;
			}
		}
	}
	//cout<<"Unknown Pieces:"<<numUnknown;
	return ret;
}
vector<BoardCollection>  createBoard()
{
	//get client DC
	HDC desktophandle= GetDC(NULL);
	//create memory DC
	HDC memHDC= CreateCompatibleDC(desktophandle);
	//create new HBITMAP compatible with client dc
	HBITMAP Teemo= CreateCompatibleBitmap(desktophandle,SCREENWIDTH,SCREENHEIGHT);
	//select bitmap into memory DC
	SelectObject(memHDC, Teemo);
	//copy from client DC to memory DC
	BitBlt(memHDC,0,0,SCREENWIDTH,SCREENHEIGHT,desktophandle,0,0,SRCCOPY);
	// GetBitmapBits
	long numBits=SCREENWIDTH*SCREENHEIGHT*4;
	LPVOID lpvBITBUF= new LPVOID;
	BYTE* bmpBuffer=(BYTE*)GlobalAlloc(GPTR, SCREENWIDTH*SCREENHEIGHT*4);
	GetBitmapBits(Teemo,numBits,bmpBuffer);
	//SaveToFile(Teemo, L"Teemo.bmp");
	vector<BoardCollection> ret =lookforbilge(bmpBuffer,SCREENWIDTH*SCREENHEIGHT*4);
	ReleaseDC(NULL,desktophandle);
	DeleteDC(memHDC);
	DeleteObject(Teemo);
	delete lpvBITBUF;
	GlobalFree(bmpBuffer);
	return ret;
}
void BOT()
{
	ofstream scoreTracker("Score Track.csv");
	scoreTracker<<"Move Counter,Score,Average Score"<<endl;
	double totalScore=0;
	double moveCounter=0;
	int myScore=0;
	int index=0;
	vector<BilgeHist> Runners;
	while(true) //run the bot ad nausium
	{

		vector <BoardCollection> curBoards;
		vector <BoardCollection> prevBoard;
		vector <BoardCollection> newestBoard;
		bool set_otherBoards=false;
		while(true) //keep trying to create a usable board until one is found
		{
			curBoards= createBoard();
			//now curboards should be constructed (if they exist)
			bool activeBoardExists=false;
			if(curBoards.size()<=0)
			{
				index=0;
				Sleep(100);
				continue;
			}
			else
			{
				for(int k=0; k<curBoards.size(); k++)
				{
					if( curBoards[(k+index+1)%curBoards.size()].status==ACTIVE)
					{
						index=(k+index+1)%curBoards.size();
						activeBoardExists=true;
						break;
					}
				}
				if(!activeBoardExists)
				{
					continue;
				}
				// reenable that later TODO
				Sleep(200);
				prevBoard= createBoard();
				Sleep(200);
				newestBoard= createBoard();
			}
			bool eqvBoards= true;
			if(curBoards.size()>0 && curBoards.size()== prevBoard.size() && curBoards.size()== newestBoard.size())
			{
				for(int k=0; k<curBoards.size(); k++)
				{
					PBOARD A,B,C;
					A.myBoard=&curBoards[k].mBoard;
					B.myBoard=&prevBoard[k].mBoard;
					C.myBoard=&newestBoard[k].mBoard;
					bool eqvBoards= (A==B  && A==C); //reads two boards to make sure they are the same
					bool breakflag=true;
					if(!eqvBoards)
					{
						curBoards[k].status= NOTVALID;
						//cout<<"Boards changed"<<endl;
					}
					else
					{
						for(int r=0; r<12; r++)
						{
							for(int c=0; c<6; c++)
							{
								if(curBoards[k].mBoard.mboard[r][c]=='@') // if we get the error char, try again
								{
									//cout<<"invalid char at r:"<<r<<" c:"<<c<<endl;
									curBoards[k].status=NOTVALID;
									breakflag=false;
								}
							}
						}
					}
				}
			}
			//now check to make sure there is still at least one active board
			bool haveActiveBoard=false;

			for(int k=0; k<curBoards.size(); k++)
			{
				if( curBoards[(k+index+1)%curBoards.size()].status==ACTIVE)
				{
					index=(k+index+1)%curBoards.size();
					activeBoardExists=true;
					break;
				}
			}
			bool scoreOrInvalid=false; //TODO: remove this for loop when your score experiment is over
		/*	for(int k=0; k<curBoards.size(); k++)
			{
				if( curBoards[(k+index+1)%curBoards.size()].status!=ACTIVE)
				{
					index=(k+index+1)%curBoards.size();
					scoreOrInvalid=true;
					break;
				}
			}*/
			if(!activeBoardExists /*|| scoreOrInvalid*/)
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
		int prevIndex=index;
		bool firstone=true;
		do
		{ //loop through active boards taking moves if there are some

			if(curBoards[index].status!=ACTIVE) // NOTVALID and SCOREBOARD should fall through here
			{
				index=(index+1)%curBoards.size();
				if(index==prevIndex)
				{
					break;
				}
				continue;
			}
			else
			{
				if(curBoards.size()> Runners.size()) //add bilge hists until we have enough
				{
					while(Runners.size()<curBoards.size())
					{
						Runners.push_back(BilgeHist());
					}
				}
				int row=-1;
				int col=-1;
				//calculate
		
		
				clock_t tStart = clock();
				
				curBoards[index].mBoard.findBestMove(myScore,1,row,col);
				//curBoards[index].mBoard.findBestMoveOMP(myScore,1,row,col);
				double timeTaken=(double)(clock() - tStart)/CLOCKS_PER_SEC;

				vector <BoardCollection> testBoards = createBoard();
				if(curBoards.size()== testBoards.size() && firstone)
				{
					firstone=false;
					PBOARD A,B;
					A.myBoard=&curBoards[index].mBoard;
					B.myBoard=&testBoards[index].mBoard;
					bool eqvBoards= (A==B); //reads two boards to make sure they are the same
					if(!eqvBoards)
					{
						cout<<"Skipping this move because the boards don't match"<<endl;
						index=(index+1)%curBoards.size();
						if(index==prevIndex)
						{
						break;
						}
						continue;
					}
				}
				if(myScore>10000)
				{
					cout<<"you expect me to believe this?"<<endl;
				}
				// Now playback the movement;
				POINT * curserposinit= new POINT();
				GetCursorPos(curserposinit);
				MMCoord start(curserposinit->x,curserposinit->y);
				delete curserposinit;
				//                     anchor +- mod + centerer + piecemod
				MMCoord destCenter=MMCoord(curBoards[index].anchorX -104 + 40 + 45*col, curBoards[index].anchorY + 40 + 22 + 45*row);
				MMCoord Dest=generateDest(destCenter, 20);
				moveCounter++;
				totalScore+=myScore;
				scoreTracker<<moveCounter<< "," << myScore<< "," << (totalScore / (double)moveCounter) <<endl;
				cout<<"Index:"<<index<<" R: "<<row<<" C: "<<col<<" score:"<<myScore<<" Average:"<< (totalScore / (double)moveCounter)<< " Time:"<<timeTaken<< " Seconds" <<endl;
				PlayHumanMouse(start, Dest,genNormalDist(0.8,0.8), DinoNugget);
				Sleep(genNormalDist(100,100));
				clickatcurmousepos();
				//Now add the board's score to the proper bilge Hist
				re_evalMove(curBoards[index].mBoard,row,col,Runners[index]);

				//increment the index
				index=(index+1)%curBoards.size();
				
			}
		}
		while(prevIndex!=index);
		Sleep(100);
	}
}
int countTheLines()
{
    std::ifstream myfile("race_results.txt");

    // new lines will be skipped unless we stop it from happening:    
    myfile.unsetf(std::ios_base::skipws);

    // count the newlines with an algorithm specialized for counting:
    unsigned line_count = std::count(
        std::istream_iterator<char>(myfile),
        std::istream_iterator<char>(), 
        '\n');

    std::cout << "Lines: " << line_count << "\n";
    return 0;
}
int coutntehliens()
{
  ifstream inFile("race_results.txt"); 
  return count(istreambuf_iterator<char>(inFile), 
             istreambuf_iterator<char>(), '\n');

}
void createScoreAlgo()
{
	ifstream infile("race_results.txt");
	ofstream outfile("turtle.csv");
	string s;
	int trialNum=0;
	outfile << "Trial Number, Client ID, Result, chain, crab, other"<<endl;
	for(int z=0; z<136; z++)
	{
		getline(infile, s); //number of hists for this lp
		
		getline(infile, s); //advance past this one: we know its 2
		for(int j=0; j<2; j++)
		{
			getline(infile, s); //text
			getline(infile, s);// client ID
			int clientID= stoi(s);
			getline(infile, s); //text
			getline(infile, s); //output
			int result= stoi(s);
			int histogram[66];
			for(int k=0; k<66; k++)
			{
				getline(infile, s);//text
				getline(infile, s);//val
				histogram[k]=stoi(s);
			}
			outfile<< trialNum<< ","<< clientID<<","<<result<<",";
			for(int i=0; i<66; i++)
			{
				outfile<< histogram[i]<<",";
			}
			outfile<<endl;
		}
		trialNum++;
	}
	return;
}
void setScoreMat(double mat_to_set[][4][2][2])
{
	for(int one=0; one<4; one++)
	{
		for(int two=0; two<4; two++)
		{
			for(int three=0; three<2; three++)
			{
				for(int four=0; four<2; four++)
				{
					mat_to_set[one][two][three][four]=0;
				}
			}
		}
	}
	//ima do diss TZZ style
	mat_to_set[0][0][0][0];
	// single 3 -> 3pts
	mat_to_set[0][0][0][1]=mat_to_set[0][0][1][0]=mat_to_set[0][1][0][0]=mat_to_set[1][0][0][0]=30;
	// single 4 -> 4pts
	mat_to_set[2][0][0][0]=mat_to_set[0][2][0][0]=40;
	// single 5 -> 5pts
	mat_to_set[3][0][0][0]=mat_to_set[0][3][0][0]=50;
	//      3x3 -> 12pts
	mat_to_set[1][1][0][0]=mat_to_set[1][0][1][0]=mat_to_set[1][0][0][1]=mat_to_set[0][1][1][0]=mat_to_set[0][1][0][1]=mat_to_set[0][0][1][1]=120;
	//      3x4 -> 14pts
	mat_to_set[2][1][0][0]=mat_to_set[2][0][1][0]=mat_to_set[2][0][0][1]=mat_to_set[1][2][0][0]=mat_to_set[0][2][1][0]=mat_to_set[0][2][0][1]=140;
	//      3x5 -> 16pts
	mat_to_set[3][1][0][0]=mat_to_set[3][0][1][0]=mat_to_set[3][0][0][1]=mat_to_set[1][3][0][0]=mat_to_set[0][3][1][0]=mat_to_set[0][3][0][1]=160;
	//		4x4 -> 16
	mat_to_set[2][2][0][0]=160;
	//		4x5 -> 18
	mat_to_set[2][3][0][0]=mat_to_set[3][2][0][0]=180;
	//		5x5 -> 20
	mat_to_set[3][3][0][0]=200;
	//333 Bingo -> 24pts
	mat_to_set[1][1][1][0]=mat_to_set[1][1][0][1]=mat_to_set[1][0][1][1]=mat_to_set[0][1][1][1]=240;
	//334 Bingo -> 27pts
	mat_to_set[2][1][1][0]=mat_to_set[2][1][0][1]=mat_to_set[2][0][1][1]=mat_to_set[1][2][1][0]=mat_to_set[1][2][0][1]=mat_to_set[0][2][1][1]=270;
	//344 Bingo -> 30pts
	mat_to_set[2][2][1][0]=mat_to_set[2][2][0][1]=300;
	//335 Bingo -> 30pts
	mat_to_set[3][1][1][0]=mat_to_set[3][1][0][1]=mat_to_set[3][0][1][1]=mat_to_set[1][3][1][0]=mat_to_set[1][3][0][1]=mat_to_set[0][3][1][1]=300;
	//345 Bingo -> 33pts
	mat_to_set[3][2][1][0]=mat_to_set[3][2][0][1]=mat_to_set[2][3][1][0]=mat_to_set[2][3][0][1]=330;
	//355 Bingo -> 36pts
	mat_to_set[3][3][1][0]=mat_to_set[3][3][0][1]=360;
	//3333donkey-> 80pts
	mat_to_set[1][1][1][1]=800;
	//3334donkey-> 88pts
	mat_to_set[2][1][1][1]=mat_to_set[1][2][1][1]=880;
	//3344donkey-> 96pts
	mat_to_set[2][2][1][1]=960;
	//3335vegas-> 144pts
	mat_to_set[3][1][1][1]=mat_to_set[1][3][1][1]=1440;
	//3345vegas-> 156pts
	mat_to_set[3][2][1][1]=mat_to_set[2][3][1][1]=1560;
	//3355vegas-> 168pts
	mat_to_set[3][3][1][1]=1680;
}
void testScoreMat(double chain, double crab)
{
	ifstream infile("race_results.txt");
	vector<BilgeHist> race_results;
	string s;
	int trialNum=0;
	for(int z=0; z<136; z++)
	{

		getline(infile, s); //number of hists for this lp
		
		getline(infile, s); //advance past this one: we know its 2
		for(int j=0; j<2; j++)
		{
			BilgeHist round;
			getline(infile, s); //text
			getline(infile, s);// client ID
			int clientID= stoi(s);
			getline(infile, s); //text
			getline(infile, s); //output
			int result= stoi(s);
			round.won=result;
			getline(infile, s); //text
			getline(infile, s); //chain
			int chain = stoi(s);
			round.numChains=chain;
			getline(infile, s); //text
			getline(infile, s); //crab
			int crab=stoi(s);
			round.crabScore=crab;
			
			for(int a=0; a<4; a++)
			{
				for(int b=0; b<4; b++)
				{
					for(int c=0; c<2; c++)
					{
						for(int d=0; d<2; d++)
						{
							getline(infile, s);//text
							getline(infile, s);//val
							round.frequencies[a][b][c][d]=stoi(s);
						}
					}
				}
			}
			//set it equal to the current one
			race_results.push_back(round);

		}
		trialNum++;
	}
	//now we have a vector that are pairwise in the same round, so we can apply the scoremat to each one and figureout how good it is
	double numCorrect=0;
	int numScored=0;
	double score_mat[4][4][2][2];
	setScoreMat(score_mat);
	for(int k=0; k+1<race_results.size(); k+=2)
	{
		assert(race_results[k].won == race_results[k+1].won);
		double scoreA=0;
		double scoreB=0;
		
		//chain
		scoreA+= race_results[k].numChains * chain;
		scoreB+= race_results[k+1].numChains * chain;

		//crab
		scoreA+= race_results[k].crabScore * crab;
		scoreB+= race_results[k+1].crabScore * crab;

		// create a scoremat to use

		// the big one
		for(int a=0; a<4; a++)
		{
			for(int b=0; b<4; b++)
			{
				for(int c=0; c<2; c++)
				{
					for(int d=0; d<2; d++)
					{
						scoreA+= race_results[k].frequencies[a][b][c][d] * score_mat[a][b][c][d];
						scoreB+= race_results[k+1].frequencies[a][b][c][d] * score_mat[a][b][c][d];
					}
				}
			}
		}
		//now compare results
		if(scoreA > scoreB)
		{
			numScored++;
			if(race_results[k].won)
			{
				numCorrect++;
			}
		}
		else if(scoreA <= scoreB)
		{
			numScored++;
			if(!race_results[k].won)
			{
				numCorrect++;
			}
		}
	}
	assert(numScored*2==race_results.size());
	cout<<"we got "<< numCorrect/((double)race_results.size()/2.0) << "% Correct"<<endl;
	return;
}
int main()
{
	//testScoreMat(9,9);
	//return 0;
	//colorPickerLoop();
	//setup
	srand (time(NULL));
	initScore();
	GetDesktopResolution(SCREENWIDTH, SCREENHEIGHT);
	//the bot part
	int c;
	cin>>c;
	recordMouseMovement(1.0);
	MMtoOrigin(DinoNugget);
	BOT();
	//end bot part

	Board turtle(true);
	turtle.displayBoard();
	cout<<endl;
	for(int r=0; r<12; r++)
	{
		for(int c=0; c<5; c++)
		{
			cout<< turtle.makeMove(r,c)<<",";
		}
		cout<<endl;
	}
	int rowrow=0;
	int colcol=0;
	int score=0;
	clock_t tStart = clock();
	//turtle.findBestMove(score,1,rowrow,colcol);
	turtle.findBestMoveOMP(score,1,rowrow,colcol);
	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
	cout<<rowrow<<endl;
	cout<<colcol<<endl;
	

	cout<<"hiu mworld"<<endl;
}