#include "ImageLoader.h"

namespace ImageLoader{
	bool loadImage(std::shared_ptr<Image> image, std::string path){
		auto findExt = path.rfind('.');
		if(findExt != std::string::npos){
			std::string extension = path.substr(findExt + 1);
			image->data.clear();
			image->width = 0;
			image->height = 0;

			if(extension == "png" || extension == "jpg"){
				int image_width = 0;
				int image_height = 0;
				unsigned char* rawData = stbi_load(path.c_str(), &image_width, &image_height, nullptr, 4);
				if(rawData == nullptr){
					return false;
				}
				std::vector<unsigned char> data(rawData, rawData + 4 * image_width * image_height);
				image->data = data;
				image->width = image_width;
				image->height = image_height;
				return true;
			}
			else if(extension == "pbm" || extension == "pgm" || extension == "ppm"){
				std::ifstream f(path);
				int header = 0, colorValue = -1;
				std::vector<unsigned char> data;
				for(std::string line; std::getline(f, line);){
					if(line.size() < 1 || line.at(0) == '#'){
						continue;
					}
					else if(line.size() > 0 && header == 0){
						header = line.at(line.find('P') + 1) - '0';
					}
					else if(line.size() > 0 && image->width == 0){
						std::stringstream ss(line);
						ss >> image->width >> image->height;
					}
					else if(line.size() > 0 && colorValue == -1 && (header == 2 || header == 3 || header == 5 || header == 6)){
						std::stringstream ss(line);
						ss >> colorValue;
						line = "";
					}
					else if(line.size() > 0 && header != 0){
						while(line.size() > 0 && line.at(0) == ' '){
							line = line.substr(1);
						}
						if(header > 3){
							if(header == 4){
								for(unsigned char i : line){
									for(int j = 7; j > -1; j--){
										data.push_back(((i >> j) & 1));
									}
								}
							}
							else{
								for(unsigned char i : line){
									data.push_back(i);
								}
							}
						}
						else{
							if(header == 1){
								for(unsigned char i : line){
									data.push_back(i - '0');
								}
							}
							else{
								data.push_back(std::stoi(line));
								
							}
						}
					}
				}
				f.close();

				if((header - 1) % 3 == 0){
					for(const auto& i : data){
						image->data.push_back(i * 255);
						image->data.push_back(i * 255);
						image->data.push_back(i * 255);
						image->data.push_back(255);
					}
				}
				else if((header - 1) % 3 == 1){
					for(const auto& i : data){
						image->data.push_back(i);
						image->data.push_back(i);
						image->data.push_back(i);
						image->data.push_back(255);
					}
				}
				else{
					for(int i = 0; i < data.size(); i++){
						image->data.push_back(data.at(i));
						if(i % 3 == 2){
							image->data.push_back(255);
						}
					}
				}

			}
		}
		return true;
	}

	bool saveImage(std::shared_ptr<Image> image, std::string path){
		if(!image->data.empty()){
			auto findExt = path.rfind('.');
			if(findExt != std::string::npos){
				std::string extension = path.substr(findExt + 1);

				if(extension == "png"){
					stbi_write_png(path.c_str(), image->width, image->height, 4, image->data.data(), 4 * sizeof(unsigned char) * image->width);
				}
				else if(extension == "jpg"){
					stbi_write_jpg(path.c_str(), image->width, image->height, 4, image->data.data(), 90);
				}
				else if(extension == "pbm"){
					std::ofstream f(path);
					if(f.is_open()){
						f << "P1\n";
						f << "# created by Advanced Image Analyzer\n";
						f << image->width << " " << image->height << "\n";
						int counter = 0;
						std::array<unsigned char, 3> vals;
						for(int i = 0; i < image->data.size(); i++){
							if(i % 4 == 3){
								counter += 1;
								int avg = (vals.at(0) + vals.at(1) + vals.at(2)) / 3;
								if(avg < 128){
									f << '0';
								}
								else{
									f << '1';
								}
								if(counter % image->width == image->width - 1){
									f << "\n";
								}
							}
							else{
								vals.at(i % 4) = image->data.at(i);
							}
						}
					}
					f.close();
				}
				else if(extension == "pgm"){
					std::ofstream f(path);
					if(f.is_open()){
						f << "P2\n";
						f << "# created by Advanced Image Analyzer\n";
						f << image->width << " " << image->height << "\n";
						f << "255\n";
						std::array<unsigned char, 3> vals;
						for(int i = 0; i < image->data.size(); i++){
							if(i % 4 == 3){
								f << (vals.at(0) + vals.at(1) + vals.at(2)) / 3 << "\n";
							}
							else{
								vals.at(i % 4) = image->data.at(i);
							}
						}
					}
					f.close();
				}
				else if(extension == "ppm"){
					std::ofstream f(path);
					if(f.is_open()){
						f << "P3\n";
						f << "# created by Advanced Image Analyzer\n";
						f << image->width << " " << image->height << "\n";
						f << "255\n";
						for(int i = 0; i < image->data.size(); i++){
							if(i % 4 != 3){
								f << int(image->data.at(i)) << "\n";
							}
						}
					}
					f.close();
				}
			}
		}
		return false;
	}
}