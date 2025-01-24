import random


def Flipping_data(input_file, output_file, modifiable_bits_percentage, x):
    # Step 1: Read the input file
    with open(input_file, 'r') as f:
        lines = f.readlines()

    # Step 2: Parse the data
    headers = lines[0].strip().split('\t')  # First line is the header
    data = []

    # Process data rows, excluding the last column (primary key)
    for line in lines[1:]:
        row = line.strip().split('\t')
        row_data = row[:-1]  # Exclude the last column (primary key)
        primary_key = row[-1]  # Save the last column as the primary key
        data.append((row_data, primary_key))  # Keep both data and primary key

    # Step 3: Extract modifiable bits from all data
    modifiable_bits = []  # Will store the modifiable bits from all columns

    for row_data, primary_key in data:
        for idx, value in enumerate(row_data):
            value = value.strip('"')  # Remove surrounding quotes
            modifiable_bits_start_index = len(value) - x  # The start index of the modifiable bits
            modifiable_part = value[modifiable_bits_start_index:]  # Part that can be modified

            # Add the modifiable part to our list
            modifiable_bits.append((idx, modifiable_part, value))  # Store index, modifiable part, and full value

    # Step 4: Calculate how many bits should be flipped
    total_modifiable_bits = sum(
        len(modifiable_part) for _, modifiable_part, _ in modifiable_bits)  # Total modifiable bits
    num_bits_to_flip = int(0.3 * total_modifiable_bits * modifiable_bits_percentage)

    # Step 5: Select random bits to flip across all modifiable parts
    bits_to_flip = random.sample(range(total_modifiable_bits), num_bits_to_flip)

    # Step 6: Apply the flips
    flipped_bits = set(bits_to_flip)
    flipped_modifiable_bits = []

    # Flip the selected bits in the corresponding parts
    for idx, modifiable_part, full_value in modifiable_bits:
        new_modifiable_part = list(modifiable_part)
        for i, bit in enumerate(modifiable_part):
            bit_index = sum(len(mp) for _, mp, _ in modifiable_bits[:idx]) + i  # Calculate global bit index
            if bit_index in flipped_bits:
                new_modifiable_part[i] = '1' if bit == '0' else '0'  # Flip the bit
        flipped_modifiable_bits.append((idx, ''.join(new_modifiable_part), full_value))

    # Step 7: Rebuild the rows with flipped bits
    processed_data = []
    for row_data, primary_key in data:
        new_row = []
        for idx, value in enumerate(row_data):
            modifiable_part = flipped_modifiable_bits.pop(0)  # Get the next flipped modifiable part
            # Rebuild the full value with flipped part, keeping double quotes around it
            # Special handling for single-bit columns
            if len(value) == 1:  # If it's a single bit, we don't need slicing
                new_value = modifiable_part[1]  # Just the flipped bit
            else:
                new_value = value[1:len(value) - x-1] + modifiable_part[
                    1]  # Concatenate the unchanged part and flipped part
            new_row.append(f'"{new_value}"')  # Add double quotes around the modified value
        new_row.append(primary_key)  # Add the primary key back
        processed_data.append(new_row)

    # Step 8: Write the processed data to the output file
    with open(output_file, 'w') as f:
        # Write the header (first line)
        f.write("\t".join(headers) + "\n")

        # Write the processed data
        for row in processed_data:
            f.write("\t".join(row) + "\n")

    print(f"Data processing complete! The output is saved in '{output_file}'.")

def Delete_data(input_file, output_file, delete_ratio):
    try:
        with open(input_file, 'r') as infile:
            lines = infile.readlines()

        total_lines = len(lines)
        lines_to_delete = int(total_lines * delete_ratio)

        remaining_lines = lines[:-lines_to_delete]

        # 将剩余部分写入新的文件
        with open(output_file, 'w') as outfile:
            outfile.writelines(remaining_lines)

        print(f"Processing complete. Results saved to {output_file}")
    except Exception as e:
        print(f"An error occurred: {e}")



def generate_random_data(format_string):
    """
    Generate random data based on the given format string.
    :param format_string: The format string of the original data (excluding the primary key column).
    :return: A randomly generated data string that follows the same format as the original data.
    """
    random_data = ''
    for char in format_string:
        if char == '0' or char == '1':
            random_data += str(random.choice([0, 1]))  # Randomly choose between 0 and 1
    return random_data



