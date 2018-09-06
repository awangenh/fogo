%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%Daniel D. Abdala
%
%01.08 .07
%
%
%Desc: This file contains some tests done during the preparatory stages to
%process and measure the accuracy of the CSC+GNM segmentation method
%
%
%
%
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%ORIGINAL IMAGE
I = imread('Z:\SOURCE\MATLAB\__SegmentationEvaluation\IMAGES\3096\3096.jpg');
imshow(I);

%GROUND TRUTH 
GT_O = imread('Z:\SOURCE\MATLAB\__SegmentationEvaluation\IMAGES\3096\3096.bmp');
BW = im2bw(GT_O);
[GT,locations] = imfill(GT_O);
[labeled, numObjects] = bwlabel(BW,8);
figure,imshow(GT_O);
figure,imshow(GT);
figure, imshow(BW);
figure,imshow(labeled);




BW = im2bw(I);  
BW2 = imfill(BW,'holes');
BW2 = imfill(BW2,'holes');
figure, imshow(BW);
[BW2,locations] = imfill(GT);


figure, imshow(BW2);
imshow(I);

%CSC(30) + GNM
I = imread('Z:\SOURCE\MATLAB\__SegmentationEvaluation\IMAGES\3096\3096.jpg');
imshow(I);




%CR(G,S) = G^S / |S|
%ER(G,S) = (S - (S^G)) / |S|
%



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%im2bw - converts an image to black and white











