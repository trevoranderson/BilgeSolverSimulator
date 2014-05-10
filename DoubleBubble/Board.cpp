#include "Board.h"
#include <cassert>
using namespace std;
// These values are initialized from a settings file
double scoreMat[4][4][2][2];
int MAXDEPTH;
int UNCERTAINTY_SCORE;
int SINGLESSCORE;

//void initScore()
//{
//	for (int one = 0; one < 4; one++)
//	{
//		for (int two = 0; two < 4; two++)
//		{
//			for (int three = 0; three < 2; three++)
//			{
//				for (int four = 0; four < 2; four++)
//				{
//					scoreMat[one][two][three][four] = 0;
//				}
//			}
//		}
//	}
//	//ima do diss TZZ style
//	scoreMat[0][0][0][0];
//	// single 3 -> 3pts
//	scoreMat[0][0][0][1] = scoreMat[0][0][1][0] = scoreMat[0][1][0][0] = scoreMat[1][0][0][0] = 30;
//	// single 4 -> 4pts
//	scoreMat[2][0][0][0] = scoreMat[0][2][0][0] = 40;
//	// single 5 -> 5pts
//	scoreMat[3][0][0][0] = scoreMat[0][3][0][0] = 50;
//	//      3x3 -> 12pts
//	scoreMat[1][1][0][0] = scoreMat[1][0][1][0] = scoreMat[1][0][0][1] = scoreMat[0][1][1][0] = scoreMat[0][1][0][1] = scoreMat[0][0][1][1] = 120;
//	//      3x4 -> 14pts
//	scoreMat[2][1][0][0] = scoreMat[2][0][1][0] = scoreMat[2][0][0][1] = scoreMat[1][2][0][0] = scoreMat[0][2][1][0] = scoreMat[0][2][0][1] = 140;
//	//      3x5 -> 16pts
//	scoreMat[3][1][0][0] = scoreMat[3][0][1][0] = scoreMat[3][0][0][1] = scoreMat[1][3][0][0] = scoreMat[0][3][1][0] = scoreMat[0][3][0][1] = 160;
//	//		4x4 -> 18
//	scoreMat[2][2][0][0] = 180;
//	//		4x5 -> 20
//	scoreMat[2][3][0][0] = scoreMat[3][2][0][0] = 200;
//	//		5x5 -> 22
//	scoreMat[3][3][0][0] = 220;
//	//333 Bingo -> 24pts
//	scoreMat[1][1][1][0] = scoreMat[1][1][0][1] = scoreMat[1][0][1][1] = scoreMat[0][1][1][1] = 240;
//	//334 Bingo -> 27pts
//	scoreMat[2][1][1][0] = scoreMat[2][1][0][1] = scoreMat[2][0][1][1] = scoreMat[1][2][1][0] = scoreMat[1][2][0][1] = scoreMat[0][2][1][1] = 270;
//	//344 Bingo -> 30pts
//	scoreMat[2][2][1][0] = scoreMat[2][2][0][1] = 300;
//	//335 Bingo -> 30pts
//	scoreMat[3][1][1][0] = scoreMat[3][1][0][1] = scoreMat[3][0][1][1] = scoreMat[1][3][1][0] = scoreMat[1][3][0][1] = scoreMat[0][3][1][1] = 300;
//	//345 Bingo -> 33pts
//	scoreMat[3][2][1][0] = scoreMat[3][2][0][1] = scoreMat[2][3][1][0] = scoreMat[2][3][0][1] = 330;
//	//355 Bingo -> 36pts
//	scoreMat[3][3][1][0] = scoreMat[3][3][0][1] = 360;
//	//3333donkey-> 80pts
//	scoreMat[1][1][1][1] = 800;
//	//3334donkey-> 88pts
//	scoreMat[2][1][1][1] = scoreMat[1][2][1][1] = 880;
//	//3344donkey-> 96pts
//	scoreMat[2][2][1][1] = 960;
//	//3335vegas-> 144pts
//	scoreMat[3][1][1][1] = scoreMat[1][3][1][1] = 1440;
//	//3345vegas-> 156pts
//	scoreMat[3][2][1][1] = scoreMat[2][3][1][1] = 1560;
//	//3355vegas-> 168pts
//	scoreMat[3][3][1][1] = 1680;
//}
double Board::uncertaintyScore()
{
	double count = 0;
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS; c++)
		{
			if (this->mboard[r][c] == 'x')
			{
				count++;
			}
		}
	}
	return count * UNCERTAINTY_SCORE;
}
Board::Board(bool useFile)
{
	ifstream input("Source.txt");
	if (!input)		        // Did opening the file fail?
	{
		cerr << "Error: Cannot open Source.txt!" << endl;
	}
	char in;

	for (int r = 0; r < NUMROWS; r++)    //This loops on the rows.
	{
		for (int c = 0; c < NUMCOLS; c++) //This loops on the columns
		{

			input.get(in);
			if (in == '\n')
				input.get(in);

			mboard[r][c] = in;  //you can also connect to the file
			//and place the name of your ifstream in the input after opening the file will
			//let you read from the file.

		}
	}


}
void Board::displayBoard()
{
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS; c++)
		{
			cout << mboard[r][c] << " ";
		}
		cout << endl;
	}
	cout << endl << endl;
}
double Board::makeMove(int row = -1, int col = -1)
{ //row and column mark the piece on the left side of the move
	// This function is essentially constant
	if (col > 4)
	{
		cerr << "col out of range" << endl;
		return -999;
	}
	//do da swap
	char temp = mboard[row][col];
	mboard[row][col] = mboard[row][col + 1];
	mboard[row][col + 1] = temp;

	int vert_left_up = 0;
	int k = 1;
	for (; mboard[row + k][col] == mboard[row][col] && (row + k) < NUMROWS; k++)
	{
	}
	vert_left_up += k - 1;

	int vert_left_down = 0;
	for (k = 1; mboard[row - k][col] == mboard[row][col] && (row - k) >= 0; k++)
	{
	}
	vert_left_down += k - 1;


	int vert_right_up = 0;
	int rCol = col + 1;
	for (k = 1; mboard[row + k][rCol] == mboard[row][rCol] && (row + k) < NUMROWS; k++)
	{
	}
	vert_right_up += k - 1;

	int vert_right_down = 0;
	for (k = 1; mboard[row - k][rCol] == mboard[row][rCol] && (row - k) >= 0; k++)
	{
	}
	vert_right_down += k - 1;

	int left = 0;
	int right = 0;
	if (col >= 2)
	{//fill left
		if (mboard[row][col - 1] == mboard[row][col] && mboard[row][col - 2] == mboard[row][col])
		{
			left = 1;
		}
	}
	if (col < 3)
	{ //fill right
		if (mboard[row][col + 1] == mboard[row][col] && mboard[row][col + 2] == mboard[row][col])
		{
			right = 1;
		}
	}

	int leftVert = vert_left_up + vert_left_down + 1 - 2;
	if (leftVert < 0)
	{
		leftVert = 0;
	}
	int rightVert = vert_right_down + vert_right_up + 1 - 2;
	if (rightVert < 0)
	{
		rightVert = 0;
	}
	int Score = scoreMat[leftVert][rightVert][left][right];
	//do da swap back
	temp = mboard[row][col];
	mboard[row][col] = mboard[row][col + 1];
	mboard[row][col + 1] = temp;
	return Score;
}
double Board::cleanBoard()
{
	//TODO: this is inefficient: make this faster
	// restrict domain of searching
	int totCleaned = 0;
	int numCleaned = 0;
	int crabsCleared = 0;
	bool cleanedSomething = false;
	Board subBoard = *this;
	//loop through rows
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS - 3; c++)
		{
			if (mboard[r][c] == mboard[r][c + 1] && mboard[r][c] == mboard[r][c + 2]
				&& mboard[r][c] < 'j')//!='x' && mboard[r][c]!='k' && mboard[r][c]!='p' && mboard[r][c]!='j')
				//	if( !((mboard[r][c] ^ mboard[r][c+1]) + (mboard[r][c] ^ mboard[r][c+2])) 
				//		&& mboard[r][c]<'j')
			{
				char temp = mboard[r][c];
				cleanedSomething = true;
				//We know we have at least 3 in a row: look for more
				for (; mboard[r][c] == temp && c < NUMCOLS; c++)
				{
					subBoard.mboard[r][c] = 'o';
				}
			}
			else if (mboard[r][c] == 'k' && r < WATERLEVEL)
			{
				subBoard.mboard[r][c] = 'o';
				crabsCleared++;
				cleanedSomething = true;
			}
		}
	}
	//loop the other way
	for (int c = 0; c < NUMCOLS; c++)
	{
		for (int r = 0; r < NUMROWS - 3; r++)
		{
			if (mboard[r][c] == mboard[r + 1][c] && mboard[r][c] == mboard[r + 2][c]
				&& mboard[r][c] < 'j')//!='x' && mboard[r][c]!='k' && mboard[r][c]!='p' && mboard[r][c]!='j' && mboard[r][c]!='o')
			{
				char temp = mboard[r][c];
				cleanedSomething = true;
				//We know we have at least 3 in a col: look for more
				for (; mboard[r][c] == temp && r < NUMROWS; r++)
				{
					subBoard.mboard[r][c] = 'o';
				}
			}
		}
	}
	if (cleanedSomething)
	{
		// asess cleaning with subboard, then copy over and start again
		for (int r = 0; r < NUMROWS; r++)
		{
			for (int c = 0; c < NUMCOLS; c++)
			{
				if (subBoard.mboard[r][c] == 'o')
				{
					numCleaned++;
					int rr = r;
					for (; rr < NUMROWS - 1 && subBoard.mboard[rr][c] != 'x'; rr++)
					{
						subBoard.mboard[rr][c] = subBoard.mboard[rr + 1][c];
					}
					subBoard.mboard[rr][c] = 'x';
					c--;
				}
			}
		}
		//now copy subBoard to board
		memcpy(this->mboard, subBoard.mboard, 6 * 12);
		totCleaned = numCleaned + crabsCleared*crabsCleared + cleanBoard();
	}
	return totCleaned;
}
double Board::pufferBoard(int row, int col)
{
	int puffR = row;
	int puffC;
	if (mboard[row][col] == 'p')
	{
		puffC = col;
	}
	else
	{
		puffC = col + 1;
	}
	//now clear the 3x3 area around the puffer
	mboard[puffR][puffC] = 'o';
	if (puffC < NUMCOLS - 1)
	{ //we can clear the right side
		mboard[puffR][puffC + 1] = 'o';
		if (puffR<NUMROWS - 1)
		{
			mboard[puffR + 1][puffC + 1] = 'o';
		}
		if (puffR>0)
		{
			mboard[puffR - 1][puffC] = 'o';

		}
	}
	if (puffC > 0)
	{ //left side
		mboard[puffR][puffC - 1] = 'o';
		if (puffR<NUMROWS - 1)
		{
			mboard[puffR + 1][puffC - 1] = 'o';
		}
		if (puffR>0)
		{
			mboard[puffR - 1][puffC] = 'o';

		}
	}
	//top and bottom
	if (puffR<NUMROWS - 1)
	{
		mboard[puffR + 1][puffC] = 'o';
	}
	if (puffR>0)
	{
		mboard[puffR - 1][puffC] = 'o';
	}

	//now clean those, and return cleanboard!
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS; c++)
		{
			if (mboard[r][c] == 'o')
			{
				int rr = r;
				for (; rr < NUMROWS - 1 && mboard[rr][c] != 'x'; rr++)
				{
					mboard[rr][c] = mboard[rr + 1][c];
				}
				mboard[NUMROWS - 1][c] = 'x';
				c--;
			}
		}
	}
	return 20 + SINGLESSCORE * cleanBoard();
}
double Board::jellyBoard(int row, int col)
{
	//In this function, we evaluate a move that contains a jelly piece
	char ClearThis;
	if (mboard[row][col] == 'j')
	{
		ClearThis = mboard[row][col + 1];
		mboard[row][col] = 'o'; //opposite of clearthis is jelly, clear it!
	}
	else
	{
		ClearThis = mboard[row][col];
		mboard[row][col + 1] = 'o';
	}
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS; c++)
		{
			if (ClearThis == mboard[r][c])
			{
				mboard[r][c] = 'o';
			}
		}
	}
	//now clean those, and return cleanboard!
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS; c++)
		{
			if (mboard[r][c] == 'o')
			{
				int rr = r;
				for (; rr < NUMROWS - 1 && mboard[rr][c] != 'x'; rr++)
				{
					mboard[rr][c] = mboard[rr + 1][c];
				}
				mboard[NUMROWS - 1][c] = 'x';
				c--;
			}
		}
	}
	return 20 + SINGLESSCORE * cleanBoard();
}
double Board::editBoard(int row, int col, int leftVert, int rightVert, int vert_left_up, int vert_right_up, int left, int right)
{
	//Here, we want to take the given move and remove pieces and shift to get the next valid board (with some uncertainty in pieces).
	if (leftVert && rightVert)
	{
		//both sides have verticle evaluation

		//calc and shift the left side
		int topRowLeft = row - vert_left_up;
		for (int k = topRowLeft; k < NUMROWS - (leftVert + 2); k++)
		{
			mboard[k][col] = mboard[k + (leftVert + 2)][col];
		}
		for (int k = 1; k <= (leftVert + 2); k++)
		{
			mboard[NUMROWS - k][col] = 'x';
		}

		//calc and shift right side
		int topRowRight = row - vert_right_up;
		int nCol = col + 1;
		for (int k = topRowRight; k < NUMROWS - (rightVert + 2); k++)
		{
			mboard[k][nCol] = mboard[k + (rightVert + 2)][nCol];
		}
		for (int k = 1; k <= (rightVert + 2); k++)
		{
			mboard[NUMROWS - k][nCol] = 'x';
		}

		//calc and shift left and right row matches
		if (left)
		{
			//double single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col - 1] = mboard[k + 1][col - 1];
				mboard[k][col - 2] = mboard[k + 1][col - 2];
			}
			mboard[NUMROWS - 1][col - 1] = 'x';
			mboard[NUMROWS - 1][col - 2] = 'x';
		}
		if (right)
		{
			//double single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col + 2] = mboard[k + 1][col + 2];
				mboard[k][col + 3] = mboard[k + 1][col + 3];
			}
			mboard[NUMROWS - 1][col + 2] = 'x';
			mboard[NUMROWS - 1][col + 3] = 'x';
		}
	}
	else if (leftVert && !rightVert)
	{
		//calc and shift the left side
		int topRowLeft = row - vert_left_up;
		for (int k = topRowLeft; k < NUMROWS - (leftVert + 2); k++)
		{
			mboard[k][col] = mboard[k + (leftVert + 2)][col];
		}
		for (int k = 1; k <= (leftVert + 2); k++)
		{
			mboard[NUMROWS - k][col] = 'x';
		}
		if (left)
		{
			//double single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col - 1] = mboard[k + 1][col - 1];
				mboard[k][col - 2] = mboard[k + 1][col - 2];
			}
			mboard[NUMROWS - 1][col - 1] = 'x';
			mboard[NUMROWS - 1][col - 2] = 'x';
		}
		if (right)
		{
			//triple single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col + 1] = mboard[k + 1][col + 1];
				mboard[k][col + 2] = mboard[k + 1][col + 2];
				mboard[k][col + 3] = mboard[k + 1][col + 3];
			}
			mboard[NUMROWS - 1][col + 1] = 'x';
			mboard[NUMROWS - 1][col + 2] = 'x';
			mboard[NUMROWS - 1][col + 3] = 'x';
		}
	}
	else if (rightVert && !leftVert)
	{
		//calc and shift right side
		int topRowRight = row - vert_right_up;
		for (int k = topRowRight; k < NUMROWS - (rightVert + 2); k++)
		{
			mboard[k][col + 1] = mboard[k + (rightVert + 2)][col + 1];
		}
		//set X's for right col
		for (int k = 1; k <= (rightVert + 2); k++)
		{
			mboard[NUMROWS - k][col + 1] = 'x';
		}

		if (left)
		{
			//triple single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col] = mboard[k + 1][col];
				mboard[k][col - 1] = mboard[k + 1][col - 1];
				mboard[k][col - 2] = mboard[k + 1][col - 2];
			}
			mboard[NUMROWS - 1][col] = 'x';
			mboard[NUMROWS - 1][col - 1] = 'x';
			mboard[NUMROWS - 1][col - 2] = 'x';

		}
		if (right)
		{
			//double single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col + 2] = mboard[k + 1][col + 2];
				mboard[k][col + 3] = mboard[k + 1][col + 3];
			}
			mboard[NUMROWS - 1][col + 2] = 'x';
			mboard[NUMROWS - 1][col + 3] = 'x';
		}
	}
	else
	{
		if (left)
		{
			//triple single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col] = mboard[k + 1][col];
				mboard[k][col - 1] = mboard[k + 1][col - 1];
				mboard[k][col - 2] = mboard[k + 1][col - 2];
			}
			mboard[NUMROWS - 1][col] = 'x';
			mboard[NUMROWS - 1][col - 1] = 'x';
			mboard[NUMROWS - 1][col - 2] = 'x';
		}
		if (right)
		{
			//double single column swaps
			for (int k = row; k < NUMROWS - 1; k++)
			{
				mboard[k][col + 1] = mboard[k + 1][col + 1];
				mboard[k][col + 2] = mboard[k + 1][col + 2];
				mboard[k][col + 3] = mboard[k + 1][col + 3];
			}
			mboard[NUMROWS - 1][col + 1] = 'x';
			mboard[NUMROWS - 1][col + 2] = 'x';
			mboard[NUMROWS - 1][col + 3] = 'x';
		}
	}
	if (left || right || leftVert || rightVert)
	{
		return SINGLESSCORE * cleanBoard();
	}
	return 0;
}
double Board::evalMove(int row, int col)
{ //row and column mark the piece on the left side of the move
	//this function evualates the move its given
	//col should be >=0 and less than 6;
	//do da swap
	char temp = mboard[row][col];
	mboard[row][col] = mboard[row][col + 1];
	mboard[row][col + 1] = temp;

	int vert_left_down = 0;
	int k = 1;
	for (; (row + k) < NUMROWS && mboard[row + k][col] == mboard[row][col]; k++)
	{
	}
	vert_left_down = k - 1;

	int vert_left_up = 0;
	for (k = 1; (row - k) >= 0 && mboard[row - k][col] == mboard[row][col]; k++)
	{
	}
	vert_left_up = k - 1;


	int vert_right_down = 0;
	int rCol = col + 1;
	for (k = 1; (row + k) < NUMROWS && mboard[row + k][rCol] == mboard[row][rCol]; k++)
	{
	}
	vert_right_down = k - 1;

	int vert_right_up = 0;
	for (k = 1; (row - k) >= 0 && mboard[row - k][rCol] == mboard[row][rCol]; k++)
	{
	}
	vert_right_up = k - 1;

	int left = 0;
	int right = 0;
	if (col >= 2)
	{//fill left
		if (mboard[row][col - 1] == mboard[row][col] && mboard[row][col - 2] == mboard[row][col])
		{
			left = 1;
		}
	}
	if (col < 3)
	{ //fill right
		if (mboard[row][col + 2] == mboard[row][col + 1] && mboard[row][col + 3] == mboard[row][col + 1])
		{
			right = 1;
		}
	}

	int leftVert = vert_left_up + vert_left_down + 1 - 2;
	if (leftVert < 0)
	{
		leftVert = 0;
	}
	int rightVert = vert_right_down + vert_right_up + 1 - 2;
	if (rightVert < 0)
	{
		rightVert = 0;
	}
	int Score = scoreMat[leftVert][rightVert][left][right];
	// now we have the score, now we just have to evaluate the move, and add the uncertainty
	return Score + editBoard(row, col, leftVert, rightVert, vert_left_up, vert_right_up, left, right);
}
double Board::lowerDepthFinder(int curDepth)
{
	//recursively finds the best move over a preset depth: sets row and col, and returns the score
	//this one only deals with deep layers
	double scoreBase = uncertaintyScore();
	double greedyScores[60];
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS - 1; c++)
		{
			greedyScores[r*(NUMCOLS - 1) + c] = scoreBase;
			/*if(mboard[r][c]!='x' && mboard[r][c+1]!='x' && mboard[r][c]!='k' && mboard[r][c+1]!='k'
					&& mboard[r][c]!='j' && mboard[r][c+1]!='j'&& mboard[r][c]!='p' && mboard[r][c+1]!='p')*/
			if (mboard[r][c] < 'h' && mboard[r][c + 1] < 'h')
			{//proceed normally
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.evalMove(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			/*	else if(((mboard[r][c]=='j') != (mboard[r][c+1]=='j')) && mboard[r][c]!='x' && mboard[r][c+1]!='x' &&
						mboard[r][c]!='k' && mboard[r][c+1]!='k' && mboard[r][c]!='p' && mboard[r][c+1]!='p')*/
			else if (((mboard[r][c] == 'j') != (mboard[r][c + 1] == 'j')) && (mboard[r][c] < 'h' || mboard[r][c + 1] < 'h'))
			{ //one of them is a jelly and the other isn't a special or unknown
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.jellyBoard(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			/*else if(((mboard[r][c]=='p') != (mboard[r][c+1]=='p')) && mboard[r][c]!='x' && mboard[r][c+1]!='x' &&
					mboard[r][c]!='k' && mboard[r][c+1]!='k' && mboard[r][c]!='j' && mboard[r][c+1]!='j')*/
			else if (((mboard[r][c] == 'p') != (mboard[r][c + 1] == 'p')) && (mboard[r][c] < 'h' || mboard[r][c] < 'h'))
			{ //one is a puffer, and the other is not
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.pufferBoard(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			// now Scores and GreedyScores are what they should be, now we setup for return by checking against the previous values
		}
	}
	int highscoreindex = 0;

	for (int k = 1; k<60; k++)
	{
		if (greedyScores[k] > greedyScores[highscoreindex])
		{
			highscoreindex = k;
		}
	}
	return greedyScores[highscoreindex];
}
double Board::findBestMove(int & scoredMove, int curDepth, int &rowref, int &colref)
{
	//recursively finds the best move over a preset depth: sets row and col, and returns the score
	// This is the top level find best move: IE it is responsible for returning the row/col/score as references
	double scoreBase = uncertaintyScore();

	double greedyScores[60];
	for (int r = 0; r < NUMROWS; r++)
	{
		for (int c = 0; c < NUMCOLS - 1; c++)
		{
			greedyScores[r*(NUMCOLS - 1) + c] = scoreBase;
			/*if(mboard[r][c]!='x' && mboard[r][c+1]!='x' && mboard[r][c]!='k' && mboard[r][c+1]!='k'
					&& mboard[r][c]!='j' && mboard[r][c+1]!='j'&& mboard[r][c]!='p' && mboard[r][c+1]!='p')*/
			if (mboard[r][c] < 'h' && mboard[r][c + 1] < 'h')
			{//proceed normally
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.evalMove(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			/*	else if(((mboard[r][c]=='j') != (mboard[r][c+1]=='j')) && mboard[r][c]!='x' && mboard[r][c+1]!='x' &&
						mboard[r][c]!='k' && mboard[r][c+1]!='k' && mboard[r][c]!='p' && mboard[r][c+1]!='p')*/
			else if (((mboard[r][c] == 'j') != (mboard[r][c + 1] == 'j')) && (mboard[r][c] < 'h' || mboard[r][c + 1] < 'h'))
			{ //one of them is a jelly and the other isn't a special or unknown
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.jellyBoard(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			/*else if(((mboard[r][c]=='p') != (mboard[r][c+1]=='p')) && mboard[r][c]!='x' && mboard[r][c+1]!='x' &&
					mboard[r][c]!='k' && mboard[r][c+1]!='k' && mboard[r][c]!='j' && mboard[r][c+1]!='j')*/
			else if (((mboard[r][c] == 'p') != (mboard[r][c + 1] == 'p')) && (mboard[r][c] < 'h' || mboard[r][c] < 'h'))
			{ //one is a puffer, and the other is not
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.pufferBoard(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			// now Scores and GreedyScores are what they should be, now we setup for return by checking against the previous values
		}
	}
	int highscoreindex = 0;

	for (int k = 1; k<60; k++)
	{
		if (greedyScores[k] > greedyScores[highscoreindex])
		{
			highscoreindex = k;
		}
	}
	Board subBoard = *this;
	rowref = highscoreindex / (NUMCOLS - 1);
	colref = highscoreindex - (rowref*(NUMCOLS - 1));
	scoredMove = subBoard.evalMove(rowref, colref);
	return greedyScores[highscoreindex];
}
double Board::findBestMoveOMP(int & scoredMove, int curDepth, int &rowref, int &colref)
{
	//recursively finds the best move over a preset depth: sets row and col, and returns the score
	double scoreBase = uncertaintyScore();
	double greedyScores[60];
	concurrency::parallel_for(0, NUMROWS, [&](size_t r){
		for (int c = 0; c < NUMCOLS - 1; c++)
		{
			greedyScores[r*(NUMCOLS - 1) + c] = scoreBase;
			/*if(mboard[r][c]!='x' && mboard[r][c+1]!='x' && mboard[r][c]!='k' && mboard[r][c+1]!='k'
					&& mboard[r][c]!='j' && mboard[r][c+1]!='j'&& mboard[r][c]!='p' && mboard[r][c+1]!='p')*/
			if (mboard[r][c] < 'h' && mboard[r][c + 1] < 'h')
			{//proceed normally
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.evalMove(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			/*	else if(((mboard[r][c]=='j') != (mboard[r][c+1]=='j')) && mboard[r][c]!='x' && mboard[r][c+1]!='x' &&
						mboard[r][c]!='k' && mboard[r][c+1]!='k' && mboard[r][c]!='p' && mboard[r][c+1]!='p')*/
			else if (((mboard[r][c] == 'j') != (mboard[r][c + 1] == 'j')) && (mboard[r][c] < 'h' || mboard[r][c + 1] < 'h'))
			{ //one of them is a jelly and the other isn't a special or unknown
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.jellyBoard(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			/*else if(((mboard[r][c]=='p') != (mboard[r][c+1]=='p')) && mboard[r][c]!='x' && mboard[r][c+1]!='x' &&
					mboard[r][c]!='k' && mboard[r][c+1]!='k' && mboard[r][c]!='j' && mboard[r][c+1]!='j')*/
			else if (((mboard[r][c] == 'p') != (mboard[r][c + 1] == 'p')) && (mboard[r][c] < 'h' || mboard[r][c] < 'h'))
			{ //one is a puffer, and the other is not
				Board subBoard = *this;
				greedyScores[r*(NUMCOLS - 1) + c] += subBoard.pufferBoard(r, c);
				if (curDepth < MAXDEPTH)
				{
					greedyScores[r*(NUMCOLS - 1) + c] += subBoard.lowerDepthFinder(curDepth + 1);
				}
			}
			// now Scores and GreedyScores are what they should be, now we setup for return by checking against the previous values
		}
	});
	int highscoreindex = 0;

	for (int k = 1; k<60; k++)
	{
		if (greedyScores[k] > greedyScores[highscoreindex])
		{
			highscoreindex = k;
		}
	}
	Board subBoard = *this;
	rowref = highscoreindex / (NUMCOLS - 1);
	colref = highscoreindex - (rowref*(NUMCOLS - 1));
	scoredMove = subBoard.evalMove(rowref, colref);
	return greedyScores[highscoreindex];
}


//concurrency::parallel_for(0,NUMROWS,[&] (size_t r){