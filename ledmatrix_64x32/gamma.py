

gamma_value = 3
gamma = [round(4096*((x/256) ** gamma_value)) for x in range(256)]
print("int gamma[] = {"+(','.join([str(x) for x in gamma]))+"};")