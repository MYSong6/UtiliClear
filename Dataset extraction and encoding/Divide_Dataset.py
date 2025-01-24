import os
import pandas as pd
import random
import csv

# Configuration
db1_size_bytes = 8 * (1024 ** 3)/400  # Target size for DB1 (8GB)
db2_size_bytes = 16 * (1024 ** 3)/400 # Target size for DB2 (16GB)
db3_size_bytes = 34 * (1024 ** 3)/400 # Target size for DB2 (34GB)

data_dir = "/Users/songmingyang/Desktop/Dataset"  # Path to the folder containing CSV files of the dataset
# Output files
db1_file = "/Users/songmingyang/Desktop/Dataset/Sub_Dataset/DB1.csv"
db2_file = "/Users/songmingyang/Desktop/Dataset/Sub_Dataset/DB2.csv"
db3_file = "/Users/songmingyang/Desktop/Dataset/Sub_Dataset/DB3.csv"

def calculate_row_size(row, encoding="utf-8"):
    """Calculate the size of a row in bytes."""
    return sum(len(str(value).encode(encoding)) for value in row)

def randomize_files_and_rows():
    """Randomly shuffle files and rows."""
    file_paths = [os.path.join(data_dir, f) for f in os.listdir(data_dir) if f.endswith(".csv")]
    random.shuffle(file_paths)

    rows = []
    for file_path in file_paths:
        print(f"Reading file: {file_path}")
        with open(file_path, "r", encoding="utf-8") as f:
            reader = csv.reader(f)
            headers = next(reader)  # Skip headers if present
            rows.extend(list(reader))

    print("Shuffling rows...")
    #random.shuffle(rows)
    return rows, headers

def extract_datasets(rows):
    """Extract random subsets for DB1 and DB2."""
    db1_rows = []
    db2_rows = []
    db3_rows = []
    db1_current_size = 0
    db2_current_size = 0
    db3_current_size = 0


    for row in rows:
        row_size = calculate_row_size(row)
        if db1_current_size < db1_size_bytes:
            db1_rows.append(row)
            db1_current_size += row_size
        if db2_current_size < db2_size_bytes:
            db2_rows.append(row)
            db2_current_size += row_size
        if db3_current_size < db3_size_bytes:
            db3_rows.append(row)
            db3_current_size += row_size
        if db3_current_size >= db1_size_bytes and db2_current_size >= db2_size_bytes and db3_current_size >= db3_size_bytes:
            break

    return db1_rows, db2_rows, db3_rows

def write_to_csv(rows, output_file, headers):
    """Write rows to a CSV file."""
    df = pd.DataFrame(rows, columns=headers)
    df.to_csv(output_file, index=False, encoding="utf-8")
    print(f"Saved {output_file} with size: {os.path.getsize(output_file) / (1024 ** 3):.2f} GB")

def main():
    # Randomly shuffle and process rows
    rows, headers = randomize_files_and_rows()

    # Extract DB1, DB2, and DB3
    db1_rows, db2_rows, db3_rows = extract_datasets(rows)

    # Save results
    write_to_csv(db1_rows, db1_file, headers)
    write_to_csv(db2_rows, db2_file, headers)
    write_to_csv(db3_rows, db3_file, headers)

if __name__ == "__main__":
    main()

