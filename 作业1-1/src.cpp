//显示图像文件  
#include <opencv2/opencv.hpp> 
#include<opencv2/opencv.hpp>
#include<math.h>
using namespace std;  
using namespace cv;
  
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  


IplImage* m[6];


double mask[5][5] = {1/273,4/273,7/273,4/273,1/273,
					 4/273,16/273,26/273,16/273,4/273,
					 7/273,26/273,41/273,26/273,7/273,
					 4/273,16/273,26/273,16/273,4/273,
					 1/273,4/273,7/273,4/273,1/273};

/*//高斯函数
double Gaussian(double x) {
	double a = 0.02*5*sqrt(2);
	return (exp((-(x*x))/(2*(a*a)))/(a*sqrt(2*3.414)));

}
*/

//模块的滑动
IplImage* slide(IplImage* img1,int ID) {
	int i,j,p,q,n;
	IplImage* img11; 

	
/*		//建立高斯模板
	double mask[5][5] = {sqrt(8),sqrt(5),2,sqrt(5),sqrt(8),
		sqrt(5),sqrt(2),1,sqrt(2),sqrt(5),
		2,1,0,1,2,
		sqrt(5),sqrt(2),1,sqrt(2),sqrt(5),
		sqrt(8),sqrt(5),2,sqrt(5),sqrt(8),
	};
	double num=0;
	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++)
			num += Gaussian(mask[i][j]);
	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++)
			mask[i][j] = Gaussian(mask[i][j])/num;
*/				
	double mask[5][5] = {1,4,7,4,1,
					 4,16,26,16,4,
					 7,26,41,26,7,
					 4,16,26,16,4,
					 1,4,7,4,1};
	for(i = 0; i < 5; i++)
		for(j = 0; j < 5; j++)
			mask[i][j] = mask[i][j]/273;

	
	img11=cvCloneImage(img1);
	//m[ID]=cvCloneImage(img1);
	m[ID]=cvCreateImage(cvSize(231,338),IPL_DEPTH_32F,3);

	
	int adx[5] = {-2,-1,0,1,2};
	int ady[5] = {-2,-1,0,1,2};
	for(i = 2; i < (img1->height-2); i++) {
		for(j = 2; j < (img1->width-2); j++) {
			//处理每个像素的梯度m
			CvScalar ms;
			ms = cvGet2D(m[ID],i,j);
			double a,b;
			for(n = 0; n < 3; n++) {
				a = (((uchar *)(img1->imageData+(i+1)*img1->widthStep))[j*img1->nChannels + n] - ((uchar *)(img1->imageData+(i)*img1->widthStep))[j*img1->nChannels + n])^2;
				b = (((uchar *)(img1->imageData+(i)*img1->widthStep))[(j+1)*img1->nChannels+n] - ((uchar *)(img1->imageData+(i)*img1->widthStep))[j*img1->nChannels+n])^2;
				ms.val[n] = sqrt(a + b);
			   //((uchar *)(m[ID]->imageData+(i)*m[ID]->widthStep))[(j)*m[ID]->nChannels+n] = sqrt(a+b);
			}
			cvSet2D(m[ID],i,j,ms);

			//每个像素梯度算子与对应的高斯模块操作 
			double sum[3] ={0,0,0};
			CvScalar nis;
			nis = cvGet2D(img11,i,j);

			for(p = 0; p < 5; p++) {
				for(q = 0; q <5; q++){
					CvScalar is;
					is = cvGet2D(img1,i+adx[p],j+ady[q]);			
					sum[0] =sum[0]+ is.val[0]*mask[p][q]; 
					sum[1] =sum[1] + is.val[1]*mask[p][q]; 
					sum[2] =sum[2]+is.val[2]*mask[p][q]; 
					
				}
			}
			//CvScalar ms;
		    //ms = cvGet2D(m[ID],i,j);
			//nis.val[0] = sum[0]*((uchar *)(m[ID]->imageData+(i)*m[ID]->widthStep))[(j)*m[ID]->nChannels+0];
			//nis.val[1] = sum[1]*((uchar *)(m[ID]->imageData+(i)*m[ID]->widthStep))[(j)*m[ID]->nChannels+1];
			//nis.val[2] = sum[2]*((uchar *)(m[ID]->imageData+(i)*m[ID]->widthStep))[(j)*m[ID]->nChannels+2];

			nis.val[0] = sum[0]*ms.val[0];
			nis.val[1] = sum[1]*ms.val[1];
			nis.val[2] = sum[2]*ms.val[2];
			cvSet2D(img11,i,j,nis);

		}
	}
	
/*	cvReleaseImage(&img11);
	const char *pstrWindowsTitle = "OpenCV第一个程序(http://blog.csdn.net/MoreWindows)"; 
	//创建窗口  
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);  
	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsTitle,img11);  
	//等待按键事件  
	cvWaitKey(); 
	*/
	return img11;
}



