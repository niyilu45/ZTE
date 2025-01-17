clear;
clc;
close all;
rand('seed',0); 
randn('seed',0);


MatrixM = 6;
MatrixN = 6;
Matrix = randi([0,1], MatrixM,MatrixN);
Idx = find(Matrix < 0.25);
Matrix = zeros(MatrixM,MatrixN);
Matrix(Idx) = 1;

csvwrite('MyMatrix6-6.csv', Matrix);

%% Load Matrix and init
Matrix = csvread('MyMatrix6-6.csv');
[M,N] = size(Matrix);
for i = 1:M
    Row(i).Idx = find(Matrix(i,:));
end
for i = 1:N
    Col(i).Idx = find(Matrix(:,i));
end

CircLen = 6;
ColPoint = zeros(CircLen/2, 1);
NewPointRemain = ColPoint - 2;

for MostLeftEdgeCol = 1:N
	
end
