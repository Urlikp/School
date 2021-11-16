function [x y r] = GN_iter_correct(X, x0, y0, r0)
% [x y r] = GN_iter(X, x0, y0, r0)
%
% makes the Gauss-Newton iteration. 
%
% INPUT: 
% X: n-by-2 vector
%    with data
% x0, y0 are the coordinates of the circle center.
% r0 is the circle radius
%
% OUTPUT: 
% coordinates and radius after one step of GN method.

r = myF(x0, y0, X);
d = dist(X,x0,y0,r0);
tmp = r\d';

x = x0 - tmp(1);
y = y0 - tmp(2);
r = r0 - tmp(3);

end
 
function r = myF(x0, y0, X) 
    [s,~] = size(X);
    oneVector = ones(s,1);
    tmpX = (x0 - X(:,1));
    tmpY = (y0 - X(:,2));
    d = sqrt(tmpX.^2 + tmpY.^2);
    r = [tmpX./d, tmpY./d, -oneVector];
end
