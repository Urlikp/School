function x=vyhra_correct(c,k)
% function that returns values we should bet on each events to maximize
% income
%
% INPUT:
%       c: Fixed-odds betting on each event
%       k: Max money we can bet
%
% OUTPUT:
%       x: vector of how much money we should bet on each event

f = [0 0 0 0 0 -1];

A = [-c(1) -c(2) 0 0 0 1;
     0 -c(2) -c(3) -c(4) 0 1;
     0 0 0 -c(4) -c(5) 1];
b = [0 0 0];

Aeq = [1 1 1 1 1 0];
beq = k;

lb = [0 0 0 0 0 -k];

result = linprog(f,A,b,Aeq,beq,lb);

x = result(1:5)';

end