function [a] = ar_fit_model(y, p)
% function [a, M, b] = ar_fit_model(y)
%
% INPUT:
% y : N-by-1 vector, sound signal
%
% p : required order of AR model
%
% OUTPUT:
% a: (p+1)-by-1 vector, estimated parameters of AR model 
% 
% ordering of vector *a* is such that: 
%   a(1) = a_0 [ from Eq. (2) in this Task description ]
%   a(2) = a_1 [ from Eq. (2) in this Task description ]
%   .
%   .
%   a(p+1) = a_p [ from Eq. (2) in this Task description ]
%
% M, b: matrix and vector as in the Task description, 
%       M*a = b (in LSQ sense) 
% discard the code from here and implement the functionality:
b = y(p+1:end,1);
T = length(y) - 1;
M = zeros(T-p+1,p+1);
M(1:end,1) = 1;
for row = 1:length(M)
    yStart = p+row-1;
    M(row, 2:end) = y(yStart:-1:row);
end
a = M\b;
end
