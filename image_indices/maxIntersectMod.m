%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel Duarte Abdala
%05.08.07
%
%
%Desc : This function return the module of the intersection between c1 and
%c2 given the following rule: the returned module will be the biggest among
%all clusters of C2.
%Broadly speaking, it takes two clusterings, receives a fixed cluster and
%find the cluster on the anoter clustering with biggest module when
%intersected with the fixed cluster. 
%This function also returns the index of such biggest matching!
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function [module, index] = maxIntersectMod(C1,C2,c1)

module = -1;
index = 0;
lastIndex = max(max(C2));
tmp = 0;
for cont = 1 : lastIndex 
  tmp = modimintersect(C1,C2,c1,cont);
  if (tmp > module)
    module = tmp;
    index = cont;   
  end; %if
end; %for




