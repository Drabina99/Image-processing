#include <iostream>
#include <string>
#include <vector>

std::string read_path();
std::vector<double> read_T();
System::Drawing::Bitmap^ RGB_to_mono(System::Drawing::Bitmap^ pic_RGB);
System::Drawing::Bitmap^ affine(System::Drawing::Bitmap^ pic, std::vector<double> T);

int main() {
	//System::String^ path = gcnew System::String(read_path().c_str());
	System::String^ path = R"(D:\AO\polar.bmp)";
	System::Drawing::Bitmap^ pic_RGB = gcnew System::Drawing::Bitmap(path, true);
	System::Drawing::Bitmap^ pic_mono = RGB_to_mono(pic_RGB);
	const int width = pic_RGB->Width;
	const int height = pic_RGB->Height;
	std::vector<double> T = read_T();
	System::Drawing::Bitmap^ affine_pic = affine(pic_RGB, T);
	affine_pic->Save(R"(D:\AO\affine_polar.png)");
	return 0;
}

std::string read_path() {
	std::string res = R"()";
	std::cout << "Prosze podac sciezke do pliku: ";
	std::getline(std::cin, res);
	return res;
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
