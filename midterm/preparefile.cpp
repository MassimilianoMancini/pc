#include <fstream>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 1) {
        std::cout << "Usage: " << argv[0] << " <filename>\n";
        return 1;
    }
    
    std::ifstream is (argv[1], std::ifstream::in);
    std::ofstream os ("preparedFile.txt", std::ofstream::out);
    
    char cc = 'a';
    
    while (is >> std::noskipws >> cc) {
        if (cc >= 'A' && cc <= 'Z') {
                cc = cc + 32;
        }
            
        if (cc >= 'a' && cc <= 'z') {
            os << cc;
        } 
    }
    
    is.close();
    os.close();
}