function Timestream_Distribution(varargin)
    %% Histogram plot for volumn distribution of presents
    % Timestream_Distribution(presents)
    % Timestream_Distribution(small_presents, large_presents)
    ys = [];
    for i = 1:nargin
        [y, b] = hist(varargin{i}(:, 1), 12500:25000:1012500);
        ys = [ys; y];
    end
    
    bar(b, ys', 1, 'stacked');
    axis([0, 1000000, 0, 25000])
    title('Timestream Distribution')
    xlabel('package id')
    ylabel('counts')
end