#include <opencv2/opencv.hpp> 
#include<opencv2/opencv.hpp>
#include<math.h>
using namespace std;  
using namespace cv;
  
//double summ[231][338][3] = {0,0,0};
IplImage* summ  = cvCreateImage(cvSize(231,338),IPL_DEPTH_32F,3);
IplImage *m[6];
 

#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  

//函数mfunction，得到每幅图的梯度图
IplImage* mfunction(IplImage* img) {
	//IplImage* m = cvCreateImage(cvSize(231,338),IPL_DEPTH_32F,3);
	IplImage* m = cvCloneImage(img);
	int i,j,n;
	double a,b;

	for(i = 2; i < (img->height-2); i++) {
		for(j = 2; j < (img->width-2); j++) {
			CvScalar ms,is,is1,is2,ss;
			ms = cvGet2D(m,i,j);
			is = cvGet2D(img,i,j);
			is1 = cvGet2D(img,i+1,j);
			is2 = cvGet2D(img,i,j+1);
			ss = cvGet2D(summ,i,j);
			for(n = 0; n < 3; n++) {
				a = is1.val[n]-is.val[n];
				b = is2.val[n]-is.val[n];
				ms.val[n] = sqrt(a*a+b*b)+1;
				ss.val[n] += ms.val[n];
				
			}
			cvSet2D(summ,i,j,ss);
			cvSet2D(m,i,j,ms);
		}
	}
	return m;

}


//函数wfunction,梯度图归一化
IplImage* wfunction(IplImage* img) {
	
	IplImage* nimg  = cvCreateImage(cvSize(231,338),IPL_DEPTH_32F,3);
	int i, j, n,p;
	for(i = 2; i < img->height-2; i++) {
		for(j = 2; j < img->width-2; j++) {
			double sum[3] = {0.0001,0.0001,0.0001};
			for(p = 0; p < 6; p++) {
				CvScalar is;
				is = cvGet2D(m[p],i,j);
				sum[0] += is.val[0];
				sum[1] += is.val[1];
				sum[2] += is.val[2];
			}
			CvScalar iis,ss,nis;
			iis = cvGet2D(img,i,j);	
			//ss = cvGet2D(summ,i,j);
			nis = cvGet2D(nimg,i,j);
			for(n = 0; n < 3; n++) {
				nis.val[n] = iis.val[n] / sum[n];
			}
			cvSet2D(nimg,i,j,nis);
		}
	}
	return nimg;
}


//对每幅图的归一化模板w做高斯滤波
IplImage* Gaussian(IplImage* img) {
	IplImage* nimg = cvCloneImage(img);
	int i, j, n, p, q;
	int adx[5] = {-2,-1,0,1,2};
	int ady[5] = {-2,-1,0,1,2};
	double paxel[3];
	double mask[5][5] = {1.0/273.0,4.0/273.0,7.0/273.0,4.0/273.0,1.0/273.0,
					 4.0/273.0,16.0/273.0,26.0/273.0,16.0/273.0,4.0/273.0,
					 7.0/273.0,26.0/273.0,41.0/273.0,26.0/273.0,7.0/273.0,
					 4.0/273.0,16.0/273.0,26.0/273.0,16.0/273.0,4.0/273.0,
					 1.0/273.0,4.0/273.0,7.0/273.0,4.0/273.0,1.0/273.0};
	for(i = 2; i < img->height-2; i++) {
		for(j = 2; j < img->width-2; j++) {
			paxel[1] = 0.0;
			paxel[2] = 0.0;
			paxel[0] = 0.0;

			for(p = 0; p < 5; p++) {
				for(q = 0; q < 5; q++) {
					CvScalar is;
					is = cvGet2D(img,i+adx[p],j+ady[q]);
					for(n = 0; n < 3; n++) {
						paxel[n] += mask[p][q] * is.val[n];
					}
				}
			}

			CvScalar nis;
			nis = cvGet2D(nimg,i,j);
			for(n = 0; n < 3; n++) {
				nis.val[n] = paxel[n];
			}
			cvSet2D(nimg,i,j,nis);
		}
	}

	return nimg;
}




