function x = fit_wages(t,M)
    A = [ones(size(t)) t];
    x = A\M;
end

