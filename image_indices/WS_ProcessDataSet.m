%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%06.08.07 - creation
%08.08.07 - lines responsible to export the produced indexes
%
%
%Desc: This file contains the coded needed to process the selected dataset
%and effectivelly produce the measurements
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
RESULTADOS = [];
INDICES = [];
indexResultados = 0;
% The list of dirs (unfortunatelly, this cell array has to be constructed
%by hand) 
dirList = cell(8, 7);
%Image Number
dirList{1,1} = '3096';
dirList{2,1} = '22090';
dirList{3,1} = '48055';
dirList{4,1} = '118035';
dirList{5,1} = '124084';
dirList{6,1} = '138078';
dirList{7,1} = '143090';
dirList{8,1} = '219090';
%Evaluator number
dirList{1,2} = '1123'; dirList{1,3} = '1105'; dirList{1,4} = '1107'; dirList{1,5} = '1121';
dirList{2,2} = '1106'; dirList{2,3} = '1107'; dirList{2,4} = '1108'; dirList{2,5} = '1109';  dirList{2,6} = '1115';  dirList{2,7} = '1121';
dirList{3,2} = '1105'; dirList{3,3} = '1115'; dirList{3,4} = '1119'; dirList{3,5} = '1121';  dirList{3,6} = '1130';
dirList{4,2} = '1105'; dirList{4,3} = '1108'; dirList{4,4} = '1115'; dirList{4,5} = '1117';  dirList{4,6} = '1130';
dirList{5,2} = '1103'; dirList{5,3} = '1107'; dirList{5,4} = '1115'; dirList{5,5} = '1116';  dirList{5,6} = '1123';
dirList{6,2} = '1105'; dirList{6,3} = '1107'; dirList{6,4} = '1108'; dirList{6,5} = '1112';  dirList{6,6} = '1116';  dirList{6,7} = '1124';
dirList{7,2} = '1105'; dirList{7,3} = '1109'; dirList{7,4} = '1115'; dirList{7,5} = '1121';  dirList{7,6} = '1124';
dirList{8,2} = '1103'; dirList{8,3} = '1107'; dirList{8,4} = '1113'; dirList{8,5} = '1115';  dirList{8,6} = '1127';


baseDir =   'Z:\SOURCE\MATLAB\__SegmentationEvaluation\IMAGES\';
%iterate over all images
for cont = 1 : 8   
  imgDir    = [dirList{cont,1} '\'];
  imMSName  = [dirList{cont,1} '.csc30.gnm.png'];
  pathMS    = [baseDir imgDir];
  pathMS    = [pathMS imMSName]
  cont
  %--------------------------------
  %Load MS image
  MS = [];
  nSegsMS = 0;
  hist = [];
  [MS, nSegsMS, hist] = im2segs2(pathMS);
  nSegsMS
  %code to compute the cardinality of the MS subset
  cardMS = cardinalityMatrix(MS);
  %code for compute the number of elements
  n = size(MS,1) * size(MS,2);
  
  for contEval = 2 : 7
    
    dirList = constructDirList;
    %Composing the files names
    if (sum(size(dirList{cont,contEval})) ~= 0) %test if the GT is defined in this point
      imgDir    = [dirList{cont,1} '\'];   
      imGTName  =  [dirList{cont,1} '_' dirList{cont,contEval} '.seg'];
      pathGT    = [baseDir imgDir];
      pathGT    = [pathGT imGTName]
      contEval
      %--------------------------------
      %Load GT image
      GT = [];
      nSegsGT = 0;
      [GT, nSegsGT] = berkeleyImLoadBySegments(pathGT);

      %code for computing the cardinality of each subset
      cardGT = cardinalityMatrix(GT);

      %code for computing the confusion matrix
      confuMat = confusionMat(MS, GT, nSegsMS, nSegsGT);

      %code for computing the Nxx indexes
      [N00, N01, N10, N11] = calcNxx( confuMat, MS, GT, nSegsMS, ...
        nSegsGT, cardMS, cardGT);
      [N00, N01, N10, N11]
      %----------------------------------------------
      %(1) CR e ER
      %statsCR_ER = [];
      %minCorrelation = 0.8; %parameter

      %for contMS = 1 : nSegsMS
      %  for contGT = 1 : nSegsGT
      %    %calc the first stat CR e ER
      %    [cr, er] = CRandER(MS, GT, contMS, contGT);
      %    if (cr >= minCorrelation)
      %      statsCR_ER(size(statsCR_ER,1)+1,:) = [contMS, contGT, cr, er];
      %    end;
      %  end;
      %end;
      %save([pathGT, '.ER_CR_file'], statsCR_ER); 

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
      for cont2 = 1 : lastIndex
        [tmp, indMaxMS] = maxIntersectMod(MS,GT,cont2);
        aC1C2 = aC1C2 + tmp;
      end;

      %aC2C1
      lastIndex = max(max(GT));
      aC2C1 = 0;
      for cont2 = 1 : lastIndex
        [tmp, indMaxMS] = maxIntersectMod(GT,MS,cont2);
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
    
      indexResultados = indexResultados + 1;
      RESULTADOS(indexResultados,:) = [
        randIndex,...
        FowlkesMallows,...
        Jacard,...
        Dongen, ...
        BGM];
      INDICES(indexResultados,:) = [str2num(dirList{cont,1}), ...
        str2num(dirList{cont,contEval}),...
        nSegsMS,...
        nSegsGT, ...
        n];
      save([baseDir imgDir imGTName '.results'],'RESULTADOS','-ASCII');
      save([baseDir imgDir imGTName '.indices'],'INDICES','-ASCII');
    end;
    
  end;
end;


