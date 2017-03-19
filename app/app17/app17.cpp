
#include <iostream>
#include <cstring>

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << " Usage: " << argv[0] << " <login> <password>\n";
        return 1;
    }
    auto login = argv[1]; // login
    auto password = argv[2]; // pass

    unsigned char expected_value = 0;

    char serial[256] = { 0 };

    memcpy(serial, password + strlen("HTS-"), 4);
    memcpy(serial + 4, password + strlen("HTS-XXXX-"), 4);
    memcpy(serial + 8, password + strlen("HTS-XXXX-XXXX-"), 4);

    for (int idx = 0; strlen(login) > idx; idx++)
    {
        int value = 0;
        sscanf(serial + idx * 2, "%02X", &value);

        int X = (login[idx] - expected_value) >> 1;
        int Y = ~(login[idx] << expected_value);

        expected_value = X & Y;

        if (expected_value != value)
        {
            std::cout << "Fail" << std::endl;
            return 12;
        }
    }
    std::cout << "OK" << std::endl;
    return 0;
}
