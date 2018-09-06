%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                            IMAGE UNION
%
%Daniel D. Abdala
%
%03.08.07
%
%
%desc = returns a binary matrix that represents the union of the two given 
%ones.
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function C = imunion(A, B)


C = zeros(size(A));
for row = 1 : size(A,1)
  for col = 1 : size(A,2)
    if ((A(row,col) == 1) || (B(row,col) == 1))
      C(row,col) = 1;
    end;%if
  end;%col
end;%row