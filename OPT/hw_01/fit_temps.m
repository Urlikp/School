function x = fit_temps(t,T,omega)
    A = [ones(size(t)) t sin(omega * t) cos(omega * t)];
    x = A\T;
end

