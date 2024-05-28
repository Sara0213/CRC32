#include <iostream>
#include <string>
#include <fstream>
#include <csignal>
#include <unistd.h> // For access function
using namespace std;

const string CRC32POLY = "100000100110000010001110110110111";

string getDataFromFile(string& path) { // pobieramy string z pliku wejściowego
    fstream file(path);
    string data;
    getline(file, data);
    file.close();
    return data;
}

void appendDataToFile(string& path, string& data) { // dopisujemy string do pliku
    fstream file;
    file.open(path, ios::app);
    file.write(data.c_str(), data.size());
    file.close();
}

string xorStrings(string a, string b) {
    string result;
    for(int i = 0; i < b.length(); i++) {
        if (a[i] == b[i])
            result += "0";
        else
            result += "1";
    }
    return result;
}

string modulo2Division(string dividend, string divisor) {
    // Ilość bitów do operacji XOR
    int n = divisor.length();
    string tmp = dividend.substr(0, n);
    while (n < dividend.length()) {
        if (tmp[0] == '1')
            // Dodajemy jeden bit z dzielnej
            tmp = xorStrings(divisor, tmp).substr(1, divisor.length() - 1) + dividend[n];
        else
            // Jeśli pierwszy bit to 0, dzielnikiem jest 0
            tmp = xorStrings(string(n, '0'), tmp).substr(1, divisor.length() - 1) + dividend[n];
        n += 1;
    }
    if (tmp[0] == '1')
        tmp = xorStrings(divisor, tmp);
    else
        tmp = xorStrings(string(n, '0'), tmp);
    return tmp;
}

string getCRC(string& s) {
    int generator_bits = CRC32POLY.size();
    string input_stream = s + string(generator_bits - 1, '0');
    string remainder = modulo2Division(input_stream, CRC32POLY);
    return remainder.substr(1, remainder.size() - 1);
}

bool isDataValid(string& s, string& rem) {
    rem = modulo2Division(s, CRC32POLY);
    for (int i = 0; i < rem.size(); ++i) {
        if(rem[i] == '1')
            return false;
    }
    return true;
}

void CRCValidate(string path) { // funkcja sprawdzająca poprawność wejścia z crc
    string input = getDataFromFile(path);
    for (int i = 0; i < input.size(); ++i) {
        if(input[i] != '0' && input[i] != '1'){
            printf("Wejście programu nieprawidłowe. Niedozwolony znak. Zatrzymuję pracę programu\n");
            return;
        }
    }
    if(input.size() < CRC32POLY.size()) {
        printf("Wejście programu jest nieprawidłowe. Błędna długość kodu. Zatrzymuję pracę programu.\n");
        return;
    }
    string remainder;
    bool data_valid = isDataValid(input, remainder);
    printf("Dzielna: %s, Dzielnik: %s, Reszta: %s\n", input.c_str(), CRC32POLY.c_str(), remainder.c_str());
    printf("Kodowanie jest %s\n", data_valid ? "poprawne" : "niepoprawne");
}

void CRCAdd(string path) { // funkcja dopisująca kod crc do pliku
    string input = getDataFromFile(path);
    string crc = getCRC(input);
    string input_with_crc = input + crc;
    printf("Dzielna: %s, Dzielnik: %s, CRC: %s\n", input.c_str(), CRC32POLY.c_str(), crc.c_str());
    printf("Wyjście: %s\n", input_with_crc.c_str());
    appendDataToFile(path, crc);
}

int main() {
    string path;
    printf("Podaj ścieżkę pliku wejściowego:\t");
    cin >> path;
    while (access(path.c_str(), F_OK) == -1) {
        printf("Plik nie istnieje lub jest niedostępny\n");
        printf("Podaj ścieżkę pliku wejściowego:\t");
        cin >> path;
    }
    printf("1 - sprawdzenie integralności pliku z dopisanym CRC\n2 - dopisanie do pliku CRC32\nPodaj wybór:\t");
    char c = 0;
    while(c != '1' && c != '2') {
        if(c != 0)
            printf("Niepoprawna opcja\n");
        cin >> c;
    }
    if(c == '1')
        CRCValidate(path);
    else if(c == '2')
        CRCAdd(path);
}
