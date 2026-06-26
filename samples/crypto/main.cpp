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

#include "mud/core.h"
#include "mud/crypto.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

/* =========================================================================
 * Help and command-line parsing
 * ========================================================================= */

mud::crypto::cipher_material_t g_cipher_material;
mud::crypto::hash_material_t g_hash_material;
mud::crypto::mac_material_t g_mac_material;
mud::crypto::kdf_material_t g_kdf_material;
const char* g_block_cipher = nullptr;
const char* g_hash = nullptr;
const char* g_mac = nullptr;
const char* g_kdf = nullptr;
std::istream* g_istr = &std::cin;
std::ostream* g_ostr = &std::cout;

enum class command_t {
    none,
    encrypt,
    decrypt,
    hash,
    mac,
    kdf,
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
    hash                 Generate a hash
    mac                  Generate a message authentication code
    kdf                  Generate a key using a key derivation function
    list                 List supported cryptographic operations

With OPTIONS:
    -h, --help            Show this help.
    --key <HEX>           Specify a hexadecimal key.
    --iv <HEX>            Specify a hexadecimal initial vector.
    --nonce <HEX>         Specify a hexadecimal nonce.
    --counter <HEX>       Specify a hexadecimal counter.
    --aad <HEX>           Specify a hexadecimal additional auth data.
    --salt <HEX>          Specify a hexadecimal salt.
    --info <HEX>          Specify a hexadecimal additional information data.
    --padding <TYPE>      The type of padding, defaults to 'none'. See
                          'list' for a complete list of supported padding.
    --block-cipher <TYPE> The block-cipher type. See 'list' for a complete
                          list of supported ciphers.
    --hash <TYPE>         The hash type. See 'list' for a complete list of
                          supported hashes.
    --mac <TYPE>          The MAC type. See 'list' for a complete list of
                          supported MACs.
    --kdf <TYPE>          The KDF type. See 'list' for a complete list of
                          supported KDFs.
    --digest-size <BITS>  The digest output size (XOF) in bits.
    --key-size <BITS>     The key size to generate in bits.
    --input <TYPE>        The input, either a file name or '-' for console
                          input. Defaults to '-'.
    --output <TYPE>       The output, either a file name or '-' for console
                          output. Defaults to '-'.

