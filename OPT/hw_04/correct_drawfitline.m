function correct_drawfitline(A)
% function drawfitline(A)
%
% INPUT: 
% A: 2-by-m matrix
%    with data
%

plot(A(1,:), A(2,:), 'xr');
hold on; axis equal;

[U,C,b0] = fitaff(A,1);

X = [U(1)*min(C(1,:)) + b0(1), U(1)*max(C(1,:)) + b0(1)];
Y = [U(2)*min(C(1,:)) + b0(2), U(2)*max(C(1,:)) + b0(2)];
plot(X, Y, 'g');

for i = 1:size(C,2)
    plot([A(1,i) U(1)*C(i)+b0(1)], [A(2,i) U(2)*C(i)+b0(2)], 'r');
end
hold off;
end
