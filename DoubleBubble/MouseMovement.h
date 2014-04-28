#pragma once
#include <vector>
#include <fstream>
#include <stdio.h>
#include <Windows.h>
#include "Point.h"
#include "Mat2.h"
#define DISTLENGTH 10000
template <class T> class Pair
{
public:
	T first;
	T second;
};
double BellRand(double width, double center)
{
	// Generates a random number around a given center and a distribution max length
	double randSum = 0;
	for (int k = 0; k < 100; k++)
	{
		randSum += rand() % 100;
	}
	randSum = randSum / (double)100.0; //now have a normally distrubuted number between 0 and 100,000
	randSum = randSum / (double)100.0; // now its between 0-1
	randSum *= width;
	randSum -= (width / 2);
	randSum += center;
	return randSum;
}
inline double TimeDifference(SYSTEMTIME A, SYSTEMTIME B)
{
	double ret = (A.wHour - B.wHour) * 3600 + (A.wMinute - B.wMinute) * 60 + A.wSecond - B.wSecond + (double)(A.wMilliseconds - B.wMilliseconds) / 1000;
	return ret;
}
void vecToOrigin(std::vector<Point> & toTransform)
{
	//Translates to origin-> rotates theta to zero -> scales to 500 total pixels in X-axis
	// first translate all points to origin
	Point firstPoint(toTransform[0].x, toTransform[0].y);
	int numPoints = toTransform.size();
	for (int cnt = 0; cnt < numPoints; cnt++)
	{
		toTransform[cnt].x -= firstPoint.x;
		toTransform[cnt].y -= firstPoint.y;
	}
	//find the angle between the ending points and set it equal to zero.
	double theta = atan2((toTransform[numPoints - 1].y), (toTransform[numPoints - 1].x));
	for (int cnt = 0; cnt < numPoints; cnt++)
	{
		toTransform[cnt] = vecmatmult(matrot(-theta), toTransform[cnt]);
	}
	//finally, scale the length to a predetermined amount
	double scaleFactor = (double)DISTLENGTH / (toTransform[numPoints - 1].x);
	for (int cnt = 0; cnt < numPoints; cnt++)
	{
		toTransform[cnt] = vecmatmult(matscale(scaleFactor, scaleFactor), toTransform[cnt]);
	}
}
void transformToPoints(Point A, Point B, std::vector<Point> & toTrans)
{
	// Move it to origin so transformations work properly
	// Now starts at (0,0) Bs at (DISTLENGTH,0)
	vecToOrigin(toTrans);
	int size = toTrans.size();
	Point dirVec(B.x - A.x, B.y - A.y);
	double displacement = sqrt((dirVec.x) * (dirVec.x) + (dirVec.y) * (dirVec.y));
	double scaleFactor = displacement / DISTLENGTH;
	int numPoints = toTrans.size();
	//scale -> rotate -> translate such that you cover the wanted path
	//Scale:
	for (auto &i : toTrans)
	{
		i = vecmatmult(matscale(scaleFactor, scaleFactor), i);
	}
	//Rotate:
	double theta = atan2(dirVec.y, dirVec.x);
	for (auto &i : toTrans)
	{
		i = vecmatmult(matrot(theta), i);
	}
	// Translate:
	for (auto &i : toTrans)
	{
		i.x += A.x;
		i.y += A.y;
	}
}
bool operator== (Point A, Point B)
{
	return A.x == B.x && A.y == B.y;
}
Point generateDest(Point center, double radius)
{
	// The goal of this function is to give our move target a valid location
	// that follows a normal distribution. radius=max radius
	double randRadius = BellRand(radius*(double)2.0, 0);
	if (randRadius == 0)
	{
		return center;
	}
	double theta = rand();
	double retX = sin(theta) * randRadius + center.x;
	double retY = cos(theta) * randRadius + center.y;
	return Point(retX, retY);
}

