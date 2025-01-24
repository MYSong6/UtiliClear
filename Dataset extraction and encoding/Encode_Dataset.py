import csv
import numpy as np
from collections import Counter


# Helper function to convert string to binary
def string_to_binary(string):
    return ''.join(format(ord(c), '08b') for c in string)


# Helper function to convert number to binary
def number_to_binary(number, bit_length=8):
    return format(number, f'0{bit_length}b')


# Helper function to convert text to one-hot vector based on given vocabulary
def text_to_one_hot(text, vocabulary_dict):
    words = text.split()  # Split the text into words
    one_hot_vector = np.zeros(len(vocabulary_dict), dtype=int)
    for word in words:
        if word in vocabulary_dict:
            one_hot_vector[vocabulary_dict[word]] = 1
    # Return a string of ones and zeros without spaces
    return ''.join(map(str, one_hot_vector))


# Helper function to create binary encoding for discrete values
def create_binary_mapping(unique_values):
    mapping = {}
    bit_length = (len(bin(len(unique_values) - 1)) - 2)  # Minimum bits needed
    for idx, value in enumerate(unique_values):
        mapping[value] = format(idx, f'0{bit_length}b')  # Binary encoding with fixed bit length
    return mapping


# Read the CSV file, process data, and write to a new TXT file
def process_csv(input_file, output_file, dict_2_file, dict_4_file, dict_5_file):
    vocabulary_counter_2 = Counter()  # To count word frequency for Column 2
    vocabulary_counter_5 = Counter()  # To count word frequency for Column 5
    discrete_values = set()  # To gather unique values for Column 4

    # First pass: read the file and gather word frequencies and discrete values
    with open(input_file, mode='r', newline='', encoding='utf-8') as infile:
        reader = csv.reader(infile)
        headers = next(reader)  # Read and save the header row
        rows = list(reader)  # Save the data rows for second pass

        for row in rows:
            # Update counters for vocabulary in Column 2 and Column 5 (text columns)
            vocabulary_counter_2.update(row[1].split())  # Column 2: Text to one-hot
            vocabulary_counter_5.update(row[4].split())  # Column 5: Text to one-hot
            # Collect unique values for Column 4
            discrete_values.add(row[3])

    # Get the 200 most common words for each text column and create dictionaries
    vocabulary_dict_2 = {word: idx for idx, (word, _) in enumerate(vocabulary_counter_2.most_common(200))}
    vocabulary_dict_5 = {word: idx for idx, (word, _) in enumerate(vocabulary_counter_5.most_common(200))}

    # Create a binary mapping for discrete values (Column 4)
    discrete_mapping = create_binary_mapping(sorted(discrete_values))

    # Save the vocabulary dictionaries to separate files
    def save_vocabulary_dict(vocabulary_dict, output_file):
        with open(output_file, mode='w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            for word, idx in vocabulary_dict.items():
                writer.writerow([word, idx])

    # Save vocabulary dictionaries
    save_vocabulary_dict(vocabulary_dict_2, dict_2_file)
    save_vocabulary_dict(discrete_mapping, dict_4_file)
    save_vocabulary_dict(vocabulary_dict_5, dict_5_file)

    # Second pass: read the file again and process data
    with open(input_file, mode='r', newline='', encoding='utf-8') as infile:
        reader = csv.reader(infile)
        headers = next(reader)  # Read and save the header row

        # Prepare to write to a new TXT file
        with open(output_file, mode='w', newline='', encoding='utf-8') as outfile:
            # Add 'key' to the headers and write to the new file
            headers.append('key')
            outfile.write("\t".join(headers) + "\n")

            key = 1  # Initialize the key counter

            for row in rows:
                # Process each row
                binary_string = '"' + string_to_binary(row[0]) + '"'  # Column 1: String to binary with quotes
                one_hot_text_2 = '"' + text_to_one_hot(row[1],
                                                       vocabulary_dict_2) + '"'  # Column 2: Text to one-hot with quotes
                one_hot_text_5 = '"' + text_to_one_hot(row[4],
                                                       vocabulary_dict_5) + '"'  # Column 5: Text to one-hot with quotes

                # Handle number conversion for Column 3: Check for float values and convert to int
                try:
                    number = float(row[2])  # Try converting to float first
                    binary_number = '"' + number_to_binary(
                        int(number)) + '"'  # Convert to binary after rounding to int with quotes
                except ValueError:
                    binary_number = '"00000000"'  # Default to all zeros in case of invalid input with quotes

                # Binary encoding for discrete values in Column 4
                discrete_binary = '"00000000' + discrete_mapping[row[3]] + '"'  # Column 4: Binary encoding with quotes

                # Write the processed data to the new TXT file, with tab-separated values
                outfile.write(
                    f"{binary_string}\t{one_hot_text_2}\t{binary_number}\t{discrete_binary}\t{one_hot_text_5}\t\"{key}\"\n")

                key += 1  # Increment the key counter



dict_2_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/vocabulary_dict_2.csv'  # Output vocabulary file for Column 2
dict_4_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/vocabulary_dict_4.csv'  # Output vocabulary file for Column 4
dict_5_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/vocabulary_dict_5.csv'  # Output vocabulary file for Column 5

input_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/DB1.csv'  # Replace with your input CSV file path
output_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/DB1_Encode.txt'  # Replace with your output CSV file path

process_csv(input_file, output_file, dict_2_file, dict_4_file, dict_5_file)
