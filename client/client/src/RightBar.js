import React, { Component } from 'react'
import './RightBar.css'

import openSocket from 'socket.io-client'
const socket = openSocket('http://127.0.0.1:5000')

class RightBar extends Component {
  state = {
    dataAvailable: false
  }

  constructor(props) {
    super(props)
    this.subscribeTo(data =>
      this.setState({
        fingerBend: data.data['finger-bend'],
        palmPressure: data.data['palm-pressure'],
        orientation: data.data.orientation,
        temp: data.data.temp
      })
    )
  }

  subscribeTo = cb => {
    socket.on('digit', msg => {
      cb(msg)
    })
  }

  render() {
    var output = 'no data'
    if (this.state.fingerBend != null && this.state.palmPressure != null && this.state.orientation != null) {
      output = `
data: {
  finger-bend: [
    ${this.state.fingerBend[0].toFixed(6)},
    ${this.state.fingerBend[1].toFixed(6)},
    ${this.state.fingerBend[2].toFixed(6)},
    ${this.state.fingerBend[3].toFixed(6)}
  ],
  palm-pressure: [
    ${this.state.palmPressure[0].toFixed(6)},
    ${this.state.palmPressure[1].toFixed(6)},
    ${this.state.palmPressure[2].toFixed(6)},
    ${this.state.palmPressure[3].toFixed(6)},
    ${this.state.palmPressure[4].toFixed(6)}
  ],
  orientation: [
    ${this.state.orientation[0].toFixed(6)},
    ${this.state.orientation[1].toFixed(6)},
    ${this.state.orientation[2].toFixed(6)}
  ],
  temp: ${this.state.temp}
}
`
    }

    return (
      <div className="RightBar">
        <div className="RightBar-divide" />
        <pre>{output}</pre>
      </div>
    )
  }
}

export default RightBar
