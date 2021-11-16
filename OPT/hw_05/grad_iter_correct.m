function [x y r] = grad_iter_correct(X, x0, y0, r0, a)
% [x y r] = grad_iter(X, x0, y0, r0, a)
%
% makes the gradient method iteration. 
%
% INPUT: 
% X: n-by-2 vector
%    with data
% x0, y0 are the coordinates of the circle center.
% r0 is the circle radius
% a is the stepsize
%
% OUTPUT: 
% coordinates and radius after one step of gradient method.

[m,~] = size(X);
x = X(:,1);
y = X(:,2);

f = sqrt((x-x0).^2 + (y-y0).^2)-r0;
dfX = -(-x0 + x)./sqrt((x0 - x).^2 + (y0 - y).^2);
dfY = -(-y0 + y)./sqrt((x0 - x).^2 + (y0 - y).^2);
dfR = -1.*ones(m,1);

tmp(:,1) = dfX;
tmp(:,2) = dfY;
tmp(:,3) = dfR;

gradDf = f'*tmp*2;

x = x0 - a*gradDf(:,1);
y = y0 - a*gradDf(:,2);
r = r0 - a*gradDf(:,3);

end
 
