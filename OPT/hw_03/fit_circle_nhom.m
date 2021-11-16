function [d, e, f] = fit_circle_nhom(X)
% function [d e f] = fit_circle_nhom(X)
%
% INPUT: 
% X: n-by-2 vector
%    with data
%
%
% OUTPUT: 
% quadric coordinates of the circle

[N, ~] = size(X);
B = ones(N, 1);
A = ones(N, 3);

for row = 1:N
    B(row) = -(X(row, 1)^2 + X(row, 2)^2);
    A(row,:) = [X(row, 1) X(row, 2) 1];
end

V = A\B;

d = V(1, 1);
e = V(2, 1);
f = V(3, 1);
end
