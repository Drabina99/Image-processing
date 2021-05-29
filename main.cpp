#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "image_processing.h"


int main() {
	std::cout << "************************************\n"
				 "| Image processing                 |\n"
		         "| 2021 Piotr Drabicki              |\n"
		         "|                                  |\n"
		         "************************************\n"
		         "************************************\n";
	std::cout << "\nPlease specify test images.\n\n";
	std::cout << "RGB\n";
	System::String^ path_RGB = gcnew System::String(read_path().c_str());
	std::cout << "\nMONOCHROME\n";
	System::String^ path_mono = gcnew System::String(read_path().c_str());
	std::cout << "\nBINARY\n";
	System::String^ path_bin = gcnew System::String(read_path().c_str());
	System::Drawing::Bitmap^ pic_RGB = gcnew System::Drawing::Bitmap(path_RGB, true);
	System::Drawing::Bitmap^ pic_mono = gcnew System::Drawing::Bitmap(path_mono, true);
	System::Drawing::Bitmap^ pic_bin = gcnew System::Drawing::Bitmap(path_bin, true);
	
	while(1) {
		char choice;
		std::cout << "\nPlease choose the operation:\n1) Affine transformation\n2) Entropy filtering\n"
			"3) Closing\n4) Geodesic map\n"
			"Press any other key to exit...\n : ";
		std::cin >> choice;
		switch(choice) {
			case '1': {
				std::cout << "\nAFFINE TRANSFORMATION\n\n";
				std::vector<double> T = read_T();
				System::Drawing::Bitmap^ affine_pic_RGB = affine(pic_RGB, T);
				System::Drawing::Bitmap^ affine_pic_mono = affine(pic_mono, T);
				affine_pic_RGB->Save(R"(D:\affine_RGB.png)");
				affine_pic_mono->Save(R"(D:\affine_mono.png)");
				break;
			}
			case '2': {
				int neigh = 0;
				std::cout << "\nENTROPY FILTERING\n\nPlease specify neighbourhood size: ";
				std::cin >> neigh;
				std::cout << "\n";
				System::Drawing::Bitmap^ ent_pic_mono = entropy_filt(pic_mono, neigh, false);
				ent_pic_mono->Save(R"(D:\ent_mono.png)");
				System::Drawing::Bitmap^ ent_pic_rgb = entropy_filt(pic_RGB, neigh, true);
				ent_pic_rgb->Save(R"(D:\ent_RGB.png)");
				break;
			}
			case '3': {
				int len = 0;
				int angle = 0;
				std::cout << "\nCLOSING\n"
					"Please specify linear structuring element length: ";
				std::cin >> len;
				std::cout << "Please specify angle: ";
				std::cin >> angle;
				std::vector<std::vector<int>> vec_strel = line_strel(len, angle);
				std::cout << "\nStructuring element:\n---------------------\n";
				for(int i = 0; i < vec_strel.size(); i++) {
					for(int j = 0; j < vec_strel[i].size(); j++)
						std::cout << vec_strel[i][j] << " ";
					std::cout << "\n";
				}
				std::cout << "---------------------\n";
				int x_cent = vec_strel.size() / 2;
				int y_cent = vec_strel[0].size() / 2;
				std::unordered_multimap<int, int> map_strel;
				const int vec_size = vec_strel.size();
				for(int i = 0; i < vec_size; i++) {
					for(int j = 0; j < vec_strel[i].size(); j++) {
						if(vec_strel[i][j])
							map_strel.insert({i - x_cent, j - y_cent});
					}
				}
				std::cout << "Please wait...\n";
				System::Drawing::Bitmap^ closed_bmp = erosion(dilatation(pic_bin, map_strel, true), 
															  map_strel, true);
				std::cout << "DONE!\n";
				std::cout << "Please wait...\n";
				System::Drawing::Bitmap^ closed_mono = erosion(dilatation(pic_mono, map_strel, false), 
															   map_strel, false);
				std::cout << "DONE!\n";
				closed_bmp->Save(R"(D:\closed_bin.png)");
				closed_mono->Save(R"(D:\closed_mono.png)");
				break;
			}
			case '4': {
				int x = 0, y = 0;
				std::cout << "\nGEODESIC MAP\n";
				std::cout << "Please specify the point.\nx: ";
				std::cin >> x;
				std::cout << "y: ";
				std::cin >> y;
				System::Drawing::Bitmap^ geodesic = geodesic_map(pic_bin, x, y);
				geodesic->Save(R"(D:\geodesic.png)");
				break;
			}
			default: {
				std::cout << "\nGoodbye!\n";
				return 0;
			}
		}
	}
	return 0;
}
