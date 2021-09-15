#include <opencv2/opencv.hpp>

typedef struct PixelMemory
{
	int x;
	int y;
};

int w, h;
int** isCopy;
int pm_cnt = 0;
int maxX, minY;
IplImage* src;
IplImage* srcv;
IplImage* msk;
CvPoint st, ed, pos;
PixelMemory* pm;
IplImage* dst, *dstO;
CvScalar white = cvScalar(255, 255, 255);
CvScalar **X, **VX;

void makePixelmemory()
{
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			CvScalar f = cvGet2D(src, y, x);
			if (f.val[0] == 255 && f.val[1] == 255 && f.val[2] == 0)
			{
				pm[pm_cnt].y = y;
				pm[pm_cnt].x = x;
				pm_cnt++;
			}
		}

}
void makeMask()
{
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			CvScalar f = cvGet2D(src, y, x);
			if (f.val[0] == 255 && f.val[1] == 255 && f.val[2] == 0)
			{
				cvSet2D(msk, y, x, cvScalar(255, 255, 255));
			}
			else
				cvSet2D(msk, y, x, cvScalar(0, 0, 0));
		}
}
void attatchSrc(int x, int y)
{

	for (int i = 0; i < pm_cnt; i++)
	{
		CvScalar f = cvGet2D(srcv, pm[i].y, pm[i].x);

		cvSet2D(dst, pm[i].y + y, pm[i].x + x, f);

		//	cvShowImage("dst", dst);
			//cvWaitKey(1);
		isCopy[pm[i].y + y][pm[i].x + x] = 0;

	}


}

bool isWhite(CvScalar f)
{
	if (f.val[0] == white.val[0] && f.val[1] == white.val[1] && f.val[2] == white.val[2])
		return true;
	return false;
}


void initColor()
{
	X = (CvScalar**)malloc(sizeof(CvScalar*)*dst->height);
	for (int i = 0; i < dst->height; i++)
		X[i] = (CvScalar*)malloc(sizeof(CvScalar)*dst->width);

	VX = (CvScalar**)malloc(sizeof(CvScalar*)*h);
	for (int i = 0; i < h; i++)
		VX[i] = (CvScalar*)malloc(sizeof(CvScalar)*w);

	for (int y = 0; y < dst->height; y++)
	{
		for (int x = 0; x < dst->width; x++)
		{
			CvScalar f = cvGet2D(dstO, y, x);
			//printf("%f \n", f.val[0]);
			X[y][x] = f;
		}
	}

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			CvScalar f = cvGet2D(srcv, y, x);
			VX[y][x] = f;
		}
	}
	/*
	for (int i = 0; i < pm_cnt; i++)
	{
		CvScalar f = cvGet2D(srcv, pm[i].y, pm[i].x);
		VX[pm[i].y][pm[i].x] = f;
	}
	*/
}

CvScalar getColor(int x, int y)
{
	CvScalar F;

	CvScalar now = X[y][x];
	CvScalar f1 =X[y][x-1];
	CvScalar f2 =X[y][x+1];
	CvScalar f3 = X[y-1][x];
	CvScalar f4 = X[y+1][x];

	//printf("pm[0].y : %d   y-pos.y = %d\n", pm[0].y, y - pos.y);
	CvScalar nowV = VX[y - pos.y][x - pos.x];
	CvScalar v1 = VX[y - pos.y][x - pos.x-1];
	CvScalar v2 = VX[y - pos.y][x - pos.x + 1];
	CvScalar v3 = VX[y - pos.y-1][x - pos.x];
	CvScalar v4 = VX[y - pos.y+1][x - pos.x];

	CvScalar v1_1 = cvGet2D(msk, y - pos.y, x - pos.x - 1);
	CvScalar v2_1 = cvGet2D(msk, y - pos.y, x - pos.x + 1);
	CvScalar v3_1 = cvGet2D(msk, y - pos.y - 1, x - pos.x);
	CvScalar v4_1 = cvGet2D(msk, y - pos.y + 1, x - pos.x);

	if (!isWhite(v1_1) || !isWhite(v2_1) || !isWhite(v3_1) || !isWhite(v4_1))
	{
		for (int i = 0; i < 3; i++)
		{
			F.val[i] = (f1.val[i] + f2.val[i] + f3.val[i] + f4.val[i]) / 4;
		}
	}

	else {	

		for (int i = 0; i < 3; i++)
		{
			F.val[i] = ((f1.val[i] + f2.val[i] + f3.val[i] + f4.val[i]) + (v1.val[i] + v2.val[i] + v3.val[i] + v4.val[i])) / 4;
			//F.val[i] = ( (v1.val[i] + v2.val[i] + v3.val[i] + v4.val[i])) / 4;
		}
	}
	
	//X[y][x] = F;
	return F;
}

void poissonEditing()
{
	int W = dst->width;
	int H = dst->height;

	while (1)
	{
		for (int y = pos.y; y < H; y++)
		{
			for (int x = pos.x; x < W; x++)
			{
				if (isCopy[y][x] == 0)
				{
					cvSet2D(dst, y, x, getColor(x, y));
				}
			}
			cvShowImage("dst", dst);
			cvWaitKey(1);
		}
		
		
	}
}


void myMouse(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
		st = cvPoint(x, y);

	if (event == CV_EVENT_MOUSEMOVE && flags == CV_EVENT_FLAG_LBUTTON)
	{
		ed = cvPoint(x, y);
		CvScalar color = cvScalar(255, 255, 0);
		cvLine(src, st, ed, color, 20);
		cvShowImage("src", src);
		st = ed;
	}

	if (event == CV_EVENT_LBUTTONUP)
	{
		makePixelmemory();
		initColor();
		makeMask();
		cvShowImage("src", src);
		cvShowImage("msk", msk);
	}
}

void myMouse2(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
		pos = cvPoint(x, y);

	if (event == CV_EVENT_LBUTTONUP)
	{
		attatchSrc(pos.x, pos.y);
		cvShowImage("dst", dst);
		
		poissonEditing();

	}
}

int main()
{
	dst = cvLoadImage("c:\\temp\\sky.png");
	dstO = cvLoadImage("c:\\temp\\sky.png");
	src = cvLoadImage("c:\\temp\\PIEsource.jpg");
	srcv = cvLoadImage("c:\\temp\\PIEsource.jpg");

	w = src->width;
	h = src->height;
	msk = cvCreateImage(cvSize(w, h), 8, 3);
	printf("%d %d %d %d\n", w, h, dst->width, dst->height);
	cvShowImage("src", src);
	cvShowImage("dst", dst);
	pm = (PixelMemory*)malloc(sizeof(PixelMemory) * (w * h));

	isCopy = (int**)malloc(sizeof(int*) * (dst->height));

	for (int i = 0; i < dst->height; i++) {
		isCopy[i] = (int*)malloc(sizeof(int) * (dst->width));
	}

	for (int y = 0; y < dst->height; y++)
		for (int x = 0; x < dst->width; x++)
			isCopy[y][x] = -1;



	cvSetMouseCallback("src", myMouse);
	cvSetMouseCallback("dst", myMouse2);


	cvWaitKey();

	return 0;
}