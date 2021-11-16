function [a, b, r] = minimaxfit(x, y)
[N, M] = size(x);
f = [zeros(1, N + 1), 1];
A = [x' ones(M, 1) -ones(M, 1); -x' -ones(M, 1) -ones(M, 1)];
b0 = [y'; -y'];

LP = linprog(f, A, b0);
a = LP(1:N);
b = LP(end - 1);
r = LP(end);
end

