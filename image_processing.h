#pragma once

#include <iostream>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include <algorithm>

std::string read_path();
System::Drawing::Bitmap^ RGB_to_mono(System::Drawing::Bitmap^ pic_RGB);
std::vector<double> read_T();
System::Drawing::Bitmap^ affine(System::Drawing::Bitmap^ pic, std::vector<double> T);
System::Drawing::Bitmap^ entropy_filt(System::Drawing::Bitmap^ pic, int nhood, bool is_RGB);
std::vector<std::vector<int>> line_strel(int len, int angle);
std::vector<std::vector<int>> mirror_matrix(std::vector<std::vector<int>>);
System::Drawing::Bitmap^ dilatation(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel,
									bool is_binary);
System::Drawing::Bitmap^ erosion(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel,
								 bool is_binary);
std::vector<std::vector<int>> normalize_vector(std::vector<std::vector<double>> input);
System::Drawing::Bitmap^ geodesic_map(System::Drawing::Bitmap^ pic, int x, int y);
