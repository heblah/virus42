
print('\tchar conv_tab[][5] = {\n\t\t', end='')

for i in range(0, 256):
    print('"' + format(i, '#04x') + '"', end='')
    if i != 0 and (i + 1) % 10 == 0:
        print(',\n\t\t', end='')
    elif i != 255:
        print(', ', end='')
print('\n\t};')
