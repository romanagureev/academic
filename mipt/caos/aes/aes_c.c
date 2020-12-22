#include <stdlib.h>
#include <stdio.h>
#include <openssl/evp.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {
  char* password = argv[1];
  unsigned char salted[16];
  read(0, salted, 16);
  unsigned char* salt = salted + 8;
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  
  unsigned char key[EVP_MAX_KEY_LENGTH];
  unsigned char iv[EVP_MAX_IV_LENGTH];
  EVP_BytesToKey(
    EVP_aes_256_cbc(),    // algorithm
    EVP_sha256(),         // algorithm for password hash
    salt,                 // salt
    (unsigned char*)password, strlen(password), // password
    1,                    // number of iterations
    key,                  // result: key of needed size
    iv                    // result: initial vector
  );
  
  EVP_DecryptInit(
    ctx,                  // context to store state
    EVP_aes_256_cbc(),    // algorithm
    key,                  // key of needed size
    iv                    // initial vector
  );

  unsigned char buf[4096];
  int sz = 0;
  unsigned char dec[4096];
  int len = 0;
  while ((sz = read(0, buf, sizeof(buf))) > 0) {
    EVP_DecryptUpdate(ctx, dec, &len, buf, sz);
    write(1, dec, len);
  }
  EVP_DecryptFinal_ex(ctx, dec, &len);
  write(1, dec, len);

  return 0;
}
