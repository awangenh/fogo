%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                         CORRESPONDANCE RATIO &
%                         ERROR RATIO
%
%Daniel D. Abdala
%
%03.08.07 - creation
%06.08.07 -
%
%desc: this function compute, given two segments, the error radio and the
%correspondance ration of them.
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function [cr, er] = CRandER(S1, S2, segIdx1, segIdx2)


width  = size(S1, 2);
height = size(S1, 1);

%compute the module of S1
modS1 = 0;
for row = 1 : height
  for col = 1 : width
    if (S1(row,col) == segIdx1)
      modS1 = modS1 + 1;
    end;%if
  end;%for col
end; %for row

%compute the intersection between S1 & S2 (index = 255)
s1ands2 = imintersect(S1, S2, segIdx1, segIdx2);

%compute the intersection between S1 & S2 (index = segIdx1)
s1ands2_ = imintersect2(S1, S2, segIdx1, segIdx2,segIdx1);

%creates a fake ONE matrix to separate only the desired segment
s1_ = ones(size(S1));
s1_ = imintersect2(S1, s1_, segIdx1, 1, segIdx1);

%compute CR & ER
cr = sum(sum(s1ands2)) / 255;
cr =  cr / modS1; 

if (segIdx1 == 0 )
  segIdx1 = 1;
end;
er = (s1_ - s1ands2_);
er = sum(sum(er)) / segIdx1;
er = er / modS1;


%------------OLD METHOD------------
%totalPixels = sum(sum(S1));
%s1ands2 =  imintersect(S1,S2);

%cr = sum(sum(s1ands2)) / totalPixels; 
%er = sum(sum(S1 - s1ands2)) / totalPixels;















