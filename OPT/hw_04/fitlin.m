function [U, C] = fitlin(A, k)
% function [U,C] = fitlin(A,k)
%
% INPUT: 
% A: m-by-n matrix
%    with data
% k: scalar, dimension of linear approximation
%
% OUTPUT:
% U: m-by-k matrix
%	columns form an orthonormal basis
% C: k-by-n matrix
%	columns contain coordinates w.r.t the basis
%

[U, ~, ~] = svd(A);
U = U(:, 1:k);
C = U' * A;

end