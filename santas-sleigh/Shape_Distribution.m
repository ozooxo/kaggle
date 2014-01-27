function Shape_Distribution(presents)
    %% Shape distribution of presents
    % Shape_Distribution(presents)
    sorted_edges = sort(presents(:, [2,3,4]), 2);
    small_edge = sorted_edges(:, 1);
    medium_edge = sorted_edges(:, 2);
    large_edge = sorted_edges(:, 3);
    plot(small_edge./medium_edge, medium_edge./large_edge, '.r')
    axis([0 1 0 1])
    xlabel('small edge / medium edge')
    ylabel('medium edge / large edge')
end
