#include <iostream>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include <algorithm>


std::string read_path();
//System::Drawing::Bitmap^ RGB_to_mono(System::Drawing::Bitmap^ pic_RGB);
std::vector<double> read_T();
System::Drawing::Bitmap^ affine(System::Drawing::Bitmap^ pic, std::vector<double> T);
System::Drawing::Bitmap^ entropy_filt(System::Drawing::Bitmap^ pic, int nhood);
std::vector<std::vector<int>> line_strel(int len, int angle);
//std::vector<std::vector<int>> bresenham(int x1, int y1, int x2, int y2);
System::Drawing::Bitmap^ dilatation(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel,
									bool is_binary);
System::Drawing::Bitmap^ erosion(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel,
								 bool is_binary);
std::vector<std::vector<int>> normalize_vector(std::vector<std::vector<double>> input);


int main() {
	//std::cout << "WCZYTYWANIE OBRAZU RGB\n";
	//System::String^ path = gcnew System::String(read_path().c_str());
	System::String^ path_RGB = R"(D:\AO\polar.bmp)";
	System::String^ path_RGB_small = R"(D:\AO\maslo.bmp)";
	System::String^ path_mono = R"(D:\AO\cameraman.bmp)";
	System::String^ path_bin = R"(D:\AO\dziury.bmp)";
	System::Drawing::Bitmap^ pic_RGB = gcnew System::Drawing::Bitmap(path_RGB, true);
	System::Drawing::Bitmap^ pic_RGB_small = gcnew System::Drawing::Bitmap(path_RGB_small, true);
	System::Drawing::Bitmap^ pic_mono = gcnew System::Drawing::Bitmap(path_mono, true);
	System::Drawing::Bitmap^ pic_bin = gcnew System::Drawing::Bitmap(path_bin, true);
	
	/*std::vector<double> T = read_T();
	System::Drawing::Bitmap^ affine_pic_RGB = affine(pic_RGB, T);
	System::Drawing::Bitmap^ affine_pic_mono = affine(pic_mono, T);
	affine_pic_RGB->Save(R"(D:\AO\affine_polar.png)");
	affine_pic_mono->Save(R"(D:\AO\affine_cameraman.png)");*/
	
	System::Drawing::Bitmap^ ent_pic_mono = entropy_filt(pic_mono, 9);
	ent_pic_mono->Save(R"(D:\AO\ent_cameraman.png)");
	System::Drawing::Bitmap^ ent_pic_rgb = entropy_filt(pic_RGB_small, 3);
	ent_pic_rgb->Save(R"(D:\AO\ent_maslo.png)");
	
	/*std::vector<std::vector<int>> vec_strel = line_strel(17, 60);
	for(int i = 0; i < vec_strel.size(); i++) {
		for(int j = 0; j < vec_strel[i].size(); j++)
			std::cout << vec_strel[i][j] << " ";
		std::cout << "\n";
	}
	int x_cent = vec_strel.size() / 2;
	int y_cent = vec_strel[0].size() / 2;
	//std::cout << x_sr << " " << y_sr << "\n";
	std::unordered_multimap<int, int> map_strel;
	const int vec_size = vec_strel.size();
	for(int i = 0; i < vec_size; i++) {
		for(int j = 0; j < vec_strel[i].size(); j++) {
			if(vec_strel[i][j])
				map_strel.insert({i - x_cent, j - y_cent});
		}
	}
	for(auto it : map_strel)
		std::cout << it.first << " " << it.second << "\n";
	System::Drawing::Bitmap^ closed_bmp = erosion(dilatation(pic_bin, map_strel, true), map_strel, true);
	System::Drawing::Bitmap^ closed_mono = erosion(dilatation(pic_mono, map_strel, false), map_strel, false);
	closed_bmp->Save(R"(D:\AO\closed_bmp.png)");
	closed_mono->Save(R"(D:\AO\closed_mono.png)");*/

	return 0;
}

std::string read_path() {
	std::string res = R"()";
	std::cout << "Prosze podac sciezke do pliku: ";
	std::getline(std::cin, res);
	return res;
}

/*System::Drawing::Bitmap^ RGB_to_mono(System::Drawing::Bitmap^ pic_RGB) {
	const int width = pic_RGB->Width;
	const int height = pic_RGB->Height;
	System::Drawing::Bitmap^ pic_mono = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			System::Drawing::Color Px = pic_RGB->GetPixel(kx, kz);
			int mono_px = (int)(0.299 * (double)Px.R + 0.587 * (double)Px.G + 0.114 * (double)Px.B);
			pic_mono->SetPixel(kx, kz, System::Drawing::Color::FromArgb(mono_px, mono_px, mono_px));
		}
	}
	return pic_mono;
}*/

