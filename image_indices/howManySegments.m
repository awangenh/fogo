%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                       HOY MANY SEGMENTS
%
%Daniel D. Abdala
%
%02.08.07
%
%
%desc = returns how many different colors are used in a given image
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function [howMany D] = howManySegments(I)

C = [];

width  = size(I, 2);
height = size(I, 1);

for col = 1 : width
  for row = 1 : height
    cor = I(row,col,:);
    cor = squeeze(cor);
    
    addIt = 1;
    for each = 1 : size(C,1)
      act = C(each,1:3)';
      if (sum(cor == act) == 3)
        addIt = 0;
        C(each,4) = C(each,4) + 1;
      end; %if
    end; %each
    if (addIt == 1)
      cor = [cor; 1];
      C(size(C,1)+1,:) = cor;
    end; %if
  end; %row
end; %col

howMany = size(C,1);

threshold = 50;
D = [];
for cont = 1 : size(C,1)
  if (C(cont, 4) >= threshold)
    D(size(D,1)+1,:) = C(cont,:);
  end; %if
end; %for

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  





