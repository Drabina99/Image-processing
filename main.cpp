#include <iostream>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>


std::string read_path();
//System::Drawing::Bitmap^ RGB_to_mono(System::Drawing::Bitmap^ pic_RGB);
std::vector<double> read_T();
System::Drawing::Bitmap^ affine(System::Drawing::Bitmap^ pic, std::vector<double> T);
System::Drawing::Bitmap^ entropy_filt(System::Drawing::Bitmap^ pic, int nhood, bool is_RGB);
std::vector<std::vector<int>> line_strel(int len, int angle);
//std::vector<std::vector<int>> bresenham(int x1, int y1, int x2, int y2);
System::Drawing::Bitmap^ dilatation(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel);
System::Drawing::Bitmap^ erosion(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel);

int main() {
	//std::cout << "WCZYTYWANIE OBRAZU RGB\n";
	//System::String^ path = gcnew System::String(read_path().c_str());
	System::String^ path_RGB = R"(D:\AO\polar.bmp)";
	System::String^ path_mono = R"(D:\AO\cameraman.bmp)";
	System::String^ path_bin = R"(D:\AO\dziury.bmp)";
	System::Drawing::Bitmap^ pic_RGB = gcnew System::Drawing::Bitmap(path_RGB, true);
	System::Drawing::Bitmap^ pic_mono = gcnew System::Drawing::Bitmap(path_mono, true);
	System::Drawing::Bitmap^ pic_bin = gcnew System::Drawing::Bitmap(path_bin, true);
	/*std::vector<double> T = read_T();
	System::Drawing::Bitmap^ affine_pic_RGB = affine(pic_RGB, T);
	System::Drawing::Bitmap^ affine_pic_mono = affine(pic_mono, T);
	affine_pic_RGB->Save(R"(D:\AO\affine_polar.png)");
	affine_pic_mono->Save(R"(D:\AO\affine_cameraman.png)");*/
	//System::Drawing::Bitmap^ ent_pic_mono = entropy_filt(pic_RGB, 3, false);
	std::vector<std::vector<int>> vec_strel = line_strel(13, 60);
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
	System::Drawing::Bitmap^ closed_bmp = erosion(dilatation(pic_bin, map_strel), map_strel);
	closed_bmp->Save(R"(D:\AO\closed_bmp.png)");

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
	const int width = pic->Width;
	const int height = pic->Height;
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

System::Drawing::Bitmap^ entropy_filt(System::Drawing::Bitmap^ pic, int nhood, bool is_RGB) {
	const int width = pic->Width;
	const int height = pic->Height;
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			//if(!is_RGB) {
				int sum = 0;
				int count = 0;
				std::vector<int> R(255, 0);
				std::vector<int> G(255, 0);
				std::vector<int> B(255, 0);
				for(int kz2 = kz - (nhood / 2); kz2 <= kz + (nhood / 2); kz2++) {
					for(int kx2 = kx - (nhood / 2); kx2 <= kx + (nhood / 2); kx2++) {
						System::Drawing::Color Px = pic->GetPixel(kx, kz);
						if(kz2 >= 0 && kz2 < height && kx2 >= 0 && kx2 < width &&
						   (kz2 != kz || kx2 != kx)) {
							//sum += Px.R + Px.G + Px.B;
							//std::cout << (int)Px.R << "\n";
							//std::cout << (int)Px.G << "\n";
							//std::cout << (int)Px.B << "\n";
							R[(int)Px.R]++;
							//std::cout << R[156] << "\n";
							G[(int)Px.G]++;
							B[(int)Px.B]++;
							count++;
							std::cout << count << "\n";
						}
						//std::cout << kz2 << " " << kx2 << "\n";
					}
				}
				//std::cout << sum << " " << count << "\n";
				/*double p_white = (double)(sum / count)/255;
				double p_black = 1 - p_white;*/

				//std::cout << p_white << " " << p_black << "\n";
				double ent_R = 0, ent_G = 0, ent_B = 0;
				double prob_R, prob_G, prob_B;
				for(int i = 0; i < 255; i++) {
					prob_R = (double)R[i] / (double)count;
					prob_G = (double)G[i] / (double)count;
					prob_B = (double)B[i] / (double)count;
					//if(prob_R > 0)
						//std::cout << prob_R << "\n";
					ent_R -= log(prob_R)*prob_R > 0 ? log(prob_R)*prob_R : 0;
					ent_G -= log(prob_G)*prob_G > 0 ? log(prob_G)*prob_G : 0;
					ent_B -= log(prob_B)*prob_B > 0 ? log(prob_B)*prob_B : 0;
				}
				std::cout << "ent_R = " << ent_R << "\n";
				std::cout << "ent_G = " << ent_G << "\n";
				std::cout << "ent_B = " << ent_B << "\n-\n";
				//System::Drawing::Color Px = System::Drawing::Color::FromArgb(ent_R, ent_G, ent_B);
				//res_pic->SetPixel(kx, kz, Px);
		}
		//}
	}
	return res_pic;
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

System::Drawing::Bitmap^ dilatation(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel) {
	const int width = pic->Width;
	const int height = pic->Height;
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			System::Drawing::Color Px = pic->GetPixel(kx, kz);
			bool found_true = false;
			for(auto it : strel) {
				if(kz + it.first >= 0 && kz + it.first < height && kx + it.second >= 0 && kx + it.second < width &&
				   it.first != 0 && it.second != 0) {
					System::Drawing::Color Px = pic->GetPixel(kx + it.second, kz + it.first);
					//std::cout << px_color << "\n";
					if((int)Px.R == 255) {
						res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(255, 255, 255));
						found_true = true;
						break;
					}
				}
			}
			if(!found_true) {
				res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(0, 0, 0));
			}
		}
	}
	return res_pic;
}
	
System::Drawing::Bitmap^ erosion(System::Drawing::Bitmap^ pic, std::unordered_multimap<int, int> strel) {
	const int width = pic->Width;
	const int height = pic->Height;
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			System::Drawing::Color Px = pic->GetPixel(kx, kz);
			bool found_false = false;
			for(auto it : strel) {
				if(kz + it.first >= 0 && kz + it.first < height && kx + it.second >= 0 && kx + it.second < width &&
				   it.first != 0 && it.second != 0) {
					System::Drawing::Color Px = pic->GetPixel(kx + it.second, kz + it.first);
					//std::cout << px_color << "\n";
					if((int)Px.R == 0) {
						res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(0, 0, 0));
						found_false = true;
						break;
					}
				}
			}
			if(!found_false) {
				res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(255, 255, 255));
			}
		}
	}
	return res_pic;
}
