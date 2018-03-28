import React, { Component } from 'react'
import './ThumbBar.css'

class ThumbBar extends Component {
  render() {
    return (
      <div className="ThumbBar">
        <div className="ThumbBar-divide"></div>
        <div className="ThumbBar-highlight"></div>
        This is where timeline data will go for easy scrubbing.
      </div>
    )
  }
}

export default ThumbBar
