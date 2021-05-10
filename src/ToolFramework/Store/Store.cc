#include "Store.hh"

void Store::Initialize(std::string configFilePath)
{
    std::ifstream file(configFilePath.c_str());
    std::string line;

    if (file.is_open()) {
        while (getline(file,line)) {
            if (line.size() > 0) {
                if (line.at(0) == '#') continue;
                std::string key, value;
                std::stringstream stream(line);
                if (stream >> key >> value)
                    storeMap[key] = value;
            }
        }
    }
    else
        std::cout << "\033[38;5;196m WARNING: Config file "
                  << configFilePath << " does not exist. No config loaded \033[0m" << std::endl;
    file.close();
}

void Store::Print()
{
    for (auto const& pair: storeMap)
        std::cout << pair.first << ": " << pair.second << std::endl;
}

std::istream& operator>>(std::istream& istr, TVector3& vec)
{
    std::string coordinate;
    for (int i = 0; i < 3; i++) {
        std::getline(istr, coordinate, ',');
        vec[i] = ::atof(coordinate.c_str());
    }
    return istr;
}

std::ostream& operator<<(std::ostream& ostr, TVector3 vec)
{
    ostr << vec.x() << "," << vec.y() << "," << vec.z();
    return ostr;
}