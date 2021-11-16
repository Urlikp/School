function [x y r success] = LM_iter_correct(X, x0, y0, r0, mu)
% [x y r success] = LM_iter(X, x0, y0, r0, mu)
%
% makes the Levenberg-Marquardt iteration. 
%
% INPUT: 
% X: n-by-2 vector
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

r = myF(x0, y0, X);
[~,i] = size(r);
d = dist(X,x0,y0,r0);

tmp = ((r.'*r + mu * eye(i))^(-1))*r.'*d';
x = x0 - tmp(1);
y = y0 - tmp(2);
r = r0 - tmp(3);

if (norm(dist(X, x, y, r)) < norm(dist(X, x0, y0, r0)))
    success = 1;
else
    success = 0;
    x = x0;
    y = y0;
    r = r0;
end

end

function r = myF(x0, y0, X) 
    [s,~] = size(X);
    oneVector = ones(s,1);
    tmpX = (x0 - X(:,1));
    tmpY = (y0 - X(:,2));
    d = sqrt(tmpX.^2 + tmpY.^2);
    r = [tmpX./d, tmpY./d, -oneVector];
end
