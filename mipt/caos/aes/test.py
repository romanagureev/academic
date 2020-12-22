import aes256            


with open('data/password', 'r') as f:
    password = f.read()

password = password[:len(password)-1]
print(f'password: {password}')


with open('data/unsalteddata.aes', 'rb') as f:
    unsalted_encoded = f.read()


print('started without salt')
print(unsalted_encoded)
print()

print(aes256.decode(unsalted_encoded, password))

try:
    aes256.decode(unsalted_encoded)
    print('ok, but no password set')
except BaseException as e:
    print('Can\'t decode: {}'.format(e))

aes256.set_default_password(password)
print(aes256.decode(unsalted_encoded))


print('checking bad password')
try:
    print(aes256.decode(unsalted_encoded, ''))
    print('ok')
except BaseException as e:
    print('Can\'t decode: {}'.format(e))


print()

with open('data/salteddata.aes', 'rb') as f:
    salted_encoded = f.read()


print('started with salt')
print(salted_encoded)
print()

decoded = aes256.decode(salted_encoded, password)
print(decoded)

try:
    aes256.decode(salted_encoded)
    print('ok, but no password set')
except BaseException as e:
    print('Can\'t decode: {}'.format(e))

aes256.set_default_password(password)
print(aes256.decode(salted_encoded))


print('checking bad password')
try:
    print(aes256.decode(salted_encoded, ''))
    print('ok')
except BaseException as e:
    print('Can\'t decode: {}'.format(e))
