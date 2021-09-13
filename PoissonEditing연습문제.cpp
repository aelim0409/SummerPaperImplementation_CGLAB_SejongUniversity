#include <opencv2/opencv.hpp>
#pragma warning(disable:4996)

IplImage *src ;
IplImage *msk ;
IplImage *dst;
int w, h;
CvScalar white = cvScalar(255, 255, 255);

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
CvScalar PoissonEditing(CvScalar f1, CvScalar f2, CvScalar f3, CvScalar f4,CvScalar F)
{
	CvScalar f;
	f.val[0] = (f1.val[0] + f2.val[0] + f3.val[0] + f4.val[0]) - 4 * F.val[0];
	f.val[1] = (f1.val[1] + f2.val[1] + f3.val[1] + f4.val[1]) - 4 * F.val[1]; 
	f.val[2] = (f1.val[2] + f2.val[2] + f3.val[2] + f4.val[2]) - 4 * F.val[2];
	return f;
}

CvScalar PoissonEditing1(CvScalar f1, CvScalar f2, CvScalar f3, CvScalar f4 )
{
	CvScalar f;
	f.val[0] = (f1.val[0] + f2.val[0] + f3.val[0] + f4.val[0])/4;
	f.val[1] = (f1.val[1] + f2.val[1] + f3.val[1] + f4.val[1]) / 4;
	f.val[2] = (f1.val[2] + f2.val[2] + f3.val[2] + f4.val[2]) / 4;

	return f;
}
/*
CvScalar FindMin(CvScalar f1, CvScalar f2, CvScalar f3, CvScalar f4, CvScalar f)
{
	float min = FLT_MIN;
	CvScalar minF= cvScalar(255, 255, 255);
	CvScalar F[4];
	F[0] = f1;
	F[1] = f2;
	F[2] = f3;
	F[3] = f4;

	for (int i = 0; i < 4; i++)
	{
		if (getDist(f, F[i]) < min)
		{
			min = getDist(f, F[i]);
			minF = F[i];
		}
	}
	return minF;
}*/
int main()
{
	src = cvLoadImage("c:\\temp\\PIESample.bmp");
	msk = cvLoadImage("c:\\temp\\PIEMask.bmp");
	w = src->width;
	h = src->height;

	dst = cvCreateImage(cvSize(w, h), 8, 3);
	cvShowImage("src", src);
	cvShowImage("msk", msk);

	float sumF = 0;
	float prevSumF = 0;
	int cnt = 0;
	int start = 1;
	while (start<4)
	{
		prevSumF = sumF;
		sumF = 0;
		
		for (int y = 0; y < h ; y++)
			for (int x = 0 ;x < w ; x++)
			{
				CvScalar m = cvGet2D(msk, y, x);
				if (isWhite(m))
				{
					if (x-start < 0 ||x + 1+start > w - 1)
						continue;
				
					if (y-start< 0 || y +1+start > h - 1)
						continue;

					CvScalar f = cvGet2D(src, y, x);

					CvScalar f1 = cvGet2D(src, y, x - start);
					CvScalar f2 = cvGet2D(src, y, x + start);
					CvScalar f3 = cvGet2D(src, y - start, x);
					CvScalar f4 = cvGet2D(src, y + start, x);

					CvScalar F1 = PoissonEditing(f1, f2, f3, f4,f);

					/*
					CvScalar f1_2 = cvGet2D(src, (y), (x+1) - 1);
					CvScalar f2_2 = cvGet2D(src, (y), (x + 1) + 1);
					CvScalar f3_2 = cvGet2D(src, (y) - 1, (x + 1));
					CvScalar f4_2 = cvGet2D(src, (y) + 1, (x + 1));

					CvScalar F2 = PoissonEditing(f1_2, f2_2, f3_2, f4_2,f);
					*/
					cvSet2D(src, y, x, PoissonEditing1(f1,f2,f3,f4));
					//sumF += (getDist(F1, F2));
					sumF += F1.val[0];
				}
			}
	
		cvShowImage("dst", src);
		cvWaitKey(1);
		
		cnt++;
		if (sumF == prevSumF)
		{
			printf("%d\n", start);
			start++;
		}
	}
	cvWaitKey();
	return 0;

}