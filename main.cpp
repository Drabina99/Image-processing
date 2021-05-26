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


int main() {
	std::cout << "************************************\n"
				 "| Program do przetwarzania obrazow |\n"
		         "| Piotr Drabicki 2021              |\n"
		         "|                                  |\n"
		         "************************************\n"
		         "************************************\n";
	std::cout << "\nDzien dobry!\n\nNa poczatku prosze o skonfigurowanie obrazow testowych.\n"
		"Obrazy te beda uzywane w poszczegolnych operacjach.\n\n";
	std::cout << "OBRAZ RGB:\n";
	System::String^ path_RGB = gcnew System::String(read_path().c_str());
	std::cout << "OBRAZ MONOCHROMATYCZNY:\n";
	System::String^ path_mono = gcnew System::String(read_path().c_str());
	std::cout << "OBRAZ LOGICZNY:\n";
	System::String^ path_bin = gcnew System::String(read_path().c_str());
	System::Drawing::Bitmap^ pic_RGB = gcnew System::Drawing::Bitmap(path_RGB, true);
	System::Drawing::Bitmap^ pic_mono = gcnew System::Drawing::Bitmap(path_mono, true);
	System::Drawing::Bitmap^ pic_bin = gcnew System::Drawing::Bitmap(path_bin, true);
	
	while(1) {
		char choice;
		std::cout << "\nProsze wybrac operacje:\n1) przeksztalcenie afiniczne\n2) filtracja entropii\n"
			"3) zamkniecie liniowym elementem strukturalnym\n4) mapa odleglosci geodezyjnej\n"
			"Dowolny inny klawisz, aby wyjsc z programu...\n : ";
		std::cin >> choice;
		switch(choice) {
		case '1': {
			std::cout << "PRZEKSZTALCENIE AFINICZNE\n\n";
			std::vector<double> T = read_T();
			System::Drawing::Bitmap^ affine_pic_RGB = affine(pic_RGB, T);
			System::Drawing::Bitmap^ affine_pic_mono = affine(pic_mono, T);
			affine_pic_RGB->Save(R"(D:\affine_RGB.png)");
			affine_pic_mono->Save(R"(D:\affine_mono.png)");
			break;
		}
		case '2': {
			int neigh = 0;
			std::cout << "FILTRACJA ENTROPII\n\nProsze podac rozmiar okna: ";
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
			std::cout << "ZAMKNIECIE LINIOWYM ELEMENTEM STRUKTURALNYM\n"
				"Prosze podac dlugosc elementu: ";
			std::cin >> len;
			std::cout << "Prosze podac kat: ";
			std::cin >> angle;
			std::vector<std::vector<int>> vec_strel = line_strel(len, angle);
			std::cout << "Element strukturalny:\n\n";
			for(int i = 0; i < vec_strel.size(); i++) {
				for(int j = 0; j < vec_strel[i].size(); j++)
					std::cout << vec_strel[i][j] << " ";
				std::cout << "\n";
			}
			std::cout << "n";
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
			for(auto it : map_strel)
				std::cout << it.first << " " << it.second << "\n";
			System::Drawing::Bitmap^ closed_bmp = erosion(dilatation(pic_bin, map_strel, true), map_strel, true);
			System::Drawing::Bitmap^ closed_mono = erosion(dilatation(pic_mono, map_strel, false), map_strel, false);
			closed_bmp->Save(R"(D:\closed_bin.png)");
			closed_mono->Save(R"(D:\closed_mono.png)");
			break;
		}
		default: {
			std::cout << "\nDziekuje za skorzystanie z programu!\nDo zobaczenia!\n";
			return 0;
		}
		}
	}
	
	/*std::vector<std::vector<int>> vec_strel = line_strel(13, 60);
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

	//System::Drawing::Bitmap^ geodesic = geodesic_map(pic_bin, 93, 96);
	//geodesic->Save(R"(D:\AO\geodesic.png)");

	//return 0;
}

std::string read_path() {
	std::string res = R"()";
	std::cout << "Prosze podac sciezke do pliku: ";
	std::getline(std::cin, res);
	return res;
}

System::Drawing::Bitmap^ RGB_to_mono(System::Drawing::Bitmap^ pic_RGB) {
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
}

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
	std::cout << "\nPrzetwarzanie obrazu, prosze czekac...\n";
	const int width = pic->Width;
	const int height = pic->Height;
	int max_up = height;
	int max_right = width;
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			int nx = (int)((double)T[0] * kx + (double)T[2] * kz);
			int nz = (int)((double)T[1] * kx + (double)T[3] * kz);
			if(nz > max_up)
				max_up = nz;
			if(nx > max_right)
				max_right = nx;
		}
	}
	max_right = max_right != width ? max_right + 1 : width;
	max_up = max_up != height ? max_up + 1 : height;
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(max_right, max_up);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			int nx = (int)((double)T[0] * kx + (double)T[2] * kz);
			int nz = (int)((double)T[1] * kx + (double)T[3] * kz);
			if(nz >= res_pic->Height)
				std::cout << nz << "\n";
			res_pic->SetPixel(nx, nz, pic->GetPixel(kx, kz));
			
		}
	}
	std::cout << "PRZEKSZTALCANIE AFINICZNE ZAKONCZONE!\n";
	return res_pic;
}

System::Drawing::Bitmap^ entropy_filt(System::Drawing::Bitmap^ pic, int nhood, bool is_RGB) {
	std::cout << "\nPrzetwarzanie obrazu, prosze czekac...\n";
	std::cout << "\n|0%            100%|\n";
	if(is_RGB) {
		pic = RGB_to_mono(pic);
	}
	const int width = pic->Width;
	const int height = pic->Height;
	std::vector<std::vector<double>> entropy_vector_R(height, std::vector<double>(width, 0));
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		if(kz % (height / 10) == 0 && kz != 0)
			std::cout << "**";
		for(int kx = 0; kx < width; kx++) {
			int sum = 0;
			int count = 0;
			std::vector<int> R(256, 0);
			for(int kz2 = kz - (nhood / 2); kz2 <= kz + (nhood / 2); kz2++) {
				for(int kx2 = kx - (nhood / 2); kx2 <= kx + (nhood / 2); kx2++) {
					if(kz2 >= 0 && kz2 < height && kx2 >= 0 && kx2 < width &&
						(kz2 != kz || kx2 != kx)) {
						System::Drawing::Color Px = pic->GetPixel(kx2, kz2);
						R[(int)Px.R]++;
						count++;
					}
				}
			}
			double ent = 0;
			double prob;
			for(int i = 0; i <= 255; i++) {
				prob = (double)R[i] / (double)count;
				ent -= prob > 0 ? log(prob) * prob : 0;
			}
			entropy_vector_R[kz][kx] = ent;
		}
	}
	std::vector<std::vector<int>> normalized_R = normalize_vector(entropy_vector_R);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			System::Drawing::Color Px = System::Drawing::Color::FromArgb(normalized_R[kz][kx], 
											normalized_R[kz][kx], normalized_R[kz][kx]);
			res_pic->SetPixel(kx, kz, Px);
		}
	}
	std::cout << "\nFILTRACJA ENTROPII ZAKONCZONA!\n\n";
	return res_pic;
}

std::vector<std::vector<int>> normalize_vector(std::vector<std::vector<double>> input) {
	std::vector<std::vector<int>> res_vector(input.size(), std::vector<int>(input[0].size(), 0));
	double min = std::numeric_limits<double>::max();
	double max = 0;
	for(int i = 0; i < input.size(); i++) {
		for(int j = 0; j < input[i].size(); j++) {
			if(input[i][j] > max) {
				max = input[i][j];
			}
			if(input[i][j] < min) {
				min = input[i][j];
			}
		}
	}
	double old_range = max - min;
	for(int i = 0; i < input.size(); i++) {
		for(int j = 0; j < input[i].size(); j++) {
			double scale = (input[i][j] - min) / old_range;
			res_vector[i][j] = (int)(255 * scale);
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
	int rows = strel_vec.size();
	int cols = strel_vec[0].size();
	strel_vec = mirror_matrix(strel_vec);	
	return strel_vec;
}

std::vector<std::vector<int>> mirror_matrix(std::vector<std::vector<int>> matrix) {
	int rows = matrix.size();
	int cols = matrix[0].size();
	std::vector<std::vector<int>> mirror(rows, std::vector<int>(cols));
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			mirror[i][j] = matrix[i][cols - 1 - j];
		}
	}
	return mirror;
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

System::Drawing::Bitmap^ geodesic_map(System::Drawing::Bitmap^ pic, int x, int y) {
	std::cout << "MAPA ODLEGLOSCI GEODEZYJNEJ\nPrzetwarzanie obrazu, prosze czekac...\n";
	const int width = pic->Width;
	const int height = pic->Height;
	std::vector<std::vector<int>> marker(width, std::vector<int>(height));
	std::vector<std::vector<int>> distance(width, std::vector<int>(height));
	System::Drawing::Bitmap^ res_pic = gcnew System::Drawing::Bitmap(width, height);
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			distance[kx][kz] = -1;
		}
	}
	distance[x][y] = 0;
	int min_x = x;
	int max_x = x;
	int min_y = y;
	int max_y = y;

	int iter = 1;
	int flag;
	int temp;
	while(1) {
		flag = 0;
		min_x--;
		max_x++;
		min_y--;
		max_y++;

		for(int kz = min_y; kz <= max_y; kz++) {
			for(int kx = min_x; kx <= max_x; kx++) {
				if(kz >= 0 && kz < height && kx >= 0 && kx < width) {
					System::Drawing::Color Px = pic->GetPixel(kx, kz);
					if((int)Px.R == 255) {
						marker[kx][kz] = 1;
						if(distance[kx][kz] == -1) {
							distance[kx][kz] = iter;
							flag++;
						}
					}
				}
			}
		}
		iter++;
		if(flag == 0) {
			break;
		}
	}
	iter--;
	for(int kz = 0; kz < height; kz++) {
		for(int kx = 0; kx < width; kx++) {
			if(distance[kx][kz] > 0) {
				temp = distance[kx][kz] * 255 / iter;
			}
			else {
				temp = 0;
			}
			res_pic->SetPixel(kx, kz, System::Drawing::Color::FromArgb(temp, temp, temp));
		}
	}
		return res_pic;
}


