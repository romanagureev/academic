Caesar, Vigenere and Vernam encryptor.
Caesar hack counts deviation of every letter in power *p* and chooses the min sum.

## Usage:

```
./encryptor.py encode --cipher {caesar, vigenere, vernam} --key {number|word} [--input-file input.txt] [--output-file output.txt]  
./encryptor.py decode --cipher {caesar, vigenere, vernam} --key {number for caesar or word for vigenere, vernam} [--input-file input.txt] [--output-file output.txt]  
./encryptor.py hack [--input-file input.txt] [--output-file output.txt] --model-file {model}  
./encryptor.py train --text-file {input.txt} --model-file {model}
```
