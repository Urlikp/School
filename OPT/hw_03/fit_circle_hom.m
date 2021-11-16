function [d, e, f] = fit_circle_hom(X)
% function [d e f] = fit_circle_hom(X)
%
% INPUT: 
% X: n-by-2 vector
%    with data
%
% OUTPUT: 
% quadric coordinates of the circle
 
[N, ~] = size(X);
S = ones(N, 1);
A = ones(N, 4);

for row = 1:N
    S(row) = X(row, 1)^2 + X(row, 2)^2;
    A(row,:) = [S(row) X(row, 1) X(row, 2) 1];
end

[V, ~] = eig(A' * A);

a = V(1, 1);
d = V(2, 1)/a;
e = V(3, 1)/a;
f = V(4, 1)/a;
end
