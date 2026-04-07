/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/crypto.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

/* =========================================================================
 * Help and command-line parsing
 * ========================================================================= */

mud::crypto::material_t g_material;
const char* g_block_cipher = nullptr;
std::istream* g_istr = &std::cin;
std::ostream* g_ostr = &std::cout;

enum class command_t {
    none,
    encrypt,
    decrypt,
    list
} g_command = command_t::none;

void
help()
{
    std::string msg = R"EOF(
Usage: sample_crypto [COMMAND] [OPTIONS]

With COMMAND:
    encrypt              Encrypt plaintext into ciphertext
    decrypt              Decrypt ciphertext into plaintext
    list                 List supported cryptographic operations

With OPTIONS:
    -h, --help                Show this help.
    -k, --key <HEX>           Specify a hexadecimal key.
    -i, --iv <HEX>            Specify a hexadecimal initial vector.
    -n, --nonce <HEX>         Specify a hexadecimal nonce.
    -c, --counter <HEX>       Specify a hexadecimal counter.
    -a, --aad <HEX>           Specify a hexadecimal additional auth data
    -p, --padding <TYPE>      The type of padding, defaults to 'none'. See
                              'list' for a complete list of supported padding.
    -b, --block-cipher <TYPE> The block-cipher type. See 'list' for a complete
                              list of supported ciphers.
    -I, --input <TYPE>        The input, either a file name or '-' for console
                              input. Defaults to '-'.
    -O, --output <TYPE>       The output, either a file name or '-' for console
                              output. Defaults to '-'.
)EOF";
    msg.erase(0, 1);
    std::cout << msg;
}

int
check_argument(const char* option, int argc)
{
    if (argc == 0) {
        std::cerr << "Option '" << option << "' requires an argument"
                  << std::endl;
        return 1;
    }
    return 0;
}

int
parse_hex(const char* option, int argc, char** argv, mud::crypto::data_t& data)
{
    if (check_argument(option, argc) != 0) {
        return 1;
    }
    std::stringstream sstr(*argv);
    sstr >> data;
    if (!sstr.eof()) {
        std::cerr << "Option '" << option
                  << "' unable to parse a hexadecimal value" << std::endl;
        return 1;
    }
    return 0;
}

int
parse_cmd(int argc, char** argv)
{
    // Expect a COMMAND (or -h/--help)
    ++argv, --argc;
    if (!argc) {
        help();
        return 1;
    }
    if (strcmp(*argv, "-h") == 0 || strcmp(*argv, "--help") == 0) {
        help();
        return 1;
    }
    else
    if (strcmp(*argv, "encrypt") == 0) {
        g_command = command_t::encrypt;
    }
    else
    if (strcmp(*argv, "decrypt") == 0) {
        g_command = command_t::decrypt;
    }
    else
    if (strcmp(*argv, "list") == 0) {
        g_command = command_t::list;
    }
    else {
        std::cerr << "Unknown command: " << *argv << std::endl;
        return 1;
    }

    // Optional
    while (--argc && *(++argv)[0] == '-') {
        const char* option = *argv;
        if (strcmp(*argv, "-k") == 0 || strcmp(*argv, "--key") == 0) {
            mud::crypto::data_t data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_material.key(data);
        }
        else
        if (strcmp(*argv, "-i") == 0 || strcmp(*argv, "--iv") == 0) {
            mud::crypto::data_t data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_material.iv(data);
        }
        else
        if (strcmp(*argv, "-n") == 0 || strcmp(*argv, "--nonce") == 0) {
            mud::crypto::data_t data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_material.nonce(data);
        }
        else
        if (strcmp(*argv, "-c") == 0 || strcmp(*argv, "--counter") == 0) {
            mud::crypto::data_t data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_material.counter(data);
        }
        else
        if (strcmp(*argv, "-a") == 0 || strcmp(*argv, "--aad") == 0) {
            mud::crypto::data_t data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_material.aad(data);
        }
        else
        if (strcmp(*argv, "-b") == 0 || strcmp(*argv, "--block-cipher") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            g_block_cipher = *argv;
        }
        else
        if (strcmp(*argv, "-p") == 0 || strcmp(*argv, "--padding") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            if (strcmp(*argv, "none") == 0) {
                g_material.padding(mud::crypto::padding_t::none);
            }
            else
            if (strcmp(*argv, "pkcs7") == 0) {
                g_material.padding(mud::crypto::padding_t::pkcs7);
            }
            else {
                std::cerr << "Option '" << option << "' has unrecognised "
                          << "padding '" << *argv << "'" << std::endl;
                return 1;
            }
        }
        else
        if (strcmp(*argv, "-I") == 0 || strcmp(*argv, "--input") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            if (strcmp(*argv, "-") == 0) {
                g_istr = &std::cin;
            }
            else {
                g_istr = new std::ifstream(*argv, std::ios::binary);
                if (g_istr->fail()) {
                    std::cerr << "Unable to open '" << *argv << "'"
                              << std::endl;
                    break;
                }
            }
        }
        else
        if (strcmp(*argv, "-O") == 0 || strcmp(*argv, "--output") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            if (strcmp(*argv, "-") == 0) {
                g_ostr = &std::cout;
            }
            else {
                g_ostr = new std::ofstream(*argv, std::ios::binary);
                if (g_ostr->fail()) {
                    std::cerr << "Unable to open '" << *argv << "'"
                              << std::endl;
                    break;
                }
            }
        }
        else {
            std::cerr << "Unrecognised option '" << option << "'" << std::endl;
            return 1;
        }
    }
    return 0;
}

