function [U, C, b0] = fitaff(A, k)
% function [U,C,b0] = fitaff(A,k)
%
% INPUT: 
% A: m-by-n matrix
%    with data
% k: scalar, dimension of affine approximation
%
% OUTPUT:
% U: m-by-k matrix
%	columns form an orthonormal basis
% C: k-by-n matrix
%	columns contain coordinates w.r.t the basis
% b0: m-by-1 matrix
%	point of the affine space
%

[~, N] = size(A);
b0 = A * ones(N, 1) / N;
B = A - b0;
[U, C] = fitlin(B, k);

end