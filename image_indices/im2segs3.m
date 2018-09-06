%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%03.08.07 - creation
%06.08.07 - revision
%
%Desc: 
%
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function   [segsImg, numSegs, colArray] = im2segs2(filePath,thresh)

width     = 0;
height    = 0;
segments  = 0;

I = imread(filePath);

%this array holds ocorrences of all colors in this image
colArray = [];

  segsVol = [];
  width  = size(I, 2);
  height = size(I, 1);
  for row = 1 : height
    for col = 1 : width
      cor = squeeze(I(row,col,:))';
      index = 0;
      for cont = 1 : size(colArray,1)
        cor2 = colArray(cont,1:3);
        if (sum(cor == cor2) == 3)
          index = cont;
          break;
        end;
      end;    
  
      if (index ~= 0)
        %light up the specified pixel
        segsImg(row,col) = index;
        %colArray(index,4) = colArray(index,4) + 1;
      else
        index = size(colArray,1)+1;
        colArray(index,1:3) = cor;
        %colArray(index,1:4) = 1;
        
        %light up the specified pixel
        segsImg(row,col) = index;
      end; %if
   
    end; % col
    row
  end; %row

  
  %remove all segments bellow the thresh parameter
  %removedIndexes = [];
  %for row = 1 : height
  %  for col = 1 : width
  %    index = segsImg(row,col);
  %    if(colArray(index,4) < thresh)
  %      segsImg(row,col) = 0;  %index 0 represent the pixels removed
  %    end;%if
  %  end;
  %end;
  
 
  %compute the real number of segments
  numSegs = size(colArray,1);
  %for cont = 1 : sise(colArray,1)
  %  if (colArray(cont, 4) < thresh)
  %    numSegs = numSegs - 1;
  %  end;  
  %end;