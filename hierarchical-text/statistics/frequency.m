%freq = importdata('features_frequency.csv');
sorted_freq = sortrows(freq,2);
hist(sorted_freq(2040000:2085167,2), 50);
