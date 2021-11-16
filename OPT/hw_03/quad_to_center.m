function [x0, y0, r] = quad_to_center(d,e,f)
% function [x0 y0 r] = quad_to_center(d,e,f)
%
% INPUT: 
% quadric coordinates of a circle
%
% OUTPUT: 
% cartesian coordinates of the circle
 
x0 = -d/2;
y0 = -e/2;
r = sqrt((d^2 + e^2)/4 - f);
end
