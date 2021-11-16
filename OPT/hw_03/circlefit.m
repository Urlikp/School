close all
clc
clear

load('data.mat')
% X contain only inlier data
% A contain the same data and outliers
%
% GT.mat contains the GT outputs for the data.
% e.g., def_nh = fit_circle_nhom(X)
% x0y0r_nh = ...
% quad_to_center(def_nh(1), def_nh(2), def_nh(3))
% dnh = dist(X, x0y0r_nh(1), x0y0r_nh(2), x0y0r_nh(3))
% etc.

figure;
subplot(1,3,1);
[d, e, f] = fit_circle_nhom(X);
[x0, y0, r] = quad_to_center(d,e,f);
plot_circle(x0, y0, r, 'b', 'nhom');
plot(X(:, 1), X(:, 2), '.r');
axis equal;

subplot(1,3,2);
[d, e, f] = fit_circle_hom(X);
[x0, y0, r] = quad_to_center(d,e,f);
plot_circle(x0, y0, r, 'g', 'hom');
plot(X(:, 1), X(:, 2), '.r');
axis equal;

subplot(1,3,3);
[x0, y0, r] = fit_circle_ransac(A,2000,0.1);
plot_circle(x0, y0, r, 'y', 'ransac')
plot(A(:, 1), A(:, 2), '.r');
axis equal;


