#include <opencv2/opencv.hpp>
#pragma warning(disable:4996)

IplImage *src ;
IplImage *msk ;
IplImage *dst;
int w, h;
CvScalar white = cvScalar(255, 255, 255);
CvScalar **colorMemory;

float  getDist(CvScalar a, CvScalar b)
{
	return sqrt((a.val[0] - b.val[0]) * (a.val[0] - b.val[0])
		+ (a.val[1] - b.val[1]) * (a.val[1] - b.val[1])
		+ (a.val[2] - b.val[2]) * (a.val[2] - b.val[2]));
}

bool isWhite(CvScalar f)
{
	if (f.val[0] == white.val[0] && f.val[1] == white.val[1] && f.val[2] == white.val[2])
		return true;
	return false;
}

float PoissonEditing(CvScalar f1, CvScalar f2, CvScalar f3, CvScalar f4, CvScalar F)
{
	CvScalar f;
	f.val[0] = (f1.val[0] + f2.val[0] + f3.val[0] + f4.val[0]) - 4 * F.val[0];
	f.val[1] = (f1.val[1] + f2.val[1] + f3.val[1] + f4.val[1]) - 4 * F.val[1];
	f.val[2] = (f1.val[2] + f2.val[2] + f3.val[2] + f4.val[2]) - 4 * F.val[2];
	return f.val[0] + f.val[1] + f.val[2];
}


CvScalar PoissonEditing1(CvScalar f1, CvScalar f2, CvScalar f3, CvScalar f4 )
{
	CvScalar f;
	f.val[0] = (f1.val[0] + f2.val[0] + f3.val[0] + f4.val[0])/4;
	f.val[1] = (f1.val[1] + f2.val[1] + f3.val[1] + f4.val[1]) / 4;
	f.val[2] = (f1.val[2] + f2.val[2] + f3.val[2] + f4.val[2]) / 4;
	return f;
}
	


void initValue()
{
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			CvScalar f = cvGet2D(src, y, x);
			colorMemory[y][x] = f;
		}
}

int main()
{
	src = cvLoadImage("c:\\temp\\PIESample.bmp");
	msk = cvLoadImage("c:\\temp\\PIEMask.bmp");

	cvShowImage("src", src);
	cvShowImage("msk", msk);
	w = src->width;
	h = src->height;

	colorMemory = (CvScalar**)malloc(sizeof(CvScalar*)*(h));

	for (int i = 0; i < h; i++)
		colorMemory[i] = (CvScalar*)malloc(sizeof(CvScalar) * w);

	dst = cvCreateImage(cvSize(w, h), 8, 3);

	
	initValue();
	
	

	float sumF = 0;
	float prevSumF = 0;
	int cnt = 0;
	int start = 1;
	
	while (cnt<3000)
	{
		prevSumF = sumF;
		sumF = 0;
		
		for (int y = 0; y < h ; y++)
			for (int x = 0 ;x < w ; x++)
			{
				CvScalar m = cvGet2D(msk, y, x);			
				if (isWhite(m))
				{
					if (x-1 < 0 ||x + 2 > w - 1)
						continue;
				
					if (y-1< 0 || y +2 > h - 1)
						continue;

					CvScalar F;
					CvScalar f1 = colorMemory[y][x - 1];
					CvScalar f2 = colorMemory[y][x + 1];
					CvScalar f3 = colorMemory[y-1][x];
					CvScalar f4 = colorMemory[y+1][x];

					F = PoissonEditing1(f1, f2, f3, f4);
					//printf("%.f\n", F.val[0]);
					cvSet2D(src, y, x,F);
					colorMemory[y][x] = F;
					//sumF += (getDist(F1, F2));
					//sumF += PoissonEditing(f1,f2,f3,f4,F);
					
				}

			}
		cnt++;
		//printf("%d\n", cnt);
		//printf("print\n");
		cvShowImage("dst", src);
		cvWaitKey(1);

	
		
	}
	printf("end!");
	
	cvWaitKey();
	return 0;

}