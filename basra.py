# import nltk
# nltk.download('punkt_tab')


import nltk
import json
import ijson
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from nltk.stem import WordNetLemmatizer

# # # Download necessary NLTK resources (uncomment these if needed)
nltk.download('punkt')  # For tokenization
nltk.download('stopwords')  # For stopwords
nltk.download('wordnet')  # For lemmatization

# Function to process text (tokenization, lowercase, stopword removal, and lemmatization)
def process_text(text):
    tokens = word_tokenize(text.lower())  # Tokenize and convert to lowercase
    stop_words = set(stopwords.words('english'))
    filtered_tokens = [word for word in tokens if word not in stop_words and word.isalpha()]
    lemmatizer = WordNetLemmatizer()
    lemmatized_tokens = [lemmatizer.lemmatize(word) for word in filtered_tokens]
    return lemmatized_tokens

# Function to create lexicon from specific fields in a large JSON file using ijson
def create_lexicon_from_large_data(json_file_path):
    i =2 
    lexicon = set()  # Use a set to avoid duplicates
    with open(json_file_path, 'r') as file:
        # Initialize ijson to parse the file incrementally
        objects = ijson.items(file, 'item')  # Assumes the JSON is an array of objects at the top level
        
        # Iterate through each object (publication) in the JSON file
        for obj in objects:
            print(i)
            i=i+1 
            # Process title
            if 'title' in obj and obj['title']:
                lexicon.update(process_text(obj['title']))
            
            # Process keywords
            if 'keywords' in obj:
                for keyword in obj['keywords']:
                    if keyword:  # Make sure keyword is not None
                        lexicon.update(process_text(keyword))  # Process each keyword
            
            # Process abstract
            if 'abstract' in obj and obj['abstract']:
                lexicon.update(process_text(obj['abstract']))
            
            # Process author names
            if 'authors' in obj:
                for author in obj['authors']:
                    if 'name' in author and author['name']:
                        lexicon.update(process_text(author['name']))  # Process each author's name

            # Optionally, you can process 'year' and 'lang' if needed (though they are less relevant for a lexicon)
            if 'year' in obj and obj['year']:
                lexicon.add(str(obj['year']))  # Add the year to lexicon (convert to string for consistency)

    return lexicon


# Function to write the lexicon to a JSON file
def write_lexicon_to_json(lexicon, output_file_path):
    with open(output_file_path, 'w') as file:
        json.dump(list(lexicon), file, indent=4)  # Convert set to list before saving

# Example usage
json_file_path = r"e:/SEECS CS Data/Semester 3 Fall 2024/Data Structures And Algorithms(DSA)/DSA Project/All Proj Codes/TheCleanData.json"
output_file_path = r"e:/SEECS CS Data/Semester 3 Fall 2024/Data Structures And Algorithms(DSA)/DSA Project/All Proj Codes/Theprocessed_lexicondata.json"
# Create the lexicon from the specified fields in the large JSON file
lexicon = create_lexicon_from_large_data(json_file_path)

print(' we have returned from the lexicon functoin  ')
# Write the lexicon to a JSON file
write_lexicon_to_json(lexicon, output_file_path)

# Confirmation
print(f"Lexicon has been written to {output_file_path}")