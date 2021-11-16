function [U,C,b0] = correct_fitaff(A,k)
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

b0 = mean(A,2);
B = bsxfun(@minus, A, b0);
[Y,~] = eig(B*B');
U = Y(:,end-k+1:end);
C = U'*B;
