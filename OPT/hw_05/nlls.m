clf
clear all

x0 = 5;
y0 = 5;
r = 5;
n = 200;
theta = rand(n,1)*2*pi;
X = zeros(n,2);
X(:,1) = r*cos(theta)+x0;
X(:,2) = r*sin(theta)+y0;
X = X + randn(n,2)*1.2;

params = ones(3)*5;
params(:,1:2) = params(:,1:2) + mean(X,1);
d = sum(dist(X, params(1,1), params(1,2), params(1,3)).^2);
mu = 1;
alpha = 0.01;
dists = zeros(3,0);

fun  = get_objective_function(X);
[x, fval] = fminsearch(fun, params(1,:));

h1 = subplot(2,2,1); hold on; title LM
h2 = subplot(2,2,2); hold on; title GN
h3 = subplot(2,2,3); hold on; title grad
h4 = subplot(4,2,6); hold on; title errs
h5 = subplot(4,2,8); hold on; title 'last 5 errs'
axes(h1); plot(X(:,1), X(:,2), '.', 'MarkerSize', 4);
axes(h2); plot(X(:,1), X(:,2), '.', 'MarkerSize', 4);
axes(h3); plot(X(:,1), X(:,2), '.', 'MarkerSize', 4);
h = [];

for i = 1:100
  fprintf('Iterace %i:\n',i);

  x0 = params(1,1);
  y0 = params(1,2);
  r0 = params(1,3);
  d = dist(X, x0, y0, r0);
  dists(1,i) = sum(d.^2);
  [x, y, r, success] = LM_iter(X, x0, y0, r0, mu);
  if success == 1
    params(1,:) = [x;y;r];
    mu = mu/3;
  else
    mu = 2*mu;
  end
  fprintf('  LM: f=%g, success=%i, mu=%g\n',dists(1,i),success,mu);

  x0 = params(2,1); 
  y0 = params(2,2);
  r0 = params(2,3);
  d = dist(X, x0, y0, r0);
  dists(2,i) = sum(d.^2);
  [x, y, r] = GN_iter(X, x0, y0, r0);
  params(2,:) = [x;y;r];
  fprintf('  GN: f=%g\n',dists(2,i));

  x0 = params(3,1);
  y0 = params(3,2);
  r0 = params(3,3);
  d = dist(X, x0, y0, r0);
  dists(3,i) = sum(d.^2);
  [x, y, r] = grad_iter(X, x0, y0, r0, alpha);
  if sum(dist(X, x, y, r).^2) < dists(3,i)
    alpha = alpha*2;
    params(3,:) = [x;y;r];
  else
    alpha = alpha/2;
  end
  fprintf('  GM: f=%g, alpha=%g\n',dists(3,i),alpha);
  
  delete(h)
  axes(h1); h(1) = plot_circle(params(1,1), params(1,2), params(1,3), 'r','LineWidth',2);
  axes(h2); h(2) = plot_circle(params(2,1), params(2,2), params(2,3), 'g','LineWidth',2);
  axes(h3); h(3) = plot_circle(params(3,1), params(3,2), params(3,3), 'b','LineWidth',2);
  
  axes(h4); cla; hold on
  plot(dists(1, :), 'r-' ,'LineWidth',3);
  plot(dists(2, :), 'g--','LineWidth',2);
  plot(dists(3, :), 'b-.','LineWidth',2);
  if i>=5
    axes(h5); cla; hold on
    plot(dists(1, end-4:end), 'r- ','LineWidth',3);
    plot(dists(2, end-4:end), 'g--','LineWidth',2);
    plot(dists(3, end-4:end), 'b-.','LineWidth',2);
  end
  
  pause
end
