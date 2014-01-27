function edges = Small_Edges(presents)
    edges = min(presents(:, [2,3,4]), [], 2);
end