//returns array size 2 POINTS CANNOT BE THE SAME
Pair<Point> getControlPoints(Point A, Point B, Point C, double t = 0)
{
	double d01 = sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));
	double d12 = sqrt(pow(C.x - B.x, 2) + pow(C.y - B.y, 2));
	double fa = (t * d01) / (d01 + d12); // CARE FOR DIV0!
	double fb = (t * d12) / (d01 + d12);
	double p1x = B.x - fa * (C.x - A.x);
	double p1y = B.y - fa * (C.y - A.y);
	double p2x = B.x + fb * (C.x - A.x);
	double p2y = B.y + fb * (C.y - A.y);
	Pair<Point> ret;
	ret.first = Point(p1x, p1y);
	ret.second = Point(p2x, p2y);
	return ret;
}
struct SetPoint
{
	SetPoint()
	{
		b = false;
	}
	SetPoint(Point pp, bool bb)
	{
		p = pp;
		b = bb;
	}
	// The actual values
	Point p;
	// Whether or not the point has been set
	bool b;
};
std::vector<Point> setPointsToPoints(const std::vector<SetPoint> & toTransform)
{
	std::vector<Point> points;
	points.resize(toTransform.size());
	for (int i = 0; i < toTransform.size(); i++)
	{
		points[i].x = toTransform[i].p.x;
		points[i].y = toTransform[i].p.y;
	}
	return points;
}
void pointsToSetPoints(const std::vector<Point> & toCopy, std::vector<SetPoint> & toChange)
{
	for (int i = 0; i < toCopy.size(); i++)
	{
		toChange[i].p.x = toCopy[i].x;
		toChange[i].p.y = toCopy[i].y;
		toChange[i].b = true;
	}
}
class MouseMovement
{
public:
	MouseMovement()
	{ /* Without arguments, cannot construct stuff */
	}
	MouseMovement(std::string path)
	{
		Load(path);
	}
	void LinearMove(Point start, Point end, double time_to_move)const
	{
		if (start.x == end.x && start.y == end.y)
		{
			return;
		}
		SYSTEMTIME start_time;
		GetSystemTime(&start_time);
		Point parametricvector(end.x - start.x, end.y - start.y);
		SYSTEMTIME now_time;
		double time_percent = 0;
		while (time_percent < 1)
		{
			GetSystemTime(&now_time);
			time_percent = (TimeDifference(now_time, start_time)) / time_to_move;
			SetCursorPos(start.x + time_percent*(parametricvector.x), start.y + time_percent*(parametricvector.y));
		}
	}
	void Click(double waitTime = 0, double waitRadius = 0)
	{
		if (waitTime == 0 && waitRadius == 0)
		{
			POINT p;
			POINT * curserpos = &p;
			GetCursorPos(curserpos);
			INPUT input;
			input.type = INPUT_MOUSE;
			input.mi.dx = curserpos->x;
			input.mi.dy = curserpos->y;
			input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE |/*MOUSEEVENTF_MOVE|*/MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP);
			input.mi.mouseData = 0;
			input.mi.dwExtraInfo = NULL;
			input.mi.time = 0;
			SendInput(1, &input, sizeof(INPUT));
		}
		else
		{
			//Calculate sleep time
			double sleepDuration = BellRand(waitTime, waitRadius);

			POINT p;
			POINT * curserpos = &p;
			GetCursorPos(curserpos);
			INPUT input;
			input.type = INPUT_MOUSE;
			input.mi.dx = curserpos->x;
			input.mi.dy = curserpos->y;
			input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE |/*MOUSEEVENTF_MOVE|*/MOUSEEVENTF_LEFTDOWN);
			input.mi.mouseData = 0;
			input.mi.dwExtraInfo = NULL;
			input.mi.time = 0;
			SendInput(1, &input, sizeof(INPUT));

