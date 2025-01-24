import os
import csv


input_folder = "/Users/songmingyang/Desktop/Dataset/Original"
output_folder = "/Users/songmingyang/Desktop/Dataset/Original2"


if not os.path.exists(output_folder):
    os.makedirs(output_folder)


for filename in os.listdir(input_folder):
    input_file_path = os.path.join(input_folder, filename)


    if os.path.isfile(input_file_path) and filename.endswith('.csv'):
        try:

            with open(input_file_path, 'r', encoding='utf-8', errors='ignore') as infile:
                lines = infile.readlines()


            clean_lines = [line.replace('\x00', '') for line in lines]


            output_file_path = os.path.join(output_folder, filename)

           
            with open(output_file_path, 'w', encoding='utf-8') as outfile:
                outfile.writelines(clean_lines)

            print(f"Processed {filename} and saved to {output_file_path}")
        except Exception as e:
            print(f"Error processing file {filename}: {e}")
