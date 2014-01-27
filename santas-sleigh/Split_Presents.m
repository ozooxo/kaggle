load presents.mat
remain = presents;

presents_4 = remain(find(Small_Edges(remain) >= 65 & Large_Edges(remain) >= 71), :);
remain = remain(find(Small_Edges(remain) <= 64 | Large_Edges(remain) <= 70), :);

presents_3 = remain(find(Large_Edges(remain) >= 71), :);
remain = remain(find(Large_Edges(remain) <= 70), :);

presents_1 = remain(find(Large_Edges(remain) <= 10), :);
remain = remain(find(Large_Edges(remain) >= 11), :);

presents_2 = remain;

%subplot(2,2,1)
%Shape_Distribution(presents_1)
%title('presents_1')
%subplot(2,2,2)
%Shape_Distribution(presents_2)
%title('presents_2')
%subplot(2,2,3)
%Shape_Distribution(presents_3)
%title('presents_3')
%subplot(2,2,4)
%Shape_Distribution(presents_4)
%title('presents_4')
