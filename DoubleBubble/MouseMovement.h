#ifndef MOUSEMOVEMENT_H
#define MOUSEMOVEMENT_H
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <time.h>
#include <wtypes.h>
#include <vector>
#include <math.h>
#include <cassert>
#include <limits>
using namespace std;
ofstream outfile("result.txt");
#define NUMPOINTS 10000
#define DISTLENGTH 500
double genNormalDist(double width, double center)
{
	// Generates a random number around a given center and a distribution max length
	double randSum=0;
	for(int k=0; k<100; k++)
	{
		randSum+= rand()%100;
	}
	randSum= randSum/(double)100.0; //now have a normally distrubuted number between 0 and 100,000
	randSum= randSum/(double)100.0; // now its between 0-1
	randSum *=width;
	randSum -= (width/2);
	randSum += center;
	return randSum;
}
class vec2
{
public:
	vec2(double first=0, double second=0)
	{
		x=first;
		y=second;
	}
	double x;
	double y;
};
class mat2
{
public:
	mat2(double x0y0=1,double x1y0=0, double x0y1=0, double x1y1=1)
	{
		data[0][0]=x0y0;
		data[0][1]=x1y0;
		data[1][0]=x0y1;
		data[1][1]=x1y1;
	}
	double data[2][2];
};
inline mat2 loadIdentity()
{
	return mat2(1,0,0,1);
}
inline mat2 mat2mult(mat2 A, mat2 B)
{// A*B
	return mat2(A.data[0][0]*B.data[0][0]+A.data[0][1]*B.data[1][0],
				A.data[0][0]*B.data[0][1]+A.data[0][1]*B.data[1][1],
				A.data[1][0]*B.data[0][0]+A.data[1][1]*B.data[1][0],
				A.data[1][0]*B.data[0][1]+A.data[1][1]*B.data[1][1]);
}
inline vec2 vecmatmult(mat2 A, vec2 B)
{ // A[] * B[]
	return vec2((A.data[0][0])*B.x + (A.data[0][1])*B.y, (A.data[1][0])*B.x + (A.data[1][1])*B.y);
}
struct MMCoord
{
	MMCoord(double xx=-1, double yy=-1)
	{
		x=xx;
		y=yy;
	}
	double x;
	double y;
};
MMCoord DinoNugget[10000];
MMCoord generateDest(MMCoord center, double radius)
{
	// The goal of this function is to give our move target a valid location
	// that follows a normal distribution. radius=max radius
	double randRadius=genNormalDist(radius*(double)2.0,0);
	if(randRadius==0)
	{
		return center;
	}
	double theta= rand();
	double retX= sin(theta) * randRadius + center.x;
	double retY= cos(theta) * randRadius + center.y;
	return MMCoord(retX, retY);
}
inline MMCoord MMCoordmatmult(mat2 A, MMCoord B)
{ // A[] * B[]
	return MMCoord((A.data[0][0])*B.x + (A.data[0][1])*B.y, (A.data[1][0])*B.x + (A.data[1][1])*B.y);
}
inline mat2 matrot(double theta)
{
	return mat2(cos(theta), -sin(theta),
				sin(theta), cos(theta));
}
inline mat2 matscale(double amt_x, double amt_y)
{
	return mat2(amt_x,0,0,amt_y);
}
inline double dif_frac_seconds(SYSTEMTIME A, SYSTEMTIME B)
{
	double ret= (A.wHour-B.wHour)*3600 + (A.wMinute-B.wMinute)*60 + A.wSecond-B.wSecond + (double)(A.wMilliseconds-B.wMilliseconds)/1000;
	return ret;
}
void smoothmove(MMCoord start, MMCoord end, double time_to_move)
{
	if(start.x==end.x && start.y==end.y)
	{
		return;
	}
	SYSTEMTIME start_time;
    GetSystemTime(&start_time);
	MMCoord parametricvector(end.x-start.x,end.y-start.y);
	SYSTEMTIME now_time;
	double time_percent=0;
	while(time_percent<1)
	{
		GetSystemTime(&now_time);
		time_percent=(dif_frac_seconds(now_time, start_time))/time_to_move;
	//	cout<< dif_frac_seconds(now_time, start_time)<<endl;
		MMCoord MouseDest(start.x + time_percent*(parametricvector.x),start.y + time_percent*(parametricvector.y));
		
		SetCursorPos(start.x + time_percent*(parametricvector.x),start.y + time_percent*(parametricvector.y));
	}
	

}
void moveAround()
{
	while(true)
	{
		string c1;
		//cin>> c1;
		string c2;
	//	cin>>c2;
		string c3;
		cin>>c3;
		if(c3[0]=='Q')
		{
			return;
		}
		string c4;
		cin>>c4;
		if(c1.length()<1)
		{
			c1+="0";
		}
		if(c2.length()<1)
		{
			c2+="0";
		}
		if(c3.length()<1)
		{
			c3+="0";
		}

		if(c4.length()<1)
		{
			c4+="0";
		}
		MMCoord A;
		MMCoord B;
		/*A.x=stoi(c1);
		A.y=stoi(c2);*/
		B.x=stoi(c3);
		B.y=stoi(c4);
		smoothmove(A,B,1);
	}
	
}
void clickatcurmousepos()
{
	POINT * curserpos= new POINT();
	GetCursorPos(curserpos);
   INPUT input;
    input.type=INPUT_MOUSE;
	input.mi.dx=curserpos->x;
    input.mi.dy=curserpos->y;
    input.mi.dwFlags=(MOUSEEVENTF_ABSOLUTE|/*MOUSEEVENTF_MOVE|*/MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP);
    input.mi.mouseData=0;
    input.mi.dwExtraInfo=NULL;
    input.mi.time=0;
    SendInput(1,&input,sizeof(INPUT));
}
void KeyInput(int key=0x75)
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
 
    // Press the key
    ip.ki.wVk = key; // virtual-key code for the f6 key -> replace later
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
 
    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));

}
void simpleStationHop()
{
	string cont;
	POINT * curserposinit= new POINT();
	GetCursorPos(curserposinit);
	MMCoord start(curserposinit->x,curserposinit->y);
	smoothmove(start, MMCoord(450,380),1);
	
	clickatcurmousepos();
	//cin>>cont;
	smoothmove(MMCoord(450,380), MMCoord(500,340),1);
	
	clickatcurmousepos();
	//cin>>cont;
	smoothmove(MMCoord(500,340), MMCoord(620, 310 ),1);
	
	clickatcurmousepos();
	//cin>>cont;
	smoothmove(MMCoord(620, 310 ), MMCoord(640, 420),1);
	
	clickatcurmousepos();
	//cin>>cont;
 }
