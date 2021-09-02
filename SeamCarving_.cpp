#include <opencv2/opencv.hpp>

typedef struct PixelMemory
{
	int x;
	int y;
};

float** q;
float** p;
int w, h;
int pm_cnt = 0;;
int notMin = 0;
IplImage* src;
CvPoint st, ed;
PixelMemory* pm;

bool changeEnergyMax(int x, int y)
{
	for (int i = 0; i < pm_cnt; i++)
	{
		if (x == pm[i].x && y == pm[i].y)
		{
			
			return true;
		}
		
	}
	//printf("false\n");
	return false;
}
void changeMemoryX()
{
	for (int i = 0; i < pm_cnt; i++)
	{
		if (pm[i].x > 0)
			pm[i].x = pm[i].x - 1;
	}
}
float get_min(float a, float b, float c)
{
	return  (a < b&& a < c) ? a :
		(b < a&& b < c) ? b : c;
}



float getEnergy_vertical(IplImage* src, int i, int j)
{
	if (changeEnergyMax(i, j) == false)
	{
		if (i + 1 <= w - 1)
		{
			CvScalar a = cvGet2D(src, j, i);
			CvScalar b = cvGet2D(src, j, i + 1);
			
			return abs(((a.val[0] - b.val[0])
				+ (a.val[1] - b.val[1])
				+ (a.val[2] - b.val[2])) / 3);
		}
		else
		{
			CvScalar a = cvGet2D(src, j, i);
			return abs((a.val[0])
				+ (a.val[1])
				+ (a.val[2]) / 3);
		}
	}
	else
	{
		//printf("TRUE\n");
		return 10000;
	}
}

void  seam_vertical(IplImage* src) //누적?에너지 계산
{
	float minE;
	for (int x = 0; x < w - 1; x++)
		q[0][x] = getEnergy_vertical(src, x, 0);


	for (int y = 1; y < h; y++)
		for (int x = 0; x < w; x++)
		{
				if (x - 1 < 0)
					minE = MIN(q[y - 1][x], q[y - 1][x + 1]);
				else if (x + 1 > w - 1)
					minE = MIN(q[y - 1][x - 1], q[y - 1][x]);
				else
					minE = get_min(q[y - 1][x - 1], q[y - 1][x], q[y - 1][x + 1]);
				
				q[y][x] = minE + getEnergy_vertical(src, x, y);
			//	if (q[y][x] >= 10000)
				//	printf("q[y][x] = %.f\n", q[y][x]);
				/*
				if (q[y][x] >= 10000)
				{
					//printf("q[y][x] : %.f\n", q[y][x]);
					//printf("M %d\n", x);
					if (x > w / 2)
					{
						p[y][x] = -40;
						//printf("p[y][x]>w/2: %d \n", p[y][x]);
					}
					else {
						p[y][x] = 40;
						//printf("p[y][x]<w/2: %d \n", p[y][x]);
					}
						
				}
				*/
				//else
				//{
					if (minE == q[y - 1][x - 1])
						p[y][x] = -1;
					else if (minE == q[y - 1][x])
						p[y][x] = 0;
					else
						p[y][x] = 1;
				//}
			
		}
}

void removeSeam_vertical(int x, int y, IplImage* src)
{
	for (int i = x; i < w - 1; i++)
	{
		CvScalar after = cvGet2D(src, y, i + 1);
		cvSet2D(src, y, i, after);
	}
	cvSet2D(src, y + 1, w - 1, cvScalar(0, 0, 0));

}

void WhereSeam(int x, int y, IplImage* src)//seam 파랑색으로 표시
{
	cvSet2D(src, y, x, cvScalar(255, 0, 0));
	//printf("WS:%d %d\n", x, y);
	//printf("0");
}

void SeamPath_vertical1(int x, int y, IplImage* src)//seam 표시
{
	if (y == 0)
	{
		WhereSeam(x + p[y][x], y, src);
		cvShowImage("src", src);

	}

	else
	{
	//	printf("x+p = %d\n", p[y][x]);
		WhereSeam(x + p[y][x], y - 1, src);
		
		SeamPath_vertical1(x + p[y][x], y - 1, src);

	}

}

void SeamPath_vertical2(int x, int y, IplImage* src)
{
	if (y == 0)
	{
		removeSeam_vertical(x + p[y][x], y, src);
	}

	else
	{
		//WhereSeam(x + p[y][x], y - 1, src);
		removeSeam_vertical(x + p[y][x], y - 1, src);

		SeamPath_vertical2(x + p[y][x], y - 1, src);
	}
}

bool SeamPath_vertical3(int x, int y)
{
	int X = x + p[y][x];
	if (y == 0)
	{
		return true;
	}
	else
	{
		
		if (q[y][X] > 10000)
			return false;
		SeamPath_vertical3(X, y - 1);
	}
	
}

void minSeam_vertical(IplImage* src)//최소 seam 가장 마지막 값 구하기
{

	seam_vertical(src);
	
	int minIdx = 0;
	float min_totalE = FLT_MAX;

	for (int x = 0; x < w; x++)
	{
		//printf("minTotal = %.f ,%d때 에너지: %.f\n",min_totalE,x, q[h - 1][x]);
		
		if (q[h - 1][x] <= min_totalE && x!=notMin)
		{
			minIdx = x;
			min_totalE = q[h - 1][x];
		}
	}
	printf("minIDx: %d\n", minIdx);
	if (SeamPath_vertical3(minIdx, h - 1) == false)
	{
		printf("false");
		notMin = minIdx;
		minSeam_vertical(src);
	}
	else
	{
		SeamPath_vertical1(minIdx, h - 1, src);
		cvWaitKey(100);
		SeamPath_vertical2(minIdx, h - 1, src);
	}
}

