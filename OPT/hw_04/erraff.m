function d = erraff(A)
% function d = erraff(A)
%
% INPUT: 
% A: m-by-n matrix
%    with data
%
% OUTPUT:
% d: m-by-1 matrix
%

[M, ~] = size(A);
d = zeros(M, 1);

[~, N] = size(A);
b0 = A * ones(N, 1) / N;
B = A - b0;
[V, ~, ~] = svd(B);

for i = 1:M
    U = V(:, 1:i);
    C = U' * B;
    D = U * C + b0;
    X = A - D;
    d(i) = norm(X, 'fro')^2;
end

end