function edges = Large_Edges(presents)
    edges = max(presents(:, [2,3,4]), [], 2);
end