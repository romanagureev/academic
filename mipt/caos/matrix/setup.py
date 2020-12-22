from distutils.core import setup, Extension

def main():
    setup(name="matrix",
          version="1.0",
          description="matrix module",
          author="Roman",
          author_email="Roman@gmail.com",
          ext_modules=[Extension("matrix", ["matrixmodule.c"])])


if __name__ == "__main__":
    main()
