%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%14.08.07 - creation
%
%
%Desc: Workspace that iterate over all images to be evaluated
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

RESULTADOS = [];
INDICES = []
dirList = constructDirList;

for cont = 2 : 2   
  for contEval = 2 : 7

    %Composing the files names
    if (sum(size(dirList{cont,contEval})) ~= 0) %test if the GT is defined in this point
      
      baseDir =   'Z:\SOURCE\MATLAB\__SegmentationEvaluation\IMAGES\';
      imgDir =    [dirList{cont,1} '\'];
      imMSName =  [dirList{cont,1} '.csc30.gnm.png'];
      imGTName =  [dirList{cont,1} '_' dirList{cont,contEval} '.seg'];
      pathMS =    [baseDir imgDir];
      pathGT = pathMS;
      pathMS = [pathMS imMSName];
      pathGT = [pathGT imGTName];
      
      resultRow = calcDistanceIndexes(pathMS, pathGT);
      INDICES = [ INDICES; 
                  str2num(dirList{cont,1}), ...
                  str2num(dirList{cont,contEval})...
                  ]; 
      RESULTADOS = [RESULTADOS; resultRow];
    end;%if
    
  end; %contEval
end;%cont
