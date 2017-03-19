
#include <iostream>
#include <cstring>

static int var_20 = 0;
static int var_10 = 0;
static int var_2C = 0;
static int var_24 = 0;
static int var_1C = 0x10;

int main(int argc, char **argv)
{
    auto arg_0 = argv[1]; // login
    auto arg_4 = argv[2]; // pass

    char var_43C[256] = { 0 };

    memcpy(var_43C, arg_4 + strlen("HTS-"), 4); 
    memcpy(var_43C + 4, arg_4 + strlen("HTS-XXXX-"), 4); 
    memcpy(var_43C + 8, arg_4 + strlen("HTS-XXXX-XXXX-"), 4); 

    std::cout << var_43C << std::endl;

    int var_18 = 0;
    for (int var_14 = 0; strlen(arg_0) > var_14; var_14++)
    {
        sscanf(var_43C + var_18, "%02X", &var_2C);

        int X = arg_0[var_14] - var_20; // 6F - X
        X >>= 1;
        // CAREFUL: the long here is what make the server be successful BUT not
        // the app (as long differs on 32 and 64 bits platforms
        long  Y = arg_0[var_14];
        Y <<= var_20;
        Y = ~Y;

        var_1C = X & Y;
        var_20 = var_1C;
        var_24 = var_2C;

        for (var_10 = 0; var_10 < strlen(arg_4); var_10++) {
            for (int var_C = 0; var_C < strlen(arg_0); var_C++) {
                auto edx = var_C; // mov     edx, [ebp+var_C]
                auto eax = arg_0;//  <=> char *eax = "ozl" // mov     eax, [ebp+arg_0]
                eax += edx; // add     eax, edx
                int l = *eax; // movzx   eax, byte ptr [eax]
                //(keeps sign) // movsx   eax, al
                l -= var_20; // sub     eax, [ebp+var_20]
                auto ebx = l; // mov     ebx, eax
                ebx >>= 1; // sar     ebx, 1
                edx = var_10; // mov     edx, [ebp+var_10]
                eax = arg_4; //<=> char *eax = "HTS-XXX-XXX..."; // mov     eax, [ebp+arg_4]
                eax += edx;        // add     eax, edx
                int c = *eax;   // movzx   eax, byte ptr [eax]
                //(operation keeps signedness) // movsx   eax, al
                long  ecx = var_20; // mov     ecx, [ebp+var_20]
                c <<= (ecx & 0xFF); // shl     eax, cl
                c = ~c; // not     eax
                c &= ebx; // and     eax, ebx
                var_20 &= c; // and     [ebp+var_20], eax
                var_20 += 1; // add     [ebp+var_20], 1
            }
        }
        if (var_1C != var_2C)
        {
            std::cout << "Fail" << std::endl;
            return 12; // fail
        }
        var_20 = var_24;
        var_18 += 2;
    }
    std::cout << "OK" << std::endl;
    return 0;
}
