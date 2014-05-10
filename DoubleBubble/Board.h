#pragma once
#include <iostream>
#include <fstream>
//#include <omp.h>
#include <ppl.h>
#define NUMROWS 12
#define NUMCOLS 6

//Global
const int WATERLEVEL=9;
// Set with settings file
extern double scoreMat[4][4][2][2];
extern int UNCERTAINTY_SCORE;
extern int SINGLESSCORE;
extern int MAXDEPTH;
void initScore();
struct Coord
{
	Coord(int rr, int cc)
	{
		r=rr;
		c=cc;
	}
	int r;
	int c;
};
class Board
{
public:
	Board()
	{}
	Board(bool useFile);
	Board( Board & other )
	{
		// use memcpy instead since boardsize doesn't change
		memcpy(this->mboard,other.mboard,6*12);
	}
	double uncertaintyScore();
	void displayBoard();
	double makeMove(int row, int col);
	double evalMove(int row, int col);
	double jellyBoard(int row, int col);
	double pufferBoard(int row, int col);
	double cleanBoard();
	double editBoard(int row, int col, int leftVert, int rightVert, int vert_left_up, int vert_right_up, int left, int right);
	double findBestMove(int & scoredMove, int curDepth, int &row, int &col);
	double lowerDepthFinder(int curDepth);
	double findBestMoveOMP(int & scoredMove, int curDepth, int &row, int &col);

	char mboard[12][6];
private:
};