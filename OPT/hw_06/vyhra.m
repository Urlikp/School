function x = vyhra(c, k)
[~, N] = size(c);
f = [zeros(1, N), -1];
A = [-c(1:2) zeros(1, N - 2) 1; zeros(1, (N - 3)/2) -c(2:4) zeros(1, (N - 3)/2) 1; zeros(1, N - 2) -c(4:5) 1];
[M, ~] = size(A);
b = zeros(M, 1);
Aeq = [ones(1, N) 0];
beq = k;
lb = zeros(1, N + 1);

LP = linprog(f, A, b, Aeq, beq, lb);
x = LP(1:N)';
end

