x = [1 2 3 3 2; 4 1 2 5 6; 7 8 9 -5 7];
y = [7 4 1 2 5];
[a, b, r] = minimaxfit(x,y);
%plotline(x, y, a, b, r);