import React, { Component } from 'react'
import rewind from './rewind.svg'
import play from './play.svg'
import forward from './forward.svg'
import './Controls.css'

class Controls extends Component {
  render() {
    return (
      <div className="Controls">
        <img src={rewind} />
        <img src={play} />
        <img src={forward} />
      </div>
    )
  }
}

export default Controls
