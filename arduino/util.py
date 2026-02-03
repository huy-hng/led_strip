import math

name = 'gamma_correction_3'
# gamma_correction = math.e
gamma_correction = 3
print(f'const PROGMEM uint8_t {name}[256] = {{')
for i in range(256):
    normalized = round(pow(i / 255, gamma_correction) * 255);
    if i>0 and normalized == 0:
        normalized = 1
    print(f'{normalized},'.rjust(5), end='')
    if i>0 and (i+1) % 16 == 0:
        print()

print('};')
