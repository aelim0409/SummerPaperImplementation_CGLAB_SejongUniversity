#include <opencv2/opencv.hpp>

float** q;
float** p;
int w, h;

float get_min(float a, float b, float c)
{
	return  (a < b&& a < c) ? a :
		(b < a&& b < c) ? b : c;
}

float getEnergy_vertical(IplImage* src, int i, int j)
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

			if (minE == q[y - 1][x - 1])
				p[y][x] = -1;
			else if (minE == q[y - 1][x])
				p[y][x] = 0;
			else
				p[y][x] = 1;
		}
}

void removeSeam_vertical(int x, int y, IplImage* src)
{
	for (int i = x; i < w-1; i++)
	{
		CvScalar after = cvGet2D(src, y, i + 1);
		cvSet2D(src, y, i, after);
	}
	cvSet2D(src, y+1, w-1, cvScalar(0, 0, 0));

}

void WhereSeam(int x, int y, IplImage* src)//seam 파랑색으로 표시
{
	cvSet2D(src, y, x, cvScalar(255, 0, 0));
	printf("0");
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

void minSeam_vertical(IplImage* src)//최소 seam 가장 마지막 값 구하기
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

	seam_vertical(src);
	int minIdx = 0;
	float min_totalE = FLT_MAX;

	for (int x = 0; x < w; x++)
	{
		if (q[h - 1][x] < min_totalE)
		{
			minIdx = x;
			min_totalE = q[h - 1][x];
		}
	}

	SeamPath_vertical1(minIdx, h-1 , src);
	//cvWaitKey(1000);
	SeamPath_vertical2(minIdx, h - 1, src);
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
	cvSet2D(src, h-1, x+1, cvScalar(0,0,0));
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

	seam_horizontal(src);
	int minIdx = 0;
	float min_totalE = FLT_MAX;

	for (int y = 0; y < h; y++)
	{
		if (q[y][w - 1] < min_totalE)
		{
			minIdx = y;
			min_totalE = q[y][w - 1];
		}
	}
	//printf("%d %d\n", w-1, minIdx);
	SeamPath_horizontal1(w - 1, minIdx, src);
	cvWaitKey(1000);
	SeamPath_horizontal2(w - 1, minIdx, src);
}
int main()
{
	IplImage* src = cvLoadImage("c:\\temp\\sample.jpg");
	
	cvShowImage("src", src);
	w = src->width;
	h = src->height;
	IplImage* dst = cvCreateImage(cvSize(w,h),8,3);
	
	while (1)
	{
		if (cvWaitKey() == 'v')
		{
			minSeam_vertical(src);
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