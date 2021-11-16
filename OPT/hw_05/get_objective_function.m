function [f] = get_objective_function(X)
% [f] = get_objective_function(X)
%
% returns objective function
%
% INPUT: 
% X: n-by-2 vector
%    with data
%
% OUTPUT: 
% function f such that f(c) = sum(dist(X, c(1), c(2), c(3)).^2).

f = @(c) (sum(dist(X, c(1), c(2), c(3)).^2));

end
 
