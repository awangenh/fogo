%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%14.08.07 - creation
%
%
%Desc: This method receives as parameter tow paths representing the images
%to be matched
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function ResultRow = calcDistanceIndexes(pathMS, pathGT)


%Load MS image
[MS, nSegsMS, hist] = im2segs2(pathMS);

%Load GT image
[GT, nSegsGT] = berkeleyImLoadBySegments(pathGT);

%code for computing the cardinality of each subset
cardGT = cardinalityMatrix(GT);
cardMS = cardinalityMatrix(MS);

%code for compute the number of elements
n = size(GT,1) * size(GT,2);
%n = prod(size(GT));

%code for computing the confusion matrix
confuMat = confusionMat(MS, GT, nSegsMS, nSegsGT);

%code for computing the Nxx indexes
[N00, N01, N10, N11] = calcNxx( confuMat, MS, GT, nSegsMS, ...
  nSegsGT, cardMS, cardGT);

%----------------------------------------------
%(2) The rand index
%R(C1,C2)
randIndex = 1 - ((N11 + N00) / ((n *(n-1)) /2));

%----------------------------------------------
%(3) Fowlkes & Mallows index
W1 = 0;
W2 = 0;
for i = 1 : nSegsMS
  W1 = W1 + ((cardMS(i)*(cardMS(i) - 1)) /2);
end;
W1 = N11 / W1;
for j = 1 : nSegsGT
  W2 = W2 + ((cardGT(j)*(cardGT(j) - 1)) /2);
end;
W2 = N11 / W2;
FowlkesMallows = 1 - sqrt(W1*W2);

%----------------------------------------------
%(4) Jacard Index
Jacard = 1 - (N11 / (N11 + N10 + N01));

%----------------------------------------------
%(5) Dongen Index (Metric)
lastIndex = max(max(MS));
aC1C2 = 0;
for cont = 1 : lastIndex
  [tmp, indMaxMS] = maxIntersectMod(MS,GT,cont);
  aC1C2 = aC1C2 + tmp;
end;

%aC2C1
lastIndex = max(max(GT));
aC2C1 = 0;
for cont = 1 : lastIndex
  [tmp, indMaxMS] = maxIntersectMod(GT,MS,cont);
  aC2C1 = aC2C1 + tmp;
end;

Dongen = 2*n - aC1C2 - aC2C1;
Dongen = Dongen / n;

%----------------------------------------------
%(6) Bipartite Graph matching
w = 0;
for cGT = 1 : size(confuMat,2)
  w = w + max(confuMat(:,cGT));
end;
BGM = 1 - (w / n);


ResultRow = [ randIndex,...
              FowlkesMallows,...
              Jacard,...
              Dongen, ...
              BGM];
