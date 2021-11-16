function [U,C] = correct_fitlin(A,k)
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

[Y,~] = eig(A*A');
U = Y(:,end-k+1:end);
C = U'*A;
