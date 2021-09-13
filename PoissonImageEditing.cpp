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
CvPoint st, ed,pos;
PixelMemory* pm;
IplImage* dst,*dstO;
CvScalar white = cvScalar(255, 255, 255);


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
		CvScalar f = cvGet2D(srcv,pm[i].y, pm[i].x);
		
		cvSet2D(dst,  pm[i].y+y,  pm[i].x+x, f);

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

CvScalar PoissonEditing1(int x,int y)
{
	//printf("dddddd\n");
	CvScalar f1 = cvGet2D(dstO, y, x - 1);
	CvScalar f2 = cvGet2D(dstO, y, x + 1);
	CvScalar f3 = cvGet2D(dstO, y - 1, x);
	CvScalar f4 = cvGet2D(dstO, y + 1, x);

	CvScalar v1 = cvGet2D(srcv, y-pos.y, x-pos.x - 1);
	CvScalar v2 = cvGet2D(srcv, y-pos.y, x-pos.x+ 1);
	CvScalar v3 = cvGet2D(srcv, y-pos.y - 1, x-pos.x);
	CvScalar v4 = cvGet2D(srcv, y-pos.y+ 1, x-pos.x);

	CvScalar F;
	F.val[0] = ((f1.val[0] + f2.val[0] + f3.val[0] + f4.val[0])+ (v1.val[0] + v2.val[0] + v3.val[0] + v4.val[0])) / 4;
	F.val[1] = ((f1.val[1] + f2.val[1] + f3.val[1] + f4.val[1])+(v1.val[1] + v2.val[1] + v3.val[1] + v4.val[1])) / 4;
	F.val[2] = ((f1.val[2] + f2.val[2] + f3.val[2] + f4.val[2])+(v1.val[2] + v2.val[2] + v3.val[2] + v4.val[2])) / 4;

	return F;
}

void poissonSolver()
{
	int W = dst->width;
	int H = dst->height;
	
	while (1)
	{
		for (int y = pos.y; y < pos.y + pm[pm_cnt - 1].y; y++)
		{
			//printf("xxxxxxx\n");
			for (int x = pos.x; x < W; x++)
			{

				if (x - pos.x < 0 || y - pos.y < 0)
					continue;
				//CvScalar m = cvGet2D(msk, y-pos.y,x-pos.x);
				if ( isCopy[y][x] == 0)
				{
					//printf("DDDDDD\n");

					if (x  < 0 || x + 1  > w - 1)
						continue;

					if (y < 0 || y + 1  > h - 1)
						continue;
					//printf("%d %d %d %d %d\n", y - pos.y, x - pos.x, isCopy[y][x], y, x);
					cvSet2D(dst, y, x, PoissonEditing1(x, y));
					//sumF += (getDist(F1, F2));
				//	sumF += F1.val[0];
				}
				//printf("ed\n");
			}
		}
		printf("end\n");
		cvShowImage("dst", dst);
		cvWaitKey(1);
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
		attatchSrc(pos.x,pos.y);
		cvShowImage("dst",dst);
		poissonSolver();
		
	}
}

int main()
{
	dst = cvLoadImage("c:\\temp\\sky.png");
	dstO = cvLoadImage("c:\\temp\\sky.png");
	src = cvLoadImage("c:\\temp\\PIEsource.jpg");
	srcv= cvLoadImage("c:\\temp\\PIEsource.jpg");

	w = src->width;
	h = src->height;
	msk = cvCreateImage(cvSize(w, h), 8, 3);
	printf("%d %d %d %d\n", w, h,dst->width,dst->height);
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