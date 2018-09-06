%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                       HOY MANY SEGMENTS
%                         INCOMPLETE
%
%Daniel D. Abdala
%
%02.08.07
%
%
%desc = returns how many different colors are used in a given image that
%have more than <thresh> pixels in the image
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%function [howMany D] = howManySegments2(I)
function D = howManySegments2(I, thresh)

C = [];

width  = size(I, 2);
height = size(I, 1);
cs = 1;                                   % column start
ce = 1;                                   % column end
mark = squeeze(I(1,1,:))';
C(1,:) = [mark 1 cs ce];
for row = 1 : height
  for col = 1 : width
    
    cor = squeeze(I(row,col,:))';
    
    if (sum(cor == mark) ~= 3)
      %register the last position to ce
      %C(size(C,1),6) = col - 1;
      C(size(C,1),6) = col;
      %mark the new color
      mark = cor;
      %add a new entry to the current color
      C(size(C,1)+1,:) = [mark row col col];

    end; %if
  end;
  %register the last position to ce
  %C(size(C,1),6) = col - 1;
  C(size(C,1),6) = col;
  %mark the new color
  mark = cor;
  %add a new entry to the current color
  C(size(C,1)+1,:) = [mark row+1 1 1];
end;
    
index = 1;
first = 1;
next = 2;
last = size(C,1);
D=[];
%copy the first entry
D(1,:) = C(1,:); 
D(1,4) = C(1,6) - C(1,5) + 1; %put at the row position the number of ocurrences

while (first <=last)
  
  mark = C(first,1:3);
  for cont = first + 1 : last
    cor = C(cont, 1:3);
    if (sum(cor == mark) == 3)
      %add the current number of entries to the current color
      D(first,4) = D(first,4) + C(cont,6) - C(cont,5);
    else
      %add a new entry
      %D(next,:) = [cor, (C(cont,6) - C(cont,5))];
      D(next,:) = C(cont,:);
      D(next,4) = C(cont,6) - C(cont,5) + 1;

      %points to the next free position at the response matrix
      next = next + 1;
    end;%if
  end;%for
  first = first + 1;
  next = first + 1;
  C = D;
  D = C(1:first,:);
  last = size(C,1);
end;%while

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  





