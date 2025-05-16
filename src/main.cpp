#include <iostream>
#include <fstream>

#include "stream_coder.hpp"
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

void show_help()
{
    std::cout << "Usage: lan_1 -e/d [DES/DEAL] -k [Key] -p [padding mode] -m [crypt mode] -j [thread] -i [input file] -o [output file]\n";
    std::cout << "-e -- Encrypt mode\n";
    std::cout << "-d -- Decrypt mode\n";
    std::cout << "Padding mode: 0 - Zeros, 1 - ANSI_X923, 2 - PKCS7, 3 - ISO_10126\n";
    std::cout << "Crypt mode: 0 - ECB, 1 - CBC, 2 - PCBC, 3 - CFB, 4 - OFB, 5 - CTR, 6 - Random_Delta\n";
}

int main(int argc, char* argv[]) 
{
    if (argc > 1)
    {
        std::string key = "1234567890123456";
        int crypt = -1;
        bool decoder = false;
        padding_mode padding = padding_mode::Zeros;
        crypt_mode mode = crypt_mode::ECB;
        std::string input_file_name;
        std::string output_file_name;
        int use_thread = 4;
        for(int i = 1; i < argc - 1; ++i)
        {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
            {
                show_help();
                return 0;
            }

            if ((strcmp(argv[i], "-e") == 0) || (strcmp(argv[i], "-d") == 0))
            {
                decoder = strcmp(argv[i], "-d") == 0;

                if (strcmp(argv[i + 1], "DES") == 0)
                    crypt = 0;
                else
                {
                    if (strcmp(argv[i + 1], "DEAL") == 0)
                        crypt = 1;
                    else
                    {
                        std::cout << "Unknown crypt mode!" << std::endl;
                        show_help();
                        return 1;
                    }
                }
            }

            if (strcmp(argv[i], "-k") == 0)
            {
                key = argv[i + 1];
            }

            if (strcmp(argv[i], "-p") == 0)
            {
                int const raw_padding = atoi(argv[i + 1]);
                if (raw_padding >= 0 && raw_padding <= 3)
                {
                    padding = static_cast<padding_mode>(raw_padding);
                }
                else
                {
                    std::cout << "Unknown padding mode!" << std::endl;
                    show_help();
                    return 1;
                }
            }

            if (strcmp(argv[i], "-m") == 0)
            {
                int const raw_crypt_mode = atoi(argv[i + 1]);
                if (raw_crypt_mode >= 0 && raw_crypt_mode <= 6)
                {
                    mode = static_cast<crypt_mode>(raw_crypt_mode);
                }
                else
                {
                    std::cout << "Unknown crypt mode!" << std::endl;
                    show_help();
                    return 1;
                }
            }

            if (strcmp(argv[i], "-j") == 0)
            {
                use_thread = atoi(argv[i + 1]);
                if (use_thread < 1)
                {
                    std::cout << "Thread count must be greater then zero!" << std::endl;
                    return 1;
                }
            }

            if (strcmp(argv[i], "-i") == 0)
                input_file_name = argv[i + 1];

            if (strcmp(argv[i], "-o") == 0)
                output_file_name = argv[i + 1];
        }
        
        if (input_file_name.empty() || output_file_name.empty())
        {
            show_help();
            return 1;
        }

        auto const start_code = std::chrono::system_clock::now();
        std::fstream file_in(input_file_name, std::fstream::in | std::fstream::binary);
        if (!file_in.is_open())
        {
            std::cout << "File  << " << input_file_name << "  can`t be opened!" << std::endl;
            return 1;
        }
        
        std::fstream file_out(output_file_name, std::fstream::out | std::fstream::binary);

        I_coder *coder;
        switch (crypt)
        {
            case 0:
            {
                DES_coder *des_coder = new DES_coder();
                des_coder->set_padding(padding);
                des_coder->set_key(key);
                des_coder->set_crypt_mode(mode);
                des_coder->set_number_thread(use_thread);
                coder = des_coder;
                break;
            }
            case 1:
            {
                DEAL_128_coder *deal_coder = new DEAL_128_coder();
                deal_coder->set_padding(padding);
                deal_coder->set_key(key);
                deal_coder->set_crypt_mode(mode);
                deal_coder->set_number_thread(use_thread);
                coder = deal_coder;
                break;
            }
            default:
                std::cout << "Unknown crypt mode!" << std::endl;
                show_help();
                return 1;
                break;
        }
        
        stream_coder stream;
        try
        {
            if (decoder)
                stream.decode(coder, file_in, file_out);
            else
                stream.code(coder, file_in, file_out);
        }
        catch (std::exception const &e)
        {
            std::cout << "Error: " << e.what() << std::endl;
        }
        auto const end_decode = std::chrono::system_clock::now();                
        std::cout << "Time decode: " << std::chrono::duration<double>(end_decode - start_code).count() << "s\n";
        delete coder;
    }
    else
    {
        int size_test = 100 * 1024;

        std::cout << "Begin testing...\n";

        for(int c = 0; c < 2; ++c)
        {
            I_coder *coder;

            if (c == 0)
            {
                std::cout << "DES\n";
                auto real_coder = new DES_coder();
                real_coder->set_key("1234567887654321");
                real_coder->set_padding(padding_mode::ANSI_X923);
                real_coder->set_delta_counter(10);
                real_coder->set_number_thread(10);

                coder = real_coder;
            }
            else
            {
                std::cout << "DEAL_128_coder\n";
                auto real_coder = new DEAL_128_coder();
                real_coder->set_key("1234567887654321");
                real_coder->set_padding(padding_mode::ANSI_X923);
                real_coder->set_delta_counter(10);
                real_coder->set_number_thread(10);
                
                coder = real_coder;
            }
        
            byte_array input;
            input.reserve(size_test);
            for(int i = 0; i < size_test; ++i)
                input.push_back(rand()%255);
            std::cout << "Size: " << input.size() / 1024 << "K\n\n";


            for(int cm = 0; cm < 7; ++cm)
            {
                coder->set_crypt_mode(static_cast<crypt_mode>(cm));
                auto const start_code = std::chrono::system_clock::now();
                {
                    char const * const crypt_mode_str[] = {"ECB", "CBC", "PCBC", "CFB", "OFB", "CTR", "Random_Delta"};
                    std::cout << "Crypt mode: " << crypt_mode_str[cm] << '\n'; 
                }


                byte_array res;
                coder->code(input.cbegin(), input.cend(), res);

                auto const end_code = std::chrono::system_clock::now();
                std::cout << "Time code:" << std::chrono::duration<double>(end_code - start_code).count() << "s\n";

                auto const start_decode = std::chrono::system_clock::now();

                byte_array res_dec;
                coder->decode(res.cbegin(), res.cend(), res_dec);
                
                auto const end_decode = std::chrono::system_clock::now();                
                std::cout << "Time decode: " << std::chrono::duration<double>(end_decode - start_decode).count() << "s\n";

                if (input.size() < 100)
                {
                    std::cout << "Input:" << std::endl;
                    output(input);
                    std::cout << "Code:" << std::endl;
                    output(res);
                    std::cout << "Decode:" << std::endl;
                    output(res_dec);
                }

                bool error = false;
                for(size_t i = 0; i < input.size(); ++i)
                    error |= input[i] != res_dec[i];
                if (!error)
                    std::cout << "Ok" << std::endl;
                else
                    std::cout << "!!! Error found !!!" << std::endl;
                std::cout << std::endl;
            }

            delete coder;
        }
        std::cout << "End testing" << std::endl;        
    }

    return 0;
}
