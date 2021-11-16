function [x, y, r, success] = LM_iter(X, x0, y0, r0, mu)
% [x y r success] = LM_iter(X, x0, y0, r0, mu)
%
% makes the Levenberg-Marquardt iteration. 
%
% INPUT: 
% X: n-by-2 matrix
%    with data
% x0, y0 are the coordinates of the circle center.
% r0 is the circle radius
% mu is the damping factor (the factor which multiplies the
% regularizing identity matrix)

% OUTPUT: 
% success == 1 if the iteration is successful, i.e. 
% value of criterion f is decreased after the update 
% of x. 
% success == 0 in the oposite case. 
%
% x,y,r are updated parameters if success == 1. 
% x,y,r = x0,y0,r0 if success == 0.

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

LM = (g' * g + mu * eye(3))\g' * dist(X, x0, y0, r0)';
x = x0 - LM(1);
y = y0 - LM(2);
r = r0 - LM(3);

if (norm(dist(X, x, y, r)) < norm(dist(X, x0, y0, r0)))
    success = 1;
else
    success = 0;
    x = x0;
    y = y0;
    r = r0;
end
end
