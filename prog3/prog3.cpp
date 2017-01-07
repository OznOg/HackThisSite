#include <iostream>
#include <iomanip>
#include <vector>
#include <openssl/md5.h>
#include <boost/lexical_cast.hpp>


int toInt(unsigned char value)
{
    unsigned int x;   
    std::stringstream ss;
    ss << std::hex << "0x" + std::string(1, value);
    ss >> x;
    return x;
}

int evalCrossTotal(const std::string &strMD5)
{
    int intTotal = 0;
    for (auto value : strMD5)
    {
        intTotal += toInt(value);
    }
    return intTotal;
}//-----------------------------------------------------------------------------------

std::string md5(const std::string &str)
{
    std::vector<unsigned char> digest(MD5_DIGEST_LENGTH);
    
    MD5((unsigned char*)str.c_str(), str.size(), digest.data());    

    std::stringstream ss;
    for (auto value : digest)
        ss << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)value;

   return ss.str();
}

// $strString is the content of the entire file with serials
std::string encryptString(const std::string &strString, const std::string &strPassword)
{
 
    const std::string strPasswordMD5 = md5(strPassword);
    int intMD5Total = evalCrossTotal(strPasswordMD5);

    std::cout << strPasswordMD5 << std::endl;

    std::string arrEncryptedValues;

    for (int i = 0; i < strString.size(); i++)
    {
        arrEncryptedValues += boost::lexical_cast<std::string>((int)strString[i]
                                     + toInt(strPasswordMD5[i % 32])
                                     - intMD5Total) + " ";

        const std::string gni(strString, 0, i + 1);
        const std::string md5Str(md5(gni), 0, 16);
        intMD5Total = evalCrossTotal(md5Str + std::string(md5(boost::lexical_cast<std::string>(intMD5Total)), 0, 16));
    }
    return arrEncryptedValues;
}//-----------------------------------------------------------------------------------



int main(int argc, char **argv)
{
    std::cout << encryptString(argv[1], argv[2]) << std::endl;
    return 0;
}
