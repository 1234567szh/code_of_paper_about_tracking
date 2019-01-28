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
#include "get_rec_pos.h"       //�ͱ��˵ıȽϵ�һ�������������ο��
#include "TemplateTracking.h"

using namespace cv;
using namespace std;

std::vector<cv::Rect>   GetGroundtruth(std::string txt_file);
std::vector<double>   PrecisionCalculate(std::vector<cv::Rect> groundtruth_rect, 
				      std::vector<cv::Rect> result_rect);



cv::Rect split_line(string &line);   //�����ַ�����������
									 //��ȡgroundtruth��Ϣ



vector<cv::Rect> read_groundtruth(const string &groundtruth_txt, int &num_of_line)    //const�����ſ������ַ�����ʽת��
{
	vector<cv::Rect> groundtruth;            // vector<rect>  ������groundtruth
	ifstream groundtruth_file;               // �ļ�����
	groundtruth_file.open(groundtruth_txt);      //��txt�ļ�
	string line;         //��ǰ��
	Rect rect_tmp;       //ÿһ�и��һ��rect
	while (getline(groundtruth_file, line))
	{
		rect_tmp = split_line(line);    //�ֽ��ַ���ΪRECT
		groundtruth.push_back(rect_tmp);    //ѹ��vector
		num_of_line++;
	}
	groundtruth_file.close();
	return groundtruth;
}

cv::Rect split_line(string &line)
{
	double pos[8];            //�˸���
	int index = 0;              //�������
	string tmp;               //�ݴ��string����ת��Ϊdouble
	tmp.clear();              //����
	for (auto l : line)          //�����ַ�������������һ���Ƚϼ򵥵��ַ��������ض��ַ������һ���㷨
	{
		if (l == ',')
		{
			pos[index] = stod(tmp);
			index++;
			tmp.clear();     //һ��Ҫ�ǵ�����
		}
		else
		{
			tmp += l;
		}
	}
	pos[index] = stod(tmp);    //�������һ��
							   //�ĸ��㣬��Ӧ���ε��ĸ���

							   /* �Һ������ֱ�ע�ĵ㲢������ѭ�����Ĺ��ɣ���һ��һ��ʼ�����Ͻǵĵ㣬��ȡ���ڵ�ʱ��ע��
							   ���ȴ��ĸ��㿪ʼ��ģ�����Ӧ����ʹ������֮��Ĵ�С��ϵ��ȷ���������ĸ���
							   cv::Point2f up_left(pos[0],pos[1]);
							   cv::Point2f up_right(pos[2],pos[3]);
							   cv::Point2f down_right(pos[4],pos[5]);
							   cv::Point2f down_left(pos[6],pos[7]);
							   //cout<<up_left<<" "<<up_right<<" "<<down_right<<" "<<down_left<<endl;


							   int x=round(up_left.x);
							   int y=round(up_left.y);
							   int weidth=round(down_right.x-up_left.x);
							   int height=round(down_right.y-up_left.y);
							   cv::Rect res(x,y,weidth,height);
							   */

	double xmin = min(pos[0], min(min(pos[2], pos[4]), pos[6]));
	double ymin = min(pos[1], min(min(pos[3], pos[5]), pos[7]));
	double xmax = max(pos[0], max(max(pos[2], pos[4]), pos[6]));
	double ymax = max(pos[1], max(max(pos[3], pos[5]), pos[7]));

	cv::Rect res(xmin, ymin, xmax - xmin, ymax - ymin);
	//cout<<res<<endl;

	return res;
}

// ��ȡlist�б����Ϣ
vector<string> read_list(const string &list_name)
{
	vector<string> list_mes;
	ifstream list(list_name);
	string line;
	while (getline(list, line))   //��ȡlist�б���Ϣ
	{
		//cout<<line<<endl;
		list_mes.push_back(line);
	}
	list.close();
	return list_mes;
}


void setROI_4(cv::Rect &rect)
{
	if (rect.width % 4 == 1)   rect.width -= 1;
	else if (rect.width % 4 == 2) { rect.x -= 1; rect.width += 2; }
	else if (rect.width % 4 == 3)   rect.width += 1;

	if (rect.height % 4 == 1)   rect.height -= 1;
	else if (rect.height % 4 == 2) { rect.y -= 1; rect.height += 2; }
	else if (rect.height % 4 == 3)   rect.height += 1;
}




