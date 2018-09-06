%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                       IMAGE INTERSECTION
%
%Daniel D. Abdala
%
%03.08.07 - creation
%06.08.07 - adjustment to work with given indexes
%
%desc = returns a binary matrix that represents the intersection of the two
%given ones.
%#It returns 255 or 1 for a binary image to each match
%#to compute the numer of matches one must simply divide se sum by /255
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function C = imintersect(A, B, idxA, idxB)


C = zeros(size(A));
for row = 1 : size(A,1)
  for col = 1 : size(A,2)
    if ((A(row,col) == idxA) && (B(row,col) == idxB))
      C(row,col) = 255;
    end;%if
  end;%col
end;%row

%TESTING
%A = [1 1 1; 2 1 1; 2 2 2]
%B = [2 2 2; 1 2 1; 2 2 2]
%imintersect(A,B, 1, 1)
%A,B,imintersect(A,B, 2, 2)
%A,B,imintersect(A,B, 1, 2)
%-----------OLD METHOD------------
%C = zeros(size(A));
%for row = 1 : size(A,1)
%  for col = 1 : size(A,2)
%    if ((A(row,col) == 255) && (B(row,col) == 255))
%      C(row,col) = 255;
%    end;%if
%  end;%col
%end;%row