std::vector<double> read_T() {
	std::vector<double> T_vec(4);
	char T_ind = 'A';
	for(double& x : T_vec) {
		std::cout << "Prosze podac wartosc T" << T_ind++ << ": ";
		std::cin >> x;
	}
	return T_vec;
}

System::Drawing::Bitmap^ affine(System::Drawing::Bitmap^ pic, std::vector<double> T) {
	const int width = pic->Width * T[0];
	const int height = pic->Height * T[3];
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			int nx = (int)((double)T[0] * kx + (double)T[2] * kz);
			int nz = (int)((double)T[1] * kx + (double)T[3] * kz);
			if(nz >= 0 && nz < height && nx >= 0 && nx < width) {
				res_pic->SetPixel(nx, nz, pic->GetPixel(kx, kz));
			}
		}
	}
	return res_pic;
}

System::Drawing::Bitmap^ entropy_filt(System::Drawing::Bitmap^ pic, int nhood) {
	const int width = pic->Width;
	const int height = pic->Height;
	std::vector<std::vector<double>> entropy_vector_R(height, std::vector<double>(width, 0));
	std::vector<std::vector<double>> entropy_vector_G(height, std::vector<double>(width, 0));
	std::vector<std::vector<double>> entropy_vector_B(height, std::vector<double>(width, 0));
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		std::cout << kz << "\n";
		for(int kx = 0; kx < width; kx++) {
			int sum = 0;
			int count = 0;
			std::vector<int> R(256, 0);
			std::vector<int> G(256, 0);
			std::vector<int> B(256, 0);
				
			for(int kz2 = kz - (nhood / 2); kz2 <= kz + (nhood / 2); kz2++) {
				for(int kx2 = kx - (nhood / 2); kx2 <= kx + (nhood / 2); kx2++) {
					if(kz2 >= 0 && kz2 < height && kx2 >= 0 && kx2 < width &&
						(kz2 != kz || kx2 != kx)) {
						System::Drawing::Color Px = pic->GetPixel(kx2, kz2);
						R[(int)Px.R]++;
						G[(int)Px.G]++;
						B[(int)Px.B]++;
						count++;
						//std::cout << count << "\n";
					}
						//std::cout << kz2 << " " << kx2 << "\n";
				}
			}
				
			double ent_R = 0, ent_G = 0, ent_B = 0;
			double prob_R, prob_G, prob_B;
			for(int i = 0; i <= 255; i++) {
				prob_R = (double)R[i] / (double)count;
				prob_G = (double)G[i] / (double)count;
				prob_B = (double)B[i] / (double)count;
				//if(prob_R > 0)
					//std::cout << prob_R << " " << log10(prob_R) << "\n";
				//std::cout << R[i] << "\n";
					
				ent_R -= prob_G > 0 ? log(prob_G)*prob_G : 0;
				ent_G -= prob_G > 0 ? log(prob_G)*prob_G : 0;
				ent_B -= prob_B > 0 ? log2(prob_B)*prob_B : 0;

				//if(ent_R > 0)
					//std::cout << ent_R << "\n";
			}

			entropy_vector_R[kz][kx] = ent_R;
			entropy_vector_G[kz][kx] = ent_G;
			entropy_vector_B[kz][kx] = ent_B;
			//std::cout << "R " << entropy_vector_R[kz][kx] << "\n";
			//std::cout << "G" << entropy_vector_G[kz][kx] << "\n";
			//std::cout << "B" << entropy_vector_B[kz][kx] << "\n";

			//System::Drawing::Color Px = System::Drawing::Color::FromArgb((int)(255*ent_R), 
			//											(int)(255*ent_G), (int)(255*ent_B));
			//std::cout << "R" << ent_R << " G" << ent_G << " B" << ent_B << "\n-\n";
			//res_pic->SetPixel(kx, kz, Px);
			//std::cout << "var = " << Px.R << "\n";
		}
		
	}
	/*std::cout << "aaa\n";
	for(int i = 0; i < entropy_vector_R.size(); i++) {
		for(int j = 0; j < entropy_vector_R[i].size(); j++) {
			std::cout << entropy_vector_R[i][j] << "\n";
		}
	}*/
	std::cout << "ok\n";
	std::vector<std::vector<int>> normalized_R = normalize_vector(entropy_vector_R);
	//std::cout << "size = " << normalized_R.size() << "\n";
	//std::cout << normalized_R[5][6] << "\n";
	std::vector<std::vector<int>> normalized_G = normalize_vector(entropy_vector_G);
	std::vector<std::vector<int>> normalized_B = normalize_vector(entropy_vector_B);
	/*std::cout << "--------------------------\n";
		for(int i = 0; i < normalized_R.size(); i++) {
			for(int j = 0; j < normalized_R[i].size(); j++) {
				std::cout << normalized_R[i][j] << "\n";
			}
		}
		std::cout << "---------------------------";*/

	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			System::Drawing::Color Px = System::Drawing::Color::FromArgb(normalized_R[kz][kx], 
											normalized_G[kz][kx], normalized_B[kz][kx]);
			res_pic->SetPixel(kx, kz, Px);
		}
	}
	
	return res_pic;
}

