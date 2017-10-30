/*******************************************************************************
* Created by Qiang Wang on 16/7.24
* Copyright 2016 Qiang Wang.  [wangqiang2015-at-ia.ac.cn]
* Licensed under the Simplified BSD License
*******************************************************************************/



#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "kcf.hpp"
#include "getROI_Mouse.h"
using namespace cv;
using namespace std;

std::vector<cv::Rect>   GetGroundtruth(std::string txt_file);
std::vector<double>   PrecisionCalculate(std::vector<cv::Rect> groundtruth_rect, 
				      std::vector<cv::Rect> result_rect);

int main(/*int argc, char **argv*/) 
{


	//��������Լ������õ�
	//Mat patch = imread("zx.jpg");
	//imshow("test ", patch);
	//FHoG fhog;
	//std::vector<Mat> x_vector;
	//cv::cvtColor(patch, patch, CV_BGR2GRAY);    //�ҶȻ�
	//patch.convertTo(patch, CV_32FC1, 1.0 / 255);  //��һ��

	//x_vector = fhog.extract(patch,1);   //��
	//cout << "ԭͼ��С" <<patch.size()<< endl;
	//cout <<"����ά��"<< x_vector.size() << endl;
	//string x = "hog";
	//int i = 1;
	////cout << x_vector[0] << endl;
	//for (auto m : x_vector)
	//{
	//	
	//	cout << m.size() << endl;
	//	namedWindow(x + std::to_string(i),2);
	//	cout << x + std::to_string(i);
	//	imshow(x+std::to_string(i), m);
	//	i++;
	//}
	//
	//	
	//waitKey(0);
	//return 0;
	//�������Ҳ����õ�


  //if (argc != 3) 
  //{
  //  std::cout << "Usage:" 
  //            << argv[0] << " video_base_path[./girl] Verbose[0/1]" << std::endl;
  //  return 0;
  //}
 /* cout << argv[1] << endl;*/
  
  
  //��ͼƬ�Ļ������
  std::string video_base_path;
  video_base_path ="C:/Users/zhxing/Desktop/���Ĵ���/KCF_WQ/KCF_WQ/david";
  std::string pattern_jpg = video_base_path+ "/image/*.jpg";
  cout << pattern_jpg << endl;
  std::string pattern_png = video_base_path+ "/image/*.png";
  std::vector<cv::String> image_files;
  cv::glob(pattern_jpg, image_files);
 
   //������

   //���ϣ�������

  if (image_files.size() == 0)
    cv::glob(pattern_png, image_files);
  if (image_files.size() == 0) 
  {
    std::cout << "No image files[jpg png]" << std::endl;
    return 0;
  }
  std::string txt_base_path = video_base_path + "/groundtruth.txt";

  
  std::vector<cv::Rect> groundtruth_rect;
  groundtruth_rect = GetGroundtruth(txt_base_path);
 
//  for (auto i : groundtruth_rect)
//	  cout << i << endl;
//�����ȡ��gt��Ϣ�����Ͳ��ԣ���Ҳ��Ҫ����ˣ����Լ�����

  groundtruth_rect[0] = Rect(330, 260, 85, 50);
  cv::Mat image;
  std::vector<cv::Rect> result_rect;
  int64 tic, toc;      //����ʱ���õ�
  double time = 0;     
  bool show_visualization =true;

  std::string kernel_type = "gaussian";  //gaussian polynomial linear  ��ʲô��
  std::string feature_type = "hog";      //hog gray fhog             ��ʲô����

  KCF kcf_tracker(kernel_type, feature_type);    //����д�õ�һ���ࡣ
  cout << "test1" << endl;
  
  //����Ƶ�Ļ���������ԡ�
  VideoCapture video("C:\\Users\\zhxing\\Desktop\\���Ĵ���\\KCF_WQ\\KCF_WQ\\dat1.avi");
  Mat frame;
  unsigned video_num= video.get(CV_CAP_PROP_FRAME_COUNT);
  cout << "��֡��: "<<video_num << endl;
  
  
  //������
  //for (unsigned i = 0; i < video_num;i++)
  //{
	 // video.read(frame);
	 // imshow("��Ƶ", frame);
	 // waitKey(10);
  //}

  
  for (unsigned  frame = 0; frame < video_num; ++frame)   //����ͼƬ����
  {
	  
	  video.read(image);
	  //image = cv::imread(image_files[frame]);
	 //imwrite("test.jpg",image)
	  tic = getTickCount();
      if (frame == 0) 
	  {    //��һֱ֡�ӳ�ʼ������
		  cv::String frame1 = "frame1";
		  imshow(frame1, image);
		  groundtruth_rect[0]= GetRoi(frame1, image);   
		  //�������¼ӵ�һ��������ѡ�����ʱ��ESC,�᷵��һ��REC�Ķ�����Գ�ʼ���������ĳ�ʼλ��
		  //����˵��װ��һ����ȽϺã�����û�з�װ�ɹ�����Ҫԭ�������ص�����������Ϊ���Ա����������
		  //����ԪҲ���ܷ��ʳ�Ա������˵�ǿ��Է��ʵģ����ż������Ч�������������Ŀ�в���װҲ���Ǻ�Ӱ�죩
		  //���Ծ���д��ͷ�ļ��ˡ�
		  cout << "���ٳߴ�:  " << groundtruth_rect[0] << endl;
        kcf_tracker.Init(image, groundtruth_rect[0]);
        result_rect.push_back(groundtruth_rect[0]);     //0-index
      }
	  //result_rect����ǳ�����ٵ���λ��
	  else 
	  {
        result_rect.push_back(kcf_tracker.Update(image));
      }
      toc = cv::getTickCount() - tic;
      time += toc;

      if (show_visualization) {
        cv::putText(image, to_string(frame+1), cv::Point(20, 40), 6, 1,
                    cv::Scalar(0, 255, 255), 2);
        //cv::rectangle(image, groundtruth_rect[frame], cv::Scalar(0, 255, 0), 2);
        cv::rectangle(image, result_rect[frame], cv::Scalar(0, 255, 255), 2);
		
        cv::imshow(video_base_path, image);
      
        char key = cv::waitKey(1);
        if(key == 27 || key == 'q' || key == 'Q')
          break;
      }
    }
    time = time / double(getTickFrequency());
    double fps = double(result_rect.size()) / time;
	cout << "֡��: ----------" <<fps << endl;
    /*std::vector<double> precisions = PrecisionCalculate(groundtruth_rect,
                                                        result_rect);
    std::cout<<"%12s - Precision (20px) : %1.3f, FPS : %4.2f\n"<<
           video_base_path.c_str()<<" "<<precisions[20]<<"  "<< fps<<endl;*/
    cv::destroyAllWindows();
  
 
  return 0;
}


