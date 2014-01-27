function Edge_Length_Distribution(varargin)
    %% Edge length distribution of presents
    % Edge_Length_Distribution(presents)
    % Edge_Length_Distribution(presents1, presents2, ...)

    subplot(3,1,1)
    ys = [];
    for i = 1:nargin
        [y, b] = hist(Small_Edges(varargin{i}), 2.5:5:252.5);
        ys = [ys; y];
    end
    ys
    bar(b, ys', 1, 'stacked');
    axis tight
    title('Small Edge')
    xlabel('length')
    ylabel('count')
    
    subplot(3,1,2)
    ys = [];
    for i = 1:nargin
        [y, b] = hist(Median_Edges(varargin{i}), 2.5:5:252.5);
        ys = [ys; y];
    end
    bar(b, ys', 1, 'stacked');
    axis tight
    title('Median Edge')
    xlabel('length')
    ylabel('count')
    
    subplot(3,1,3)
    ys = [];
    for i = 1:nargin
        [y, b] = hist(Large_Edges(varargin{i}), 2.5:5:252.5);
        ys = [ys; y];
    end
    bar(b, ys', 1, 'stacked');
    axis tight
    title('Large Edge')
    xlabel('length')
    ylabel('count')
    
    %ys = [];
    %for i = 1:nargin
    %    [y, b] = hist(log(Volumn(varargin{i})), 1:0.5:18);
    %    ys = [ys; y];
    %end
    %bar(b, ys', 1, 'stacked');
    %xlabel('log (package volumn)')
    %ylabel('counts')
end

