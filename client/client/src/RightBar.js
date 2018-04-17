import React, { Component } from 'react'
import './RightBar.css'

class RightBar extends Component {
  render() {
    return (
      <div className="RightBar">
        <div className="RightBar-divide"></div>
        <pre>
{`
  data: {
    finger-bend: [
      0.129837,
      0.128302,
      0.213911,
      0.238768
    ],
    palm-pressure: [
      0.834793,
      0.465692,
      0.232837,
      0.937984,
      0.237480
    ],
    orientation: [
      0.072334,
      0.238473,
      0.238663
    ],
    temp: 32
  }
`}
      </pre>
      </div>
    )
  }
}

export default RightBar
