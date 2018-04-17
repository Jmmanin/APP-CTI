import React, { Component } from 'react'

import RightBar from './RightBar'
import './App.css'

class App extends Component {
  render() {
    return (
      <div className="App">
        <div id="gameContainer" style={{'width': '960px', 'height': '600px'}}></div>
        <RightBar />
      </div>
    )
  }
}

export default App
