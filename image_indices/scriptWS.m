%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                              WORKSPACE                      
%
%Daniel D. Abdala
%
%03.08.07 - Creation
%05.08.07 - programming of the first distance metrics
%         - tests of the methods to image intersection  
%06.08.07 - tests to string contatenating in order to contruct iterativelly
%           the file paths to be processed
%
%
%desc: This workspace is used to test and document the advances in the
%issue to measure automatically the quality of machine segmented images by
%comparing it with ground truth (hand segmented) images.
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%How to pull out and iteractivelly iterate over the paths and names of the
%images to be processed
baseDir = 'Z:\SOURCE\MATLAB\__SegmentationEvaluation\';
imgDir = '3096\';
imMSName = '3096.csc30.gnm.png';
imGTName = '3096_1107.seg';
pathMS = [baseDir imgDir];
pathGT = pathMS;
pathMS = [pathMS imMSName];
pathGT = [pathGT imGTName];

%--------------------------------
%Load MS image
path = 'Z:\SOURCE\MATLAB\__SegmentationEvaluation\IMAGES\3096\3096.csc30.gnm.png';
[MS, numSegsMS, hist] = im2segs2(path);

%filtering and pre-processing

%--------------------------------
%Load GT image
path = 'Z:\SOURCE\MATLAB\__SegmentationEvaluation\IMAGES\3096\3096_1107.seg';
[GT, numSegsGT] = berkeleyImLoadBySegments(path);

%filtering and pre-processing

%--------------------------------
%e.g.
imshow(imintersect(MS(:,:,5), GT(:,:,6)))
[cr er] = CRandER(MS(:,:,5), GT(:,:,6))

sum(sum( showSeg(MS,5) )) / 255
sum(sum( showSeg(GT,6) )) / 255

%--------------------------------
%intersection of two pre-defined segments
modimintersect(MS,GT,5,6)


%--------------------------------
%Code to compute the confusion matrix
k = numSegsMS;
l = numSegsGT;
confMatrix = zeros(k,l);

for i=1 : k
  for j=1 : l
    confMatrix(i,j) = modimintersect(MS,GT,i,j);
  end;
  i
end;

%--------------------------------
%code for computing the cardinality of each subset 
cardGT = cardinalityMatrix(GT);
cardMS = cardinalityMatrix(MS);



%--------------------------------
%code to compute the Nxx index
width = size(GT,1);
height = size(GT,2);
n = width * height; %cardinality of O;
N11 = 0;
N00 = 0;
N01 = 0;
N10 = 0;
sumMije2 = 0;
nie2 = 0;
nje2 = 0;
for i=1 : k
  for j=1 : l
    mije2 = confMatrix(i,j)^2;
    sumMije2 = sumMije2 + mije2;
    nie2 = nie2 + (cardMS(i)^2);
    nje2 = nje2 + (cardGT(j)^2);
    N11 = N11 + (mije2) - n;
    
  end;
  i
end;

N11 = 0.5 * N11;
N10 = 0.5 * (nie2 - mije2); 
N01 = 0.5 * (nje2 - mije2); 
N00 = (n*(n-1)) / 2;
N00 = N00 - N11 - N10 - N01;


%--------------------------------
%Prova
%N11 + N00 + N10 + N01 = (n*(n-1))/2

%--------------------------------
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%The rand index 
%R(C1,C2)

randIndex = 1 - ((N11 + N00) / ((n *(n-1)) /2));

%Fowlkes & Mallows index

for i = 1 : k
  W1 = (cadMS(i)*(cadMS(i) - 1)) /2;  
end;
W1 = N11 / W1;

for j = 1 : l
  W2 = (cadMS(i)*(cadGT(j) - 1)) /2;  
end;
W2 = N11 / W2;

FowlkesMallows = 1 - sqrt(W1*W2);

%Jacard Index 

Jacard = 1 - (N1 / (N11 + N10 + N01));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Distance if clusterings by set matching

%Dongen Index (Metric)
%aC1C2
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

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

