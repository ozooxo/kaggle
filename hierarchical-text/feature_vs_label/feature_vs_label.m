m = importdata('feature_vs_label.csv');

subplot(2,2,1)
plot(m(:, 1), m(:, 2), '.')
axis([0 500 0 50])
xlabel('# of features')
ylabel('# of labels')

subplot(2,2,2)
density = hist2(m, 500, 50);
image(density)
xlabel('# of features')
ylabel('# of labels')

subplot(2,2,3)
loglog(m(:, 1), m(:, 2), '.')
xlabel('# of features')
ylabel('# of labels')

subplot(2,2,4)
density = hist2(int8(log(m)/log(2))+1, 13, 10);
image(density)
xlabel('log2(# of features)')
ylabel('log2(# of labels)')
