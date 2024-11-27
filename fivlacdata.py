import ijson
import json

# Step 1: Open the large JSON file
input_file = r"e:/SEECS CS Data/Semester 3 Fall 2024/Data Structures And Algorithms(DSA)/DSA Project/All Proj Codes/TheCleanData.json"
output_file = r"e:/SEECS CS Data/Semester 3 Fall 2024/Data Structures And Algorithms(DSA)/DSA Project/All Proj Codes/Thesmaller_data.json'"

# json_file_path = r'D:\4. fourth semester\dsa projec\TheCleanData.json'
# output_file_path = r'D:\4. fourth semester\dsa projec\Theprocessed_lexicondata.json'

# Initialize a list to store the first 5 lakh objects
first_5_lakh = []

# Step 2: Parse the JSON file incrementally using ijson
with open(input_file, 'r') as file:
    # We assume the data is an array of objects, so we use 'item' to iterate over each object in the array
    objects = ijson.items(file, 'item')
    # Loop through the first 5 lakh objects and add them to the list
    for count, obj in enumerate(objects, start=1):
        if count > 500000:
            break
        first_5_lakh.append(obj)

# Step 3: Save the first 5 lakh objects to a new JSON file
with open(output_file, 'w') as outfile:
    json.dump(first_5_lakh, outfile, indent=4)

print(f"Successfully created '{output_file}' with the first 5 lakh objects.")