import React, { Component } from 'react';
import logo from '../images/logo.jpg';

export class Logo extends Component {
  render() {
    // Inline styles
    const logoContainerStyle = {
      display: 'flex',
      justifyContent: 'center',
      alignItems: 'center',
      paddingTop: '5.5%',
      // border: '2px solid red',
    };

    const logoStyle = {
      maxWidth: '100%',
      height: '200px',
      borderRadius: '50%',
      // border: '2px solid red',
    };

    return (
      <div style={logoContainerStyle}>
        <img src={logo} alt="Logo" style={logoStyle} />
      </div>
    );
  }
}

export default Logo;
