%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%01.08 .07
%
%
%Desc: Open a Berkeley segmentation file format and return it as a GS image
%
%
%Known Problems : 
%(1)Images with more than 256 segments will not be correctly
%presented.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function   bklImg = berkeleyImLoad(filePath)

width     = 0;
height    = 0;
segments  = 0;


%fid = fopen('D:\images\BSDS300-human\BSDS300\ ...
%human\color\1105\3096.seg','rt');

fid = fopen(filePath,'rt');

goBabyGo = 1;
while ((feof(fid) == 0) && (goBabyGo == 1))
   tline = fgetl(fid);
   match = findstr(tline, 'width');
   if size(match,1) ~= 0
     width = str2double(tline(7:size(tline,2)));
     goBabyGo = 0;
   end
end %while width

goBabyGo = 1;
while ((feof(fid) == 0) && (goBabyGo == 1))
   tline = fgetl(fid);
   match = findstr(tline, 'height');
   if size(match,1) ~= 0
     height = str2double(tline(8:size(tline,2)));
     goBabyGo = 0;
   end
end %while height
   
goBabyGo = 1;
while ((feof(fid) == 0) && (goBabyGo == 1))
   tline = fgetl(fid);
   match = findstr(tline, 'segments');
   if size(match,1) ~= 0
     segments = str2double(tline(10:size(tline,2)));
     goBabyGo = 0;
   end
end %while segments

goBabyGo = 1;
while ((feof(fid) == 0) && (goBabyGo == 1))
   tline = fgetl(fid);
   match = findstr(tline, 'data');
   if size(match,1) ~= 0
     goBabyGo = 0;
   end
end %skip trash til find the data begining 

%create the destination image
bklImg = zeros(width,height);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%parse each line and construct iterativelly the resulting image
A = fscanf(fid, '%d %d %d %d', [4 inf]);
A = A';

for cont=1 : size(A,1)
  s  = real(A(cont, 1));
  r  = real(A(cont, 2));
  cs = real(A(cont, 3));
  ce = real(A(cont, 4));
  
  bklImg(r+1, cs+1 : ce+1) = s;
  bklImg(1, 1:100) = s;
end;

fclose(fid);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%SEGMENTATION FILE FORMAT
%David Martin
%8/2/2001
%
%This document describes the segmentation file format.  Segmentation
%files end in ".seg".
%
%The overall structure of the file is as follows:
%
%	<header>
%	data
%	<data>
%
%The first part of the file is the header.  The header is ascii text,
%and can contain comments.  The comment character is '#'.  The header
%is separated from the data with a line containing the literal text
%"data".
%
%The header can contain the following information, in any order:
%
%	format {*ascii|binary} {*cr|map}
%	date <date string>
%	image <int>	# image ID number
%	user <int>	# user ID number
%	width <int>	# width of image
%	height <int>	# height of image
%	segments <int>	# number of segments
%	gray {*0|1}	# image presented in grayscale?
%	invert {*0|1}	# image presented with pixel values inverted?
%	flipflop {*0|1}	# image presented upside-down and backwards?
%
%The {width,height,segments} lines are required.  All others lines are
%optional.  Default values are marked with a '*'.
%
%The format line describes the format of the data section of the file.
%The default and recommended format is 'ascii cr' (cr = compressed
%row).  This document does not describe the other formats, as they are
%probably superfluous.
%
%The 'ascii cr' format is designed to be very easy to parse; it is not
%optimized for space.  Use gzip if you want smaller files!  Each line
%in the data section contains 4 integers:
%
%	<s> <r> <c1> <c2>
%
%All values start counting at 0.  <s> is the segment number; <r> is the
%row; <c1> and <c2> are column numbers.  The line means that columns
%[<c1>..<c2>] of row <r> belong to segment <s>.  Lines of this sort can
%appear in any order, and can be reordered without harm.  The only
%restriction is that each pixel must be named exactly once.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%