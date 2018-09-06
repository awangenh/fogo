%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%03.08 .07
%
%
%Desc: 
%
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function   segsVol = im2segs(filePath)

width     = 0;
height    = 0;
segments  = 0;

I = imread(filePath);

%this array holds ocorrences of all colors in this image
colArray = [];

  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  function idx = knownColor(cor1)
    idx = 0;
    for cont = 1 : size(colArray,1)
      cor2 = colArray(cont,:);
      if (sum(cor1 == cor2) == 3)
        idx = cont;
        break;
      end;
    end;    
  end;
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  segsVol = [];
  width  = size(I, 2);
  height = size(I, 1);
  for row = 1 : height
    for col = 1 : width
      cor = squeeze(I(row,col,:))';
      index = knownColor(cor);
      if (index ~= 0)
        %light up the specified pixel
        segsVol(row,col, index) = 255;
      else
        index = size(colArray,1)+1;
        colArray(index,:) = cor;
        
        %light up the specified pixel
        segsVol(row,col, index) = 255;
      end;
   
    end; % col
    row
  end; %row
  
end