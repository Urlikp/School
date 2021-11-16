function correct_plottraj2(C)
% function plottraj2(C)
%
% INPUT: 
% C: 2-by-m matrix
%    with data
%

size(C)

for i = 2:size(C,2)
    plot([C(1,i-1) C(1,i)], [C(2,i-1) C(2,i)]);
    hold on;
end

hold off;
end
