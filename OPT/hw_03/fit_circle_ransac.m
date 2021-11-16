function [x0, y0, r] = fit_circle_ransac(X, num_iter, threshold)
% function [x0 y0 r] = fit_circle_ransac(X, num_iter, threshold)
%
% INPUT: 
% X: n-by-2 vector
%    with data
% num_iter: number of RANSAC iterations
% threshold: maximal  distance of an inlier from the circumference
%
% OUTPUT: 
% cartesian coordinates of the circle
 
[N, ~] = size(X);
x0 = 0;
y0 = 0;
r = 0;
inlier_count = 0;

for iter = 1:num_iter
   ran = randi([1, N], 1, 3);
   ranX = ones(3, 2);
   
   for iter2 = 1:3
       ranX(iter2,:) = X(ran(iter2),:);
   end
   
   [d, e, f] = fit_circle_nhom(ranX);
   [temp_x0, temp_y0, temp_r] = quad_to_center(d, e, f);
   distX = dist(X, temp_x0, temp_y0, temp_r);
   
   temp_inlier_count = 0; 
   
   for iter2 = 1:N
       if (abs(distX(iter2)) < threshold)
           temp_inlier_count = temp_inlier_count + 1;
       end
   end
   
   if (temp_inlier_count > inlier_count)
      x0 = temp_x0;
      y0 = temp_y0;
      r = temp_r;
      inlier_count = temp_inlier_count;
   end  
end
end
