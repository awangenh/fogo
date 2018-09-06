%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                          Nxx INDEXES 
%
%Daniel D. Abdala
%
%15.08.07 - transposition from a workspace to a method
%
%desc: this function compute, four indexes, namely, N11, N00, N01, N10
%
%
%Given two clusterings C1 and C2 of a set O ob Objects, we consider all
%pairs of objects (oi,oj), i ~= j from OxO. A pair (oi,oj) falls into one
%of the four categories:
%
%(i)    In the same cluster under both C1 and C2 (the total number of such
%       pair is represented by N11); 
%(ii)   In different clusters under both C1 and C2 (N00)
%(iii)  in the same cluster under C1 but not C2 (N10)
%(iv)   in the same cluster under C2 but not C1 (N01)
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function [N00, N01, N10, N11] = calcNxx2(confMatrix, MS, GT, ... 
                                        numSegsMS, numSegsGT, ...
                                        cardMS, cardGT)


%get the image sizes. They must be equal to both images
width     = size(GT,1);
height    = size(GT,2);
k = numSegsMS;
l = numSegsGT;

%variable initialization 
n         = width * height; %cardinality of O;
N11       = 0;
N00       = 0;
N01       = 0;
N10       = 0;
sumMije2  = 0;
nie2      = 0;
nje2      = 0;
mij       = 0;
mijprod   = 0;
mijsum    = 0;

for i = 1 : k
  for j = 1 : l
    %mij(mij-1) /2
    mij = confMatrix(i,j);
    mijprod = (mij * (mij -1) )/2;
    N11 = N11 + mijprod;
  end;
  i
end;
indexN10 = 0;
for i = 1 : k
  indexN10 =  ( cardMS(i)*( cardMS(i) - 1))/2;
  mijsum  = 0;
  for j = 1 : l 
    mij = confMatrix(i,j);
    mijprod = (mij * (mij -1) )/2;  
    mijsum = mijsum + mijprod;
  end;
  N10 = N10 + indexN10 - mijsum;
end;

indexN01 = 0;
for j = 1 : l
  indexN01 = (( cardGT(j)*( cardGT(j) - 1))/2);
  mijsum    = 0;
  for i = 1 : k 
    mij = confMatrix(i,j);
    mijprod = (mij * (mij -1))/2;  
    mijsum = mijsum + mijprod;
  end;
  N01 = N01 + indexN01 - mijsum;
end;

%N00 = 0.5 * ( (n^2) - nie2 - nje2 + sumMije2);



N00 = (n*(n-1)) / 2;
N00 = N00 - N11 - N10 - N01;
%obs, the above equation is not working properly 
