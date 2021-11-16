function [a,b,r]=minimaxfit_correct(x,y)
% function that counts parameters a,b and r
%
%
%
%

[m,n] = size(x);

f = [zeros(1, m+1), 1];

A = [x' ones(n,1) -ones(n,1);
    -x' -ones(n,1) -ones(n,1)];
b = [y -y];

result = linprog(f,A,b);

a = result(1:m);
b = result(m+1);
r = max(abs(a'*x+b-y));
end