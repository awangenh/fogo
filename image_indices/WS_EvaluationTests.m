%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%06.08.07
%
%
%Desc: This file contains some tests done during the preparatory stages to
%process and measure the accuracy of the CSC+GNM segmentation method
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%TEST TO EVALUATE THE ACCURACY OF THE CONFUSION MATRIX METHOD

Ams = [ 01 01 01 01 01 01 01 01 01 01; ...
        01 02 02 01 01 01 01 01 01 01; ...
        01 02 02 01 03 03 03 03 01 01; ...
        01 02 02 01 03 03 03 01 01 01; ...
        01 02 02 01 03 03 01 01 01 01; ...
        01 02 02 01 01 01 01 01 01 01; ...
        01 02 02 01 04 01 01 04 01 01; ...
        01 02 02 01 04 04 04 04 01 01; ...
        01 02 02 01 04 04 04 04 01 01; ...
        01 02 02 01 04 01 01 04 01 01
        ];

Agt = [ 02 02 02 01 01 01 01 01 01 01; ...
        01 02 02 01 01 01 01 01 01 01; ...
        01 02 02 01 03 03 03 03 03 01; ...
        01 02 02 01 03 03 03 01 01 01; ...
        01 02 02 01 03 03 01 01 01 01; ...
        01 02 02 01 01 01 01 01 01 01; ...
        01 02 02 01 04 01 01 04 01 01; ...
        01 02 02 01 04 04 04 04 01 01; ...
        01 02 02 01 04 04 04 04 01 01; ...
        01 02 02 01 04 01 01 04 01 01
        ];
        

%segment 1 is the background.
nSegsMS = 4; %3 segments + background
nSegsGT = 4; %3 segments + background
n = size(Agt,1) * size(Agt,2);
confuMat = confusionMat(Ams, Agt, nSegsMS, nSegsGT);
        

%code for computing the cardinality of each subset 
cardGT = cardinalityMatrix(Agt);
cardMS = cardinalityMatrix(Ams);

%code for computing the confusion matrix

[N00, N01, N10, N11] = calcNxx( confuMat, Ams, Agt, nSegsMS, ...
                                nSegsGT, cardMS, cardGT);

%[N00q, N01q, N10q, N11q] = calcNxx2( confuMat, Ams, Agt, nSegsMS, ...
%                                nSegsGT, cardMS, cardGT);
                           

%--------------------------------------------------------------------------
%Start Producing Statistics
%--------------------------------------------------------------------------


%----------------------------------------------
%(1) CR e ER
statsCR_ER = [];
minCorrelation = 0.8; %parameter

for contMS = 1 : nSegsMS 
  for contGT = 1 : nSegsGT 
    %calc the first stat CR e ER
    [cr, er] = CRandER(Ams, Agt, contMS, contGT);
    if (cr >= minCorrelation)    
      statsCR_ER(size(statsCR_ER,1)+1,:) = [contMS, contGT, cr, er];
    end;
  end;
end;


%----------------------------------------------
%(2) The rand index 
%R(C1,C2)
randIndex = 1 - ((N11 + N00) / ((n *(n-1)) /2));

%----------------------------------------------
%(3) Fowlkes & Mallows index
W1 = 0;
W2 = 0;
for i = 1 : nSegsMS
  W1 = W1 + ((cardMS(i)*((cardMS(i) - 1))) /2);  
end;
W1 = N11 / W1;
for j = 1 : nSegsGT
  W2 = W2 + ((cardGT(j)*((cardGT(j) - 1))) /2);  
end;
W2 = N11 / W2;
FowlkesMallows = 1 - sqrt(W1*W2);

%----------------------------------------------
%(4) Jacard Index 
Jacard = 1 - (N11 / (N11 + N10 + N01));

%----------------------------------------------
%(5) Dongen Index (Metric)
%aC1C2
lastIndex = max(max(Ams));
aC1C2 = 0;
for cont = 1 : lastIndex
  [tmp, indMaxMS] = maxIntersectMod(Ams,Agt,cont);
  aC1C2 = aC1C2 + tmp;
end;

%aC2C1
lastIndex = max(max(Agt));
aC2C1 = 0;
for cont = 1 : lastIndex
  [tmp, indMaxMS] = maxIntersectMod(Agt,Ams,cont);
  aC2C1 = aC2C1 + tmp;
end;

Dongen = 2*n - aC1C2 - aC2C1;
Dongen = Dongen / n;
%----------------------------------------------






































