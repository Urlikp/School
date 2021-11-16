function drawfitline(A)
% function drawfitline(A)
%
% INPUT: 
% A: m-by-2 matrix
%    with data
%
 
[U, C, b0] = fitaff(A, 1);
[~, N] = size(A);

Beg = U * min(C) + b0;
End = U * max(C) + b0;

hold on;
plot(A(1, :), A(2, :), 'rx');
plot([Beg(1), End(1)], [Beg(2), End(2)], 'g');

for i = 1:N
    Beg = U * C(i) + b0;
    End = A(:, i);
    plot([Beg(1), End(1)], [Beg(2), End(2)], 'r');
end

hold off;
axis equal;

end