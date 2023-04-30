// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <conio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <algorithm>
#include <iterator>

using namespace cv;
using namespace std;

static int parseParameters(int argc, char** argv, string* imageFile);
void constructHuffmanTree(Mat normalizedHistogram);
void constructHuffLUT(struct huffNode *node, string code);
struct nodeComparator;
struct huffNode;
float entropyCalc(Mat histogram);
float averageLengthCalc(Mat histogram);