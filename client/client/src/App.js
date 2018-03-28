import React, { Component } from 'react'
// import Unity from 'react-unity-webgl'

import ThumbBar from './ThumbBar'
import RightBar from './RightBar'
import Controls from './Controls'
import './App.css'

class App extends Component {
  render() {
    return (
      <div className="App">
        <div id="gameContainer" style={{'width': '960px', 'height': '600px'}}></div>
        <RightBar />
        <ThumbBar />
        <Controls />
        This is where the arm model will go.
      </div>
    )
  }
}

export default App
