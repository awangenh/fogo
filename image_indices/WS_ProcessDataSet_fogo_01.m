%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
% Autor: Daniel D. Abdala
% Modificação: Fogo
%
% Através desse método é possível varrer um conjunto de imagens e gerar
% um conjunto de valores de índices de avaliação de segmentação. Os índices
% aqui gerados são:
%
% -- Rand
% -- Fowlkes & Mallows
% -- Jacard
% -- Dongen
% -- Bipartite Graph matching
%
% Os resultados são armazenados em arquivos .indices e .results em um
% diretório indicado pela variável $outputDir$
% 
% OBSERVAÇÃO 1: deve ser passado um diretório, através da variável 
% $baseDir$ que conterá as imagens a serem avaliadas. Espera-se que essas 
% imagens estejam organizadas da seguinte forma:
%
% -- Um diretório para cada imagem a ser avaliada.
% -- O nome do diretório deve ser o nome da imagem avaliada.
% -- Cada diretório deve conter a imagem a ser avaliada e as imagens de
% comparação (provavelmente, *.seg)
%
% Exemplo:
%
% ./113016
% ./113016/113016.2.seg
% ./113016/113016.0.seg
% ./113016/113016.4.seg
% ./113016/113016.3.seg
% ./113016/113016.1.seg
% ./113016/113016.cscgnm.png
% ./113016/113016.5.seg
% ./12003
% ./12003/12003.9.seg
% ./12003/12003.cscgnm.png
% ./12003/12003.8.seg
% ./12003/12003.7.seg
% ./12003/12003.6.seg
% ./12003/12003.10.seg
% 
% OBSERVAÇÃO 2: devem ser indicadas as extensões esperadas para os arquivos
% de imagens a serem avaliados e dos arquivos contendo os alvos de
% comparação, respectivamente, nas variáveis $imageExt$ e $gtExt$
%
% PROBLEMAS QUE RESTAM: 
% 
% -- sem verificações de erros/ parada em caso de problemas
% -- sem indicação de pareamento dos arquivos de resultados com os arquivos
% de ground truth
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function sucess = generateImageIndexes(baseDir outputDir imageExt gtExt)

RESULTADOS = [];
INDICES = [];

%folder containing the images folders
%baseDir =   '/home/fogo/mystuff/mestrado/testes/set_teste/eval_msgnm';
%folder where the results will be stored
%outputDir = '/home/fogo/mystuff/mestrado/testes/set_teste/output_msgnm';

%expected extension of the images to be evaluated
%imageExt = 'png';
%expected extension of the ground truth files
%gtExt = 'seg';

[stat listgnm_raw] = unix(['ls -1 ' baseDir]);
listgnm = split_lines(listgnm_raw);

%iterate over all images
for cont = 1 : size(listgnm,1)
  %GAMBIARRA pra garantir que não restem espaços. NÃO ALTERAR! trim não é
  %uma solução completamente confiável!
  true_value = num2str(str2num(listgnm(cont,:)));
  
  %getting name of the image file to be evaluted
  %WARNING: make sure there is EXACLTY ONE image file of the specified type
  %in each image directory
  [stat aux_pathMS_raw] = unix(['find ' baseDir '/' true_value ... 
                                ' -name *.' imageExt]);]
  aux_pathMS = split_lines(aux_pathMS_raw);
  pathMS = aux_pathMS(1,:);
  
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
    
  [stat listseg_raw] = unix(['find ' baseDir '/' true_value ...
                             ' -name *.' gtExt]);
  listseg = split_lines(listseg_raw);
  
  for contEval = 1 : size(listseg,1)    
      pathGT    = listseg(contEval,:);   
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
      
      RESULTADOS = [
        randIndex,...
        FowlkesMallows,...
        Jacard,...
        Dongen, ...
        BGM];
      
      INDICES = [str2num(true_value), ...
        contEval,...
        nSegsMS,...
        nSegsGT, ...
        n];
      save([outputDir '/' true_value '.' int2str(contEval) '.results'],'RESULTADOS','-ASCII');
      save([outputDir '/' true_value '.' int2str(contEval) '.indices'],'INDICES','-ASCII');
        
  end;
end;

fprintf('PROCESSO ENCERRADO!\n');

sucess = 1;
