function x=vyhra2_correct(c,k,m)
% function that returns values we should bet on each events to maximize
% income
%
% INPUT:
%       c: Fixed-odds betting on each event
%       k: Max money we can bet
%       m: Minimum bet on each event
%
% OUTPUT:
%       x: vector of how much money we should bet on each event

maxBet = k;
minBet = m;

f = [0 0 0 -1];

A = [-c(1) 0     0     1
     0     -c(2) 0     1
     0     0     -c(3) 1];
b = [0 0 0];

Aeq = [1 1 1 0];
beq = maxBet;

lb = [minBet minBet minBet -maxBet];

result = linprog(f,A,b,Aeq,beq,lb);

x = result(1:3)';

end