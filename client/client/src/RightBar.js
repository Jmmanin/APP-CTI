import React, { Component } from 'react'
import './RightBar.css'

import hand from './hand.png'

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
    if (
      this.state.fingerBend != null &&
      this.state.palmPressure != null &&
      this.state.orientation != null
    ) {
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

    var palm0 = this.state.palmPressure != null ? this.state.palmPressure[4] : 0
    var palm1 = this.state.palmPressure != null ? this.state.palmPressure[3] : 0
    var palm2 = this.state.palmPressure != null ? this.state.palmPressure[2] : 0
    var palm3 = this.state.palmPressure != null ? this.state.palmPressure[1] : 0
    var palm4 = this.state.palmPressure != null ? this.state.palmPressure[0] : 0

    palm0 = 1 - (Math.min(200, palm0) / 200)
    palm1 = 1 - (Math.min(100, palm1) / 100)
    palm2 = 1 - (Math.min(100, palm2) / 100)
    palm3 = 1 - (Math.min(100, palm3) / 100)
    palm4 = 1 - (Math.min(100, palm4) / 100)

    var l1a = Math.max(40, Math.min(240, Math.max(palm0, palm1, palm2) * 240 * 2))
    var l1b = Math.max(40, Math.min(240, Math.max(palm3, palm4) * 240 * 2))

    var l2a = Math.max(30, Math.min(240, palm0 * 240 - 20))
    var l2b = Math.max(30, Math.min(240, Math.max(palm1, palm2) * 240 - 20))
    var l2c = Math.max(30, Math.min(240, Math.max(palm3, palm4) * 240 - 20))

    var l3a = Math.max(20, Math.min(240, palm0 * 240 - 50))
    var l3b = Math.max(20, Math.min(240, palm1 * 240 - 50))
    var l3c = Math.max(20, Math.min(240, palm2 * 240 - 50))
    var l3d = Math.max(20, Math.min(240, palm3 * 240 - 50))
    var l3e = Math.max(20, Math.min(240, palm4 * 240 - 50))

    var l4a = Math.max(10, Math.min(240, palm0 * 240 - 100))
    var l4b = Math.max(10, Math.min(240, palm1 * 240 - 100))
    var l4c = Math.max(10, Math.min(240, palm2 * 240 - 100))
    var l4d = Math.max(10, Math.min(240, palm3 * 240 - 100))
    var l4e = Math.max(10, Math.min(240, palm4 * 240 - 100))

    var l5a = Math.max(0, Math.min(240, palm0 * 240 - 150))
    var l5b = Math.max(0, Math.min(240, palm1 * 240 - 150))
    var l5c = Math.max(0, Math.min(240, palm2 * 240 - 150))
    var l5d = Math.max(0, Math.min(240, palm3 * 240 - 150))
    var l5e = Math.max(0, Math.min(240, palm4 * 240 - 150))

    return (
      <div className="RightBar">
        <div className="RightBar-divide" />
        <pre>{output}</pre>
        <div className="RightBar-hand">
          <img className="RightBar-hand-base" src={hand} />

          <svg
            className="RightBar-hand-base"
            version="1.0"
            xmlns="http://www.w3.org/2000/svg"
            width="330.000000px"
            height="446.000000px"
            viewBox="0 0 330.000000 446.000000"
            preserveAspectRatio="xMidYMid meet"
          >
            <g
              class="customBackground2"
              transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
              fill={`hsl(${l1a},75.94%,54.29%)`}
              stroke="none"
            >
              <path d="M698 2320 c-54 -29 -75 -62 -89 -141 -14 -77 -3 -175 25 -244 l15
        -34 38 16 c27 12 77 18 180 21 166 5 153 -8 153 148 0 100 0 101 -38 155 -48
        70 -103 99 -187 99 -39 -1 -73 -8 -97 -20z" />
              <path d="M905 1884 c-106 -16 -181 -38 -213 -63 l-24 -18 17 -139 c33 -273 90
        -555 126 -613 10 -17 35 -39 54 -48 31 -15 43 -15 93 -5 144 30 226 6 334 -97
        85 -82 97 -82 224 11 55 39 117 79 139 89 55 25 119 25 145 -1 20 -20 20 -20
        20 31 0 81 -27 159 -69 197 -20 18 -47 35 -61 39 -14 3 -90 37 -170 74 -80 38
        -149 69 -153 69 -3 0 -31 12 -62 26 -67 32 -85 59 -120 184 -33 118 -80 213
        -123 251 -37 33 -34 32 -157 13z" />
            </g>
          </svg>

          <svg
            className="RightBar-hand-base"
            version="1.0"
            xmlns="http://www.w3.org/2000/svg"
            width="330.000000px"
            height="446.000000px"
            viewBox="0 0 330.000000 446.000000"
            preserveAspectRatio="xMidYMid meet"
          >
            <g
              transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
              fill={`hsl(${l2b},75.94%,54.29%)`}
              stroke="none"
            >
              <path d="M833 1781 c-66 -41 -83 -97 -72 -231 5 -72 13 -98 47 -165 83 -166
          142 -222 339 -325 140 -73 164 -83 213 -84 172 -3 242 216 100 312 -19 12 -45
          25 -59 28 -100 19 -137 37 -186 89 -34 35 -61 76 -79 121 -52 128 -77 173
          -120 220 -39 43 -46 47 -96 51 -43 3 -62 0 -87 -16z" />
            </g>
          </svg>

          <svg
            className="RightBar-hand-base"
            version="1.0"
            xmlns="http://www.w3.org/2000/svg"
            width="330.000000px"
            height="446.000000px"
            viewBox="0 0 330.000000 446.000000"
            preserveAspectRatio="xMidYMid meet"
          >
            <g
              transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
              fill={`hsl(${l3c},75.94%,54.29%)`}
              stroke="none"
            >
              <path d="M1191 1285 c-43 -55 -23 -129 57 -212 72 -74 135 -83 201 -28 62 53
          63 146 1 198 -28 24 -120 53 -201 64 -30 4 -39 1 -58 -22z" />
            </g>
          </svg>

          <svg
            className="RightBar-hand-base"
            version="1.0"
            xmlns="http://www.w3.org/2000/svg"
            width="330.000000px"
            height="446.000000px"
            viewBox="0 0 330.000000 446.000000"
            preserveAspectRatio="xMidYMid meet"
          >
            <g
              transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
              fill={`hsl(${l4c},75.94%,54.29%)`}
              stroke="none"
            >
              <path d="M1219 1236 c-8 -18 -7 -30 5 -54 28 -55 76 -120 92 -126 96 -37 185
          69 123 147 -23 31 -101 57 -166 57 -36 0 -45 -4 -54 -24z" />
            </g>
          </svg>

          <svg
            className="RightBar-hand-base"
            version="1.0"
            xmlns="http://www.w3.org/2000/svg"
            width="330.000000px"
            height="446.000000px"
            viewBox="0 0 330.000000 446.000000"
            preserveAspectRatio="xMidYMid meet"
          >
            <g
              transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
              fill={`hsl(${l5c},75.94%,54.29%)`}
              stroke="none"
            >
              <path d="M1316 1199 c-19 -15 -26 -30 -26 -54 0 -36 36 -75 70 -75 10 0 29 9
          44 21 34 27 36 79 4 109 -28 26 -58 26 -92 -1z" />
            </g>
          </svg>

          <svg
            className="RightBar-hand-base"
            version="1.0"
            xmlns="http://www.w3.org/2000/svg"
            width="330.000000px"
            height="446.000000px"
            viewBox="0 0 330.000000 446.000000"
            preserveAspectRatio="xMidYMid meet"
          >
            <g
              transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
              fill={`hsl(${l3b},75.94%,54.29%)`}
              stroke="none"
            >
              <path d="M865 1719 c-35 -17 -48 -51 -53 -135 -4 -71 -1 -89 20 -135 14 -30
          37 -69 52 -87 33 -38 126 -102 149 -102 51 0 97 58 97 121 0 47 -65 203 -110
          264 -56 76 -104 99 -155 74z" />
            </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l4b},75.94%,54.29%)`} stroke="none">
          <path d="M861 1624 c-25 -31 -28 -90 -7 -137 22 -51 101 -125 141 -133 52 -9
          65 6 65 81 0 79 -25 140 -74 183 -48 42 -95 44 -125 6z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l5b},75.94%,54.29%)`} stroke="none">
          <path d="M896 1579 c-36 -28 -36 -80 0 -108 15 -12 34 -21 44 -21 10 0 29 9
          44 21 34 27 36 79 4 109 -28 26 -58 26 -92 -1z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l2a},75.94%,54.29%)`} stroke="none">
          <path d="M745 2276 c-124 -55 -139 -227 -28 -302 156 -105 343 82 239 239 -42
          63 -143 93 -211 63z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l3a},75.94%,54.29%)`} stroke="none">
          <path d="M760 2232 c-43 -21 -80 -75 -80 -116 0 -70 66 -136 136 -136 70 0
          143 86 130 154 -8 44 -41 85 -82 102 -44 18 -62 18 -104 -4z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l4a},75.94%,54.29%)`} stroke="none">
          <path d="M751 2184 c-26 -22 -31 -33 -31 -70 0 -34 6 -51 25 -69 57 -58 165
          -12 165 70 0 27 -8 44 -29 66 -39 38 -87 39 -130 3z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l5a},75.94%,54.29%)`} stroke="none">
          <path d="M766 2169 c-36 -28 -36 -80 0 -108 15 -12 34 -21 44 -21 10 0 29 9
          44 21 34 27 36 79 4 109 -28 26 -58 26 -92 -1z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l1b},75.94%,54.29%)`} stroke="none">
          <path d="M2315 1815 c-92 -25 -139 -54 -225 -137 -119 -115 -178 -190 -218
          -278 -36 -79 -38 -112 -12 -202 14 -47 90 -142 127 -156 48 -19 133 -30 166
          -22 119 30 138 44 232 164 148 190 230 376 211 477 -3 20 -22 55 -42 78 -63
          73 -148 100 -239 76z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l2c},75.94%,54.29%)`} stroke="none">
          <path d="M2275 1754 c-49 -18 -92 -58 -161 -151 -29 -39 -76 -97 -104 -129
          -101 -115 -132 -221 -90 -303 35 -69 114 -110 183 -96 80 17 174 91 214 169
          14 27 59 81 103 123 79 75 102 112 114 189 25 148 -112 252 -259 198z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l3d},75.94%,54.29%)`} stroke="none">
          <path d="M2055 1407 c-43 -24 -101 -94 -111 -133 -23 -90 70 -183 160 -160 66
          16 146 123 146 193 0 50 -21 97 -49 113 -33 17 -103 11 -146 -13z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l4d},75.94%,54.29%)`} stroke="none">
          <path d="M2080 1369 c-14 -6 -42 -26 -62 -47 -32 -31 -38 -42 -38 -79 0 -57
          33 -93 84 -93 50 0 63 7 102 58 28 36 34 54 34 92 0 72 -46 98 -120 69z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l5d},75.94%,54.29%)`} stroke="none">
          <path d="M2026 1299 c-19 -15 -26 -30 -26 -54 0 -36 36 -75 70 -75 10 0 29 9
          44 21 34 27 36 79 4 109 -28 26 -58 26 -92 -1z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l3e},75.94%,54.29%)`} stroke="none">
          <path d="M2303 1709 c-104 -51 -134 -218 -50 -269 48 -30 96 -26 153 13 61 40
          99 110 90 161 -8 44 -41 85 -82 102 -45 19 -62 18 -111 -7z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l4e},75.94%,54.29%)`} stroke="none">
          <path d="M2300 1663 c-52 -47 -54 -147 -5 -179 24 -15 29 -15 70 0 92 36 123
          119 66 177 -39 39 -89 40 -131 2z"/>
          </g>
          </svg>

          <svg className="RightBar-hand-base" version="1.0" xmlns="http://www.w3.org/2000/svg"
           width="330.000000px" height="446.000000px" viewBox="0 0 330.000000 446.000000"
           preserveAspectRatio="xMidYMid meet">

          <g transform="translate(0.000000,446.000000) scale(0.100000,-0.100000)"
          fill={`hsl(${l5e},75.94%,54.29%)`} stroke="none">
          <path d="M2316 1649 c-19 -15 -26 -30 -26 -54 0 -36 36 -75 70 -75 10 0 29 9
          44 21 34 27 36 79 4 109 -28 26 -58 26 -92 -1z"/>
          </g>
          </svg>
        </div>
      </div>
    )
  }
}

export default RightBar