			//wait
			Sleep(sleepDuration);
			input.mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
			SendInput(1, &input, sizeof(INPUT));
		}
	}
	void PressKey(int key = 0x75, double waitTime = 0, double waitRadius = 0)
	{
		// Calculate wait Time inbetween key down and key up
		double sleepDuration = BellRand(waitTime, waitRadius);

		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0; // hardware scan code for key
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		// Press the key
		ip.ki.wVk = key; // virtual-key code for the f6 key -> replace later
		ip.ki.dwFlags = 0; // 0 for key press
		SendInput(1, &ip, sizeof(INPUT));

		// Sleep for specified time
		Sleep(sleepDuration);

		// Release the key
		ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
		SendInput(1, &ip, sizeof(INPUT));
	}
	void Play()
	{
		Play(Storage.size() / pointspersecond);
	}
	void Play(double time_to_move)
	{
		MouseMovement * toPlay = this;
		MouseMovement other;
		// Resize if we are smaller than Storage
		if (time_to_move > (Storage.size() / pointspersecond));
		{
			toPlay = &other;
			toPlay->Storage = Storage;
			toPlay->pointspersecond = pointspersecond;
			toPlay->resizeWithResolution(time_to_move);
		}
		// Build a lighter point vector for playing
		std::vector<Point> points = setPointsToPoints(toPlay->Storage);
		// Don't use Storage after this point
		double elapsedTime = 0;
		int numPoints = points.size();
		int bucket = 0;
		SYSTEMTIME start_time;
		GetSystemTime(&start_time);
		SYSTEMTIME now_time;
		while (bucket < numPoints)
		{
			// Apply the current bucket's action
			SetCursorPos(points[bucket].x, points[bucket].y);
			// Prepare for next
			GetSystemTime(&now_time);
			elapsedTime = (TimeDifference(now_time, start_time)) / time_to_move;
			//select the proper bucket for smooth movement
			bucket = (double)numPoints*elapsedTime;
		}
	}
	void PlayBetweenPoints(Point begin, Point end)
	{
		PlayBetweenPoints(begin, end, Storage.size() / pointspersecond);
	}
	void PlayBetweenPoints(Point begin, Point end, double time_to_move)
	{
		MouseMovement toPlay(*this);
		std::vector<Point> points = setPointsToPoints(toPlay.Storage);
		// don't use toPlay.Storage until you re set it
		transformToPoints(begin, end, points);
		// Resize if the ending path length is larger than the original
		Point vec = Point((end.x - begin.x), (end.y - begin.y));
		double disp = sqrt(vec.x * vec.x + vec.y * vec.y);
		Point origVec = Point((Storage[Storage.size() - 1].p.x - Storage[0].p.x), (Storage[Storage.size() - 1].p.y - Storage[0].p.y));
		double origDisp = sqrt(origVec.x*origVec.x + origVec.y*origVec.y);
		// Copy points to Storage
		pointsToSetPoints(points, toPlay.Storage);
		if (disp / origDisp > 1)
		{
			toPlay.resizeWithResolution(time_to_move * (disp / origDisp));
			toPlay.pointspersecond *= (disp / origDisp);
		}
		toPlay.Play(time_to_move);
	}
	void Record(double time_to_move, int resolutionpps = 3000)
	{
		// This algorithm works by filling a large vector of Points
		// according to the time elapsed, then filling the the gaps by 
		// linear interpolation
		pointspersecond = resolutionpps;
		ClearStorage();
		//Allocate enough Storage that the vector to have the correct resolution
		int sizeToUse = resolutionpps * time_to_move;
		Storage.resize(sizeToUse);
		int numPoints = Storage.size();

		POINT Q;
		POINT * curserposinit = &Q;
		SYSTEMTIME start_time;
		GetSystemTime(&start_time);
		SYSTEMTIME now_time;
		double elapsedTime = 0;
		// This structure guarantees that Storage[0] is nonempty
		// and we never attempt to put something out of range
		int bucket = 0;
		while (bucket < numPoints)
		{
			// Put something in the bucket
			GetCursorPos(curserposinit);
			Storage[bucket].p = Point(curserposinit->x, curserposinit->y);
			Storage[bucket].b = true;
			//Calculate next bucket
			GetSystemTime(&now_time);
			elapsedTime = (TimeDifference(now_time, start_time)) / time_to_move;
			bucket = (int)numPoints*elapsedTime;
		}
		// Interpolate
		smoothInterp(Storage);
		//Interpolate(Storage);
	}
	void Save(std::string path)const
	{
		// copy Storage into a large char array.
		std::ofstream file(path, std::ios::binary);
		int pointsSize = Storage.size();
		// pare it down to just the values
		std::vector<Point> points = setPointsToPoints(Storage);
		// Don't use storage after this
		int bytesize = sizeof(Point)*(points.size()) + sizeof(pointspersecond);
		char * bytearr = new char[bytesize];
		memcpy(bytearr, &pointspersecond, sizeof(pointspersecond));
		memcpy(bytearr + sizeof(pointspersecond), &points[0], sizeof(Point)* pointsSize);
		file.write(bytearr, bytesize);
		file.close();
		delete bytearr;
	}
	void Load(std::string path)
	{
		FILE * file = NULL;
		if ((file = fopen(path.c_str(), "rb")) == NULL)
		{
			std::cout << "Could not open specified file" << std::endl;
		}
		long lCurPos, lEndPos;
		lCurPos = ftell(file);
		fseek(file, 0, 2);
		lEndPos = ftell(file);
		fseek(file, lCurPos, 0);

		int fileSize = lEndPos;
		void * fileBuff = new char[fileSize];
		fread(fileBuff, fileSize, 1, file);
		pointspersecond = *((unsigned int*)fileBuff);
		//Cast it to Points and fill Storage
		int StorageSize = (fileSize - sizeof(pointspersecond)) / (sizeof(Point));
		Storage.resize(StorageSize);
		Point * ArrToCpy = (Point*)((char*)fileBuff + sizeof(pointspersecond));
		for (int k = 0; k < StorageSize; k++)
		{
			Storage[k].p.x = ArrToCpy[k].x;
			Storage[k].p.y = ArrToCpy[k].y;
			Storage[k].b = true;
		}
		fclose(file);
		delete fileBuff;
	}
	void ChangeResolution(double factor, double tval = 0)
	{
		resizeWithResolution(((double)Storage.size() / (double)pointspersecond) * factor, tval);
		pointspersecond *= factor;
	}
