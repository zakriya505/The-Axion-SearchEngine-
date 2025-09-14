import React, { Component } from 'react';
import PropTypes from 'prop-types';

class SearchBar extends Component {
  state = {
    searchQuery: '',
  };

  handleInputChange = (event) => {
    const { value } = event.target;
    this.setState({ searchQuery: value });
    this.props.onInputChange(value); // Pass the updated value to the parent (App)
  };

  handleSearch = () => {
    const { searchQuery } = this.state;
    console.log('Sending searchText:', searchQuery); // Log the search query
    this.props.onButtonClick(searchQuery); // Pass the search query to the parent (App)
  };

  render() {
    return (
      <div style={styles.container}>
        <div style={styles.searchBar}>
          <input
            type="text"
            value={this.state.searchQuery}
            onChange={this.handleInputChange} // Update the searchQuery state
            placeholder="Search Axion"
            style={styles.input}
          />
          <button onClick={this.handleSearch} style={styles.button}>
            🔍
          </button>
        </div>
      </div>
    );
  }
}

SearchBar.propTypes = {
  onInputChange: PropTypes.func.isRequired, // Expect a function to handle input changes
  onButtonClick: PropTypes.func.isRequired, // Expect a function to handle button clicks
};

const styles = {
  container: {
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    height: '20vh',
  },
  searchBar: {
    display: 'flex',
    alignItems: 'center',
    width: '600px',
    backgroundColor: '#fff',
    borderRadius: '30px',
    boxShadow: '0 4px 6px rgba(0, 0, 0, 0.1)',
    padding: '10px 20px',
  },
  input: {
    flex: 1,
    border: 'none',
    outline: 'none',
    fontSize: '16px',
    padding: '10px',
  },
  button: {
    border: 'none',
    backgroundColor: 'transparent',
    fontSize: '20px',
    cursor: 'pointer',
    outline: 'none',
  },
};

export default SearchBar;
