function d = correct_erraff(A)
% function d = erraff(A)
%
% INPUT: 
% A: m-by-n matrix
%    with data
%
% OUTPUT:
% d: m-by-1 matrix
%

b0 = mean(A,2);
B = bsxfun(@minus, A, b0);
[Y,~] = eig(B*B');
d = zeros(size(B,1), 1);

for i = 1:size(B,1)
    U = Y(:,end-i+1:end);
    C = U'*B;
    D = U*C + b0;
    diff = A - D;
    diff = bsxfun(@times, diff, diff);
    d(i) = sum(sum(diff, 1));
end
