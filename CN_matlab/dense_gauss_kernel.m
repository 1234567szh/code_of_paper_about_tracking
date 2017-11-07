function k = dense_gauss_kernel(sigma, x, y)

% k = dense_gauss_kernel(sigma, x, y)
%
% Computes the kernel output for multi-dimensional feature maps x and y
% using a Gaussian kernel with standard deviation sigma.

xf = fft2(x);      %x in Fourier domain
xx = x(:)' * x(:);  %squared norm of x

if nargin >= 3,  %general case, x and y are different
    yf = fft2(y);
    yy = y(:)' * y(:);
else
    %auto-correlation of x, avoid repeating a few operations
    yf = xf;
    yy = xx;
end

%cross-correlation term in Fourier domain
xyf = xf .* conj(yf);          %��Ӧά���
xy = real(ifft2(sum(xyf,3)))/3;    %to spatial domain,�����Ǹ�����ֱ�Ӽ�������Ȼ������������Ҷ�任����KCF�е�����
%KCF��ÿ��ͨ����ifft֮��Ȼ������������������Ҿ��ü������������ȽϺã���һ����ֵ����һЩ���Ͼ��ǰ�����ͨ����ֵ�������˰���
%���ó�����֮�����Ҳ����ʵ�飬�Խ������û��Ӱ�죬׼ȷ�ʺ�֡�ʶ�û��ʲôӰ��

%calculate gaussian response for all positions
k = exp(-1 / sigma^2 * max(0, (xx + yy - 2 * xy) / numel(x)));
end