int main()
{
	printf("this is a kcf test code!!!\n");
	vector<string> list = read_list("C:\\Users\\zhxing\\Desktop\\code_of_paper_about_tracking//vot2015//list.txt");
	for (int i = 0; i < list.size(); i++)
	{

		cout << "this is the VOT tracking test!!" << endl;
		cout << "and this is " << list[i] << endl;

		//������ٽ��
		ofstream res_ground("results//" + list[i] + "_res_ground.txt");
		ofstream res_kcf("results//" + list[i] + "_res_kcf.txt");
		ofstream res_kcf_inter("results//" + list[i] + "_res_kcf_interpolation.txt");
		ofstream ave_fps("results//" + list[i] + "_avefps.txt");

		//��ͷ��Ϣ
		res_ground<< "frame\tx\ty\twidth\theight\n";
		res_kcf << "frame\tx\ty\twidth\theight\n";
		res_kcf_inter << "frame\tx\ty\twidth\theight\n";
		ave_fps<< "frame\tave_fps\n";

		string path = "C:\\Users\\zhxing\\Desktop\\code_of_paper_about_tracking//vot2015//" + list[i] + "//";
		
		int num_of_line = 0;

		//��ȡgroundtruth��Ϣ
		vector<cv::Rect> groundtruth = read_groundtruth(path + "groundtruth.txt", num_of_line);


		// ground_truth��Ϣ������
		int index = 1;
		for (auto gg : groundtruth)
		{
			setROI_4(gg);
			res_ground << index++ << "\t" << gg.x << "\t" << gg.y << "\t" << gg.width << "\t" << gg.height << "\n";
		}
		res_ground.close();      //�ر�txt�ļ�


	    //���ٽ�������vector
		vector<cv::Rect> track_res;
		setROI_4(groundtruth[0]);
		cout <<"groundtruth[0]\t" <<groundtruth[0] << endl;

		track_res.push_back(groundtruth[0]);

		//��ȡ��һ��ͼƬ��Ϣ
		string zeros8 = "00000000";
		cv::Mat img = imread(path + "00000001.jpg");
		cout << "the first one image_size\t" << img.size() << endl;
		cout << "the first roi size is\t" << groundtruth[0] << endl;
		imshow("img", img);
		double all_time = 0;


		std::string kernel_type = "gaussian";  //gaussian polynomial linear  ��ʲô��,��ѡ����ʽ�͸�˹������������ľͲ��ú˺���
		std::string feature_type = "hog";      //hog gray fhog             ��ʲô����,����hogĬ���õ���fhog

		// ��˹�ˣ�hog������i�ǲ�ֵ�ģ�����i�ǲ���ֵ�ġ�
		KCF kcf_tracker_i(kernel_type, feature_type);    //����д�õ�һ���ࡣ
		KCF kcf_tracker(kernel_type, feature_type);    //����д�õ�һ���ࡣ


		kcf_tracker_i.Init(img, groundtruth[0]);
		kcf_tracker.Init(img, groundtruth[0]);

		double start, end;
		Rect Rect_kcf_i, Rect_kcf;
		
		for (int j = 2; j < num_of_line; j++)
		{
			cout << "frame\t" << j << endl;
			string img_name = zeros8 + std::to_string(j);
			string img_path = path + string(img_name.end() - 8, img_name.end()) + ".jpg";
			cv::Mat frame = imread(img_path);      //��ȡ��ǰ����Ƭ
			start= static_cast<double>(getTickCount());
			Rect_kcf = kcf_tracker.Update1(frame);
			Rect_kcf_i = kcf_tracker_i.Update(frame); 
			rectangle(frame, Rect_kcf, Scalar(0, 0, 255));
			rectangle(frame, Rect_kcf_i, Scalar(0, 255, 0));

			imshow("test", frame);
			waitKey(10);
		}

		destroyAllWindows();

	
		







	}

	//
	//


	////������
	////for (unsigned i = 0; i < video_num;i++)
	////{
	//   // video.read(frame);
	//   // imshow("��Ƶ", frame);
	//   // waitKey(10);
	////}
	//ofstream pos("pos_kcf2.txt");       //����
	//ofstream pos_tt("pos_tempTrack.txt");
	////��ͷ
	////pos << "x" << "\t" << "y" << "\t" << "width"
	////	<< "\t" << "height" << "\t" << "center.x" << "\t " << "center.y" << "\n";
	//Rect TemTrackingRes;
	//Mat model;
	//Rect TrackBox = groundtruth_rect[0];

	//for (unsigned frame = 0; frame < video_num; ++frame)   //����ͼƬ����
	//{


	//	video.read(image);
	//	/*Mat img_cut;
	//	img_cut = image(Range(215, 380),Range(175, 460));
	//	imwrite(".\\��ͼ\\" + to_string(frame) + ".jpg", img_cut);*/
	//	//image = cv::imread(image_files[frame]);
	//	//imwrite("test.jpg", image);
	//	tic = getTickCount();
	//	
	//		if (frame == 0)
	//		{    //��һֱ֡�ӳ�ʼ������
	//			cv::String frame1 = "frame1";
	//			imshow(frame1, image);
	//			
	//			Mat img_gray;
	//			cvtColor(image, img_gray, CV_RGB2GRAY);
	//			
	//			
	//			//model = img_gray(TrackBox);       //��ȡģ��
	//			//imshow("model", model);
	//		

	//			//groundtruth_rect[0] = get_rec_pos(image);
	//			//groundtruth_rect[0] = GetRoi(frame1, image);

	//			//�������¼ӵ�һ��������ѡ�����ʱ��ESC,�᷵��һ��REC�Ķ�����Գ�ʼ���������ĳ�ʼλ��
	//			//����˵��װ��һ����ȽϺã�����û�з�װ�ɹ�����Ҫԭ�������ص�����������Ϊ���Ա����������
	//			//����ԪҲ���ܷ��ʳ�Ա������˵�ǿ��Է��ʵģ����ż������Ч�������������Ŀ�в���װҲ���Ǻ�Ӱ�죩
	//			//���Ծ���д��ͷ�ļ��ˡ�
	//			cout << "���ٿ��С" << groundtruth_rect[0] << endl;
	//			kcf_tracker.Init(image, groundtruth_rect[0]);
	//			kcf_trackerG.Init(image, groundtruth_rect[0]);
	//			result_rect.push_back(groundtruth_rect[0]);     //0-index
	//			result_rectG.push_back(groundtruth_rect[0]);
	//		}
	//		//result_rect����ǳ�����ٵ���λ��
	//		else
	//		{
	//			//tracking(image, model, TrackBox);
	//			//imshow("model", model);
	//			result_rect.push_back(kcf_tracker.Update(image));
	//			result_rectG.push_back(kcf_trackerG.Update1(image));
	//		}
	//		
	//		if (show_visualization) {
	//			cv::putText(image, to_string(frame + 1), cv::Point(20, 40), 6, 1,
	//				cv::Scalar(0, 255, 255), 2);
	//			//cv::rectangle(image, groundtruth_rect[frame], cv::Scalar(0, 255, 0), 2);
	//			cv::rectangle(image, TrackBox, cv::Scalar(0, 255, 0), 1);
	//			//cv::circle(image, Point(TrackBox.x + 0.5*TrackBox.width, TrackBox.y + 0.5*TrackBox.height), 1, Scalar(0, 255, 0));
	//			cv::rectangle(image, result_rect[frame], cv::Scalar(0, 0, 255), 1);
	//			cv::rectangle(image, result_rectG[frame], cv::Scalar(0, 255,255), 1);
	//			//cv::circle(image, kcf_tracker.getPos(), 3, Scalar(0, 0, 255), -1);  //��Ŀ������ĵ㻭����
	//			cv::imshow("tracking", image);

	//			//imwrite(to_string(frame) + ".jpg", image);
	//			//�������txt
	//			//Rect Pos_kcf = kcf_tracker.getRec();
	//			//pos << Pos_kcf.x +0.5*Pos_kcf.width<< "\t" << Pos_kcf.y+0.5*Pos_kcf.height << "\n";
	//			pos_tt << TrackBox.x + 0.5*TrackBox.width << "\t" << TrackBox.y + 0.5*TrackBox.height << "\n";
	//			//	<< Pos.height << "\t" << Pos.x + Pos.width / 2 << "\t" << Pos.y + Pos.height / 2 << "\n";
	//			
	//			cv::imwrite(".\\image1\\" + std::to_string(frame) + ".jpg", image);
	//			char key = cv::waitKey(1);
	//			if (key == 27 || key == 'q' || key == 'Q')
	//				break;
	//			

	//	}
	//}
	//	time = time / double(getTickFrequency());
	//	double fps = double(result_rect.size()) / time;
	//	cout << "֡��: ----------" << fps << endl;
	//	/*std::vector<double> precisions = PrecisionCalculate(groundtruth_rect,
	//														result_rect);
	//	std::cout<<"%12s - Precision (20px) : %1.3f, FPS : %4.2f\n"<<
	//		   video_base_path.c_str()<<" "<<precisions[20]<<"  "<< fps<<endl;*/
	//	cv::destroyAllWindows();

	
		
	
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

