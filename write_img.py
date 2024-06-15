# Open the disk image file in binary write mode
with open('mydisk.img', 'wb') as f:
    # Define the data to write
    data_to_write = b'Hello, World!\n' * 10  # Example data (repeated 10 times)

    # Write the data to the disk image file
    f.write(data_to_write)

print("Data has been written to mydisk.img")
