function [b0, B] = pivot(A, b, x, y)
[N, M] = size(A);
B = [A b];
B(x, :) = B(x, :)/B(x, y);

for i = 1:N
    if i ~= x
        B (i, :) = B(i, :) - B(i, y) * B(x, :);
    end
end

b0 = B(:, M + 1);
B = B(:, 1:M);

if b0 >= 0
    fprintf('x = %d, y = %d is pivot', x, y)
else
    fprintf('x = %d, y = %d is not pivot', x, y)
end
end