//horizontal seam

float getEnergy_horizontal(IplImage* src, int i, int j)
{
	if (j + 1 <= h - 1)
	{
		CvScalar a = cvGet2D(src, j, i);
		CvScalar b = cvGet2D(src, j + 1, i);
		return abs(((a.val[0] - b.val[0])
			+ (a.val[1] - b.val[1])
			+ (a.val[2] - b.val[2])) / 3);
	}
	else
	{
		CvScalar a = cvGet2D(src, j, i);
		return abs((a.val[0])
			+ (a.val[1])
			+ (a.val[2]) / 3);
	}
}
void  seam_horizontal(IplImage* src) //누적?에너지 계산
{
	float minE;


	for (int y = 0; y < h; y++)
		q[y][0] = getEnergy_horizontal(src, 0, y);


	for (int x = 1; x < w; x++)
		for (int y = 0; y < h; y++)
		{
			if (y - 1 < 0)
				minE = MIN(q[y][x - 1], q[y + 1][x - 1]);
			else if (y + 1 > h - 1)
				minE = MIN(q[y - 1][x - 1], q[y][x - 1]);
			else
				minE = get_min(q[y - 1][x - 1], q[y][x - 1], q[y + 1][x - 1]);
			q[y][x] = minE + getEnergy_horizontal(src, x, y);

			if (y - 1 < 0)
			{
				if (minE == q[y + 1][x - 1])
					p[y][x] = 1;

				else if (minE == q[y][x - 1])
					p[y][x] = 0;
			}
			else if (minE == q[y - 1][x - 1])
				p[y][x] = -1;

			else if (minE == q[y][x - 1])
				p[y][x] = 0;
			else
				p[y][x] = 1;
		}
}

void removeSeam_horizontal(int x, int y, IplImage* src)
{
	for (int i = y; i < h - 1; i++)
	{
		CvScalar after = cvGet2D(src, i + 1, x);
		cvSet2D(src, i, x, after);
	}
	cvSet2D(src, h - 1, x + 1, cvScalar(0, 0, 0));
}

void SeamPath_horizontal2(int x, int y, IplImage* src)
{
	if (x == 0)
		removeSeam_horizontal(x, y + p[y][x], src);
	else
	{
		removeSeam_horizontal(x - 1, y + p[y][x], src);
		SeamPath_horizontal2(x - 1, y + p[y][x], src);
	}
}


void SeamPath_horizontal1(int x, int y, IplImage* src)
{
	if (x == 0)
	{
		WhereSeam(x, y + p[y][x], src);
		cvShowImage("src", src);
	}
	else
	{
		WhereSeam(x - 1, y + p[y][x], src);
		SeamPath_horizontal1(x - 1, y + p[y][x], src);
	}
}

void minSeam_horizontal(IplImage* src)//최소 seam 가장 마지막 값 구하기
{


	seam_horizontal(src);
	int minIdx = 0;
	float min_totalE = FLT_MAX;

	for (int y = 0; y < h; y++)
	{
		printf("%d때 에너지: %d\n", y, q[y][w - 1]);
		if (q[y][w - 1] < min_totalE)
		{
			minIdx = y;
			min_totalE = q[y][w - 1];
		}
	}
	//printf("%d %d\n", w-1, minIdx);
	SeamPath_horizontal1(w - 1, minIdx, src);
	cvWaitKey(100);
	SeamPath_horizontal2(w - 1, minIdx, src);
}

void myMouse(int event, int x, int y, int flags, void* param)
{

	if (event == CV_EVENT_LBUTTONDOWN)
	{
		st = cvPoint(x, y);
		
		pm[pm_cnt].x = st.x;
		pm[pm_cnt].y = st.y;
		pm_cnt++;
		printf("mouse %d %d\n", st.x, st.y);
		//q[st.y][st.x] = 10000;

	}
	if (event == CV_EVENT_MOUSEMOVE && flags == CV_EVENT_FLAG_LBUTTON)
	{
		ed = cvPoint(x, y);
		CvScalar color = cvScalar(255, 255, 0);
		cvLine(src, st, ed, color, 1);
		cvShowImage("src", src);
		
		pm[pm_cnt].x = st.x;
		pm[pm_cnt].y = st.y;
		pm_cnt++;
		
		//q[ed.y][ed.x] = 10000;
		st = ed;
		//printf("mouse %d %d\n", st.x, st.y);
		//cvSet2D(src, y, x, color);
	}
}

int main()
{
	src = cvLoadImage("c:\\temp\\dolphin.jpg");

	cvShowImage("src", src);
	w = src->width;
	h = src->height;
	IplImage* dst = cvCreateImage(cvSize(w, h), 8, 3);

	pm = (PixelMemory*)malloc(sizeof(PixelMemory*) * (w*h));
	//printf("%d\n", w / 2);

	while (1)
	{
		q = (float**)malloc(sizeof(float*) * (h));

		for (int i = 0; i < h; i++) {
			q[i] = (float*)malloc(sizeof(float) * (w));
		}

		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
				q[y][x] = -1;

		p = (float**)malloc(sizeof(float*) * (h));


		for (int i = 0; i < h; i++) {
			p[i] = (float*)malloc(sizeof(float) * (w));
		}

		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
				p[y][x] = 0;

		cvSetMouseCallback("src", myMouse);

		if (cvWaitKey() == 'v')
		{
			minSeam_vertical(src);
			changeMemoryX();
			w--;
		}

		if (cvWaitKey() == 'h')
		{
			minSeam_horizontal(src);
			h--;
		}

		cvShowImage("src", src);

		delete[] p;
		delete[] q;
	}


	return 0;
}