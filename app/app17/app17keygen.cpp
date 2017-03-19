
#include <iostream>
#include <sstream>
#include <iomanip>

template <typename T>
std::string keygen(const std::string &login)
{
    std::stringstream ss;
    ss << std::uppercase << "HTS";

    unsigned int expected_value = 0;
    for (size_t idx = 0; idx < login.size(); idx++)
    {
        if (idx % 2 == 0)
            ss << "-";
        unsigned int X = (login[idx] - expected_value) >> 1;
        unsigned int Y = ~((T)login[idx] << expected_value);

        expected_value = X & Y;

        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(expected_value);
    }
    return ss.str();
}
int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " hts_user_name" << std::endl;

        return 1;
    }

    const auto login = argv[1]; // login

    std::cout << "Code for app17:          " << keygen<uint32_t>(login) << std::endl;
    std::cout << "Code for HTS validation: " << keygen<uint64_t>(login) << std::endl;

    return 0;
}