int main()  
{  
	const char *pstrImageName = "door1.jpg";  
	const char *pstrWindowsTitle = "OpenCV第一个程序(http://blog.csdn.net/MoreWindows)";  
	int i, j, n,p;

	//从文件中读取图像  
	IplImage *image1 = cvLoadImage("door1.jpg"); 
	IplImage *image2 = cvLoadImage("door2.jpg");  
	IplImage *image3 = cvLoadImage("door3.jpg");  
	IplImage *image4 = cvLoadImage("door4.jpg");  
	IplImage *image5 = cvLoadImage("door5.jpg");  
	IplImage *image6 = cvLoadImage("door6.jpg");  
	

	//创建窗口  
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);  


	IplImage* output = cvCreateImage(cvSize(231,338),IPL_DEPTH_32F,3);
	

	
    //对每幅图调用slide函数
	IplImage *newImage1 = slide(image1,0);
	IplImage *newImage2 = slide(image2,1);
	IplImage *newImage3 = slide(image3,2);
	IplImage *newImage4 = slide(image4,3);
	IplImage *newImage5 = slide(image5,4);
	IplImage *newImage6 = slide(image6,5);
	
	//图像归一化相加
	CvScalar os;	
	for(i = 2; i < (image1->height-2); i++) {
		for(j = 0; (j < image1->width-2); j++) {
			os = cvGet2D(output,i,j);
			CvScalar nis[6];//,nis2,nis3,nis4,nis5,nis6;
				nis[0] = cvGet2D(newImage1,i,j);
				nis[1] = cvGet2D(newImage2,i,j);
				nis[2] = cvGet2D(newImage3,i,j);
				nis[3] = cvGet2D(newImage4,i,j);
				nis[4] = cvGet2D(newImage5,i,j);
				nis[5] = cvGet2D(newImage6,i,j);
				CvScalar ms[6];//,ms2,ms3,ms4,ms5,ms6;
				ms[0] = cvGet2D(m[0],i,j);
				ms[1] = cvGet2D(m[1],i,j);
				ms[2] = cvGet2D(m[2],i,j);
				ms[3] = cvGet2D(m[3],i,j);
				ms[4] = cvGet2D(m[4],i,j);
				ms[5] = cvGet2D(m[5],i,j); 
			for(n = 0; n < 3; n++) {
				double sum = 0;
				sum = ms[0].val[n]+ms[1].val[n]+ms[2].val[n]+ms[3].val[n]+ms[4].val[n]+ms[5].val[n];
				
				
					//ms[p].val[n] = ms[p].val[n]/sum;
				os.val[n] = (nis[1].val[n]+nis[2].val[n]+nis[3].val[n]+nis[4].val[n]+nis[5].val[n]+nis[6].val[n])/(sum+0.0001);
				//os.val[n] = (nis[1].val[n]+nis[2].val[n]+nis[3].val[n]+nis[4].val[n]+nis[5].val[n]+nis[6].val[n]);
			}

		
			
			cvSet2D(output,i,j,os);
			
		}
	}
					
					
				



	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsTitle,output);  


	//等待按键事件  
	cvWaitKey();  

	//释放图片
	//cvDestroyWindow(pstrWindowsTitle);  
	cvReleaseImage(&image1);
	cvReleaseImage(&image2);
	cvReleaseImage(&image3);
	cvReleaseImage(&image4);
	cvReleaseImage(&image5);
	cvReleaseImage(&image6);
	cvReleaseImage(&newImage1);
	cvReleaseImage(&newImage2);
	cvReleaseImage(&newImage3);
	cvReleaseImage(&newImage4);
	cvReleaseImage(&newImage5);
	cvReleaseImage(&newImage6);
	cvReleaseImage(&m[1]);
	cvReleaseImage(&m[2]);
	cvReleaseImage(&m[3]);
	cvReleaseImage(&m[4]);
	cvReleaseImage(&m[5]);
	cvReleaseImage(&m[6]);
	cvReleaseImage(&output);
	
		
	return 0;  
 }  