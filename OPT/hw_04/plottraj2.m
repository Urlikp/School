function plottraj2(C)
% function plottraj2(C)
%
% INPUT: 
% C: m-by-2 matrix
%    with data
%

[~, N] = size(C);
N = N - 1;

hold on;
for i = 1:N
    plot([C(1, i), C(1, i + 1)], [C(2, i), C(2, i + 1)], 'r');
end
hold off;

end