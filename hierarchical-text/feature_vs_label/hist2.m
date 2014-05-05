function mat = hist2(data, xmax, ymax)

mat = zeros(ymax, xmax);

for i = 1:length(data)
	if (data(i,1) <= xmax && data(i,2) <= ymax)
		mat(data(i,2), data(i,1)) = mat(data(i,2), data(i,1)) + 1;
	end
end
