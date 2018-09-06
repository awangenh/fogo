%retrieves an specific segment

function S = showSeg(MS, seg)

width  = size(MS, 2);
height = size(MS, 1);
S = zeros(height,width);
for row = 1 : height
  for col = 1 : width
    if (MS(row,col) == seg)
      S(row,col) = 255;
    end;
  end;
end;
imshow(S);