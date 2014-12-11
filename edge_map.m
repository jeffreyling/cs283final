% Testing the contour extraction
addpath(fullfile(pwd,'../BSR/grouping/lib'));

clear all; close all; clc;

%imgFile = 'images/ISPY_silhouettes_spread.jpg';
imgFile = '~/Downloads/frog_pond.jpg';
img = double(imread(imgFile));

outFile = 'ispy_test.mat';
[gPb_orient,gPb_thin] = globalPb(imgFile, outFile);

% pb = max(gPb_orient,[],3);
% ws = watershed(pb);
% ws_bw = (ws == 0);

%contours = fit_contour(double(ws_bw));

ucm = contours2ucm(gPb_orient, 'imageSize');

%bw = edge(img);
%contours = fit_contour(double(bw));

%figure; imshow(max(gPb_orient,[],3)); colormap(jet);

%ucm = contours2ucm(gPb_orient, 'imageSize');
%figure;imshow(ucm);