
#include <iostream>
#include <cstring>
#include <iomanip>

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " hts_user_name" << std::endl;

        return 1;
    }

    const auto login = argv[1]; // login

    unsigned int expected_value = 0;

    std::cout << std::uppercase << "HTS";

    for (size_t idx = 0; idx < strlen(login); idx++)
    {
        if (idx % 2 == 0)
            std::cout << "-";
        unsigned int X = (login[idx] - expected_value) >> 1;
        unsigned int Y = ~((long)login[idx] << expected_value);

        expected_value = X & Y;

        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(expected_value);
    }

    std::cout << std::endl;

    return 0;
}
