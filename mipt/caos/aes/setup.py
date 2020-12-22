from distutils.core import setup, Extension

def main():
    setup(name="aes256",
          version="1.0",
          description="aes256 module",
          author="Roman",
          author_email="Roman@gmail.com",
          ext_modules=[
              Extension("aes256", ["aes.c"],
                        extra_compile_args = [
                            '-I/usr/local/opt/openssl@1.1/include/',
                        ],
                        extra_link_args = [
                            '-lssl',
                            '-lcrypto',
                        ],
              )
          ]
    )


if __name__ == "__main__":
    main()

