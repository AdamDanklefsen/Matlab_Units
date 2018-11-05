clear; close all; clc;
%mex -g units_C.cpp

a = units(10,'m',1);
b = units(5,'mm',1);
c = a-b