private:
	/* Left and Right correspond to the part of Storage we are operating on
	*  It should recursively divide Storage placing the pivots in output
	*/
	void resizeHelper(int left, int right, std::vector<SetPoint> &output)
	{ //Right should be out of range by one for intuitive iteration
		if (right - left <= 0)
		{
			return;
		}
		int pivot = (right + left) / 2;
		int outInd = ((double)pivot / Storage.size())* output.size();
		output[outInd].p.x = Storage[pivot].p.x;
		output[outInd].p.y = Storage[pivot].p.y;
		output[outInd].b = true;
		// Divide into left and right sides
		resizeHelper(left, pivot, output);
		resizeHelper(pivot + 1, right, output);
	}
	void resizeWithResolution(double timeToPlay, double tval = 0)
	{
		// maintains resolution, but builds a new vector
		double nSize = timeToPlay*pointspersecond;
		std::vector<SetPoint> nStorage;
		nStorage.resize(nSize);
		resizeHelper(0, Storage.size(), nStorage);
		smoothInterp(nStorage, tval);
		//Interpolate(nStorage);
		Storage = nStorage;
	}
	void ClearStorage()
	{
		for (auto &i : Storage)
		{
			// only need to flag it as unset
			i.b = false;
		}
	}
	// unused function: delete later
	int Interpolate(std::vector<Point> & toInterpolate)
	{
		int numCleaned = 0;
		int restCleaned = 0;
		//Here we want to take our array of mouse positions and interpolate between measurements
		Point Previous(0, 0);
		Point Current(0, 0);
		int firstUnfilled = 0;
		int lastUnfilled = 0;
		int totUnfilled = 0;
		int numPoints = toInterpolate.size();
		// The bucket at index zero is guaranteed to have something, if it was recorded using our Record
		for (int cnt = 0; cnt < numPoints - 1; cnt++)
		{
			if (toInterpolate[cnt].x < 0 && toInterpolate[cnt].y < 0)
			{
				firstUnfilled = cnt;
				lastUnfilled = cnt;
				Previous.x = toInterpolate[cnt - 1].x;
				Previous.y = toInterpolate[cnt - 1].y;
				for (cnt = cnt + 1; (cnt < numPoints - 1) && toInterpolate[cnt].x < 0 && toInterpolate[cnt].y < 0; cnt++)
				{
					lastUnfilled++;
				}
				if (cnt >= numPoints - 1)
				{
					break;
				}
				//Now we have Previous, first unfilled, last unfilled
				Current.x = toInterpolate[cnt].x;
				Current.y = toInterpolate[cnt].y;

				//now loop through unfilled buckets, interpolating the values
				double divider = lastUnfilled - firstUnfilled + 2; // if we have 3,5, we need to divide the gap into 4 spaces
				Point dirVec = Point(Current.x - Previous.x, Current.y - Previous.y);
				for (int p = 0; p < divider - 1; p++)
				{
					toInterpolate[p + firstUnfilled].x = dirVec.x*(((double)p + 1) / divider) + Previous.x;
					toInterpolate[p + firstUnfilled].y = dirVec.y*(((double)p + 1) / divider) + Previous.y;
					numCleaned++;
				}
				totUnfilled += lastUnfilled - firstUnfilled + 1;

			}
		}
		//if the last value is garbage, we need to put a value there
		// and loop back through the array in order to fill things.
		int last = numPoints - 1;
		while (toInterpolate[last].x < 0 || toInterpolate[last].y < 0)
		{
			toInterpolate[last].x = Previous.x;
			toInterpolate[last].y = Previous.y;
			restCleaned++;
			last--;
		}
		return numCleaned + restCleaned;
	}
	int smoothInterp(std::vector<SetPoint> & toInterpolate, double tval = 0)
	{
		std::vector<SetPoint> interpClone = toInterpolate;
		int origSize = interpClone.size();
		const int prePoints = 3;
		// push back three dummy points so we get values near the end
		for (int i = interpClone.size() - 1; i >= 0; i--)
		{
			if (interpClone[i].b)
			{
				// dirty hack: the control point generator requires points to not coincide.
				SetPoint toInsert = interpClone[i];;
				interpClone.push_back(toInsert);
				toInsert.p.x += 0.001;
				interpClone.push_back(toInsert);
				toInsert.p.x += 0.001;
				interpClone.push_back(toInsert);
				break;
			}
		}
		// insert a couple dummies to the front as well, to provide a default threes that works
		for (int i = 0; i < interpClone.size(); i++)
		{
			if (interpClone[i].b)
			{
				// dirty hack: the control point generator requires points to not coincide.
				SetPoint toInsert = interpClone[i];
				interpClone.insert(interpClone.begin(), toInsert);
				toInsert.p.x += 0.01;
				interpClone.insert(interpClone.begin(), toInsert);
				toInsert.p.x += 0.01;
				interpClone.insert(interpClone.begin(), toInsert);
				break;
			}
		}
		// for every set of 3 points, call control point helper
		// then plug into formula based on missing numbers
		int threes[3] = { -1, -1, -1 };
		int tmpind = 0;
		int ptsInd = 0;
		while (tmpind < 3)
		{
			if (interpClone[ptsInd].b)
			{
				threes[tmpind] = ptsInd;
				tmpind++;
			}
			ptsInd++;
		}
		// threes now has 3 valid points
		while (ptsInd < interpClone.size())
		{
			// get control points
			auto controls = getControlPoints(interpClone[threes[0]].p, interpClone[threes[1]].p, interpClone[threes[2]].p, tval);
			// (threes[0] * (1-t)^3) + (3*(1-t)^2 * t * controls.first) + (3*(1-t)*t^2 * controls.second) + (t^3 * threes[1])
			int segments = threes[1] - threes[0];
			if (segments - 1 > 0)
			{

				if (interpClone[threes[0]].p == interpClone[threes[1]].p && interpClone[threes[0]].p == interpClone[threes[2]].p)
				{
					// Can't call the control point function if the points are coincident
					for (int k = threes[0] + 1; k != threes[1]; k++)
					{
						interpClone[k].p.x = interpClone[threes[2]].p.x;
						interpClone[k].p.y = interpClone[threes[2]].p.y;
						interpClone[k].b = true;
					}
				}
				else
				{
					//fill them based off of the bezier curve given by the calculated control points
					for (int k = threes[0] + 1; k != threes[1]; k++)
					{
						double t = double(k - threes[0]) / double(segments);
						//(1 - t)3P0 + 3(1-t)2tP1 + 3(1-t)t2P2 + t3P3
						double pX = (1 - t)*(1 - t)*(1 - t) * interpClone[threes[0]].p.x +
							3 * (1 - t)*(1 - t)*t*controls.first.x +
							3 * (1 - t) * t * t * controls.second.x +
							t * t * t * interpClone[threes[1]].p.x;
						double pY = (1 - t)*(1 - t)*(1 - t) * interpClone[threes[0]].p.y +
							3 * (1 - t)*(1 - t)*t*controls.first.y +
							3 * (1 - t) * t * t * controls.second.y +
							t * t * t * interpClone[threes[1]].p.y;
						interpClone[k].p.x = pX;
						interpClone[k].p.y = pY;
						interpClone[k].b = true;
					}
				}
			}
			// prepare for next iteration
			threes[0] = threes[1];
			threes[1] = threes[2];
			while (ptsInd < interpClone.size() && !interpClone[ptsInd].b)
			{
				ptsInd++;
			}
			//ptsInd is now the next valid point index
			threes[2] = ptsInd;
			ptsInd++;
		}
		// set toInterpolate to the relavent values of the clone
		// toInterpolate will be larger, but it doesn't matter since we copy off of the other's size
		for (int j = 0; j < toInterpolate.size(); j++)
		{
			toInterpolate[j] = interpClone[j + prePoints];
		}
		return 0;
	}
	// Holds all the structs.
	std::vector<SetPoint> Storage;
	unsigned int pointspersecond;
};