    crypto hash --hash SHA-256 \
               --input text.bin
Examples:
    Encrypting data
        crypto encrypt \
                   --block-cipher AES-R128256-GCM \
                   --key feffe9928665731c6d6a8f9467308308 \
                   --iv cafebabefacedbaddecaf888 \
                   --aad abaddad2 \
                   --input text.bin
    Decrypting data
        crypto decrypt \
                   --block-cipher AES-R128256-GCM \
                   --key feffe9928665731c6d6a8f9467308308 \
                   --iv cafebabefacedbaddecaf888 \
                   --aad abaddad2 \
                   --input text.bin
    Calculating a Hash
        crypto hash \
                   --hash SHA-256 \
                   --input text.bin
    Calculating a MAC
        crypto mac \
                   --mac HMAC-SHA-1 \
                   --key 0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b \
                   --input text.bin
    Generating a KDF key
        crypto kdf \
                   --kdf HKDF --mac HMAC-SHA-256 \
                   --key 0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b \
                   --salt 000102030405060708090a0b0c \
                   --info f0f1f2f3f4f5f6f7f8f9 -key-size 42
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
parse_hex(const char* option, int argc, char** argv, mud::core::buffer& data)
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
    if (strcmp(*argv, "hash") == 0) {
        g_command = command_t::hash;
    }
    else
    if (strcmp(*argv, "mac") == 0) {
        g_command = command_t::mac;
    }
    else
    if (strcmp(*argv, "kdf") == 0) {
        g_command = command_t::kdf;
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
        if (strcmp(*argv, "--key") == 0) {
            mud::core::buffer data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_cipher_material.key(data);
            g_mac_material.key(data);
            g_kdf_material.ikm(data);
        }
        else
        if (strcmp(*argv, "--iv") == 0) {
            mud::core::buffer data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_cipher_material.iv(data);
        }
        else
        if (strcmp(*argv, "--nonce") == 0) {
            mud::core::buffer data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_cipher_material.nonce(data);
        }
        else
        if (strcmp(*argv, "--counter") == 0) {
            mud::core::buffer data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_cipher_material.counter(data);
        }
        else
        if (strcmp(*argv, "--aad") == 0) {
            mud::core::buffer data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_cipher_material.aad(data);
        }
        else
        if (strcmp(*argv, "--info") == 0) {
            mud::core::buffer data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_kdf_material.info(data);
        }
        else
        if (strcmp(*argv, "--salt") == 0) {
            mud::core::buffer data;
            if (parse_hex(option, --argc, ++argv, data) != 0) {
                return 1;
            }
            g_kdf_material.salt(data);
        }
        else
        if (strcmp(*argv, "--block-cipher") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            g_block_cipher = *argv;
        }
        else
        if (strcmp(*argv, "--hash") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            g_hash = *argv;
        }
        else
        if (strcmp(*argv, "--mac") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            g_mac = *argv;
            g_kdf_material.mac_algorithm(g_mac);
        }
        else
        if (strcmp(*argv, "--kdf") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            g_kdf = *argv;
        }
        else
        if (strcmp(*argv, "--padding") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            if (strcmp(*argv, "none") == 0) {
                g_cipher_material.padding(mud::crypto::padding_t::none);
            }
            else
            if (strcmp(*argv, "pkcs7") == 0) {
                g_cipher_material.padding(mud::crypto::padding_t::pkcs7);
            }
            else {
                std::cerr << "Option '" << option << "' has unrecognised "
                          << "padding '" << *argv << "'" << std::endl;
                return 1;
            }
        }
        else
        if (strcmp(*argv, "--digest-size") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            g_hash_material.digest_size(atoi(*argv));
        }
        else
        if (strcmp(*argv, "--key-size") == 0) {
            --argc, ++argv;
            if (check_argument(option, argc) != 0) {
                return 1;
            }
            g_kdf_material.key_size(atoi(*argv));
        }
        else
        if (strcmp(*argv, "--input") == 0) {
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
        if (strcmp(*argv, "--output") == 0) {
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
    std::cout << "Hashes:" << std::endl;
    auto hashes = mud::crypto::hash_factory::instance().keys();
    std::sort(hashes.begin(), hashes.end());
    for (auto& key: mud::crypto::hash_factory::instance().keys()) {
        std::cout << "  " << key << std::endl;
    }
    std::cout << "Message Authentication Codes:" << std::endl;
    auto macs = mud::crypto::mac_factory::instance().keys();
    std::sort(macs.begin(), macs.end());
    for (auto& key: mud::crypto::mac_factory::instance().keys()) {
        std::cout << "  " << key << std::endl;
    }
    std::cout << "Key Derivation Functions:" << std::endl;
    auto kdfs = mud::crypto::kdf_factory::instance().keys();
    std::sort(kdfs.begin(), kdfs.end());
    for (auto& key: mud::crypto::kdf_factory::instance().keys()) {
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
                        create(g_block_cipher, g_cipher_material);

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
                        create(g_block_cipher, g_cipher_material);

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
 * Hash
 * ========================================================================= */

int
hash()
{
    if (g_hash == nullptr) {
        std::cerr << "No hash specified" << std::endl;
        return 1;
    }

    /* Hash the input */
    try {
        /* Create the hash */
        auto hash = mud::crypto::hash_factory::instance().
                    create(g_hash, g_hash_material);

        /* Assign it to the input stream for on the fly decryption */
        std::istream istr(hash->sbuf(g_istr->rdbuf()));

        /* Read all input data and ignore it. The hash is calculated as the
         * stream is read. */
        char c;
        while (istr.get(c)) ;

        /* Close an input file stream */
        hash->sbuf()->close();
        if (g_istr != &std::cin) {
            delete g_istr;
        }

        /* Output the hash */
        *g_ostr << hash->hash() << std::endl;
    }
    catch (std::invalid_argument& ex) {
        std::cerr << "Unknown hash '" << g_hash << "'" << std::endl;
        return 1;
    }
    catch (mud::crypto::size_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}

/* =========================================================================
 * MAC
 * ========================================================================= */

int
mac()
{
    if (g_mac == nullptr) {
        std::cerr << "No mac specified" << std::endl;
        return 1;
    }

    /* Mac the input */
    g_mac_material.hash_material(g_hash_material);
    try {
        /* Create the hash */
        auto mac = mud::crypto::mac_factory::instance().
                    create(g_mac, g_mac_material);

        /* Assign it to the input stream for on the fly decryption */
        std::istream istr(mac->sbuf(g_istr->rdbuf()));

        /* Read all input data and ignore it. The hash is calculated as the
         * stream is read. */
        char c;
        while (istr.get(c)) ;

        /* Close an input file stream */
        mac->sbuf()->close();
        if (g_istr != &std::cin) {
            delete g_istr;
        }

        /* Output the tag */
        *g_ostr << mac->tag() << std::endl;
    }
    catch (std::invalid_argument& ex) {
        std::cerr << "Unknown MAC '" << g_mac << "'" << std::endl;
        return 1;
    }
    catch (mud::crypto::size_error& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}

/* =========================================================================
 * KDF
 * ========================================================================= */

int
kdf()
{
    if (g_kdf == nullptr) {
        std::cerr << "No kdf specified" << std::endl;
        return 1;
    }

    /* Generate a KDF key */
    g_mac_material.hash_material(g_hash_material);
    g_kdf_material.mac_material(g_mac_material);
    try {
        /* Create the KDF algorithm */
        auto kdf = mud::crypto::kdf_factory::instance().
                    create(g_kdf, g_kdf_material);

        /* Generate and output the key */
        *g_ostr << kdf->key() << std::endl;
    }
    catch (std::invalid_argument& ex) {
        std::cerr << "Unknown KDF '" << g_kdf << "'" << std::endl;
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
        case command_t::hash:
            return hash();
        case command_t::mac:
            return mac();
        case command_t::kdf:
            return kdf();
        case command_t::none:
        default:
            break;
    }
    return 0;
}

/* vi: set ai ts=4 expandtab: */
