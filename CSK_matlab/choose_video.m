function video_path = choose_video(base_path)
%CHOOSE_VIDEO
%   Allows the user to choose a video (sub-folder in the given path).
%   Returns the full path to the selected sub-folder.
   %����Ŀ����Ƶ������·�����м䴦�������鷳������Ϊ�˳�����Ѻþ�ֻ������д�ˡ�
   %170905  ������ϸ����ע�͡�
%
%   Jo�o F. Henriques, 2012
%   http://www.isr.uc.pt/~henriques/

	%process path to make sure it's uniform
	if ispc(), base_path = strrep(base_path, '\', '/'); end
    %��base_path���  \  ȫ������  /������matlab��ʶ���·��
	if base_path(end) ~= '/', base_path(end+1) = '/'; end
	%���·�����һ������ /  �����ں������  /
    
	%list all sub-folders
	contents = dir(base_path);   
    %������е����ļ�������һ���ṹ��
	names = {};
	for k = 1:numel(contents), %�ļ��ĸ���
		name = contents(k).name;      %�ļ�������nameϸ����
		if isdir([base_path name]) && ~strcmp(name, '.') && ~strcmp(name, '..'),
            
            %isdir:�ж�·�����Ƿ����ļ��У�����ǣ�����true��
            %strcmp:�ж������ַ����Ƿ���ͬ,�����ͬ,����TRUE;
            %���data���ڸ��ļ����£��ܻ�������������ļ����ֱ�ʱ  .  he  .. ��������˼����
            %Ҫ�ų����������������ļ�������namesϸ�����
			names{end+1} = name;  %#ok  ����names������name
		end
	end
	
	%no sub-folders found
	if  isempty(names),
        video_path = []; 
        return;
    end
    
	
	%choice GUI  %ѡ��ͼ�ν���
	choice = listdlg('ListString',names, 'Name','Choose video', 'SelectionMode','single');
	%�������ο����û�ѡ�񣬵�ѡ��ѡ��names�µģ����ȥ���������������ĺ����
	if isempty(choice),  %user cancelled   
		video_path = [];
        %���û��ѡ��video_path��Ϊ��
	else
		video_path = [base_path names{choice} '/'];
        %���ѡ���ˣ��Ͱ��Ǹ��ļ��д��벢������ /
	end
	
end


