#include <iostream>

#include "DES_coder.hpp"

#include "DEAL_128_coder.hpp"

void output(std::vector<uint8_t> array)
{
    std::cout << "[" << std::hex;
    for(auto data : array)    
    {
        int byte = data;
        std::cout << std::uppercase << byte << ' ';
    }
    std::cout << "]" << std::dec << std::endl;
}

int main(int argc, char* argv[]) 
{
    {
        DEAL_128_coder coder("1234567887654321", padding_mode::ANSI_X923);
        byte_array test{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
        std::cout << "DEAL-128" << std::endl;
        std::cout << "Input:" << std::endl;
        output(test);

        auto res = coder.code(test);
        auto res_out = coder.decode(res);

        std::cout << "Code:" << std::endl;
        output(res);

        std::cout << "Decode:" << std::endl;
        output(res_out);
    }


    std::vector<uint8_t> input;
    if (argc == 4)
    {   //      1     2       3
        // file e [input] [output]
        // file d [input] [output]


        DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::CBC, padding_mode::Zeros);

        FILE *f_in = fopen(argv[2], "rb");
        if (f_in != NULL)
        {
            fseek(f_in, 0, SEEK_END);
            size_t size = ftell(f_in);
            fseek(f_in, 0, SEEK_SET);
            input.resize(size);
            fread(&input[0], 1, size, f_in);
            fclose(f_in);
        }

        std::vector<uint8_t> res;
        switch (argv[1][0])
        {
            case 'e':
                res = coder.code(input);
                break;

            case 'd':
                res = coder.decode(input);
                break;

            default:
                std::cout << "Unknown mode!" << std::endl; return -1;
        }

        FILE *f_out = fopen(argv[3], "wb");
        if (f_out != NULL)
        {
            fwrite(&res[0], 1, res.size(), f_out);
            fclose(f_out);
        }
    }
    else
    {
        DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::ECB, padding_mode::ANSI_X923);
        // DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::CBC, padding_mode::ANSI_X923);
        // DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::PCBC, padding_mode::ANSI_X923);
        // DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::CFB, padding_mode::ANSI_X923);
        // DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::OFB, padding_mode::ANSI_X923);
        // DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::CTR, padding_mode::ANSI_X923);
        // DES_coder coder(0x0f'1f'2f'3f'4f'5f'6f'7fULL, crypp_mode::Random_Delta, padding_mode::ANSI_X923, 123, 10);

        int size_test = 20;

        std::vector<uint8_t> input;
        input.reserve(size_test);

        for(int i = 0; i < size_test; ++i)
            input.push_back(rand()%255);

        std::cout << "\nDES" << std::endl;
        auto res = coder.code(input);
        auto res_dec = coder.decode(res);
        std::cout << "Input:" << std::endl;
        output(input);
        std::cout << "Code:" << std::endl;
        output(res);
        std::cout << "Decode:" << std::endl;
        output(res_dec);
    }



    return 0;
}
