#include<opencv2/core/core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\/imgproc\imgproc.hpp>
#include<iostream>
#include<string>
#include<fstream>
#include"get_rec_pos.h"
using namespace std;
using namespace cv;





using std::vector;
using cv::Mat;




void Image_To_Video(int frame_start, int frame_end, VideoCapture video);
int main()
{

	VideoCapture video("tracking.mp4");
	Mat img;
	ofstream pos("pos.txt");
	//��ͷ
	pos << "x"<< "\t" << "y" << "\t" << "width" 
		<< "\t" << "height" <<"\t" <<"center.x"<<"\t "<<"center.y"<<"\n";
	
	for (unsigned i = 0; i < video.get(CAP_PROP_FRAME_COUNT); i++)
	{
		video.read(img);
		Rect Pos = get_rec_pos(img);
		rectangle(img, Pos, Scalar(255, 0, 0));
		imshow("test", img);
		waitKey(10);
		cout << i << endl;
		pos << Pos.x << "\t" << Pos.y << "\t" << Pos.width << "\t" 
			<< Pos.height << "\t" << Pos.x+Pos.width/2 <<"\t"<<Pos.y+Pos.height/2<< "\n";
		
	}
	cout << "done" <<"\a"<< endl;
	return 0;
}

void Image_To_Video(int frame_start,int frame_end,VideoCapture video)   //����Ӧ������ʼ�ͽ���֡�Լ�ԭ��Ƶ����
{
	string video_name = "data_cut1.avi";   //��Ƶ����
	int hight = video.get(CAP_PROP_FRAME_HEIGHT);   //��͸߱��ֲ���
	int wed = video.get(CAP_PROP_FRAME_WIDTH);
	VideoWriter writer;             //
	writer = VideoWriter(video_name, -1, video.get(CAP_PROP_FPS), Size(wed,hight), 0);
	//д��Ķ��󣬱���ԭ����֡�ʺʹ�С��
	//writer.open(video_name,-1, video.get(CAP_PROP_FPS), Size(wed, hight), true);  
	Mat image;
	for (unsigned i = 0; i < video.get(CAP_PROP_FRAME_COUNT); i++)
	{
		cout << i << endl;
		video.read(image);
		imshow(" ", image);
		waitKey(20);
		if (i >= frame_start&&i < frame_end)
		{
			/*imshow("test", image);
			waitKey(10);*/
			writer.write(image);
		}
		if (i == frame_end)
		{
			cout << "transform task was done!" << endl;
			break;
		}
			
	}
	
}