/* =========================================================================
 * List
 * ========================================================================= */

int
list()
{
    std::cout << "Block-ciphers:" << std::endl;
    auto block_ciphers = mud::crypto::block_cipher_factory::instance().keys();
    std::sort(block_ciphers.begin(), block_ciphers.end());
    for (auto& key: mud::crypto::block_cipher_factory::instance().keys()) {
        std::cout << "  " << key << std::endl;
    }
    std::cout << "Padding:" << std::endl;
    std::cout << "  none" << std::endl;
    std::cout << "  pkcs7" << std::endl;
    return 0;
}

/* =========================================================================
 * Encrypt
 * ========================================================================= */

int
encrypt()
{
    if (g_block_cipher == nullptr) {
        std::cerr << "No block-cipher specified" << std::endl;
        return 1;
    }

    /* Encrypt and capture any exception */
    try {
        /* Create the block cipher */
        auto block_cipher = mud::crypto::block_cipher_factory::instance().
                        create(g_block_cipher, g_material);

        /* Assign it to the output stream for on the fly encryption */
        std::ostream ostr(block_cipher->sbuf(g_ostr->rdbuf()));

        /* Copy all data from input to output */
        ostr << g_istr->rdbuf();

        /* Close an output file stream */
        block_cipher->sbuf()->close();
        if (g_ostr != &std::cout) {
            delete g_ostr;
        }
    }
    catch (std::invalid_argument& ex) {
        std::cerr << "Unknown block-cipher '" << g_block_cipher << "'"
                  << std::endl;
        return 1;
    }
    catch (mud::crypto::size_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}

/* =========================================================================
 * Decrypt
 * ========================================================================= */

int
decrypt()
{
    if (g_block_cipher == nullptr) {
        std::cerr << "No block-cipher specified" << std::endl;
        return 1;
    }

    /* Decrypt and capture any exception */
    try {
        /* Create the block cipher */
        auto block_cipher = mud::crypto::block_cipher_factory::instance().
                        create(g_block_cipher, g_material);

        /* Assign it to the input stream for on the fly decryption */
        std::istream istr(block_cipher->sbuf(g_istr->rdbuf()));

        /* Copy all data from input to output */
        *g_ostr << istr.rdbuf();

        /* Close an input file stream */
        block_cipher->sbuf()->close();
        if (g_istr != &std::cin) {
            delete g_istr;
        }
    }
    catch (std::invalid_argument& ex) {
        std::cerr << "Unknown block-cipher '" << g_block_cipher << "'"
                  << std::endl;
        return 1;
    }
    catch (mud::crypto::size_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}

/* =========================================================================
 * Main
 * ========================================================================= */

int
main(int argc, char** argv)
{
    // Command line parsing
    if (parse_cmd(argc, argv) != 0) {
        return 1;
    }

    // Execute the command
    switch (g_command) {
        case command_t::list:
            return list();
        case command_t::encrypt:
            return encrypt();
        case command_t::decrypt:
            return decrypt();
        case command_t::none:
        default:
            break;
    }
    return 0;
}

/* vi: set ai ts=4 expandtab: */
