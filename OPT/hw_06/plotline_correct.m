function plotline_correct(x,y,a,b,r)
% function plot line from assigment
%

m = size(y,2);

scatter(x, y, 'k','x');
hold on;

direc = linspace(0,1,m);
line = a*direc + b;

plot(direc, line + r, 'red')
plot(direc, line - r, 'red')
plot(direc, line, 'green')

axis tight equal

end