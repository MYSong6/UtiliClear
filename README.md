

# Descriptions

The source code consists of three parts: dataset extraction and encoding, UtiliClear implementation, and robustness and utility testing.  The code for dataset extraction and encoding, as well as robustness and utility testing is written in Python, while the implementation of UtiliClear is written in C++. 
The execution sequence and input-output relationships of the code files are demonstrated in the following figure. 

![image](https://github.com/MYSong6/UtiliClear/blob/main/Code_Overvoew.jpg)



*Executing all the processes, including dataset division, encoding, preprocessing, fingerprinting, verification, and so on, requires significant runtime. Therefore, we directly provide two encoded subsets (186 MB and 9.7 GB) along with their corresponding dictionary files to evaluate the functionality and reproducibility. Moreover, executing all the experiments in the paper takes approximately several days. Therefore, testing can be done on a smaller dataset to estimate the resource consumption on a larger dataset, as the algorithm's consumptions are linearly related to the dataset size. In addition, the new testing environment may not align exactly with that used in the paper's experiments, so the consumptions may vary, but the overall trend will remain consistent.*


## Dataset extraction and encoding

### Dataset extraction

The code file “<u>Remove_NULL.py</u>” is used to remove invald data from the dataset. Before dividing the dataset into random subsets, execute this code file, where the 'input_folder' contains the files of the origianl dataset and the 'output_folder' is the folder to store the output files. 

The code file “<u>Divide_Dataset.py</u>” is used to randomly extract two subsets (8GB and 16GB) from the original dataset randomly. Before running the code, the dataset input folder ('datadir') and output paths ('db1_file', 'db2_file', and 'db3_fil3') should be adjusted according to the specific dataset storage directory (make sure that the input folder only contains the csv files of the dataset).

| Path       | Content                                                     |
|------------|-------------------------------------------------------------|
| 'datadir'  | The folder storing the csv files from the original dataset. |
| 'db1_file' | One file to store the DB1 (8GB).                            |
| 'db2_file' | One file to store the DB2 (16GB).                           |
| 'db3_file' | One file to store the DB3 (34GB).                           |

  

### Dataset encoding

The code file “<u>Encode_Dataset.py</u>” is used to encode the attribute values of these datasets into bit-strings. Categorical attributes are encoded into word vectors. Text sentences are encoded inot one-hot vectors. Numeric attributes are encoded into corresponding binary bit-strings. Before running the code, the dataset input and output paths should be adjusted according to the specific dataset storage directory. After obtaining the encoded datasets, you should manually build corresponding databases on your device and import the datasets into the [MySQL-5.1.51-Win32](https://downloads.mysql.com/archives/installer/) database (Either Navicat tool or command-line interface can be used to import encoded datasets into MySQL). The code file “<u>Encode_Dataset.py</u>” additionally outputs dictionary files for the cloumns containing words. The dictionary files should be sent to the recipient for further decode the binary bit-strings to the words and sentences.


| Path           | Content                                                   |
|----------------|-----------------------------------------------------------|
| 'input_file'   | The file storing the original dataset (DB1, DB2, or DB3). |
| 'output_file'  | The file storing the encoded dataset.                     |
| 'dirct_2_file' | The dictionary file for the second column.                |
| 'dict_4_file'  | The dictionary file for the fourth column.                |
| 'dict_5_file'  | The dictionary file for the fifth column.                 |



## UtiliClear implementation

The UtiliClear implementation consists of five parts, i.e., setup, preprocessing, fingerprint embedding, verification, and fingerprint extraction. Before running the programs, the libraries of [OpenSSL-3.0.4-Win32](https://docs.openssl.org/3.4/man7/ossl-guide-migration/#legacy-algorithms), [PBC-0.5.14-Win32](https://crypto.stanford.edu/pbc/), [GMP-Win32](https://gmplib.org), and [Schifa RS ECC-0.0.1](https://www.schifra.com/downloads.html) should be installed and included in the programs. 

### Setup
 
During the setup process, the recipient and the database owner interact to exchange public parameters, including the public parameters of database owner, database information (e.g., the number of columns), the number of insignificant bits, the number of groups, the maximum number of modified bits. The code files “<u>Setup_DO.cpp</u>” and “<u>Setup_Recipient.cpp</u>” serve as the setup programs for the database owner and recipient, respectively (The 'Parameter_path' of the code file “<u>Setup_Recipient.cpp</u>” is used to store the parameters in the recipient side). 

Note that before running the code, the database owner side should manually build three tables in MySQL databse to store the parameters of the database owner, the dataset information, and the parameter specified by the recipient. 



* The do_parameters table:

    | p | q | N | g | h |
    |---|---|:--|:--|:--| 
    |   |   |   |   |   |
* The recipient_parameters table:

    | Name | Num_InsignBits | Num_Group | Fingerprint|
    |---|---|:--|:--| 
    |   |   |   |   |

* The db_info table: (Attr6 is additionally added, as the primary key to unique index a record. It cannnot be modified)

    | DB | Row_Num | Col_Num |  Attr1  |  Attr2  |  Attr3  |  Attr4  |  Attr5  |  Attr6  |
    |----|:--------|:--------|:-----|:-----------|:--------|:---------|---------|-----|
    |  DB1  |  54846036  |  6  |  asin  |  reviewText  |  overall  |category | summary |  key  |
    |  DB2  |  109693107  |  6  |  asin  |  reviewText  |  overall  |category | summary |  key  |
    |  DB3  |  230139802  |  6  |  asin  |  reviewText  |  overall  |category | summary |  key  |

You can also modify the table headers and styles, but you must update the corresponding lines in the code to adjust the database operation-related logic accordingly.  If the table settings above are followed, no changes to the database operations in the code are required.

### Preprocessing

During the preprocessing process, the database owner first groups the database based on the specified number of groups and the number of insignificant bits determined by the recipient. 

**Database grouping:**

The code file “<u>DB_Grouping_DO.cpp</u>” is used to execute database grouping. It takes the encoded database file ('inputFile') as the input and outputs the grouping results of each column. Each conlum corresponds to two output files: one for the grouping of significant bits ('outputDir1') and the other for the grouping of insignificant bits ('outputDir2').

| Path         | Content                                                                         |
|--------------|---------------------------------------------------------------------------------|
| 'inputFile'  | The encoded dataset.                                                            |
| 'outputDir1' | The group results of significant bits (each conlum corresponds to one file).    |
| 'outputDir2' | The group results of insignificant bits (each conlum corresponds to one file).  |


**Commitment for significant bit group:**

The code file “<u>Com_SignBits_DO.cpp</u>” is used to compute commitments for significant bit groups. Before executing the code, make sure that the group files for significant bits are stored in an identical folder, and on other files are stored in this folder. Then you should modify the folder 'inputDir' in the code file correspond to the folder you have selected. You should also modify the folder 'outputDir' to store the output commitment files.

| Folder         | Content                                                                         |
|--------------|---------------------------------------------------------------------------------|
| 'inputDir'  | The group results of significant bits (each conlum corresponds to one file).                                                           |
| 'outputDir' | The commitment results of significant bits (each conlum corresponds to one file).    |


**Locking for insignificant bit group:**

* Generate ``C_x``: The code file “<u>Lock1_Gen_CX_DO.cpp</u>” enables the DO to compute the GM ciphertext for insignificant bits. Before executing the program, the grouped insignificant bit results (each conlum croresponds to a file) should be contatined within an identical folder 'inputDir', and on other files are stored in this folder. You should also modify the folder 'outputDir' to store the output GM ciphertext files (each conlum croresponds to a file). 

    | Folder         | Content                                                                         |
    |--------------|---------------------------------------------------------------------------------|
    | 'inputDir'  | The group results of insignificant bits (each conlum corresponds to one file).                                                           |
    | 'outputDir' | The ``C_x`` results of insignificant bits (each conlum corresponds to one file).    |


*  Generate ``C_y``: The code file “<u>Lock2_Gen_CY_Recipient.cpp</u>” enables the recipient to embed perturbation to the GM ciphertext of insignificant bits. Before executing the program, the GM ciphertext files (each conlum croresponds to a file) should be contatined within an identical folder 'inputDir1', and on other files are stored in this folder. You should also modify the 'outputDir1' and 'outputDir2' to store the pertrubed GM ciphertext and the used perturbations (each conlum croresponds to a file), respectively.

   
    | Path        | Content                                                                                             |
    |-------------|-----------------------------------------------------------------------------------------------------|
    | 'inputDir'  | The folder storing ``C_x`` (each conlum corresponds to one file). |
    | 'Para_path' | The file storing the parameters of the recipient.                                                   |
    | 'output1'   | The folder to store the perturbed ciphertexts ``C_y`` (each conlum corresponds to one file).                      |
    | 'output2'   | The folder to store the perturbations (each conlum corresponds to one file).                  |
    

 
* Generate ``y``: The code file “<u>Lock3_Gen_Y_DO.cpp</u>” enables the DO to recover ``y`` from the perturbed ciphertexts received from the recipient. Before executing the program, the perturbed ciphertext files (each conlum croresponds to a file) should be stored in an identical folder, and on other files are stored in this folder. Then you should modify the "inputDir" in the code file correspond to the folder you have selected. You should also modify the "outputDir" to store the decrypted result ``y`` (each conlum croresponds to a file). 

      
    | Folder         | Content                                                                         | 
    |--------------|---------------------------------------------------------------------------------|
    | 'inputDir'  | The perturbed ciphertexts ``C_y`` (each conlum corresponds to one file).                                                                                |
    | 'outputDir' | The decrypted results ``y`` (each conlum corresponds to one file).                          | 
    

### Fingerprint embedding and extraction

**Fingerprint embedding:** 

The DO executes the “<u>Fingerprinting_DO.cpp</u>” code file, while the recipient executes the “<u>Fingerprinting_Recipient.cpp</u>” code file. The recipient sends its specific fingerprint to the DO. The DO recevies the fingerprint and embeds it into the database.


| Path          | Content                       |
|---------------|-------------------------------|
| 'DB_path'     | The original encoded dataset. |
| 'output_path' | The fingerprinted dataset.    |



**Fingerprint extraction:** 

The code file “<u>Fingerprint_Extraction_DO.cpp</u>”  is executed by the DO to exract the fingerprint from the fingerprinted database.

| Path          | Content                       |
|---------------|-------------------------------|
| 'DB_path'     | The original dataset. |
| 'FDB_path' | The fingerprinted dataset.    |

### Verification

**Fingerprinted database group:** 

The code file “<u>FDB_Grouping_Recipient.cpp</u>” is used to group the fingerprinted database following the rule of the “<u>DB_Grouping_DO.cpp</u>” code file. It takes the fingerprinted database file 'inputFile' as input and outputs the grouping results of each column. Each conlum corresponds to two output files: one for the grouping of significant bits ('outputDir1') and the other for the grouping of insignificant bits ('outputDir2').

| Path          | Content                       |
|---------------|-------------------------------|
| 'inputFile'     | The fingerprinted dataset. |
| 'outputDir1'    | The group results of significant bits (each conlum corresponds to one file).  |
| 'outputDir2'    | The group results of insignificant bits (each conlum corresponds to one file).  |

**Commitment verification for signifincat bit group:** 

The code file “<u>Com_Verify_Recipient.cpp</u>” is used to verify commitments for significant bit groups. Before executing the code, make sure that the grouping results of significant bits for all the columns are are stored in an identical folder 'inputDir1', and on other files are stored in this folder. Make sure that the commitment files for distinct conlums are stored in another identical folder 'inputDir2', and on other files are stored in this folder. 

| Path          | Content                       |
|---------------|-------------------------------|
| 'inputDir1'    | The group results of significant bits (each conlum corresponds to one file).  |
| 'inputDir2'    | The commitment results of insignificant bits (each conlum corresponds to one file).  |
| 'Par_path'    | The file storing the parameters of the recipient.  | 

**Verification for insignificant bit group:**

* The code file “<u>InsignificantBits_Verify1_Recipient.cpp</u>” is used by the recipient to embed the insignificant bits and the random bit-strings used during the locking process to random selected codewords. It takes two folders, i.e., "inputDir1" and "inputDir2", as inputs, and output files to two folders, i.e., "outputDir1" and "outputDir2" (Each column croresponds to one file in every folder).
   
    | Folder       | Content                                                                |
    |--------------|------------------------------------------------------------------------|
    |"inputDir1"  | Grouped insignificant bits of the fingerprinted database                                                                    |
    | "inputDir2"  | The random bit-strings selected during the locking process                                                                     |
    |  "outputDir1" | The results ``z`` (the value ``z``=Perm``(x \oplus u, \delta) \oplus cw`` in the paper). |
    |  "outputDir2" |  The original messages (``m``) of the selected codewords                  |


* The code file “<u>InsignificantBits_Verify2_DO.cpp</u>” is executed by the DO to extract the codewords ``cw`` selected by the recipient and decode these codewords to obtain messages (``m'``). It takes two folders, i.e., "inputDir1" and "inputDir2", as inputs, and output files to a folder, i.e., "outputDir" (Each column croresponds to one file in every folder).

    | Folder       | Content                                                                |
    |--------------|------------------------------------------------------------------------|
    | "inputDir1"  |  The results (``z``) received from the recipient.                                                                |
    | "inputDir2"  |  The locked bit-strings (``y``) during the locking process.                                                                    |
    | "outputDir" | The decoded messages (``m'``). |
    
* The code file “<u>InsignificantBits_Verify3_Recipient.cpp</u>” is executed by the recipient to verify whether its original messages (``m``) equal to the decoded messages (``m'``) received form the DO. It takes two folders, i.e., "inputDir1" and "inputDir2", as inputs (each column croresponds to one file in every folder), and outputs the verification result (success or failure).

  | Folder       | Content                                                                |
    |--------------|------------------------------------------------------------------------|
    | "inputDir1"  |  The original messages (``m``) selected by the recipient.                                                                |
    | "inputDir2"  |  The locked decoded messages (``m'``)  received from the DO.                                                                  |


## Robustness and utility testing

### Robustness

**Fingerprint removal attack:** 

The code file “<u>Fingerprint_Remove.py</u>”  is executed by the recipient to reomve the fingerpring from the dataset. It takes the fingerprinted database as input, and outputs three files corresponding to the results of flipping attack, superset attack and subset attack, respectively.


| Path           | Content                                  |
|----------------|------------------------------------------|
| 'input_file'   | The fingerprinted database.              |
| 'output_file1' | The result database of flipping attack.  |
| 'output_file2' | The result database of subset attack.    |
| 'output_file3' | The result database  of superset attack. |

The parameters 'modifiable_bits_percentage', 'deletion_percentage', and 'superset_percentage' control the ratio of flipping bits, deleting records and adding records, respectively.

**Robustness checking:**

Using the code file “<u>Fingerprint_Extraction_DO.cpp</u>” and modify the parameter 'FDB_path' to point to the databases under distinct attacks, we can obtain the ratio of matched fingerprint bits.

### Utility

**Database Query:** 

Before checking the query accuracy, convert the binary fingerprinted database to the dataset retaining format of the original dataset (e.g., integers, words, and sentences) by executing the code file “<u>Decode_Dataset.py</u>”. The input and output of this code are presented as follows.

| Path                   | Content                                    |
|------------------------|--------------------------------------------|
| 'input_file'           | The fingerprinted binary dataset.          |
| 'restored_output_file' | The decoded dataset.                       |
| 'dict_2_file'          | The dictionary file for the second column. |
| 'dict_4_file'          | The dictionary file for the fourth column. |
| 'dict_5_file'          | The dictionary file for the fifth column.  |


Manually import the decoded fingerprint dataset and the original dataset into the MySQL database, and then test the SQL query results.

**Classifier training:**

The code file “<u>Classifier_Training.py</u>” is uesd to training and evaluate the classifiers (i.e., KNN, LR, and SVM), where the 'file_path' storing the encoded dataset.


