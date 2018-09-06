%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 
%
%Daniel Duarte Abdala
%05.08.07 - creation
%06.08.07 - indexes adjustments 
%
%Desc: Compute the cardinality of each segments in a givem segmented image
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function mat = cardinalityMatrix(S)

lastIndex = max(max(S));
mat = zeros(lastIndex,1);
for row = 1 : size(S,1)
  for col = 1 : size(S,2)
    mat(S(row,col)) = mat(S(row,col)) + 1;   
  end;
end;