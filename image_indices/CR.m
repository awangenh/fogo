%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                         CORRESPONDANCE RATIO                      
%
%Daniel D. Abdala
%
%03.08.07
%
%
%desc:
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function val = CR(S1, S2)


totalPixels = sum(sum(S1));

val = sum(sum(imintersect(S1,S2)) / totalPixels; 
















