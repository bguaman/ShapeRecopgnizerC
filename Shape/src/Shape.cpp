/* play a video file */
#include <stdio.h>
#include <math.h>
#include <typeinfo>
/*#include<highgui.h>*/
#include <iostream>
#include <iomanip>
/*#include <opencv2/opencv.hpp>*/
#include <opencv.hpp>
#include <list>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include"storage_adaptors.hpp"
using namespace std;
using namespace boost::numeric::ublas;
#include <fstream>
//includes del JNI
#include <jni.h>
#include <string.h>
#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif



std::vector<float> MaptoPolarAvgMem(IplImage * input){

	std::vector<float> promedios;

	float resultado = 0;
	int i,j=0;
	for(i=0;i<input->height;i++){


		char * temp=input->imageData + i*input->widthStep;

		float acumulador=0.0;
		int contador=0;

		for(j=0;j<input->width;j++){



			if(temp[j]!=0){ contador=contador+1; acumulador=acumulador+j;}


		}
		if(contador==0){
					if(sizeof(promedios.size())!=0){  contador=1;  acumulador=resultado;}
					else {contador=1; acumulador=0.0;}
				}
		resultado=float(acumulador/contador);
		promedios.push_back(resultado);

	}

	return promedios;
}

int main(int argc, char** argv) {
	//Variables del JNI
	  JavaVMOption options[1];
	  JNIEnv *env;
	  JavaVM *jvm;
	  JavaVMInitArgs vm_args;
	  long status;
	  jint resultado=-1;
	  jclass cls2;
	  jmethodID ide,ide2;

	  bool jvmok=false;

/********************************************/
	  //INIZIALIZACION DE VARIABLES JNI

	  options[0].optionString = "-Djava.class.path=./weka.jar:./shapecls.jar:.";
	  memset(&vm_args, 0, sizeof(vm_args));
	  vm_args.version = JNI_VERSION_1_2;
	  vm_args.nOptions = 1;
	  vm_args.options = options;
	  status = JNI_CreateJavaVM(&jvm,(void**)&env, &vm_args);
	  cls2 = env->FindClass("shapecls/MyClusterer");
/********************************************/
	  //CARGANDO METODOS Y OTROS ATRIBUTOS JNI
	    if (status != JNI_ERR)
	    	     {
	    		    if(cls2 !=0){

	    		    ide=env->GetStaticMethodID(cls2,"loadmodel","(Ljava/lang/String;)I");
	    			jstring cadena=	env->NewStringUTF("/home/bguaman/weka-3-6-6/modelofinal.model");
	    			jint resul =env->CallStaticIntMethod(cls2,ide,cadena);
	    			//cout<<"cargo:"<<resul<<endl;
	    			ide2=env->GetStaticMethodID(cls2,"classify","([D)I");


	    			jvmok=true;


	    		    }


	    	   }


/*****************************************************************************************8

    /* Create a window */
    cvNamedWindow("Example2", CV_WINDOW_AUTOSIZE);
    /* capture frame from video file */
    CvCapture* capture = cvCreateFileCapture(argv[1]);

	cv::Mat mattemp;
	cv::Mat aux,aux2,abvalue,slice,doble;


	int threshold=200;
	int colour=255;
	//Areas maxima y minima
	double areaminima=5.0;
	double areamaxima=75.0;

	//Area total del video

	int htotal=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT);
	int wtotal=cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH);

	float inv_areatotal=1/(float(htotal*wtotal));

	std::vector<float> promed;


	IplImage* gray=cvCreateImage(cvSize(wtotal,htotal),8,1);

	IplImage* dst=cvCreateImage(cvSize(360,360), gray->depth,gray->nChannels);
	IplImage* dst2=cvCreateImage(cvSize(360,360), gray->depth,gray->nChannels);


    /* Create IplImage to point to each frame */
    IplImage* frameImg;
    std::vector<std::vector<cv::Point> > contours;
    CvMat  dst_mat;
    cv::Mat contourOutput;
    cv::Mat graymat4bound,graymat4draw;
    cv::Mat normalized;
    cv::Mat mag;
    cv::Mat magI,imagI;
    cv::Mat complexI;

    ofstream myfile;

	CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
    FILE * fp = fopen("./CSV/pruebaa.csv", "w");
   // FILE * fp = fopen("./CSV/datos.txt", "w");

    /* Loop until frame ended or ESC is pressed */
    while(1) {
        /* grab frame image, and retrieve */
        frameImg = cvQueryFrame(capture);

        /* exit loop if fram is null / movie end */
        if(!frameImg) break;

        cvCvtColor(frameImg,gray,CV_BGR2GRAY);
        //Aplicamos un filtro binrio
        cvThreshold(gray,gray, threshold,colour,CV_THRESH_BINARY);
        //clonamos la imagen original porque al aplicar findcontours se modifica

         //contourOutput= cvCloneImage(gray);
        //cv::Mat contourOutput(gray);
		graymat4bound=cv::Mat(gray,1);
        graymat4draw=cv::Mat(frameImg);
        cv::findContours( graymat4bound, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );


        int size=contours.size();
        int iter=0;


        while (iter<size){

        	mattemp=cv::Mat(contours[iter]);

        	double area=cv::contourArea(mattemp);
        	//divarea=func.getAreaPercentage(area,areatotal)
        	double percentage=(area*inv_areatotal)*100.0;
        	//cout<<percentage<<" "<<"Printed out variable percentage"<<endl;
            if (percentage < areaminima || percentage > areamaxima){
                    iter++;

                    continue;
            }


            CvRect rect =cv::boundingRect(mattemp);

    		cvGetSubRect(gray,&dst_mat,rect);

    		cv::rectangle(graymat4draw,rect,cv::Scalar(255));




            cvLogPolar(&dst_mat,dst,cvPoint2D32f(dst_mat.cols/2,dst_mat.rows/2), 40, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS );

            cvCanny(dst, dst2, 70, 140);

            promed=MaptoPolarAvgMem(dst2);
            aux=cv::Mat(promed);






            //cv::dft(aux,aux2);
           // slice=aux2.rowRange(0,29);
          /*  abvalue=cv::abs(aux2);
                         doble=cv::Mat_<double>(abvalue);
                         //double * arreglo=(double *)abvalue.data;
                         double first=doble.at<double>(0);
                         double inv=double(1.0/first);
                         normalized=((doble*inv)*1000.0);
                         double* arreglo3=(double*)normalized.data;
*/

            cv::Mat planes[] = {cv::Mat_<double>(aux), cv::Mat::zeros(aux.size(), CV_64F)};

            cv::merge(planes, 2, complexI);

            cv::dft(complexI,complexI);

            cv::split(complexI,planes);




                magnitude(planes[0], planes[1], planes[0]);

                magI = planes[0];



               // cv::dft(complexI,complexI,cv::DFT_INVERSE);
                //cout<<"inversa:"<<complexI<<endl;

                //slice=magI.rowRange(0,29);

                double first=magI.at<double>(0);
				double inv=1/first;
				//string s = typeid(magI.at<float>(0)).name();

				normalized=((magI*inv)*1000.0);
				double* arreglo3=(double*)normalized.data;
				double* visualize=(double*)magI.data;


				for(int count=0;count<magI.rows;count++){

		    			fprintf(fp,"%.6f,",visualize[count]);

				}
				fprintf(fp,"\n");


    		  if(jvmok !=false){

    		    	jdoubleArray arreglo=env->NewDoubleArray(30);


    		    		    			//double arreglo2 []= {1000.000000,16.446404,3.729186,2.771154,17.800634,33.837307,10.238957,2.233379,1.377190,3.409533,6.067201,1.782658,1.118950,0.948547,2.728277,1.183576,1.976642,1.173565,0.271126,1.756574,0.323660,1.086649,0.466114,0.493768,0.694117,0.384536,0.624780,0.392404,0.267215,0.661685};
    		    		    			env->SetDoubleArrayRegion( arreglo, 0, 30, &arreglo3[0] );
    		    		    			resultado=env->CallStaticIntMethod(cls2,ide2,arreglo);

    		    		    			//std::cout<<"Resultado:"<<resultado<<std::endl;


    		    }

    		  char * string="";
    		switch(resultado){
    		case 0:
    			string="Cuadrado";
    			break;
    		case 1:
    			string="Estrella";
    			break;
    		case 3:
    			string="Rectangulo";
    			break;
    		case 4:
    			string="Triangulo";
    			break;
    		case 2:
    			string="Unknown";
    			break;
    		default:
    			string="Error";
    			break;

    		}

    		cvPutText(frameImg, string, cvPoint(rect.x,rect.y), &font, CV_RGB(0,0,255));

  //  		  std::cout<<"Resultado:"<<string<<std::endl;




            iter++;
        }



        /* display frame into window */
        cvShowImage("Example2", frameImg);
        cvShowImage("Little", dst2);
        //cvShowImage("Inverse", matras);
    	imshow("Visualize",magI);
        cvWaitKey(2);

    }


    jvm->DestroyJavaVM();


    //myfile.close();
    /* destroy pointer to video */
    cvReleaseCapture(&capture);
    /* delete window */
    cvDestroyWindow("Example2");
    cvDestroyWindow("Little");
    return EXIT_SUCCESS;
}
