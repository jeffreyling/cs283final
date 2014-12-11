% Testing the contour extraction
addpath(fullfile(pwd,'../BSR/grouping/lib'));

clear all; close all; clc;

%imgFile = '../ETHZShapeClasses-V1.2/Bottles/beach.jpg';
imgFile = '../66obj/images/image000.JPG';
img = double(imread(imgFile));

outFile = 'asdf.mat';
[gPb_orient,gPb_thin] = globalPb(imgFile, outFile);

pb = max(gPb_orient,[],3);
ws = watershed(pb);
ws_bw = (ws == 0);

contours = fit_contour(double(ws_bw));

ucm = contours2ucm(gPb_orient, 'imageSize');

%bw = edge(img);
%contours = fit_contour(double(bw));

%figure; imshow(max(gPb_orient,[],3)); colormap(jet);

%ucm = contours2ucm(gPb_orient, 'imageSize');
%figure;imshow(ucm);