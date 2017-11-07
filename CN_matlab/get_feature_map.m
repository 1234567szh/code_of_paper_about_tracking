function out = get_feature_map(im_patch, features, w2c)

% out = get_feature_map(im_patch, features, w2c)
%
% Extracts the given features from the image patch. w2c is the
% Color Names matrix, if used.

if nargin < 3
    w2c = [];
end

% the names of the features that can be used
valid_features = {'gray', 'cn'};    %���õ�����

% the dimension of the valid features
feature_levels = [1 10]';         %

num_valid_features = length(valid_features);   %2
used_features = false(num_valid_features, 1);  % 2*1�ľ���ȫ0

% get the used features
for i = 1:num_valid_features    %2
    used_features(i) = any(strcmpi(valid_features{i}, features));   %������һ������ֻ��һ����ʱ��Ż��use_feature�����Ϊ1
end
% used_features  [1,0]         [0,1]         [1,  1]
%                gray no       no cn         gray  cn
% total number of used feature levels
num_feature_levels = sum(feature_levels .* used_features);
%ע���ǵ�ˣ��ûҶȵĻ���1����cn�Ļ���10

level = 0;

% If grayscale image
if size(im_patch, 3) == 1      %����ǻҶ�ͼ��Ļ���ֻ���ûҶ�������
    % Features that are available for grayscale sequances
    
    % Grayscale values (image intensity)
    out = single(im_patch)/255 - 0.5;    %��һ��ȥ����
else 
    % Features that are available for color sequances
    
    % allocate space (for speed)   �ȷ���ռ䣬
    out = zeros(size(im_patch, 1), size(im_patch, 2), num_feature_levels, 'single');    %��������ͨ����singleռ�ĸ��ֽڣ��������Կ죬�ĸ��ֽ�Ҳ��ȫ����
    
    % Grayscale values (image intensity)
    if used_features(1)        %����ûҶȵĻ�
        out(:,:,level+1) = single(rgb2gray(im_patch))/255 - 0.5;    %�ҶȻ�->��һ��->���Ļ�������һͨ���������gray��Ҳ��ֻ��һ��ͨ����
        level = level + feature_levels(1);       %��ʱlevel=0+1Ϊ1
    end
    
    % Color Names       ���������ɫ�ռ�Ļ�
    if used_features(2)
        if isempty(w2c)   %���û�м��ص�w2c
            % load the RGB to color name matrix if not in input
            temp = load('w2crs');
            w2c = temp.w2crs;
        end
        
        % extract color descriptor
        out(:,:,level+(1:10)) = im2c(single(im_patch), w2c, -2);   %�����������һ��������,�õ�ʮ��ͨ���������������ϸ�ڻ�û�и㶮���ٿ�
        level = level + feature_levels(2);      %��һ������ͨ�����������10��
    end
end