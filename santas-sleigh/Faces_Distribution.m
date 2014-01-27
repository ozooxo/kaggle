function Faces_Distribution(varargin)
    %% Faces distribution of presents
    % Faces_Distribution(presents)
    % Faces_Distribution(presents1, presents2, ...)
    for i = (nargin+1):5
        varargin{i} = [1 0 0 0];
    end
    Small_Edges(varargin{1});

    subplot(3,1,1)
    loglog(Small_Edges(varargin{1}), Median_Edges(varargin{1}), '.', ...
        Small_Edges(varargin{2}), Median_Edges(varargin{2}), '.', ...
        Small_Edges(varargin{3}), Median_Edges(varargin{3}), '.', ...
        Small_Edges(varargin{4}), Median_Edges(varargin{4}), '.', ...
        Small_Edges(varargin{5}), Median_Edges(varargin{5}), '.')
    axis([1, 250, 1, 250])
    title('Faces Distribution: dots may overlap')
    xlabel('small edge')
    ylabel('median edge')

    subplot(3,1,2)
    loglog(Small_Edges(varargin{1}), Large_Edges(varargin{1}), '.', ...
        Small_Edges(varargin{2}), Large_Edges(varargin{2}), '.', ...
        Small_Edges(varargin{3}), Large_Edges(varargin{3}), '.', ...
        Small_Edges(varargin{4}), Large_Edges(varargin{4}), '.', ...
        Small_Edges(varargin{5}), Large_Edges(varargin{5}), '.')
    axis([1, 250, 1, 250])
    xlabel('small edge')
    ylabel('large edge')
    
    subplot(3,1,3)
    loglog(Median_Edges(varargin{1}), Large_Edges(varargin{1}), '.', ...
        Median_Edges(varargin{2}), Large_Edges(varargin{2}), '.', ...
        Median_Edges(varargin{3}), Large_Edges(varargin{3}), '.', ...
        Median_Edges(varargin{4}), Large_Edges(varargin{4}), '.', ...
        Median_Edges(varargin{5}), Large_Edges(varargin{5}), '.')
    axis([1, 250, 1, 250])
    xlabel('median edge')
    ylabel('large edge')
end