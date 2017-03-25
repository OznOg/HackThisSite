
#include <iostream>
#include <string>

//static object ParseandDecrypt(string stringd)
//{
//    int[] numArray = new int[128];
//    int index1 = 0;
//    do
//    {
//        if (index1 % 2 == 0)
//            numArray[index1] = checked (index1 - 3);
//        if (index1 % 2 == 1)
//            numArray[index1] = checked (index1 + 4);
//        checked { ++index1; }
//    }
//    while (index1 <= (int) sbyte.MaxValue);
//    int index2 = 0;
//    string str = "";
//    int num = 1;
//    int length = stringd.Length;
//    int Start = num;
//    while (Start <= length)
//    {
//        int CharCode = checked (Strings.Asc(Strings.Mid(stringd, Start, 1)) - numArray[index2]);
//        str += Conversions.ToString(Strings.Chr(CharCode));
//        checked { ++index2; }
//        if (index2 > (int) sbyte.MaxValue)
//            index2 = 0;
//        checked { ++Start; }
//    }
//    return (object) str;
//}

static std::string ParseandDecrypt(const std::string &stringd)
{
    int numArray[128];
    int index1 = 0;
    do {
        if (index1 % 2 == 0)
            numArray[index1] = index1 - 3;
        if (index1 % 2 == 1)
            numArray[index1] = index1 + 4;
        ++index1;
    } while (index1 <= (int) 127);

    int index2 = 0;
    std::string str = "";
    int num = 0;
    int length = stringd.size();
    int Start = num;
    while (Start <= length)
    {
        int CharCode = stringd[Start] - numArray[index2];
        str.append(1, CharCode);
        ++index2;
        if (index2 > (int) 127)
            index2 = 0;
        ++Start;
    }
    return str;
}

// "fm`{f}kpwrn"
int main()
{
    std::cout << ParseandDecrypt("fm`{f}kpwrn") << std::endl;
    return 0;
}
