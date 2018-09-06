%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel Duarte Abdala
%05.08.07
%
%
%Desc:
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function module = modimintersect(C1,C2,ind1,ind2)

module = 0;
for row = 1 : size(C1,1)
  for col = 1 : size(C1,2)
    if ((C1(row,col) == ind1) && (C2(row,col) == ind2))
      module = module + 1;
    end;%if
  end;%col
end;%row