def generate_random_data(format_string):
    """
    Generate random data based on the given format string (excluding the primary key column).
    :param format_string: The format string of the original data (excluding the primary key column).
    :return: A randomly generated data string that follows the same format as the original data.
    """
    random_data = ''
    for char in format_string:
        if char == '0' or char == '1':
            random_data += str(random.choice([0, 1]))  # Randomly choose between 0 and 1
    return random_data


def Superset_data(input_file, output_file, new_data_ratio):
    """
    Process the file by generating new data and swapping data with the original data while keeping primary key intact.
    :param input_file: Path to the input dataset file.
    :param output_file: Path to the output dataset file.
    :param new_data_ratio: The ratio of new data to generate (e.g., 0.1 for 10%).
    """
    try:
        with open(input_file, 'r') as infile:
            lines = infile.readlines()

        # Extract header and data rows from the input file
        header = lines[0].strip().split('\t')  # Header row
        data = [line.strip().split('\t') for line in lines[1:]]  # Data rows (excluding the header)

        # Get the number of columns and rows in the data
        num_columns = len(header)
        num_rows = len(data)

        # Calculate the number of new data rows to generate
        num_new_data = int(num_rows * new_data_ratio)

        # Generate new data based on the format of existing data (excluding primary key column)
        new_data = []
        for i in range(num_new_data):
            new_row = []
            for col_index in range(num_columns - 1):  # Exclude the primary key column (last column)
                col_data = data[random.randint(0, num_rows - 1)][col_index]  # Randomly select data from original data
                random_generated_data = generate_random_data(col_data)  # Generate random data based on the format
                frandom_generated_data = f"\"{random_generated_data}\""
                new_row.append(frandom_generated_data)

            # Add the primary key column (incremental value as a string with double quotes)
            primary_key_value = int(data[-1][-1].strip('"')) + i + 1  # Increment the primary key
            new_primary_key = f"\"{primary_key_value}\""  # Format primary key with double quotes
            new_row.append(new_primary_key)
            new_data.append(new_row)

        # Now we will replace original rows with the new generated data
        for i in range(num_new_data):
            # Randomly select an index from the original data to swap
            swap_index = random.randint(0, num_rows - 1)

            # Replace the selected row with the new data (keep the primary key intact)
            original_row = data[swap_index]
            replaced_row = new_data[i]
            data[swap_index] = replaced_row
            new_data[i] = original_row
            #data[swap_index] = [original_row[0: num_columns - 1]]
            #data[swap_index].append(replaced_row[num_columns - 1])
            #new_data[i] = [replaced_row[0: num_columns - 1]]
            #new_data[i].append(original_row[num_columns - 1])

        # Write the processed data to the output file
        with open(output_file, 'w') as outfile:
            outfile.write('\t'.join(header) + '\n')  # Write the header
            for row in data:
                outfile.write('\t'.join(row) + '\n')  # Write each data row
            for row in new_data:
                outfile.write('\t'.join(row) + '\n')  # Write each data row

        print(f"Processing complete. Results saved to {output_file}")

    except Exception as e:
        print(f"An error occurred: {e}")


# The 'input_file' is the path storing the fingerprinted database
# The 'output_file1' is the path to store the database with random bit flipping
# The 'output_file2' is the path to store the database with records removed
# The 'output_file3' is the path to store the database with records added
input_file = "/Users/songmingyang/Desktop/Dataset/Sub_Dataset/FDB1_Encode.txt"
output_file1 = "/Users/songmingyang/Desktop/Dataset/Sub_Dataset/Flipping_DB1.txt"
output_file2 = "/Users/songmingyang/Desktop/Dataset/Sub_Dataset/Subset_DB1.txt"
output_file3 = "/Users/songmingyang/Desktop/Dataset/Sub_Dataset/Superset_DB1.txt"
modifiable_bits_percentage = 0.5                        # The ratio of bits to be modified for attacking
d = 1                                                   # Last d bits are modifiable for attacking
deletion_percentage = 0.4                               # The ratio of records to be deleted for attacking
superset_percentage = 0.3                               # The ratio of records to be added for attacking
#Flipping_data(input_file, output_file1, modifiable_bits_percentage, d)
#Delete_data(input_file,output_file2, deletion_percentage)
Superset_data(input_file, output_file3, superset_percentage)