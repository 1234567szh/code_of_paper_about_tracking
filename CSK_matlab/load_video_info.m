function [img_files, pos, target_sz, resize_image, ground_truth, ...
	video_path] = load_video_info(video_path)
%LOAD_VIDEO_INFO
%   Loads all the relevant information for the video in the given path:
%   the list of image files (cell array of strings), initial position
%   (1x2), target size (1x2), whether to resize the video to half
%   (boolean), and the ground truth information for precision calculations
%   (Nx2, for N frames). The ordering of coordinates is always [y, x].
%
%   The path to the video is returned, since it may change if the images
%   are located in a sub-folder (as is the default for MILTrack's videos).
%   �������£�
%   ���ظ���·������Ƶ�����������Ϣ��
%   1���ļ��б�
%   2����ʼλ�ã�1*2��
%   3��Ŀ���С��1*2��
%   4���Ƿ�����ͼƬ��bool�ͣ�
%   5��ground_truth��N*2��,һ��N֡��������y��x.
%      ����������Ŀ����ÿһ֡��׼ȷλ�ã�Ϊͳ��׼ȷ���á�
%   ͼ��·��Ҳ���أ���Ϊ����������ļ��У����·��Ҳ�ᷢ���仯    
%   Jo�o F. Henriques, 2012
%   http://www.isr.uc.pt/~henriques/

	%load ground truth from text file (MILTrack's format)
	text_files = dir([video_path '*_gt.txt']);
    %��ý�βΪ_gt.txt���ļ���
	assert(~isempty(text_files), 'No initial position and ground truth (*_gt.txt) to load.')
    %��������Ľ�����׳��쳣���﷨�ο���������,���gt.txtû�в������׳��쳣
	f = fopen([video_path text_files(1).name]);
    %��gt�ļ�
	ground_truth = textscan(f, '%f,%f,%f,%f');    %[x, y, width, height]
    %������е���
	ground_truth = cat(2, ground_truth{:});
    %���ƶ�ά���������飬2��ʾ���򣬾���ο���������
	fclose(f);
	
	%set initial position and size
	target_sz = [ground_truth(1,4), ground_truth(1,3)];
    %��ʼ��С����gt.txt�ĵ�һ�������
	pos = [ground_truth(1,2), ground_truth(1,1)] + floor(target_sz/2);
	%��ʼλ�ã���gt.txt�ĵ�һ������ã����������ǡ���෴��������ϴ�С��һ�룿
    %gt.txt�����λ�������Ͻǵ�λ�ã�   
    
	%interpolate missing annotations, and store positions instead of boxes
    %gt.txt�����λ����ÿ��֡��һ��������Ҫ���в�ֵȷ��ȱʧ��λ��
	try
		ground_truth = interp1(1 : 5 : size(ground_truth,1), ...
			ground_truth(1:5:end,:), 1:size(ground_truth,1));
        %���Բ�ֵ��������������
		ground_truth = ground_truth(:,[2,1]) + ground_truth(:,[4,3]) / 2;
        %������е�λ��
	catch  %#ok, wrong format or we just don't have ground truth data.
		ground_truth = [];
    end
    %������͵õ�����֡Ŀ�������λ�ã�������Ҫ��Ϊ�����۸���Ч���õġ�
	
    
	%list all frames. first, try MILTrack's format, where the initial and
	%final frame numbers are stored in a text file. if it doesn't work,
	%try to load all png/jpg files in the folder.
	%�������֡�����ȣ���ȥ������û��format�ļ������Ի��֡����Ϣ�����û�еĻ���ȥ�����е�png����jpg�ļ�
    
	text_files = dir([video_path '*_frames.txt']);
	if ~isempty(text_files),
		f = fopen([video_path text_files(1).name]);
		frames = textscan(f, '%f,%f'); 
        %���ļ������Ϣ��
		fclose(f);
		
		%see if they are in the 'imgs' subfolder or not
		if exist([video_path num2str(frames{1}, 'imgs/img%05i.png')], 'file'),
            %img%05i.png����˼�ǿ��ļ������Ƿ��б��Ϊ 0001��0002�Һ�׺Ϊjpg��png��ͼƬ������з���TRUE
			video_path = [video_path 'imgs/'];   %�������ͼƬ���Ӹ���video_path
        elseif ~exist([video_path num2str(frames{1}, 'img%05i.png')], 'file'),
			error('No image files to load.')
		end
		
		%list the files
		img_files = num2str((frames{1} : frames{2})', 'img%05i.png');
        %����������ǰ�����ת��Ϊ�ַ���,û����dir���������Ǳ�������ɣ������else������dir���ģ�������õ��ַ����Ϳ���ֱ����
		img_files = cellstr(img_files);
        %���ַ�����Ϊ�ַ���ϸ����
	else
		%no text file, just list all images
		img_files = dir([video_path '*.png']);
		if isempty(img_files),
			img_files = dir([video_path '*.jpg']);
			assert(~isempty(img_files), 'No image files to load.')
		end
		img_files = sort({img_files.name});
	end
	
	
	%if the target is too large, use a lower resolution - no need for so
	%much detail
    %���Ŀ��̫�������ɸ���ʱ���������ӣ�����resize�������С��100
	if sqrt(prod(target_sz)) >= 100,   %�������ȴ���100����С
        %prod�ǰѾ����ÿһ���������������ֻ��һ�оͰ���һ�����ˣ��������÷�����������
		pos = floor(pos / 2);
		target_sz = floor(target_sz / 2);
		resize_image = true;
	else
		resize_image = false;
	end
end

