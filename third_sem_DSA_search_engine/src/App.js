import React, { Component } from 'react';
import SearchBar from './components/SearchBar';
import Content from './components/Content';
import Logo from './components/Logo';

export default class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      showContent: false,
      contentData: null,
      errorMessage: '',
      loading: false,
      searchText: '', // This holds the search text
    };
  }

  // This method will be called by SearchBar to update the searchText
  handleInputChange = (searchText) => {
    this.setState({ searchText }); // Update searchText state
  };

  handleButtonClick = async (searchText) => {
    console.log('Sending searchText:', searchText); // Log the search text before sending it

    this.setState({ loading: true, errorMessage: '' });
    try {
      const response = await fetch('http://localhost:18080/get_json', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ searchText }), // Send the searchText to the backend
      });

      if (!response.ok) {
        throw new Error('Failed to fetch data');
      }

      const data = await response.json();
      console.log('Received data:', data); // Log the received data for debugging

      this.setState({
        showContent: true,
        contentData: data.file_data, // Use file_data as the content to display
        loading: false,
      });
    } catch (error) {
      console.error('Error fetching data:', error);
      this.setState({
        errorMessage: 'Failed to fetch data from the backend',
        loading: false,
      });
    }
  };

  render() {
    const { showContent, contentData, errorMessage, loading } = this.state;

    return (
      <div>
        <Logo />
        <SearchBar
          onInputChange={this.handleInputChange} // Pass the handleInputChange method to SearchBar
          onButtonClick={this.handleButtonClick} // Pass the handleButtonClick method to SearchBar
        />
        {loading && <p>Loading...</p>}
        {errorMessage && <p style={{ color: 'red' }}>{errorMessage}</p>}
        {showContent && contentData && <Content data={contentData} />} {/* Pass contentData to Content */}
      </div>
    );
  }
}
