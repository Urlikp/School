function plotline(x, y, a, b, r)

Beg = [min(x), a * min(x) + b];
End = [max(x), a * max(x) + b];

hold on;
plot(x, y, 'bx');
plot([Beg(1), End(1)], [Beg(2), End(2)], 'g');
plot([Beg(1), End(1)], [Beg(2), End(2)] + r, 'r');
plot([Beg(1), End(1)], [Beg(2), End(2)] - r, 'r');
hold off;
axis tight equal;

end