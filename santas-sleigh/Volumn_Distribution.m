function Volumn_Distribution(varargin)
    %% Histogram plot for volumn distribution of presents
    % Volumn_Distribution(presents)
    % Volumn_Distribution(small_presents, large_presents)
    ys = [];
    for i = 1:nargin
        [y, b] = hist(log(Volumn(varargin{i})), 1:0.5:18);
        ys = [ys; y];
    end
    bar(b, ys', 1, 'stacked');
    xlabel('log (package volumn)')
    ylabel('counts')
end