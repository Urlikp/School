function x = vyhra2(c, k, m)
%[~, N] = size(c);
N = 3;
f = [zeros(1, N), -1];
A = [-c(1) zeros(1, N - 1) 1; zeros(1, (N - 1)/2) -c(2) zeros(1, (N - 1)/2) 1; zeros(1, N - 1) -c(3) 1];
[M, ~] = size(A);
b = zeros(M, 1);
Aeq = [ones(1, N) 0];
beq = k;
lb = [zeros(1, N) + m 0];

LP = linprog(f, A, b, Aeq, beq, lb);
x = LP(1:N)';
end

