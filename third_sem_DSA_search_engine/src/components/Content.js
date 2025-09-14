import React, { Component } from 'react';

class Content extends Component {
  render() {
    const { data } = this.props;

    console.log('Received data:', data); // Log the data for debugging

    // If data is not available, show a loading message
    if (!data) {
      return <p>Loading...</p>;
    }

    // If data is not an array or is empty, show a "no results found" message
    if (!Array.isArray(data) || data.length === 0) {
      return <p>No results found. Try searching for something else!</p>;
    }

    return (
      <div style={styles.container}>
        <h2>Search Results</h2>
        <div style={styles.results}>
          {data.map((result, index) => (
            <div key={index} style={styles.resultItem}>
              <h3 style={styles.heading}>{result.Title || 'No Title Available'}</h3>

              {/* Check if URLs exist, and display only the first URL */}
              {result.URLs && Array.isArray(result.URLs) && result.URLs.length > 0 ? (
                <div style={styles.urls}>
                  <a
                    href={result.URLs[0]} // Use only the first URL
                    style={styles.url}
                    target="_blank"
                    rel="noopener noreferrer"
                  >
                    {result.URLs[0]} {/* Display the first URL */}
                  </a>
                </div>
              ) : (
                <p>No URLs available</p>
              )}

              <p style={styles.description}>{result.Abstract || 'No description available'}</p>
            </div>
          ))}
        </div>
      </div>
    );
  }
}

// Styling for the Content component
const styles = {
  container: {
    padding: '20px',
    backgroundColor: '#fff',
    width: '80%',
    margin: '20px auto', // Add some spacing from the top
    borderRadius: '10px',
    boxShadow: '0 4px 8px rgba(0, 0, 0, 0.1)',
  },
  results: {
    marginTop: '20px',
    maxHeight: '400px', // Set a max height for scrollable content
    overflowY: 'auto', // Enable vertical scrolling for large results
  },
  resultItem: {
    marginBottom: '20px',
    padding: '10px',
    backgroundColor: '#f9f9f9',
    borderRadius: '8px',
    boxShadow: '0 2px 4px rgba(0, 0, 0, 0.1)',
  },
  heading: {
    fontSize: '18px',
    fontWeight: 'bold',
  },
  url: {
    color: '#1a73e8',
    textDecoration: 'none',
    fontSize: '16px',
    display: 'block',
    marginBottom: '5px', // Add spacing between URLs
  },
  description: {
    fontSize: '14px',
    color: '#555',
  },
  urls: {
    marginBottom: '10px',
  },
};

export default Content;