void PlayMouseMovement(double time_to_move)
{
	cout<<"Playing back"<<endl;
	SYSTEMTIME start_time;
    GetSystemTime(&start_time);
	SYSTEMTIME now_time;
	double elapsedTime=0;
	int bucket;
	while(elapsedTime<1)
	{
		GetSystemTime(&now_time);
		elapsedTime=(dif_frac_seconds(now_time, start_time))/time_to_move;
		//select the proper bucket for smooth movement
		bucket=(double)NUMPOINTS*elapsedTime;
		if(bucket>=NUMPOINTS)
		{
			bucket=NUMPOINTS-1;
		}
		SetCursorPos(DinoNugget[bucket].x, DinoNugget[bucket].y);
	}
	cout<<"done"<<endl;

}
void MMtoOrigin(MMCoord * MM)
{
	//Translates to origin-> rotates theta to zero -> scales to 500 total pixels in X-axis

	// first translate all points to origin
	MMCoord first(MM[0].x,MM[0].y);
	for(int cnt=0; cnt<NUMPOINTS; cnt++)
	{
		MM[cnt].x -= first.x;
		MM[cnt].y -= first.y;
	}
	//find the angle between the ending points and set it equal to zero.
	double theta=atan2((MM[NUMPOINTS-1].y),(MM[NUMPOINTS-1].x));
	for(int cnt=0; cnt<NUMPOINTS; cnt++)
	{
		MM[cnt]= MMCoordmatmult( matrot(-theta), MM[cnt]);
	}
	//finally, scale the length to a predetermined amount
	double scaleFactor= (double)DISTLENGTH/(MM[NUMPOINTS-1].x);
	for(int cnt=0; cnt<NUMPOINTS; cnt++)
	{
		MM[cnt]= MMCoordmatmult( matscale(scaleFactor,scaleFactor), MM[cnt]);
	}
}
void PlayHumanMouse(MMCoord begin, MMCoord end, double time_to_move, MMCoord * MM)
{
	//First scale -> rotate -> translate such that you cover the wanted path
	//Scale:
	MMCoord dirVec(end.x-begin.x, end.y-begin.y);
	double displacement= sqrt((dirVec.x) * (dirVec.x) + (dirVec.y) * (dirVec.y));
	double scaleFactor= displacement/ (double) DISTLENGTH;
	for(int cnt=0; cnt<NUMPOINTS; cnt++)
	{
		MM[cnt]= MMCoordmatmult( matscale(scaleFactor,scaleFactor), MM[cnt]);
	}
	//PlayMouseMovement(time_to_move);
	//Rotate:
	double theta= atan2( dirVec.y, dirVec.x);
	for(int cnt=0; cnt<NUMPOINTS; cnt++)
	{
		MM[cnt]= MMCoordmatmult( matrot(theta), MM[cnt]);
	}
	//PlayMouseMovement(time_to_move);
	// Translate:
	for(int cnt=0; cnt<NUMPOINTS; cnt++)
	{
		MM[cnt].x += begin.x;
		MM[cnt].y += begin.y;
	}
	//PlayMouseMovement(time_to_move);
	/////////////////////////////////////////////////////////////////////////


	SYSTEMTIME start_time;
    GetSystemTime(&start_time);
	SYSTEMTIME now_time;
	double elapsedTime=0;
	int bucket;
	double largenum;
	while(elapsedTime<1)
	{
		GetSystemTime(&now_time);
		elapsedTime=(dif_frac_seconds(now_time, start_time))/time_to_move;
		//select the proper bucket for smooth movement
		
		largenum=(double)NUMPOINTS*elapsedTime;
		if(largenum > INT_MAX)
		{
			bucket=NUMPOINTS-1;
		}
		else
		{
			bucket=largenum;
		}
		
		
		if(bucket>=NUMPOINTS)
		{
			bucket=NUMPOINTS-1;
		}
		SetCursorPos(DinoNugget[bucket].x, DinoNugget[bucket].y);
	}
	MMtoOrigin(DinoNugget);
	/*assert(MM[NUMPOINTS-1].x==end.x && MM[NUMPOINTS-1].y==end.y);
	assert(MM[0].x==begin.x && MM[0].y==begin.y);*/
}
void HMM_StationHop()
{
	/*
	# This should allow regrouping on bad first boards: The order of events is as follows:
	# Dismiss->ConfirmDismiss->restation->confirmStation
	#
	*/
	POINT * curserposinit= new POINT();
	GetCursorPos(curserposinit);
	MMCoord start(curserposinit->x,curserposinit->y);
	//Dismiss button:	
	//633,312: r=7
	MMCoord Dest=generateDest(MMCoord(633,312), 7);
	PlayHumanMouse(start, Dest,genNormalDist(1.0,3.0), DinoNugget);
	Sleep(genNormalDist(300,500));
	clickatcurmousepos();
	Sleep(genNormalDist(300,500));

	//Confirm Dismiss:
	//649,417 r=7
	GetCursorPos(curserposinit);
	start.x=curserposinit->x;
	start.y=curserposinit->y;
	Dest=generateDest(MMCoord(649,417),7);
	PlayHumanMouse(start, Dest,genNormalDist(1.0,3.0), DinoNugget);
	Sleep(genNormalDist(300,500));
	clickatcurmousepos();
	Sleep(genNormalDist(500,700));

	//Select station:
	//457,377: r=16
	GetCursorPos(curserposinit);
	start.x=curserposinit->x;
	start.y=curserposinit->y;
	Dest=generateDest(MMCoord(457,377),12);
	PlayHumanMouse(start, Dest,genNormalDist(1.5,3.4), DinoNugget);
	Sleep(genNormalDist(500,700));
	clickatcurmousepos();
	Sleep(genNormalDist(80,160));
	clickatcurmousepos();
	Sleep(genNormalDist(80,160));
	clickatcurmousepos();
	Sleep(genNormalDist(300,500));

	//Confirm station:
	//505,332: r=21
	/*GetCursorPos(curserposinit);
	start.x=curserposinit->x;
	start.y=curserposinit->y;
	Dest=generateDest(MMCoord(505,332),21);
	PlayHumanMouse(start, Dest,genNormalDist(1.4,3.4), DinoNugget);
	Sleep(genNormalDist(300,500));
	clickatcurmousepos();
	Sleep(genNormalDist(300,500));*/
}
void cleanMM(MMCoord * MM)
{
	//Here we want to take our array of mouse positions and interpolate between measurements
	MMCoord Previous(0,0);
	MMCoord Current(0,0);
	int firstUnfilled=0;
	int lastUnfilled=0;
	int totUnfilled=0;
	if(MM[0].x <0 && MM[0].y <0 )
	{
		for(int k=1; k<NUMPOINTS-1; k++)
		{
			if(MM[k].x >=0 && MM[k].y >=0)
			{
				MM[0]=MM[k];
				cleanMM(MM);
				return;
			}
		}
	}
	for(int cnt=0; cnt<NUMPOINTS-1; cnt++)
	{
		if(MM[cnt].x<0 && MM[cnt].y<0)
		{
			firstUnfilled=cnt;
			lastUnfilled=cnt;
			Previous.x=MM[cnt-1].x;
			Previous.y=MM[cnt-1].y;
			for(cnt=cnt+1;MM[cnt].x<0 && MM[cnt].y<0; cnt++)
			{
				lastUnfilled++;
			}
			if(cnt >= NUMPOINTS-1)
			{
				break;
			}
			//Now we have Previous, first unfilled, last unfilled
			Current.x=MM[cnt].x;
			Current.y=MM[cnt].y;

			//now loop through unfilled buckets, interpolating the values
			double divider=lastUnfilled-firstUnfilled+2; // if we have 3,5, we need to divide the gap into 4 spaces
			MMCoord dirVec=MMCoord(Current.x-Previous.x,Current.y-Previous.y);
			for(int p=0; p<divider-1; p++)
			{
				MM[p+firstUnfilled].x = dirVec.x*((p+1)/divider) + Previous.x;
				MM[p+firstUnfilled].y = dirVec.y*((p+1)/divider) + Previous.y;

			}
			totUnfilled+=lastUnfilled-firstUnfilled+1;

		}
	}
	//if the last value is garbage, we need to put a value there
	// and loop back through the array in order to fill things.
	if(MM[NUMPOINTS-1].x<0 && MM[NUMPOINTS-1].y<0)
	{
		MM[NUMPOINTS-1].x=Previous.x;
		MM[NUMPOINTS-1].y=Previous.y;
		cleanMM(MM);
	}
	cout<<"interpolated into "<<totUnfilled<<" buckets"<<endl;
}
void recordMouseMovement(double time_to_move)
{
	// This algorithm works by filling a large array of MMCoords
	// according to the time elapsed, then filling the the gaps by 
	// linear interpolation
	cout<<"RECORDING"<<endl;
	POINT Q;
	POINT * curserposinit= &Q;
	SYSTEMTIME start_time;
    GetSystemTime(&start_time);
	SYSTEMTIME now_time;
	double elapsedTime=0;
	while(elapsedTime<1)
	{
		GetSystemTime(&now_time);
		elapsedTime=(dif_frac_seconds(now_time, start_time))/time_to_move;
		GetCursorPos(curserposinit);
		int bucket=(double)NUMPOINTS*elapsedTime;
		DinoNugget[bucket]=MMCoord(curserposinit->x,curserposinit->y);
	}
	cout<<"Finished initial recording... now cleaning"<<endl;
	cleanMM(DinoNugget);
	cout<<"Finished Cleaning"<<endl;
}
void PrintMouseLocations()
{
	POINT Q;
	POINT * curserposinit= &Q;
	MMCoord M;
	while(true)
	{
		GetCursorPos(curserposinit);
		MMCoord P(curserposinit->x,curserposinit->y);
		//if(M.x!=P.x && M.y!=P.y)
	//	{
			cout<<"x="<<curserposinit->x<<" y="<<curserposinit->y<<endl;
	//	}
		M.x=P.x;
		M.y=P.y;
	}
}
double generateTimingfromDistance(MMCoord start, MMCoord dest)
{
	double randSum=0;
	for(int k=0; k<1000; k++)
	{
		randSum+= rand()%100000;
	}
	randSum= randSum/(double)1000; //now have a normally distrubuted number between 0 and 100,000
	randSum= randSum/(double)100000; // now its between 0-1
	MMCoord dirVec(dest.x-start.x,dest.y-start.y);
	double distanceToCover= sqrt( (dirVec.x )*( dirVec.x ) + (dirVec.y)*(dirVec.y)); //this calculates the distance
	double base_speed=3;
	if(distanceToCover<100)
	{
		randSum= randSum*(base_speed* (double) 0.90) + 2.0;
	}
	randSum= randSum*(base_speed* (double) 1.0) + 2.0;
	randSum = randSum * ( distanceToCover /(double) 500.0  );
	return randSum;
}
int testfunc()
{

	string start;
	cin>>start;
//	PrintMouseLocations();
	recordMouseMovement(2);
//	cin>>start;
	MMtoOrigin(DinoNugget);
	PlayMouseMovement(2);
	cin>>start;
	HMM_StationHop();
	return 0;
	//simpleStationHop();
	//moveAround();
}
/*
Notes:
Select station:
457,377: r=16
Confirm station:
505,332: r=21
Dismiss button:
633,312: r=7
Confirm Dismiss:
649,417 r=7
*/


#endif