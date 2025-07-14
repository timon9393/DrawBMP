#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>

class BMPReader {
private:
    std::ifstream file;
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::vector<std::vector<uint8_t>> pixelData;

public:
    bool openBMP(const std::string& fileName) {
        file.open(fileName, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Ошибка: Не удалось открыть файл " << fileName << std::endl;
            return false;
        }

        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

        if (fileHeader.bfType != 0x4D42 || (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32)) 
        {
            std::cerr << "Ошибка: Неправильный формат BMP файла. Поддерживаются только 24 и 32 бита." << std::endl;
            return false;
        }

        readPixelData();
        return true;
    }

    void readPixelData() {
        int width = infoHeader.biWidth;
        int height = infoHeader.biHeight;
        int bytesPerPixel = infoHeader.biBitCount / 8;
        int padding = (4 - (width * bytesPerPixel) % 4) % 4;

        pixelData.resize(height, std::vector<uint8_t>(width));

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint8_t colors[4] = { 0 };
                file.read(reinterpret_cast<char*>(colors), bytesPerPixel);

                if (colors[0] == 0 && colors[1] == 0 && colors[2] == 0) {
                    pixelData[height - y - 1][x] = 1;  
                }
                else if (colors[0] == 255 && colors[1] == 255 && colors[2] == 255) {
                    pixelData[height - y - 1][x] = 0; 
                }
            }
            file.ignore(padding);
        }
    }

    void displayBMP() const {
        for (const auto& row : pixelData) {
            for (uint8_t pixel : row) {
                std::cout << (pixel == 1 ? '#' : ' ');
            }
            std::cout << '\n';
        }
    }

    void closeBMP() {
        if (file.is_open()) {
            file.close();
        }
        pixelData.clear();
    }
};

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    if (argc != 2) {
        std::cout << argc << std::endl;
        std::cerr << "Использование: " << argv[0] << " <путь к BMP файлу>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    BMPReader bmpReader;

    if (bmpReader.openBMP(fileName)) {
        bmpReader.displayBMP();
        bmpReader.closeBMP();
    }

    return 0;
}
