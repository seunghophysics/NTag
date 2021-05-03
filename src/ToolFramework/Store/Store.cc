#include "Store.hh"

void Store::Print()
{
    for (auto const& pair: m_variables)
        std::cout << pair.first << ": " << pair.second << std::endl; 
}