std::vector<cv::Rect> GetGroundtruth(std::string txt_file) 
{
  std::vector<cv::Rect> rect;
  ifstream gt;
  gt.open(txt_file.c_str());
  if (!gt.is_open())
    std::cout << "Ground truth file " << txt_file 
              << " can not be read" << std::endl;
  std::string line;
  int tmp1, tmp2, tmp3, tmp4;
  while (getline( gt, line)) {
    std::replace(line.begin(), line.end(), ',', ' ');
    stringstream ss;
    ss.str(line);
    ss >> tmp1 >> tmp2 >> tmp3 >> tmp4;
    rect.push_back( cv::Rect(--tmp1, --tmp2, tmp3, tmp4) ); //0-index
  }
  gt.close();
  return rect;
}


//����Ǽ���׼ȷ�ʵ�
std::vector<double> PrecisionCalculate(std::vector<cv::Rect>groundtruth_rect,
                                       std::vector<cv::Rect>result_rect) {
  int max_threshold = 50;
  std::vector<double> precisions(max_threshold + 1, 0);
  if (groundtruth_rect.size() != result_rect.size()) {
    int n = min(groundtruth_rect.size(), result_rect.size());
    groundtruth_rect.erase(groundtruth_rect.begin()+n, groundtruth_rect.end());
    result_rect.erase(result_rect.begin() + n, result_rect.end());
  }
  std::vector<double> distances;
  double distemp;
  for (unsigned int i = 0; i < result_rect.size(); ++i) {
    distemp = sqrt(double(pow((result_rect[i].x + result_rect[i].width / 2) -
              (groundtruth_rect[i].x + groundtruth_rect[i].width / 2), 2) +
                          pow((result_rect[i].y + result_rect[i].height / 2) -
              (groundtruth_rect[i].y + groundtruth_rect[i].height / 2), 2)));
    distances.push_back(distemp);
  }
  for (int i = 0; i <= max_threshold; ++i) {
    for (unsigned int j = 0; j < distances.size(); ++j) {
      if (distances[j] < double(i))
        precisions[i]++;
    }
    precisions[i] = precisions[i] / distances.size();
  }
  return precisions;
}

