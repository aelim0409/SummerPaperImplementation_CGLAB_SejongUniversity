#include <opencv2/opencv.hpp>

int cnt = 0;
int window_size = 5;
int **isValid;
float **q;
float **p;
int w, h;

float get_min(float a, float b, float c)
{
	return  (a < b && a < c) ? a :
		(b < a && b < c) ? b : c;
}

float getEnergy_vertical(IplImage *src,int i,int j)
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
			+ (a.val[1] )
			+ (a.val[2] ) / 3);
	}
}

void  seam_vertical(IplImage *src) //누적?에너지 계산
{
	float minE;
	for (int x = 0; x < w-1; x++)
		q[0][x] = getEnergy_vertical(src, x, 0);
	

	for(int y = 1; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			if (x - 1 < 0)
				minE = MIN(q[y - 1][x], q[y - 1][x + 1]);
			else if (x+1>w-1)
				minE = MIN(q[y - 1][x - 1], q[y - 1][x]);
			else
				minE= get_min(q[y - 1][x - 1], q[y - 1][x], q[y - 1][x + 1]);
			q[y][x] = minE + getEnergy_vertical(src, x, y);

			if (minE == q[y - 1][x - 1])
				p[y][x] = -1;
			else if(minE== q[y - 1][x])
				p[y][x] = 0;
			else
				p[y][x] = 1;
		}
}

void removeSeam_vertical(int x, int y, IplImage * src)
{
	for (int i = x; i < w-1; i++)
	{
		//if (x + 1 > w - 1)
			//continue;
		CvScalar after = cvGet2D(src, y, i + 1);
		cvSet2D(src, y, i, after);
	}
}

void SeamPath_vertical(int x, int y, IplImage *src)
{
	if (y == 0)
		removeSeam_vertical(x + p[y][x], y, src);
	else
	{
		removeSeam_vertical(x + p[y][x], y-1, src);
		SeamPath_vertical(x + p[y][x], y - 1, src);
	}
}

void minSeam_vertical(IplImage *src)//최소 seam 가장 마지막 값 구하기
{
	seam_vertical(src);
	int minIdx = 0;
	float min_totalE = FLT_MAX;

	for (int x = 0; x < w; x++)
	{
		if (q[h-1][x] < min_totalE)
		{
			minIdx = x;
			min_totalE = q[h-1][x];
		}
	}
	//printf("%d %d\n", minIdx, h - 1);
	SeamPath_vertical( minIdx,h-1,src);
}

//horizontal seam

float getEnergy_horizontal(IplImage *src, int i, int j)
{
	if (j + 1 <= h - 1)
	{
		CvScalar a = cvGet2D(src, j, i);
		CvScalar b = cvGet2D(src, j+1, i);
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
void  seam_horizontal(IplImage *src) //누적?에너지 계산
{
	float minE;
	for (int y = 0; y < h; y++)
		q[y][0] = getEnergy_horizontal(src, 0, y);


	for (int x = 1; x < w; x++)
		for (int y = 0; y < h; y++)
		{
			if (y - 1 < 0)
				//minE = MIN(q[y - 1][x], q[y - 1][x + 1]);
				minE = MIN(q[y][x - 1], q[y + 1][x - 1]);
			else if (y+1 > h - 1)
				//minE = MIN(q[y - 1][x - 1], q[y - 1][x]);
				minE = MIN(q[y - 1][x - 1], q[y][x - 1]);
			else
				//minE = get_min(q[y - 1][x - 1], q[y - 1][x], q[y - 1][x + 1]);
				minE = get_min(q[y - 1][x - 1], q[y][x - 1], q[y + 1][x - 1]);
			q[y][x] = minE + getEnergy_horizontal(src, x, y);

			if (y - 1 < 0)
			{
				if (minE == q[y +1][x - 1])
					p[y][x] = 1;

				else if (minE == q[y][x - 1])
					p[y][x] = 0;
			}
			else if (minE == q[y - 1][x - 1])
				p[y][x] = -1;
			
			else if (minE == q[y][x-1])
				p[y][x] = 0;
			else
				p[y][x] = 1;
		}
}

void removeSeam_horizontal(int x, int y, IplImage * src)
{
	for (int i = y; i < h - 1; i++)
	{
		//if (x + 1 > w - 1)
			//continue;
		CvScalar after = cvGet2D(src, i+1, x);
		cvSet2D(src, i, x, after);
	}
}

void SeamPath_horizontal(int x, int y, IplImage *src)
{
	if (x == 0)
		removeSeam_horizontal(x , y+ p[y][x], src);
	else
	{
		removeSeam_horizontal(x -1, y + p[y][x], src);
		SeamPath_horizontal(x -1, y + p[y][x], src);
	}
}

void minSeam_horizontal(IplImage *src)//최소 seam 가장 마지막 값 구하기
{
	seam_horizontal(src);
	int minIdx = 0;
	float min_totalE = FLT_MAX;

	for (int y = 0; y < h; y++)
	{
		if (q[y][w-1] < min_totalE)
		{
			minIdx = y;
			min_totalE = q[y][w - 1];
		}
	}
	//printf("%d %d\n", w-1, minIdx);
	SeamPath_horizontal(w-1, minIdx, src);
}

void SeamCarvingResult(IplImage* src, IplImage *dst)
{
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			CvScalar c = cvGet2D(src, y, x);

			cvSet2D(dst, y, x, c);

		}
	}
}

int main()
{
	//scanf("%d", &window_size);
	int v_size;
	scanf("%d", &v_size);
	IplImage *src = cvLoadImage("c:\\temp\\SeamCarvingSrc.jpg");
	w = src->width;
	h = src->height;
	
	q = (float **)malloc(sizeof(float*)*(h));


	for (int i = 0; i < h; i++) {
		q[i] = (float*)malloc(sizeof(float) * (w));
	}

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			q[y][x] = -1;

	p = (float **)malloc(sizeof(float*)*(h));


	for (int i = 0; i < h; i++) {
		p[i] = (float*)malloc(sizeof(float) * (w));
	}

	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			p[y][x] = 0;

	cvShowImage("src", src);

	/*
	if (cvWaitKey() == 'v')
	{
		for (int i = 0; i < 100; i++)
		{
			minSeam_vertical(src);
			w--;
		}
		IplImage *dst = cvCreateImage(cvSize(w, h), 8, 3);

		SeamCarvingResult(src,dst);
		cvShowImage("dst", dst);
		cvWaitKey();
	}
	*/
	
		if (cvWaitKey() == 'v')
		{

			for (int i = 0; i < v_size; i++)
			{
				minSeam_vertical(src);
				w--;
			}

			IplImage *dst = cvCreateImage(cvSize(w, h), 8, 3);

			SeamCarvingResult(src, dst);
			cvShowImage("dst", dst);
			cvWaitKey();
		}

		if (cvWaitKey() == 'h')
		{

			//scanf("%d", &v_size);

			for (int i = 0; i < v_size; i++)
			{
				minSeam_horizontal(src);
				h--;
			}

			IplImage *dst = cvCreateImage(cvSize(w, h), 8, 3);

			SeamCarvingResult(src, dst);
			cvShowImage("dst", dst);
			cvWaitKey();
		}
	
	return 0;
}