//高斯图像与原图像的叠加
IplImage* GaussianAdd(IplImage* img1,IplImage* img2) {
	int i, j, n;
	IplImage* nimg = cvCreateImage(cvSize(231,338),IPL_DEPTH_8U,3);
	for(i = 2; i < img1->height-2; i++) {
		for(j = 2; j < img1->width-2; j++) {
			CvScalar nis, is1, is2;
			nis = cvGet2D(nimg,i,j);
			is1 = cvGet2D(img1,i,j);
			is2 = cvGet2D(img2,i,j);
			for(n = 0; n < 3; n++) {
				nis.val[n] = is1.val[n] * is2.val[n];
			}
			cvSet2D(nimg,i,j,nis);
		}
	}
	return nimg;
}

int main()  
{  
	const char *pstrImageName = "door1.jpg";  
	const char *pstrWindowsTitle = "OpenCV第一个程序(http://blog.csdn.net/MoreWindows)";  
	int i, j, n,p,q;
	//从文件中读取图像 
	IplImage *image[6];
	image[0] = cvLoadImage("door1.jpg"); 
	image[1] = cvLoadImage("door2.jpg");  
	image[2] = cvLoadImage("door3.jpg");  
	image[3] = cvLoadImage("door4.jpg");  
	image[4] = cvLoadImage("door5.jpg");  
	image[5] = cvLoadImage("door6.jpg");  
	

	//创建窗口  
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);  


	/*//初始化summ
	CvScalar ss;
	for(i = 0; i < summ->height; i++) {
		for(j = 0; j < summ->width; j++) {
			ss = cvGet2D(summ,i,j);
			for(n = 0; n < 3; n++) {
				ss.val[n] = 0;
			}
		}
	}

	*/
	
	//每幅图初始化好梯度m
	//IplImage *m[6];
	for(i = 0; i < 6; i++)
		m[i] = mfunction(image[i]);

	//对每幅梯度图求w
	IplImage *w[6];
	for(i = 0; i < 6; i++)
		w[i] = wfunction(m[i]);

	//对每幅梯度图调用Gaussian函数
	IplImage *gImage[6];
	for(i = 0; i < 6; i++)
		gImage[i] = Gaussian(w[i]);
 
	//高斯图像与原图的叠加
	IplImage *newImage[6];
	for(i = 0; i < 6; i++)
		newImage[i] = GaussianAdd(image[i],gImage[i]);
	

	//六幅图像的合并
	IplImage* output = cvCreateImage(cvSize(231,338),IPL_DEPTH_8U,3);
	CvScalar os;
	
	for(i = 2; i < output->height-2; i++) {
		for(j = 2; j < output->width-2; j++) {
			os = cvGet2D(output,i,j);
			os.val[0] = 0.0;
			os.val[1] = 0.0;
			os.val[2] = 0.0;
			for(p = 0; p < 6; p++) {
				CvScalar nis;
				nis = cvGet2D(newImage[p],i,j);
				for(n = 0; n < 3; n++) {
					os.val[n] += nis.val[n];
				}
			}
			//for(n = 0; n < 3; n++)
				//os.val[n] = os.val[n]/6;
			cvSet2D(output,i,j,os);
		}
	}

	
	


	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsTitle,output);
	cvSaveImage("output.jpg",output);


	//等待按键事件  
	cvWaitKey();  

	
	//释放图片
	cvDestroyWindow(pstrWindowsTitle);  
	for(i = 0; i < 6; i++) {
		cvReleaseImage(&image[i]);
		cvReleaseImage(&m[i]);
		cvReleaseImage(&w[i]);
		cvReleaseImage(&gImage[i]);
		cvReleaseImage(&newImage[i]);
	}
	cvReleaseImage(&output);
	
	
		
	return 0;  
 }  