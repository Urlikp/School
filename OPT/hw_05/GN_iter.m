function [x, y, r] = GN_iter(X, x0, y0, r0)
% [x y r] = GN_iter(X, x0, y0, r0)
%
% makes the Gauss-Newton iteration. 
%
% INPUT: 
% X: n-by-2 matrix
%    with data
% x0, y0 are the coordinates of the circle center.
% r0 is the circle radius
%
% OUTPUT: 
% coordinates and radius after one step of GN method.

[N, ~] = size(X);
dx = zeros(N, 1);
dy = zeros(N, 1);
d = zeros(N, 1);
g = zeros(N, 3);

for i = 1:N
    dx(i) = x0 - X(i, 1);
    dy(i) = y0 - X(i, 2);
    d(i) = sqrt(dx(i)^2 + dy(i)^2);
    g(i, :) = [dx(i)/d(i), dy(i)/d(i), -1];
end

GN = g\dist(X, x0, y0, r0)';
x = x0 - GN(1);
y = y0 - GN(2);
r = r0 - GN(3);
end
 
