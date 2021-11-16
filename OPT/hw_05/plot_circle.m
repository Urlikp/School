function h = plot_circle(x0, y0, r, varargin)
	phi = 0:0.01:2*pi;
	x = r * cos(phi) + x0;
	y = r * sin(phi) + y0;
	h = plot(x, y, varargin{:});
  axis equal
end