std::vector<std::vector<int>> normalize_vector(std::vector<std::vector<double>> input) {
	std::vector<std::vector<int>> res_vector(input.size(), std::vector<int>(input[0].size(), 0));
	double min = 5000;// std::numeric_limits<double>::max();
	double max = 0;
	//std::cout << input[2][3] << "\n";
	for(int i = 0; i < input.size(); i++) {
		for(int j = 0; j < input[i].size(); j++) {
			//std::cout << input[i][j] << "\n";
			if(input[i][j] > max) {
				max = input[i][j];
			}
			if(input[i][j] < min) {
				min = input[i][j];
			}
		}
	}
	//std::cout << "max = " << max << "\n";
	//std::cout << "min = " << min << "\n";
	double old_range = max - min;
	//if(old_range < 0)
		//std::cout << old_range << "\n";
	for(int i = 0; i < input.size(); i++) {
		for(int j = 0; j < input[i].size(); j++) {
			double scale = (input[i][j] - min) / old_range;
			res_vector[i][j] = (int)(255 * scale);
			//std::cout << res_vector[i][j] << "\n";
		}
	}
	return res_vector;

}

std::vector<std::vector<int>> line_strel(int len, int angle) {
	double angle_rad = double(angle * M_PI) / 180.0;
	int x1 = 0, y1 = 0;
	int x2 = (cos(angle_rad) * len) - 1;
	int y2 = (sin(angle_rad) * len) - 1;
	x2 = x2 % 2 ? x2 + 1 : x2;
	y2 = y2 % 2 ? y2 + 1 : y2;
	//std::cout << x2 << " " << y2 << "\n";
	std::vector<std::vector<int>> strel_vec(x2 + 1, std::vector<int>(y2 + 1, 0));
	int dx = abs(x2 - x1); 
	int sx = x1 < x2 ? 1 : -1;
	int dy = abs(y2 - y1); 
	int sy = y1 < y2 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;
	int e2;
	for(;;) {
		strel_vec[x1][y1] = 1; 
		if(x1 == x2 && y1 == y2) 
			break;
		e2 = err;
		if(e2 > -dx) { 
			err -= dy; 
			x1 += sx; 
		}
		if(e2 < dy) { 
			err += dx;
			y1 += sy; 
		}
	}
	return strel_vec;
}

System::Drawing::Bitmap^ dilatation(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel, 
									bool is_binary) {
	const int width = pic->Width;
	const int height = pic->Height;
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			System::Drawing::Color Px = pic->GetPixel(kx, kz);
			bool found_true = false; // dla obrazow binarnych
			int max = 0; // dla obrazow monochromatycznych
			for(auto it : strel) {
				if(kz + it.first >= 0 && kz + it.first < height && kx + it.second >= 0 && kx + it.second < width &&
				   it.first != 0 && it.second != 0) {
					System::Drawing::Color Px = pic->GetPixel(kx + it.second, kz + it.first);
					if(is_binary) {
						if((int)Px.R == 255) {
							res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(255, 255, 255));
							found_true = true;
							break;
						}
					}
					else {
						if((int)Px.R > max) {
							max = (int)Px.R;
						}
					}
				}
			}
			if(!is_binary) {
				res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(max, max, max));
			}
			else if(!found_true) {
				res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(0, 0, 0));
			}
		}
	}
	return res_pic;
}
	
System::Drawing::Bitmap^ erosion(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel,
								 bool is_binary) {
	const int width = pic->Width;
	const int height = pic->Height;
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			System::Drawing::Color Px = pic->GetPixel(kx, kz);
			bool found_false = false; // dla binarnych
			int min = 255; // dla monochrom.
			for(auto it : strel) {
				if(kz + it.first >= 0 && kz + it.first < height && kx + it.second >= 0 && kx + it.second < width &&
				   it.first != 0 && it.second != 0) {
					System::Drawing::Color Px = pic->GetPixel(kx + it.second, kz + it.first);
					//std::cout << px_color << "\n";
					if(is_binary) {
						if((int)Px.R == 0) {
							res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(0, 0, 0));
							found_false = true;
							break;
						}
					}
					else {
						if((int)Px.R < min) {
							min = (int)Px.R;
						}
					}
				}
			}
			if(!is_binary) {
				res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(min, min, min));
			}
			else if(!found_false) {
				res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(255, 255, 255));
			}
		}
	}
	return res_pic;
}



