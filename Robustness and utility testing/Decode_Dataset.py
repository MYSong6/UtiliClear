import csv
import numpy as np


# Helper function to convert binary to string
def binary_to_string(binary_string):
    return ''.join(chr(int(binary_string[i:i + 8], 2)) for i in range(0, len(binary_string), 8))


# Helper function to convert binary back to number
def binary_to_number(binary_string):
    return int(binary_string, 2)


# Helper function to convert one-hot vector back to text based on the given vocabulary dictionary
def one_hot_to_text(one_hot_vector, vocabulary_dict):
    words = []
    for idx, value in enumerate(one_hot_vector):
        if value == '1':  # If the vector has '1', it means this word is in the original text
            words.append(list(vocabulary_dict.keys())[list(vocabulary_dict.values()).index(idx)])
    return ' '.join(words)


# Helper function to restore the discrete values from binary
def binary_to_discrete(binary_value, discrete_mapping):
    for key, value in discrete_mapping.items():
        if value == binary_value:
            return key
    return None  # If no match is found, return None


# Read the vocabulary dictionaries
def load_vocabulary_dict(file_path):
    vocabulary_dict = {}
    with open(file_path, mode='r', newline='', encoding='utf-8') as f:
        reader = csv.reader(f)
        for row in reader:
            vocabulary_dict[row[0]] = int(row[1])
    return vocabulary_dict


# Read the discrete value mappings
def load_discrete_mapping(file_path):
    discrete_mapping = {}
    with open(file_path, mode='r', newline='', encoding='utf-8') as f:
        reader = csv.reader(f)
        for row in reader:
            discrete_mapping[row[0]] = row[1]
    return discrete_mapping


# Restore the data from output.txt to a CSV file
def restore_data(output_file, dict_2_file, dict_4_file, dict_5_file, restored_file):
    # Load dictionaries
    vocabulary_dict_2 = load_vocabulary_dict(dict_2_file)
    discrete_mapping_4 = load_discrete_mapping(dict_4_file)
    vocabulary_dict_5 = load_vocabulary_dict(dict_5_file)

    # Open the output file to read and the new file to write restored data
    with open(output_file, mode='r', newline='', encoding='utf-8') as infile, \
            open(restored_file, mode='w', newline='', encoding='utf-8') as outfile:
        reader = infile.readlines()
        headers = reader[0].strip().split("\t")

        # Skip header and start processing rows
        outfile.write(",".join(headers) + "\n")  # Write header to the restored file

        for line in reader[1:]:
            fields = line.strip().split("\t")

            # Extract and restore each column
            binary_string_1 = fields[0].strip('"')
            text_column_2 = fields[1].strip('"')
            binary_number = fields[2].strip('"')
            binary_discrete = fields[3].strip('"')
            text_column_5 = fields[4].strip('"')
            key = fields[5].strip('"')

            # Restore each column:
            string_column_1 = binary_to_string(binary_string_1)  # Column 1: binary to string
            one_hot_text_2 = one_hot_to_text(text_column_2, vocabulary_dict_2)  # Column 2: One-hot to text
            number_column_3 = binary_to_number(binary_number)  # Column 3: Binary to number
            discrete_value_4 = binary_to_discrete(binary_discrete[8:],
                                                  discrete_mapping_4)  # Column 4: Binary to discrete value
            one_hot_text_5 = one_hot_to_text(text_column_5, vocabulary_dict_5)  # Column 5: One-hot to text

            # Write restored row to the new CSV file
            restored_row = [string_column_1, one_hot_text_2, number_column_3, discrete_value_4, one_hot_text_5, key]
            outfile.write(",".join(map(str, restored_row)) + "\n")


dict_2_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/vocabulary_dict_2.csv'  # Output vocabulary file for Column 2
dict_4_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/vocabulary_dict_4.csv'  # Output vocabulary file for Column 4
dict_5_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/vocabulary_dict_5.csv'  # Output vocabulary file for Column 5

restored_output_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/DB1_D.csv'  # Replace with your input CSV file path
input_file = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/DB1_Encode.txt'  # Replace with your output CSV file path

restore_data(input_file, dict_2_file, dict_4_file, dict_5_file,restored_output_file )
